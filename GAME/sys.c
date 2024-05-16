/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <p_stats.h>

#include <errno.h>

#include <circ_buffer.h>

#include <io.h>

#include <sched.h>

#define LECTURA 0
#define ESCRIPTURA 1

void * get_ebp();

extern struct circ_buffer *pBuffer;

extern Byte x, y, color;
extern struct shared_page *pShared_mem;

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -EBADF; 
  if (permissions!=ESCRIPTURA) return -EACCES; 
  return 0;
}

void user_to_system(void)
{
  update_stats(&(current()->p_stats.user_ticks), &(current()->p_stats.elapsed_total_ticks));
}

void system_to_user(void)
{
  update_stats(&(current()->p_stats.system_ticks), &(current()->p_stats.elapsed_total_ticks));
}

int sys_ni_syscall()
{
	return -ENOSYS; 
}

int sys_getpid()
{
	return current()->PID;
}

int global_PID=1000;

int ret_from_fork()
{
  return 0;
}

void inherit_shared_mem(union task_union *uchild){
  page_table_entry *parent_PT = get_PT(current());
  page_table_entry *child_PT = get_PT(&uchild->task);

  for(int pg = NUM_PAG_KERNEL+NUM_PAG_CODE+NUM_PAG_DATA; pg<TOTAL_PAGES;++pg){    // Search in parent's PT for shared pages
    int shared = get_frame(parent_PT, pg);                                        //
    if(shared!=0){                                                                //
      for(int i=0; i<SH_PAGES; ++i){                                              // Search in shared_mem vector for a match
        if((pShared_mem)[i].frameId == shared){                                   //
          (pShared_mem)[i].refs += 1;                                             //
          set_ss_pag(child_PT, (void*)(pg), get_frame(parent_PT, pg));            // Link to child's PT
        }
      }
    }
  }
}

int sys_fork(void)
{
  struct list_head *lhcurrent = NULL;
  union task_union *uchild;
  
  /* Any free task_struct? */
  if (list_empty(&freequeue)) return -ENOMEM;

  lhcurrent=list_first(&freequeue);
  
  list_del(lhcurrent);
  
  uchild=(union task_union*)list_head_to_task_struct(lhcurrent);
  
  /* Copy the parent's task struct to child's */
  copy_data(current(), uchild, sizeof(union task_union));
  
  /* new pages dir */
  allocate_DIR((struct task_struct*)uchild);
  
  /* Allocate pages for DATA+STACK */
  page_table_entry *process_PT = get_PT(&uchild->task);
  int pag;
 // int new_ph_pag, pag, i;
 // for (pag=0; pag<NUM_PAG_DATA; pag++)
 // {
 //   new_ph_pag=alloc_frame();
 //   if (new_ph_pag!=-1) /* One page allocated */
 //   {
 //     set_ss_pag(process_PT, PAG_LOG_INIT_DATA+pag, new_ph_pag);
 //   }
 //   else /* No more free pages left. Deallocate everything */
 //   {
 //     /* Deallocate allocated pages. Up to pag. */
 //     for (i=0; i<pag; i++)
 //     {
 //       free_frame(get_frame(process_PT, PAG_LOG_INIT_DATA+i));
 //       del_ss_pag(process_PT, PAG_LOG_INIT_DATA+i);
 //     }
 //     /* Deallocate task_struct */
 //     list_add_tail(lhcurrent, &freequeue);
 //     
 //     /* Return error */
 //     return -EAGAIN; 
 //   }
 // } 

  /* Copy parent's SYSTEM and CODE to child. */
  page_table_entry *parent_PT = get_PT(current());
  for (pag=0; pag<NUM_PAG_KERNEL; pag++)
  {
    set_ss_pag(process_PT, pag, get_frame(parent_PT, pag));
  }
  for (pag=0; pag<NUM_PAG_CODE; pag++)
  {
    set_ss_pag(process_PT, PAG_LOG_INIT_CODE+pag, get_frame(parent_PT, PAG_LOG_INIT_CODE+pag));
  }
  /* Copy parent's DATA to child. We will use TOTAL_PAGES-1 as a temp logical page to map to */
  for (pag=NUM_PAG_KERNEL+NUM_PAG_CODE; pag<NUM_PAG_KERNEL+NUM_PAG_CODE+NUM_PAG_DATA; pag++)
  {
    
    /* Map one child page to parent's address space. */
    //set_ss_pag(parent_PT, pag+NUM_PAG_DATA, get_frame(process_PT, pag));
    //copy_data((void*)(pag<<12), (void*)((pag+NUM_PAG_DATA)<<12), PAGE_SIZE);
    //del_ss_pag(parent_PT, pag+NUM_PAG_DATA);
    
    int frame = get_frame(parent_PT, pag);

    set_ss_pag(process_PT, pag, frame);
    phys_mem[frame] += 1;
    process_PT[pag].bits.rw = 0;
    parent_PT[pag].bits.rw = 0;
  }

  /* -------MILESTONE 4-5 ------ */
  inherit_shared_mem(uchild);
  /* --------------------------- */

  /* Deny access to the child's memory space */
  set_cr3(get_DIR(current()));

  uchild->task.PID=++global_PID;
  uchild->task.state=ST_READY;

  int register_ebp;		/* frame pointer */
  /* Map Parent's ebp to child's stack */
  register_ebp = (int) get_ebp();
  register_ebp=(register_ebp - (int)current()) + (int)(uchild);

  uchild->task.register_esp=register_ebp + sizeof(DWord);

  DWord temp_ebp=*(DWord*)register_ebp;
  /* Prepare child stack for context switch */
  uchild->task.register_esp-=sizeof(DWord);
  *(DWord*)(uchild->task.register_esp)=(DWord)&ret_from_fork;
  uchild->task.register_esp-=sizeof(DWord);
  *(DWord*)(uchild->task.register_esp)=temp_ebp;

  /* Set stats to 0 */
  init_stats(&(uchild->task.p_stats));

  /* Queue child process into readyqueue */
  uchild->task.state=ST_READY;
  list_add_tail(&(uchild->task.list), &readyqueue);
  
  return uchild->task.PID;
}

