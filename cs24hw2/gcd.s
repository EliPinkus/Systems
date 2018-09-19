#=============================================================================
# The gcd(a,b) function recursively computes the GCD of a and b both > 0 using
# euclidean algorithm.
#
.global gcd
gcd:
    movl %edi, %eax     # save a to eax
    cqto                # sign extend rax to rdx:rax
    idivl %esi          # divide a by b; place remainder in edx
    cmpl $0, %edx       # compare remainder to 0
    jne gcd_continue    # if r!=0, continue to recursion
    movl %esi, %eax     # else move b to eax
    jmp gcd_return      # go ahead and return b below

gcd_continue:
    movl %esi, %edi     # move b into edi (it is the new a for rec call)
    movl %edx, %esi     # move remainder to esi (it is new b)
    call gcd            # call gcd recursively

gcd_return:
    ret                 # return item at eax
