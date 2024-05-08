#include <libc.h>

char buff[24];

int pid;

void my_print(char string[]){
  write(1, string, sizeof(string));
}

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  char buff2[256];

  write(1, "\nAntes de la espera.\n", sizeof("\nAntes de la espera.\n"));
  while(gettime() < 2000){}
  write(1, "\nDespues de la espera.\n", sizeof("\nDespues de la espera.\n"));

  int bytes = read(buff, 6);
  itoa(bytes, buff2);
  write(1, "He leido: ", sizeof("He leido: "));
  write(1, buff2, sizeof(buff2));
  write(1, "\nDespues del read.\n", sizeof("\nDespues del read.\n"));

  write(1, "\nAntes de escribir lo leido.\n", sizeof("\nAntes de escribir lo leido.\n"));
  write(1, buff, sizeof(buff));
  write(1, "\nDespues de escribir lo leido.\n", sizeof("\nDespues de escribir lo leido.\n"));
  while(1) { }
}
