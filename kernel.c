unsigned char *vga = (unsigned char *)0xB8000;
int cursor = 0;

void vga_port_write(unsigned short port, unsigned char data) {
    __asm__("outb %0, %1" : : "a"(data), "Nd"(port));
}

void update_cursor() {
    unsigned short pos = cursor;
    vga_port_write(0x3D4, 0x0F);
    vga_port_write(0x3D5, (unsigned char)(pos & 0xFF));
    vga_port_write(0x3D4, 0x0E);
    vga_port_write(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
}

void cursor_enable() {
    vga_port_write(0x3D4, 0x0A);
    vga_port_write(0x3D5, (0 & 0xC0) | 13);
    vga_port_write(0x3D4, 0x0B);
    vga_port_write(0x3D5, (0 & 0xE0) | 15);
}

void scroll() {
    int i = 0;
    while (i < 80 * 24) {
        vga[i * 2]     = vga[(i + 80) * 2];
        vga[i * 2 + 1] = vga[(i + 80) * 2 + 1];
        i++;
    }
    int j = 80 * 24;
    while (j < 80 * 25) {
        vga[j * 2]     = ' ';
        vga[j * 2 + 1] = 0x0F;
        j++;
    }
    cursor = 80 * 24; // Pin cursor to the start of the final row
    update_cursor();
}

void print_char(char c, unsigned char color) {
    // CRITICAL: Scroll if cursor has advanced past screen bounds before printing
    if (cursor >= 80 * 25) {
        scroll();
    }

    vga[cursor * 2]     = c;
    vga[cursor * 2 + 1] = color;
    cursor++;
    update_cursor();
}

void print_string(char *str, unsigned char color) {
    int i = 0;
    while (str[i] != '\0') {
        print_char(str[i], color);
        i++;
    }
}

void print_newline() {
    // Jump cursor to the beginning of the next row
    cursor = cursor + (80 - (cursor % 80));
    
    // CRITICAL: Immediately check boundary constraints after computing line carriage return
    if (cursor >= 80 * 25) {
        scroll();
    }
    update_cursor();
}

void clear_screen() {
    int i = 0;
    while (i < 80 * 25) {
        vga[i * 2]     = ' ';
        vga[i * 2 + 1] = 0x0F;
        i++;
    }
    cursor = 0;
    update_cursor();
}

void backspace() {
    if (cursor > 0) {
        cursor--;
        vga[cursor * 2]     = ' ';
        vga[cursor * 2 + 1] = 0x0F;
        update_cursor();
    }
}

void print_int(int num, unsigned char color) {
    if (num == 0) { print_char('0', color); return; }
    char digits[12];
    int i = 0;
    if (num < 0) { print_char('-', color); num = -num; }
    while (num > 0) { digits[i] = '0' + (num % 10); num = num / 10; i++; }
    while (i > 0) { i--; print_char(digits[i], color); }
}

void print_hex(unsigned int num, unsigned char color) {
    char hex_chars[] = "0123456789ABCDEF";
    print_string("0x", color);
    int i;
    for (i = 7; i >= 0; i--) {
        unsigned int nibble = (num >> (i * 4)) & 0xF;
        print_char(hex_chars[nibble], color);
    }
}

// User subsystem linkages
void users_init();
int users_authenticate(char *username, char *password);
char *users_get_current();

char keyboard_read();
void shell_handle_key(char c);
void idt_init();
void mem_init();
void fs_init();
void proc_init();
void cpu_detect();

void kernel_main() {
    mem_init();
    fs_init(); 
    proc_init();
    cpu_detect();
    cursor_enable();
    clear_screen();
    
    // Initialize user structures safely from disk configurations
    users_init();

    char input_user[32];
    char input_pass[32];
    int attempts = 3;
    int authenticated = 0;

    while (attempts > 0 && !authenticated) {
        int k;
        for (k = 0; k < 32; k++) { input_user[k] = 0; input_pass[k] = 0; }
        int pos = 0;

        print_string("================================================================================", 0x08);
        print_newline();
        print_string("          Welcome to DeepithOS v0.1 - Built by Deepith                         ", 0x0B);
        print_newline();
        print_string("================================================================================", 0x08);
        print_newline();
        print_newline();
        
        // Input Username
        print_string("  Enter username: ", 0x0E);
        while (1) {
            char c = keyboard_read();
            if (c == 0) continue;
            if (c == '\n') break;
            if (c == '\b') {
                if (pos > 0) {
                    pos--;
                    input_user[pos] = 0;
                    backspace();
                }
            } else if (pos < 31 && c >= 32 && c < 127) {
                input_user[pos++] = c;
                print_char(c, 0x0F);
            }
        }
        print_newline();

        // Input Password
        print_string("  Enter password: ", 0x0E);
        pos = 0;
        while (1) {
            char c = keyboard_read();
            if (c == 0) continue;
            if (c == '\n') break;
            if (c == '\b') {
                if (pos > 0) {
                    pos--;
                    input_pass[pos] = 0;
                    backspace();
                }
            } else if (pos < 31 && c >= 32 && c < 127) {
                input_pass[pos++] = c;
                print_char('*', 0x0F);
            }
        }

        // Authenticate inside the dynamic abstract subsystem
        if (users_authenticate(input_user, input_pass)) {
            authenticated = 1;
        } else {
            attempts--;
            print_newline();
            print_newline();
            if (attempts > 0) {
                print_string("  Invalid Credentials! Attempts left: ", 0x0C);
                print_char('0' + attempts, 0x0C);
                print_newline();
                print_newline();
                
                unsigned int delay;
                for (delay = 0; delay < 30000000; delay++) { __asm__("nop"); }
            } else {
                print_string("  Access Denied! System locked.", 0x0C);
                print_newline();
                while (1) { __asm__("hlt"); }
            }
            clear_screen();
        }
    }

    // Welcome message
    clear_screen();
    print_string("================================================================================", 0x08);
    print_newline();
    print_string("          Welcome to DeepithOS v0.1 - Built by Deepith                         ", 0x0B);
    print_newline();
    print_string("================================================================================", 0x08);
    print_newline();
    print_newline();
    print_string("  Access Granted! Welcome back, ", 0x0A);
    print_string(users_get_current(), 0x0A);
    print_string("!", 0x0A);
    print_newline();
    print_newline();

    unsigned int wp;
    for (wp = 0; wp < 80000000; wp++) { __asm__("nop"); }

    clear_screen();

    // Boot animation sequence
    int i;
    print_string("================================================================================", 0x08);
    print_newline();
    print_string("          Welcome to DeepithOS v0.1 - Built by Deepith                         ", 0x0B);
    print_newline();
    print_string("          x86 32-bit Protected Mode Kernel                                      ", 0x0A);
    print_newline();
    print_string("================================================================================", 0x08);
    print_newline();
    print_newline();

    char *steps[] = {
        "  Initializing memory manager   ",
        "  Loading filesystem             ",
        "  Starting process manager      ",
        "  Detecting CPU                 ",
        "  Setting up hardware cursor    ",
        "  Loading shell                 ",
        "  Starting DeepithOS            ",
        0
    };

    int step = 0;
    while (steps[step]) {
        print_string(steps[step], 0x07);
        print_string(" [", 0x08);

        int filled = ((step + 1) * 20) / 7;
        for (i = 0; i < 20; i++) {
            if (i < filled) { print_char('#', 0x0A); } 
            else { print_char('-', 0x08); }
        }
        print_string("] ", 0x08);

        int pct = ((step + 1) * 100) / 7;
        if (pct >= 100) {
            print_char('1', 0x0A); print_char('0', 0x0A); print_char('0', 0x0A);
        } else {
            print_char('0' + pct / 10, 0x0A); print_char('0' + pct % 10, 0x0A);
        }
        print_char('%', 0x0A);
        print_newline();

        unsigned int d;
        for (d = 0; d < 50000000; d++) { __asm__("nop"); }
        step++;
    }

    print_newline();
    print_string("  System ready!", 0x0A);
    print_newline();
    print_newline();

    unsigned int pause;
    for (pause = 0; pause < 10000000; pause++) { __asm__("nop"); }

    clear_screen();

    print_string("================================================================================", 0x08);
    print_newline();
    print_string("          Welcome to DeepithOS v0.1 - Built by Deepith                         ", 0x0B);
    print_newline();
    print_string("          x86 32-bit Protected Mode Kernel                                      ", 0x0A);
    print_newline();
    print_string("================================================================================", 0x08);
    print_newline();
    print_string(" Type 'help' to see all commands", 0x0E);
    print_newline();
    print_string("================================================================================", 0x08);
    print_newline();
    print_newline();
    print_string("> ", 0x0E);
    
    while (1) {
        char c = keyboard_read();
        if (c == 0) continue;

        if (c == '\b') {
            backspace();
        } else if (c == '\t' || c == 0x01 || c == 0x02) {
            // Handled inside shell
        } else if (c != '\n') {
            print_char(c, 0x0F);
        }
        shell_handle_key(c);
    }
}
