# __add_bigint
# int __add_bigint(uint64_t *a, uint64_t *b, uint8_t size)
# x86-64 implementation of addition for big int data type consisting of several
# 64 byte ints collectively represented in an array.
# ARGS: uint64_t *a  -> rdi
#       uint64_t *b  -> rsi
#       uint8_t size -> rdx
#
# RETURNS rax -> *b -> a + b

.global __add_bigint

__add_bigint:
    cmpq    $0, %rdx                # jump and return 1 if size == 0
    jz      __add_bigint_ret_one
    clc                             # clear carry flag
    movq    %rdx, %r8               # %r8 will be loop variable
    movq    %rdi, %r9               # move adr for a[0] to r9
    movq    %rsi, %r10              # move adr for b[0] to r10

__add_bigint_loop:
    movq    (%r9), %r15
    adcq    %r15, (%r10)            # perform b[i] += a[i] + carryover

    leaq    8(%r9), %r9             # lea used so no flag updates
    leaq    8(%r10), %r10           # update to consider a[i+1] and b[i+1]

    decq    %r8                     # dec loop variable
    jnz     __add_bigint_loop       # restart loop if loop variable = 0

__add_bigint_continue:
    jc      __add_bigint_ret_zero        # jump to return 0 if carry
    movq    $1, %rax                # return 1 otherwise
    ret


__add_bigint_ret_zero:
    movq    $0, %rax
    ret

__add_bigint_ret_one:
    movq    $1, %rax
    ret
