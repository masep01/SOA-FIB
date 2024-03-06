#include <asm.h>

ENTRY(write)
    # Save base pointer
    pushl %ebp
    mov %esp, %ebp

    pushl %ebx
    pushl %ecx
    pushl %edx

    # Parameter passing (reverse order)
    movl 8(%ebp),  %edx          # int size      (3)
    movl 12(%ebp), %ecx          # char *buffer  (2)
    movl 16(%ebp), %ebx          # int fd        (1)
    
    # Put syscall ID in EAX
    movl $4, %eax

    # Store ECX and EDX in user stack
    pushl %ecx
    pushl %edx

    # Store return address and sysenter
    pushl $write_return
    
    # Fake dynamic link
    pushl %ebp
    movl %esp, %ebp

    # Enter the system
    sysenter

write_return:
    # Remove temporal data
    popl %ebp
    addl $4, %esp
	popl %edx
	popl %ecx

    # Process the result
    cmpl $0, %eax
    jl write_error        # If eax < 0, error. Return otherwise.
    jmp return

write_error:
    # Obtain positive errno
    negl %eax
    movl %eax, errno
    movl $-1, %eax

return:
    popl %edx
    popl %ecx
    popl %ebx
    
    movl %esp, %ebp
    popl %ebp
	ret