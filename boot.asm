[org 0x7C00]

mov ah, 0x0E
mov al, 'B'
int 0x10

; Set es=0 so buffer address = es*16 + bx = 0 + 0x1000 = 0x1000
xor ax, ax
mov es, ax
mov bx, 0x1000

mov ah, 0x02
mov al, 64
mov ch, 0
mov cl, 2
mov dh, 0
int 0x13

cli
lgdt [gdt_descriptor]

mov eax, cr0
or eax, 1
mov cr0, eax

jmp 0x08:protected_mode

gdt_start:
    dq 0
gdt_code:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10011010b
    db 11001111b
    db 0x00
gdt_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

[bits 32]
protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov esp, 0x90000
    jmp 0x1000

times 510-($-$$) db 0
dw 0xAA55
