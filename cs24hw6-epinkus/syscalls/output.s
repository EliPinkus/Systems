# This function outputs from a buffer to std out via syscall intructions
# and by calling write()
# ID of both write and stdout are 1
# arg number of bytes -> %rsi
# arg print message -> %rdi

.global output

output:
    movq    %rsi, %rdx  # number of bytes for printing is 3rd arg for write
    movq    %rdi, %rsi  # print message is 2nd arg for write

    movq    $1, %rdi    # ID of stdout
    movq    $1, %rax    # op number for write

    syscall             # call to syscall to get write() going
    ret
