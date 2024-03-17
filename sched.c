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

	while(1)
	{
	;
	}
}

void init_idle (void)
{
	/* Get task_struct */
	struct list_head *first_e = list_first(&freeQueue);
	list_del(first_e);

	struct task_struct *ts = list_head_to_task_struct(first_e);

	/* Set PID = 0 */
	ts->PID = 0;

	/* Initialize dir_pages_baseAaddr */
	allocate_DIR(ts);

	/* Store in the stack the & of cpu_idle function (next process to execute) */
	((union task_union*)ts)->stack[KERNEL_STACK_SIZE - 1] = (unsigned long) cpu_idle;

	/* Store in the stack the initial value that we want to assign to register ebp when
	   undoing the dynamic link  
	*/
	((union task_union*)ts)->stack[KERNEL_STACK_SIZE - 2] = (unsigned long) 0;

	/* Store in a new field of task_struct the position of the stack where is the value for the ebp register*/
	((union task_union*)ts)->task.kernel_esp = &(((union task_union*)ts)->stack[KERNEL_STACK_SIZE - 2]);

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

	/* Initialize dir_pages_baseAaddr */
	allocate_DIR(ts);

	/* Initialize address space */
	set_user_pages(ts);

	/* Make TSS point to new task system stack and also MSR 0x175*/
	tss.esp0 = (DWord)((union task_union*)ts)->stack[KERNEL_STACK_SIZE];
	writeMSR(0x175, (unsigned long) tss.esp0);

	/* Set page directory */
	set_cr3(ts->dir_pages_baseAddr);
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

