#include <asm.h>

ENTRY(write)
    # Save base pointer
    pushl %ebp
    mov %esp, %ebp
    pushl %ebx
    pushl %ecx
    pushl %edx

    # Parameter passing (reverse order)
    mov 8(%ebp),  %ebx          # int size      (3)
    mov 12(%ebp), %ecx          # char *buffer  (2)
    mov 16(%ebp), %edx          # int fd        (1)
    
    # Put syscall ID in EAX
    mov $4, %eax

    # Store ECX and EDX in user stack
    pushl %ecx
    pushl %edx

    # Store return address and sysenter
    pushl $write_return
    
    # Fake dynamic link
    pushl %ebp
    mov %esp, %ebp

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
    movl -1, %eax

return:
    popl %edx
    popl %ecx
    popl %ebx
    
    mov %esp, %ebp
    popl %ebp
	ret