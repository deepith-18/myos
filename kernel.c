// VGA text mode memory address
unsigned char *vga = (unsigned char *)0xB8000;

// Current cursor position
int cursor = 0;

// Print a single character
void print_char(char c, unsigned char color) {
    vga[cursor * 2]     = c;
    vga[cursor * 2 + 1] = color;
    cursor++;
}

// Print a full string
void print_string(char *str, unsigned char color) {
    int i = 0;
    while (str[i] != '\0') {
        print_char(str[i], color);
        i++;
    }
}

// New line
void print_newline() {
    cursor = cursor + (80 - (cursor % 80));
}

// NEW — Clear entire screen
void clear_screen() {
    int i = 0;
    while (i < 80 * 25) {
        vga[i * 2]     = ' ';
        vga[i * 2 + 1] = 0x0F;
        i++;
    }
    cursor = 0;
}

// Kernel main
void kernel_main() {
    clear_screen();

    print_string("DeepithOS v0.1", 0x0B);
    print_newline();
    print_newline();
    print_string("Kernel Loaded Successfully!", 0x0A);
    print_newline();
    print_string("Welcome Deepith!", 0x0F);
}
