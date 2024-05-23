#include <libc.h>
#include <game.h>

/* -------------- Zeos Proyecto --------------*/
/*  Milestones OK hasta el 7                  */
/*                                            */
/*  Autores:                                  */
/*   - Josep Martínez                         */
/*   - Adrià Martínez                         */
/* ------------------------------------------ */


char b[20];

int __attribute__ ((__section__(".text.main"))) main(void){
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  /* ----------------- Testings ----------------- */
    int pid = fork();                            
    write(1, "\n[PID]: ", strlen("\n[PID]: "));  
    itoa(pid,b);                                   
    write(1, b, sizeof(b));                      
    if(pid==0) exit();                      
  /* -------------------------------------------- */

  /* ---- TEST GAME STUFF ---- */
    init_game();
  /* ------------------------- */

  while(1) { }
}
