# DOS 🖥️

A minimal operating system built from scratch using Assembly and C.  
Built day by day as a learning project — from raw bootloader to a working 32-bit kernel.

---

## 📸 Preview

```
DeepithOS v0.1
Kernel Loaded Successfully!
Welcome Deepith!
```

---

## 🛠️ Tech Stack

| Tool | Purpose |
|------|---------|
| NASM | Assembler for bootloader and kernel entry |
| GCC | C compiler for kernel |
| LD | Linker to combine assembly + C |
| QEMU | Virtual machine to run the OS |
| Fedora Linux | Development environment |

---

## 📁 Project Structure

```
myos/
├── boot.asm          # Bootloader — first code executed by BIOS
├── kernel_entry.asm  # Kernel entry — bridges Assembly to C
├── kernel.c          # Kernel — written in C
├── linker.ld         # Linker script — memory layout
├── boot.bin          # Compiled bootloader
├── kernel.bin        # Compiled kernel
└── os.bin            # Final OS image (boot + kernel)
```

---

## 🚀 How to Build

### Requirements

```bash
sudo dnf install qemu nasm gcc make   # Fedora
sudo apt install qemu nasm gcc make   # Ubuntu/Debian
```

### Build Commands

```bash
# 1. Assemble bootloader
nasm boot.asm -f bin -o boot.bin

# 2. Assemble kernel entry
nasm kernel_entry.asm -f elf32 -o kernel_entry.o

# 3. Compile kernel C code
gcc -m32 -ffreestanding -fno-pic -c kernel.c -o kernel.o

# 4. Link everything together
ld -m elf_i386 -T linker.ld -o kernel.bin kernel_entry.o kernel.o --oformat binary

# 5. Combine bootloader + kernel into final image
cat boot.bin kernel.bin > os.bin
```

### Run

```bash
qemu-system-x86_64 -drive format=raw,file=os.bin
```

---

## 🧠 How It Works

### Boot Process

```
QEMU
 └─→ BIOS
      └─→ Bootloader (boot.asm) loaded at 0x7C00
           └─→ Kernel loaded from disk to 0x1000
                └─→ CPU switched to 32-bit Protected Mode
                     └─→ kernel_entry.asm calls kernel_main()
                          └─→ kernel.c runs — screen output
```

### Memory Layout

```
0x7C00   → Bootloader
0x1000   → Kernel
0x90000  → Stack
0xB8000  → VGA screen memory
```

---

## ✅ Features Implemented

- [x] Bootloader (512 bytes, fits in one sector)
- [x] Disk loading using BIOS INT 13h
- [x] GDT (Global Descriptor Table)
- [x] 32-bit Protected Mode switch
- [x] Assembly to C bridge
- [x] VGA text mode output
- [x] `print_char()` — print single character with color
- [x] `print_string()` — print full strings
- [x] `print_newline()` — cursor to next line
- [x] `clear_screen()` — wipe BIOS text, full screen control

---

## 🗺️ Roadmap

- [ ] Keyboard input (Day 7)
- [ ] Memory management
- [ ] Interrupt Descriptor Table (IDT)
- [ ] Basic shell
- [ ] Filesystem
- [ ] Run custom programs

---

## 📅 Development Log

| Day | Topic |
|-----|-------|
| Day 1–3 | Environment setup, QEMU, NASM |
| Day 4 | Bootloader + kernel execution verified |
| Day 5 | 32-bit protected mode + C kernel + print functions |
| Day 6 | clear_screen() + full display control |

---

## 👤 Author

**Deepith N**  
Building an OS from scratch, one day at a time.

---

## 📜 License

This project is open source and available under the [MIT License](LICENSE).
