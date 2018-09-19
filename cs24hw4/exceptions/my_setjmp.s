.global my_setjmp
.global my_longjmp

# the my_setjmp function saves the excecution state, consisting of certain
# registers and the stack pointer to the jump buffer which is represented by
# an array of x86-64 quadwords. Adr of jump buffer passed by rdi as convention.

my_setjmp:
    # move each callee save register with quadword offset between each
    movq    %rbp, (%rdi)
    movq    %rbx, 8(%rdi)
    movq    %r12, 16(%rdi)
    movq    %r13, 24(%rdi)
    movq    %r14, 32(%rdi)
    movq    %r15, 40(%rdi)
    movq    %rsp, 48(%rdi) # move stack pointer too!

    # push return address
    movq    (%rsp), %r8
    movq    %r8, 56(%rdi)

    # return 0
    movl    $0, %eax
    ret

my_longjmp:
    # revive callee save registers
    movq    (%rdi), %rbp
    movq    8(%rdi), %rbx
    movq    16(%rdi), %r12
    movq    24(%rdi), %r13
    movq    32(%rdi), %r14
    movq    40(%rdi), %r15
    movq    48(%rdi), %rsp

    # retreive return address
    mov     56(%rdi), %r8
    mov     %r8, (%rsp)

    # return
    mov $1,        %eax
    test %esi,     %esi
    cmovnz %esi,   %eax
    ret
