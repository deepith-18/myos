# DeepithOS Makefile

CC = gcc
CFLAGS = -m32 -ffreestanding -fno-pic -c
LD = ld
LDFLAGS = -m elf_i386 -T linker.ld --oformat binary

OBJS = kernel_entry.o kernel.o keyboard.o shell.o idt.o idt_c.o pic.o \
       timer.o memory.o fs.o process.o sysinfo.o snake.o editor.o

all: os.bin

boot.bin: boot.asm
	nasm boot.asm -f bin -o boot.bin

kernel_entry.o: kernel_entry.asm
	nasm kernel_entry.asm -f elf32 -o kernel_entry.o

idt.o: idt.asm
	nasm idt.asm -f elf32 -o idt.o

kernel.o: kernel.c
	$(CC) $(CFLAGS) kernel.c -o kernel.o

keyboard.o: keyboard.c
	$(CC) $(CFLAGS) keyboard.c -o keyboard.o

shell.o: shell.c
	$(CC) $(CFLAGS) shell.c -o shell.o

idt_c.o: idt.c
	$(CC) $(CFLAGS) idt.c -o idt_c.o

pic.o: pic.c
	$(CC) $(CFLAGS) pic.c -o pic.o

timer.o: timer.c
	$(CC) $(CFLAGS) timer.c -o timer.o

memory.o: memory.c
	$(CC) $(CFLAGS) memory.c -o memory.o

fs.o: fs.c
	$(CC) $(CFLAGS) fs.c -o fs.o

process.o: process.c
	$(CC) $(CFLAGS) process.c -o process.o

sysinfo.o: sysinfo.c
	$(CC) $(CFLAGS) sysinfo.c -o sysinfo.o

snake.o: snake.c
	$(CC) $(CFLAGS) snake.c -o snake.o

editor.o: editor.c
	$(CC) $(CFLAGS) editor.c -o editor.o

kernel.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o kernel.bin $(OBJS)

os.bin: boot.bin kernel.bin
	cat boot.bin kernel.bin > os.bin

run: os.bin
	qemu-system-x86_64 -drive format=raw,file=os.bin

clean:
	rm -f *.o *.bin

.PHONY: all run clean
