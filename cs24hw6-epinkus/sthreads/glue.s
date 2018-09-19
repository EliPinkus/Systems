#============================================================================
# Keep a pointer to the main scheduler context.  This variable should be
# initialized to %rsp, which is done in the __sthread_start routine.
#
        .data
        .align 8
scheduler_context:      .quad   0


#============================================================================
# __sthread_switch is the main entry point for the thread scheduler.
# It has three parts:
#
#    1. Save the context of the current thread on the stack.
#       The context includes all of the integer registers and RFLAGS.
#
#    2. Call __sthread_scheduler (the C scheduler function), passing the
#       context as an argument.  The scheduler stack *must* be restored by
#       setting %rsp to the scheduler_context before __sthread_scheduler is
#       called.
#
#    3. __sthread_scheduler will return the context of a new thread.
#       Restore the context, and return to the thread.
#
        .text
        .align 8
        .globl __sthread_switch
__sthread_switch:

        # Save the process state onto its stack
        pushf
        push    %r8
        push    %r9
        push    %r10
        push    %r11
        push    %r12
        push    %r13
        push    %r14
        push    %r15
        push    %rax
        push    %rbx
        push    %rcx
        push    %rdx
        push    %rsi
        push    %rdi
        push    %rbp

        # Call the high-level scheduler with the current context as an argument
        movq    %rsp, %rdi
        movq    scheduler_context, %rsp
        call    __sthread_scheduler

        # The scheduler will return a context to start.
        # Restore the context to resume the thread.
__sthread_restore:

        movq    %rax, %rsp

        pop     %rbp
        pop     %rdi
        pop     %rsi
        pop     %rdx
        pop     %rcx
        pop     %rbx
        pop     %rax
        pop     %r15
        pop     %r14
        pop     %r13
        pop     %r12
        pop     %r11
        pop     %r10
        pop     %r9
        pop     %r8
        popf


        ret


#============================================================================
# Initialize a process context, given:
#    1. the stack for the process
#    2. the function to start
#    3. its argument
# The context should be consistent with that saved in the __sthread_switch
# routine.
#
# A pointer to the newly initialized context is returned to the caller.
# (This is the thread's stack-pointer after its context has been set up.)
#
# This function is described in more detail in sthread.c.
#
#
        .align 8
        .globl __sthread_initialize_context
__sthread_initialize_context:

        movq $__sthread_finish, (%rdi)

        movq %rsi, -8(%rdi) # Move function into context

        # We will 0 out register spots
        movq    $0, -16(%rdi)           # rflags
        movq    $0, -24(%rdi)           # r8
        movq    $0, -32(%rdi)           # r9
        movq    $0, -40(%rdi)           # r10
        movq    $0, -48(%rdi)           # r11
        movq    $0, -56(%rdi)           # r12
        movq    $0, -64(%rdi)           # r13
        movq    $0, -72(%rdi)           # r14
        movq    $0, -80(%rdi)           # r15
        movq    $0, -88(%rdi)           # rax
        movq    $0, -96(%rdi)           # rbx
        movq    $0, -104(%rdi)          # rcx
        movq    $0, -112(%rdi)          # rdx
        movq    $0, -120(%rdi)          # rsi
        movq    %rdx, -128(%rdi)        # shift 3rd arg to 1st
        movq    $0, -136(%rdi)          # rbp

        subq    $136, %rdi              # point to top of context
        movq    %rdi, %rax              # return top of context pointer
        ret


#============================================================================
# The start routine initializes the scheduler_context variable, and calls
# the __sthread_scheduler with a NULL context.
#
# The scheduler will return a context to resume.
#
        .align 8
        .globl __sthread_start
__sthread_start:
        # Remember the context
        movq    %rsp, scheduler_context

        # Call the scheduler with no context
        movl    $0, %edi  # Also clears upper 4 bytes of %rdi
        call    __sthread_scheduler

        # Restore the context returned by the scheduler
        jmp     __sthread_restore
