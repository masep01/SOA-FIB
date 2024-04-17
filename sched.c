/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>

union task_union task[NR_TASKS]
  __attribute__((__section__(".data.task")));


struct task_struct *list_head_to_task_struct(struct list_head *l)
{
	return (struct task_struct*)((int)l&0xfffff000);
}

extern struct list_head blocked;

struct list_head freeQueue;
struct list_head readyQueue;

struct task_struct * idle_task;

int quantum_left = 0;

/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


/* Extra useful functions */

// Returns %ebp
unsigned long * getEbp();

// Sets %esp with value of kernel_esp
void setEsp(unsigned long kernel_esp);

int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

	printk("Estoy en idle!\n");
	while(1) {}
}

void init_idle (void)
{
	/* Get task_struct */
	struct list_head *first_e = list_first(&freeQueue);
	list_del(first_e);

	struct task_struct *ts = list_head_to_task_struct(first_e);

	/* Set PID = 0 */
	ts->PID = 0;

	/* Set quantum */
	set_quantum(ts, DEFAULT_QUANTUM);

	/* Initialize dir_pages_baseAaddr */
	allocate_DIR(ts);

	/* Store in the stack the & of cpu_idle function (next process to execute) */
	((union task_union*)ts)->stack[KERNEL_STACK_SIZE - 1] = (unsigned long) cpu_idle;

	/* Store in the stack the initial value that we want to assign to register ebp when
	 * undoing the dynamic link  
	*/
	((union task_union*)ts)->stack[KERNEL_STACK_SIZE - 2] = (unsigned long) 0;

	/* Store in a new field of task_struct the position of the stack where is the value for the ebp register*/
	((union task_union*)ts)->task.kernel_esp = &(((union task_union*)ts)->stack[KERNEL_STACK_SIZE - 2]);

	/* Initialize children list */
	INIT_LIST_HEAD(&(ts->children));

	/* Initilaize idle_task variable */
	idle_task = ts;
}

void init_task1(void)
{
	/* Get task_struct */
	struct list_head *first_e = list_first(&freeQueue);
	list_del(first_e);

	struct task_struct *ts = list_head_to_task_struct(first_e);

	/* Set PID = 1 */
	ts->PID = 1;

	/* Set quantum */
	set_quantum(ts, DEFAULT_QUANTUM);
	quantum_left = ts->quantum;

	/* Initialize dir_pages_baseAaddr */
	allocate_DIR(ts);

	/* Initialize address space */
	set_user_pages(ts);

	/* Make TSS point to new task system stack and also MSR 0x175*/
	tss.esp0 = (DWord)&((union task_union*)ts)->stack[KERNEL_STACK_SIZE];
	writeMSR(0x175, (unsigned long) tss.esp0);

	/* Set page directory */
	set_cr3(ts->dir_pages_baseAddr);

	/* Initialize children list */
	INIT_LIST_HEAD(&(ts->children));
}

void init_sched()
{
	/* Free Queue */
	INIT_LIST_HEAD(&freeQueue);

	for (int i = 0; i < NR_TASKS; i++)
	{
		list_add(&(task[i].task.anchor), &freeQueue);
	}
	
	/* Ready Queue */
	INIT_LIST_HEAD(&readyQueue);
}

struct task_struct* current()
{
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000);
}

void task_switch(union task_union *new);

void inner_task_switch_as(unsigned long *current_kernel_esp, unsigned long *new_kernel_esp);

void inner_task_switch(union task_union *new){

	/* 1) Update pointer to Sys Stack of the new process */
	tss.esp0 = (DWord)&((union task_union*)new)->stack[KERNEL_STACK_SIZE];
	writeMSR(0x175, (unsigned long) tss.esp0);
	
	/* 2) Change user space address space */
	set_cr3(get_DIR(&(new->task)));
	
	/* Call Assembler part */
	//inner_task_switch_as(current()->kernel_esp, new->task.kernel_esp);
	current()->kernel_esp = (unsigned long *) getEbp();

	setEsp(new->task.kernel_esp);
}

int get_quantum(struct task_struct *t)
{
  return t->quantum;
}

void set_quantum(struct task_struct *t, int new_quantum)
{
  t->quantum=new_quantum;
}

void update_sched_data_rr (void){
	--quantum_left;
}

int needs_sched_rr (void){
	if(quantum_left > 0 ) return 0;

	if(list_empty(&readyQueue)){
		quantum_left = get_quantum(current());
		return 0;

	} else return 1;
}

void update_process_state_rr (struct task_struct *t, struct list_head *dst_queue){
	
	/*
	 *  If process is not Running, then we delete it from its queue.
	 *	If it is Running, then it is not in any queue.
	*/
	if(t->state != ST_RUN) list_del(&t->anchor);

	/*
	 *  If dest queue is not NULL (not RUNNING), means that the new state is READY or BLOCKED
	 *	We change the state by checking which Queue is the dest queue, and add process to it if needed.
	*/
	if(dst_queue != NULL){
		if(dst_queue == &readyQueue) t->state = ST_READY;
		else t->state = ST_BLOCKED;
		list_add_tail(&(t->anchor), dst_queue);

	} else t->state = ST_RUN;
}

void sched_next_rr (void){
	struct task_struct *next = idle_task;

	/* Check if a process is ready to switch, otherwise idle will run */
	if(!list_empty(&readyQueue)){
		struct list_head *lh = list_first(&readyQueue);
		list_del(lh);
		next = list_head_to_task_struct(lh);
	}
	
	/* Update the new quantum and call task_switch */
	quantum_left = get_quantum(next);
	task_switch((union task_union*)next);
}

void schedule(){
	/* Update the number of ticks that the process has executed since it got assigned the cpu */
	update_sched_data_rr();

	/* Check if is necessary to change the current process*/
	if(needs_sched_rr()) {
		/* Update the current state of a process to a new state */
		update_process_state_rr(current(), &readyQueue);
		/* Select the next process to execte and call task_switch */
		sched_next_rr();
	}
}