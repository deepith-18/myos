// VGA text mode memory address
unsigned char *vga = (unsigned char *)0xB8000;
int cursor = 0;

void print_char(char c, unsigned char color) {
    vga[cursor * 2]     = c;
    vga[cursor * 2 + 1] = color;
    cursor++;
}

void print_string(char *str, unsigned char color) {
    int i = 0;
    while (str[i] != '\0') {
        print_char(str[i], color);
        i++;
    }
}

void print_newline() {
    cursor = cursor + (80 - (cursor % 80));
}

void clear_screen() {
    int i = 0;
    while (i < 80 * 25) {
        vga[i * 2]     = ' ';
        vga[i * 2 + 1] = 0x0F;
        i++;
    }
    cursor = 0;
}

void backspace() {
    if (cursor > 0) {
        cursor--;
        vga[cursor * 2]     = ' ';
        vga[cursor * 2 + 1] = 0x0F;
    }
}

// Declare functions from other files
char keyboard_read();
void shell_handle_key(char c);

// Kernel main
void kernel_main() {
    clear_screen();

    print_string("DeepithOS v0.1", 0x0B);
    print_newline();
    print_string("Kernel Loaded Successfully!", 0x0A);
    print_newline();
    print_newline();
    print_string("> ", 0x0E);

    // Main loop
    while (1) {
        char c = keyboard_read();
        if (c == 0) continue;

        // Handle backspace on screen
        if (c == '\b') {
            backspace();
        } else if (c != '\n') {
            print_char(c, 0x0F);
        }

        // Send key to shell for command processing
        shell_handle_key(c);
    }
}

