#include <libc.h>

char buff[24];

int pid;

void show_current_time(){
    itoa(gettime(), buff);
    if(write(1, " Current time: ", strlen("Current time: ")) < 0) perror();
    if(write(1, buff, strlen(buff)) < 0) perror();
    if(write(1, "\n", 1) < 0) perror();
}

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
    /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */


    /* Uncomment this to test page fault exception */
    // char* p = 0;
    // *p = 'x';

    /* Testing Syscall: Write */
    if(write(1, "\n[!] Testing write syscall.\n", strlen("\n[!] Testing WRITE syscall.\n")) < 0) perror();
    
    /* Testing Syscall: getTime() */
    if(write(1, "\n[!] Testing getTime syscall.\n", strlen("\n [!] Testing getTime syscall.\n")) < 0) perror();
    show_current_time();
       
  while(1) { }
}
