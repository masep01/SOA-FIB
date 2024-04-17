/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <errno.h>

#define LECTURA 0
#define ESCRIPTURA 1

int global_PID = 300;

extern int zeos_ticks;
extern struct list_head freeQueue, readyQueue;

struct task_struct *global_child;
extern struct task_struct * idle_task;

char my_buffer[512];

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -EBADF; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -EACCES; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -ENOSYS; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int ret_from_fork()
{
  return 0;
}

int init_child_address_space(struct task_struct *child_ts){

  /* Free frames */
  int frameNumbers[NUM_PAG_DATA];
  for (int i = 0; i < NUM_PAG_DATA; i++)
  {
    // Search for free physical pages (frames) and get frame number
    frameNumbers[i] = alloc_frame();
    if(frameNumbers[i] < 0){
      // Set assigned frames free in case of error and put pcb in freequeue again
      for(int j = 0; j < i; ++j) free_frame(frameNumbers[j]);
      list_add_tail(&(child_ts->anchor), &freeQueue);
      return -EAGAIN;
    }
  }

  page_table_entry *PT_parent = get_PT(current());
  page_table_entry *PT_child = get_PT(child_ts);  
  int temp = 0;

  /* 1) SYSTEM CODE */
  for (int i = 0; i < NUM_PAG_KERNEL; i++)
  {
    set_ss_pag(PT_child, i, get_frame(PT_parent, i));
  }

  /* 2) USER CODE */
  for (int i = 0; i < NUM_PAG_CODE; i++)
  {
    temp = i+PAG_LOG_INIT_CODE;
    set_ss_pag(PT_child, temp, get_frame(PT_parent, temp));
  }
  
  /* 3) POINT NEW ALLOCATED FRAMES */
  for (int i = 0; i < NUM_PAG_DATA; i++)
  {
    temp = i+PAG_LOG_INIT_DATA;
    set_ss_pag(PT_child, temp, frameNumbers[i]);
  }

  /* 4) Copy DATA + STACK to child */
  int INITIAL = NUM_PAG_KERNEL;
	int TOTAL_SPACE = INITIAL + NUM_PAG_DATA;
  int FREE = NUM_PAG_KERNEL + NUM_PAG_CODE + NUM_PAG_DATA;

	for(int i = INITIAL; i < TOTAL_SPACE; i++){

		/* Temporal mapping of parent's PT free entry with child's frame */
    set_ss_pag(PT_parent, i+FREE, get_frame(PT_child, i));

		/* Copy data from parent's frame to child */
    copy_data((void *) (i << 12), (void *) ((i+FREE) << 12), PAGE_SIZE);
		
    /* Delete temporal mapping from parent's PT */
    del_ss_pag(PT_parent, i+FREE);
	}

  /* Flush parent's TLB to disable access from parent to child's TLB */
  set_cr3(get_DIR(current()));

  return 0;
}

int sys_fork()
{
  // 1) Get free task_struct
  if(list_empty(&freeQueue)) return -ENOMEM;
  struct list_head *lh = list_first(&freeQueue);
  list_del(lh);

  struct task_struct *child_ts = list_head_to_task_struct(lh);

  // 2) Copy parent's PCB to child
  union task_union *child_pcb = (union task_union*)child_ts;
  copy_data(current(), child_pcb, sizeof(union task_union));

  // 2.1) Block/unblock stuff
  INIT_LIST_HEAD(&(child_ts->children));
  list_add_tail(&(child_ts->anchor_parent), &(current()->children));
  child_ts->parent = current();

  // 3) New page directory for child
  allocate_DIR(child_ts);

  // 4) Initalize child's address space
  int res = init_child_address_space(child_ts);
  if(res < 0) return res;

  // 5) Assign PID
  child_pcb->task.PID = ++global_PID;

  // 5.1) Set state to READY
	child_pcb->task.state = ST_READY;

  // 6) Prepare child to task_switch call
  int new_ret = 1006;
    // Put @ret_from_fork in @ret
  child_pcb->stack[new_ret] = &ret_from_fork;
    // Put fake %ebp
  child_pcb->stack[new_ret-1] = 0;
    // Make %esp point to %ebp
  child_pcb->task.kernel_esp = &(child_pcb->stack[new_ret-1]);

  // 7) Add child to READY queue
  list_add_tail(&(child_pcb->task.anchor), &readyQueue);

  global_child = child_ts;
  return child_pcb->task.PID;
}

void sys_exit()
{  
  /* 1) Free the data structures and resources */
  for (int i = 0; i < NUM_PAG_DATA; i++)
  {
    // Free frames from current PAGE TABLE
    free_frame(get_frame(get_PT(current()), i+PAG_LOG_INIT_DATA));
    // Clean PAGE TABLE entries
    del_ss_pag(get_PT(current()), i+PAG_LOG_INIT_DATA);
  }

  /* 2) Invalidate PT and PID */
  current()->dir_pages_baseAddr = NULL;
  current()->PID = -1;

  /* 2.1) Block/unblock stuff */
  list_del(&(current()->anchor_parent));    // Remove child from its parent

  // Inherit childs to idle process
  struct list_head *children = &(current()->children);

  if(!list_empty(children)){
    struct list_head *child;
    list_for_each(child, children){
      list_del(child);
      list_add(child, &(idle_task->children));
    }
  }

  /* 3) Select new process to be executed */
  update_process_state_rr(current(), &freeQueue);
  sched_next_rr();
}

int sys_write(int fd, char * buffer, int size)
{
  /*
    fd: file descriptor. In this delivery it must always be 1.
    buffer: pointer to the bytes.
    size: number of bytes.
    
    return ’ Negative number in case of error (specifying the kind of error) and
    the number of bytes written if OK. 
  */

  /* Checkings */
  int c_fd = check_fd(fd, ESCRIPTURA);
  if(c_fd < 0) return c_fd;
  
  if(buffer == NULL) return -EFAULT;
  if(!access_ok(VERIFY_READ, buffer, size)) return -EFAULT;

  if(size < 0) return -EINVAL;

  /* Write data */
  int buffer_size = 512;
  
  int wr_bytes;
  int bytes_left = size;

  while(bytes_left > buffer_size){
    copy_from_user(buffer, my_buffer, buffer_size);
    wr_bytes = sys_write_console(my_buffer, buffer_size);

    bytes_left -= wr_bytes;
    buffer += wr_bytes;
  }

  if(bytes_left > 0){
    copy_from_user(buffer, my_buffer, bytes_left);
    wr_bytes = sys_write_console(my_buffer, bytes_left);
    
    bytes_left -= wr_bytes;
  }

  return size;
}

int sys_gettime(){
  return zeos_ticks;
}

void sys_block(){
  
}

int sys_unblock(int pid){

}