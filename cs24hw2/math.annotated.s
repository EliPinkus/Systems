/* This file contains x86-64 assembly-language implementations of three
 * basic, very common math operations.
 *
 *
 * naive implementations would probably use conditional jumps
 * our functions avoid using these and instead use conditional instructions.
 * When modern CPUs reach jump statements it may or may not have the necesarry
 * information to determine where to go so it will use branch prediction, which can
 * be fast in the case where correct predictions are made but they do unneeded work
 * when the prediction is incorrect. Our approach directs the dependencies onto the data
 * rather than onto the control. Thus, our CPU will operate with 'full information' and
 * not need to do work that is ultimately discarded.
 */

        .text

/*====================================================================
 * int f1(int x, int y)
 *
 *
 * returns the min of two integers
 *
 * x and y are moved into registers edx and eax respectively.
 * cmpl is used to compare x and y and populate comparison flags
 * cmovg will overwrite the larger value with the lesser value if
 * needed.
 */
.globl f1
f1:
        movl    %edi, %edx # move x to edx
        movl    %esi, %eax # move y eax
        cmpl    %edx, %eax # comp edx and eax
        cmovg   %edx, %eax # # move if (not (SF XOR OF)) AND (NOT ZF)
        ret                # return eax


/*====================================================================
 * int f2(int x)
 *
 *
 * returns the absolute value of an integer
 *
 * x is moved to eax and copied to edx
 * 31 arithmatic right shifts are performed on x which means that
 * in the case where x >= 0, we know the topmost bit is 0, thus the result
 * of the shifting is 0
 * if x < 0, then the result of this shift would be 0xffff in hex.
 * we use the above calculated value as a bitmask when we XOR it with x.
 * if the bitmask is 0 then XOR will preserve x, otherwise if bitmask is
 * 0xffff it will bitwise negate x
 * we then subtract the bitmask from the result of the XOR above.
 * this was if x >= 0, we are adding 1 to ~x which would produce the 2s
 * complement of -x.
 * in sumation we return x if x >= 0 and -x if x < 0.
 */
.globl f2
f2:
        movl    %edi, %eax # move x to eax
        movl    %eax, %edx # move x to edx
        sarl    $31, %edx  # move x >> 31 to edx (note: if x>=0 result 0x0000
                           # else result 0xffff)
        xorl    %edx, %eax # move XOR(x, above bitmask) to eax (note: if x>= 0
                           # result x else result ~x)
        subl    %edx, %eax # move above result - bitmask to eax if x >= result x
                           # else result positive of original input)
        ret


/*====================================================================
 * int f3(int x)
 *
 * f3 returns 1 if x>0, 0 if x=0 and -1 and 0 if negative
 *
 * x is moved to edx and eax.
 * x is bitshifted right 31 times as in f2 to create the bitmask.
 * (stored in eax)
 * flags are then set for x (since X AND X = X), and flags are used to determine
 * return value as seen in comments below.
 *
 *
 */
.globl f3
f3:
        movl    %edi, %edx  # move x to edx
        movl    %edx, %eax  # move x to eax
        sarl    $31, %eax   # move x >> 31 to eax
                            # if x >= 0 then 0x0000, else 0xffff
        testl   %edx, %edx  # bitwise AND something with itself = itself
                            # sets SF if x is < 0 or ZF if x = 0
        movl    $1, %edx    # move 1 to edx
        cmovg   %edx, %eax  # move if (not (SF XOR OF)) AND (NOT ZF)
                            # if x>0 :
                            # (SF flag isn't set and 0 flag isn't set), move
                            # 1 to eax
                            # if x=0 :
                            # (ZF is set but sign flag is not) keep 0
                            # if x<0 :
                            # (sign flag is set but not 0 flag) we keep 0xffff

        ret                 # return eax
