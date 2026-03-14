[bits 32]
[extern kernel_main]    ; kernel_main is defined in kernel.c

call kernel_main        ; call our C kernel

hang:
    jmp hang            ; loop forever if kernel returns
