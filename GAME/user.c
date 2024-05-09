#include <libc.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  char buff2[256];
  write(1, "\nTienes 2000 tics para escribir algo:\n", sizeof("\nTienes 2000 tics para escribir algo:\n"));
  write(1, "\nLeyendo teclas...\n", sizeof("\nLeyendo teclas...\n"));
  while(gettime() < 2000){}

  int bytes = read(buff, 20);
  gotoxy(40,12);
  set_color(0x50, 0x50);
  write(1, buff, sizeof(buff));

  while(1) { }
}
