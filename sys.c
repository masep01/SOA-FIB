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

extern int zeos_ticks;
char my_buffer[512];

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int sys_fork()
{
  int PID=-1;

  // creates the child process
  
  return PID;
}

void sys_exit()
{  
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