#include <libc.h>
#include <game.h>

void test_pfault(){
  char* p = 0;
  *p = 'x';
}

int __attribute__ ((__section__(".text.main"))) main(void){
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  //test_pfault();
  fork();
  //init_game();
  write(1, "Hola", sizeof("Hola"));
  
  while(1) { }
}