#define TAM_BUFFER 512

int sys_write(int fd, char *buffer, int nbytes) {
char localbuffer [TAM_BUFFER];
int bytes_left;
int ret;

	if ((ret = check_fd(fd, ESCRIPTURA)))
		return ret;
	if (nbytes < 0)
		return -EINVAL;
	if (!access_ok(VERIFY_READ, buffer, nbytes))
		return -EFAULT;
	
	bytes_left = nbytes;
	while (bytes_left > TAM_BUFFER) {
		copy_from_user(buffer, localbuffer, TAM_BUFFER);
		ret = sys_write_console(localbuffer, TAM_BUFFER);
		bytes_left-=ret;
		buffer+=ret;
	}
	if (bytes_left > 0) {
		copy_from_user(buffer, localbuffer,bytes_left);
		ret = sys_write_console(localbuffer, bytes_left);
		bytes_left-=ret;
	}
	return (nbytes-bytes_left);
}


extern int zeos_ticks;

int sys_gettime()
{
  return zeos_ticks;
}

void dettach_shared_mem(){
  page_table_entry *PT = get_PT(current());
  for(int pg = NUM_PAG_KERNEL+NUM_PAG_CODE+NUM_PAG_DATA; pg<TOTAL_PAGES;++pg){
    int shared = get_frame(PT, pg);

    for(int i=0; i<SH_PAGES; ++i){                                              
      if((pShared_mem)[i].frameId == shared){                               // Look for shared pages
        if((pShared_mem)[i].refs == 0 && (pShared_mem)[i].marked) {         // If is a marked page, set 0
          memset((void*)(pg<<12), 0, PAGE_SIZE);
        }        
                                                                            
        (pShared_mem)[i].refs -= 1;                                         // Decrement references
        del_ss_pag(PT, pg);                                                 // Unlink page from PT
      }
    }
  }
  set_cr3(get_DIR(current()));
}

void sys_exit()
{  
  int i;

  page_table_entry *process_PT = get_PT(current());

  // Deallocate all the propietary physical pages
  for (i=0; i<NUM_PAG_DATA; i++)
  {
    free_frame(get_frame(process_PT, PAG_LOG_INIT_DATA+i));
    del_ss_pag(process_PT, PAG_LOG_INIT_DATA+i);
  }
  
  /* --- MILESTONE 4-5 --- */
  dettach_shared_mem();
  /* --------------------- */

  /* Free task_struct */
  list_add_tail(&(current()->list), &freequeue);
  
  current()->PID=-1;
  
  /* Restarts execution of the next process */
  sched_next_rr();
}

/* System call to force a task switch */
int sys_yield()
{
  force_task_switch();
  return 0;
}

extern int remaining_quantum;

