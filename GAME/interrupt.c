/*
 * interrupt.c -
 */
#include <types.h>
#include <interrupt.h>
#include <segment.h>
#include <hardware.h>
#include <io.h>
#include <errno.h>
#include <sched.h>
#include <mm.h>

#include <zeos_interrupt.h>

#include <circ_buffer.h>

Gate idt[IDT_ENTRIES];
Register    idtR;

char char_map[] =
{
  '\0','\0','1','2','3','4','5','6',
  '7','8','9','0','\'','�','\0','\0',
  'q','w','e','r','t','y','u','i',
  'o','p','`','+','\0','\0','a','s',
  'd','f','g','h','j','k','l','�',
  '\0','�','\0','�','z','x','c','v',
  'b','n','m',',','.','-','\0','*',
  '\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0','\0','\0','\0','\0','\0','7',
  '8','9','-','4','5','6','+','1',
  '2','3','0','\0','\0','\0','<','\0',
  '\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0'
};

int zeos_ticks = 0;

struct circ_buffer KEYBOARD_BUFFER;
struct circ_buffer *pBuffer = &(KEYBOARD_BUFFER);
extern Byte phys_mem[TOTAL_PAGES];

void clock_routine()
{
  zeos_show_clock();
  zeos_ticks ++;
  
  schedule();
}

void keyboard_routine()
{
  unsigned char c = inb(0x60);
  /* Keyboard management */
  if(c&0x80){
    push_circ_buffer(pBuffer, char_map[c&0x7f]);
  }

}

void print_hexa(unsigned long num){
  char* digits = "0123456789ABCDEF";
  for (int i = 28; i >= 0; i-=4)
  {
    printc(digits[(num>>i)&0xF]);
  }
}  

void setInterruptHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE INTERRUPTION GATE FLAGS:                          R1: pg. 5-11  */
  /* ***************************                                         */
  /* flags = x xx 0x110 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}

void setTrapHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE TRAP GATE FLAGS:                                  R1: pg. 5-11  */
  /* ********************                                                */
  /* flags = x xx 0x111 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);

  //flags |= 0x8F00;    /* P = 1, D = 1, Type = 1111 (Trap Gate) */
  /* Changed to 0x8e00 to convert it to an 'interrupt gate' and so
     the system calls will be thread-safe. */
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}

void clock_handler();
void keyboard_handler();
void system_call_handler();
void my_page_fault_handler();

void my_page_fault_routine(unsigned long param, unsigned long eip){
  unsigned int cr2 = get_cr2();
  int page = (int)(cr2>>12);
  page_table_entry *PT = get_PT(current());
  int frame = get_frame(PT, page);

  if(phys_mem[frame] < 1){
    char* msg = "\nProcess generates a PAGE FAULT exception at EIP: ";
    /* Print message */
    printk(msg);
    printk("0x");
    print_hexa(eip);
    while(1){}
  }
  
  if(phys_mem[frame] > 1){
    int new_frame = alloc_frame();
    int free_page = find_free_page();
    
    if(free_page == -ENOMEM){                                           /* No free PT entries: find one */
      int temp_page = TOTAL_PAGES-1;                                    // Search temporal page
      if(temp_page==page) temp_page -= 1;                               // Ensure is not the same page as the original one.
      int old_frame = get_frame(PT, temp_page);                         // Save its frame

      set_ss_pag(PT, temp_page, new_frame);                             // Map temporal page <---> new_fame
      copy_data((void*)(page<<12), (void*)(temp_page<<12), PAGE_SIZE);  // Copy data
      
      set_ss_pag(PT, temp_page, old_frame);                             // Recover mapping of temporal page
      set_ss_pag(PT, page, new_frame);                                  // Do mapping of new_frame <---> page

    } else {                                                            /* Free entries */
      set_ss_pag(PT, free_page, new_frame);                             // Map new_frame <---> free_entry
      copy_data((void*)(page<<12), (void*)(free_page<<12), PAGE_SIZE);  // Copy data
  
      del_ss_pag(PT, free_page);                                        // Delete mapping of temporal page
      set_ss_pag(PT, page, new_frame);                                  // Do mapping of new_frame <---> page
    }

    phys_mem[frame] -= 1;

  } else if(phys_mem[frame]==1) PT[page].bits.rw = 1;
  
  set_cr3(get_DIR(current()));
}

void setMSR(unsigned long msr_number, unsigned long high, unsigned long low);

void setSysenter()
{
  setMSR(0x174, 0, __KERNEL_CS);
  setMSR(0x175, 0, INITIAL_ESP);
  setMSR(0x176, 0, (unsigned long)system_call_handler);
}

void setIdt()
{
  /* Program interrups/exception service routines */
  idtR.base  = (DWord)idt;
  idtR.limit = IDT_ENTRIES * sizeof(Gate) - 1;
  
  set_handlers();

  /* ADD INITIALIZATION CODE FOR INTERRUPT VECTOR */
  setInterruptHandler(32, clock_handler, 0);
  setInterruptHandler(33, keyboard_handler, 0);
  setInterruptHandler(14, my_page_fault_handler, 0);

  setSysenter();

  set_idt_reg(&idtR);

  init_circ_buffer(pBuffer);
}

