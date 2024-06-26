/*
 * entry.S - Entry point to system mode from user mode
 */

#include <asm.h>
#include <segment.h>
#include <errno.h>


/**************************************************/
/**** Save & Restore ******************************/
/**                                              **/
/** When we change to privilege level 0 (kernel) **/
/** (through an interrupt, a system call, an     **/
/** exception ...) we must save the state of the **/
/** currently running task (save).               **/
/**                                              **/
/** Stack layout in 'systemCall':                **/
/**                                              **/
/**   0(%esp) - %edx    \                        **/
/**   4(%esp) - %ecx     |                       **/
/**   8(%esp) - %ebx     |                       **/
/**   C(%esp) - %esi     | Register saved        **/
/**  10(%esp) - %edi     |  by 'save'            **/
/**  14(%esp) - %ebp     |                       **/
/**  18(%esp) - %eax     |                       **/
/**  1C(%esp) - %ds      |                       **/
/**  20(%esp) - %es      |                       **/
/**  24(%esp) - %fs      |                       **/
/**  28(%esp) - %gs     /                        **/
/**  2C(%esp) - %eip    \                        **/
/**  30(%esp) - %cs      |                       **/
/**  34(%esp) - %eflags  |  Return context saved **/
/**  38(%esp) - %oldesp  |   by the processor.   **/
/**  3C(%esp) - %oldss  /                        **/
/**                                              **/
/**************************************************/

#define SAVE_ALL \
      pushl %gs; \
      pushl %fs; \
      pushl %es; \
      pushl %ds; \
      pushl %eax; \
      pushl %ebp; \
      pushl %edi; \
      pushl %esi; \
      pushl %ebx; \
      pushl %ecx; \
      pushl %edx; \
      movl $__KERNEL_DS, %edx;    \
      movl %edx, %ds;           \
      movl %edx, %es;

#define RESTORE_ALL \
      popl %edx; \
      popl %ecx; \
      popl %ebx; \
      popl %esi; \
      popl %edi; \
      popl %ebp; \
      popl %eax; \
      popl %ds; \
      popl %es; \
      popl %fs; \
      popl %gs;

#define EOI \
      movb $0x20, %al ; \
      outb %al, $0x20 ;
      
ENTRY (syscall_handler_sysenter)
      push $__USER_DS
      push %ebp
      pushfl
      push $__USER_CS
      push 4(%ebp)
      SAVE_ALL
      cmpl $0, %eax
      jl sysenter_err
      cmpl $MAX_SYSCALL, %eax
      jg sysenter_err
      call *sys_call_table(, %eax, 0x04)
      jmp sysenter_fin
sysenter_err:
      movl $-38, %eax
sysenter_fin:
      movl %eax, 0x18(%esp)
      RESTORE_ALL
      movl (%esp), %edx
      movl 12(%esp), %ecx
      sti
      sysexit

ENTRY (clock_handler)
      SAVE_ALL
      EOI
      call clock_routine
      RESTORE_ALL
      iret

ENTRY (keyboard_handler)
      SAVE_ALL
      EOI
      call keyboard_routine
      RESTORE_ALL
      iret

ENTRY (my_page_fault_handler)
      # No SAVE_ALL, since we do not return to user mode.
      call my_page_fault_routine

ENTRY (writeMSR)
      # Description: Write the value in EDX:EAX to MSR specified by ECX.
      pushl %ebp
      movl %esp, %ebp

      # MSR to write
      movl 8(%ebp), %ecx

      # Value to store (EDX:EAX)
      movl $0, %edx        # Higher part 
      movl 12(%ebp), %eax  # Lower part
      
      # Call
      wrmsr

      popl %ebp
      ret

ENTRY (task_switch)
      pushl %ebp
      movl %esp, %ebp

      # Store ESI, EDI and EBX
      pushl %esi
      pushl %edi
      pushl %ebx

      # Pass *new
      pushl 8(%ebp)

      call inner_task_switch

      addl $4, %esp

      popl %ebx
      popl %edi
      popl %esi

      movl %ebp, %esp
      popl %ebp

      ret

ENTRY (inner_task_switch_as)
      pushl %ebp
      movl %esp, %ebp

      // I think this part is wrong
      movl %ebp, 8(%ebp)      // 3) current()->kernel_esp = %EBP
      movl 12(%ebp), %esp     // 4) %ESP = new->task.kernel_esp

      movl %ebp, %esp
      popl %ebp
      ret

ENTRY (getEbp)
      movl %ebp, %eax
      ret

ENTRY (setEsp)
      movl 4(%esp), %esp
      popl %ebp
      ret