int sys_get_stats(int pid, struct stats *st)
{
  int i;
  
  if (!access_ok(VERIFY_WRITE, st, sizeof(struct stats))) return -EFAULT; 
  
  if (pid<0) return -EINVAL;
  for (i=0; i<NR_TASKS; i++)
  {
    if (task[i].task.PID==pid)
    {
      task[i].task.p_stats.remaining_ticks=remaining_quantum;
      copy_to_user(&(task[i].task.p_stats), st, sizeof(struct stats));
      return 0;
    }
  }
  return -ESRCH; /*ESRCH */
}

/* GAME STUFF */
int sys_read(char* b, int maxchars){

  if(b == NULL) return -EINVAL;
  if(maxchars < 0) return -EINVAL;
  if(!access_ok(VERIFY_READ, b, maxchars)) return -EINVAL;
  
  int i = 0;
  int bStatus = 0;
  char c;

  while((i < maxchars) && (bStatus != -1)){
    bStatus = pop_circ_buffer(pBuffer, &c);
    if(bStatus != -1) {
      copy_to_user(&c, &b[i], sizeof(c));
      ++i;
    }
  }
  
  return i;
}

/* MILESTONE 3 */
int sys_gotoxy(int dest_x, int dest_y){
  if (dest_x < 0 || dest_x >= NUM_COLUMNS) return -EINVAL;
  if (dest_y < 0 || dest_y >= NUM_ROWS)    return -EINVAL;

  x = dest_x;
  y = dest_y;

  return 0;
}

int sys_set_color(int fg, int bg){

  if (fg < 0 || fg > 15) return -EINVAL;
  if (bg < 0 || bg >  7) return -EINVAL;

  color = 0 | bg << 4 | fg;

  return 0;
}

/* 
------------------- MILESTONE 4-5 --------------------- 
|  Changes to perform:                                |
|    - fork: inherit shared memory.                   |
|    - exit: delete shared memory links.              |
|    - acces_ok: check new shared memory regions.     |
-------------------------------------------------------
*/

/* Find a free LOGICAL page */
unsigned int find_free_page(){
  page_table_entry *PT = get_PT(current());
  int pag = NUM_PAG_CODE+NUM_PAG_DATA+NUM_PAG_KERNEL;
  int found = 0;
  while(pag<TOTAL_PAGES && !found){
    if(!PT[pag].bits.present){
      found = 1;

    } else ++pag;   
  } 

  if(!found) return -ENOMEM;
  return pag;
}

void *sys_shmat(int id, void* addr){

  if(!((int)(addr) & 0xfffff000)) return -EINVAL;
  if(id<0 || id>9) return -EINVAL;

  unsigned long pag = (unsigned long)addr>>12;
  if(pag > TOTAL_PAGES) return -EINVAL;

  page_table_entry *PT = get_PT(current());
  int free_addr = addr;
  
  /* If addr is null or it's not free, we must find a free addr */
  if((addr == NULL) || get_frame(PT, pag)!=0){
    unsigned int free_page = find_free_page();
    if(free_page == -ENOMEM) return -ENOMEM;
    free_addr = free_page<<12;

  } // If it's free, all correct!
  
  (pShared_mem)[id].refs += 1;
  set_ss_pag(PT, (void*)(free_addr), (pShared_mem)[id].frameId);
  return (void *)free_addr;
}

int sys_shmdt(void *addr){
  if(!((int)(addr) & 0xfffff000)) return -EINVAL;
  page_table_entry *PT = get_PT(current());

  unsigned logical_page = (unsigned long)addr>>12;
  int shared = get_frame(PT, logical_page);
  if(shared==0) return -EINVAL;

  int id = 0;
  while((id<SH_PAGES)&&((pShared_mem)[id].frameId != shared)){        // Find index in Shared_mem vector
    ++id;
  }
  
  if((pShared_mem)[id].refs == 0 && (pShared_mem)[id].marked) {       // If is a marked page, set 0
    memset((void*)logical_page, 0, PAGE_SIZE);
    (pShared_mem)[id].marked = 0;
  }                                                                            
  (pShared_mem)[id].refs -= 1;                                        // Decrement references
  free_frame(get_frame(PT, logical_page));                            // Deallocate frame
  del_ss_pag(PT, logical_page);                                       // Unlink page from PT
  set_cr3(get_DIR(current()));

  return 0;
}

int sys_shmrm(int id){
  if(id<0 || id>9) return -EINVAL;
  (pShared_mem)[id].marked = 1;
  return 0;
}