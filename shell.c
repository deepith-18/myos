// Forward declarations from kernel.c
void print_string(char *str, unsigned char color);
void print_newline();
void print_int(int num, unsigned char color);
void print_hex(unsigned int num, unsigned char color);
void clear_screen();

// Timer
unsigned int timer_seconds();

// Input buffer
char input_buffer[256];
int buffer_pos = 0;

// Current text color
unsigned char current_color = 0x0F;

// Compare two strings
int str_compare(char *a, char *b) {
    int i = 0;
    while (a[i] && b[i]) {
        if (a[i] != b[i]) return 0;
        i++;
    }
    return a[i] == b[i];
}

// Check if string starts with prefix
int str_starts_with(char *str, char *prefix) {
    int i = 0;
    while (prefix[i]) {
        if (str[i] != prefix[i]) return 0;
        i++;
    }
    return 1;
}

// Clear input buffer
void buffer_clear() {
    int i = 0;
    while (i < 256) {
        input_buffer[i] = 0;
        i++;
    }
    buffer_pos = 0;
}

// Reboot the machine
void reboot() {
    unsigned char good = 0x02;
    while (good & 0x02) {
        __asm__("inb $0x64, %0" : "=a"(good));
    }
    __asm__("outb %0, $0x64" : : "a"((unsigned char)0xFE));
}

// Execute typed command
void execute_command() {

    if (str_compare(input_buffer, "help")) {
        print_newline();
        print_string("  Commands:", 0x0E);
        print_newline();
        print_string("  help         show this list", 0x0F);
        print_newline();
        print_string("  about        about this OS", 0x0F);
        print_newline();
        print_string("  clear        clear the screen", 0x0F);
        print_newline();
        print_string("  reboot       restart the OS", 0x0F);
        print_newline();
        print_string("  echo <text>  print your text", 0x0F);
        print_newline();
        print_string("  color <n>    change text color", 0x0F);
        print_newline();
        print_string("  meminfo      show memory layout", 0x0F);
        print_newline();
        print_string("  version      show OS version", 0x0F);
        print_newline();
        print_string("  uptime       show seconds running", 0x0F);
        print_newline();

    } else if (str_compare(input_buffer, "clear")) {
        clear_screen();
        print_string("DeepithOS v0.1", 0x0B);
        print_newline();
        print_newline();

    } else if (str_compare(input_buffer, "about")) {
        print_newline();
        print_string("  DeepithOS - Built by Deepith", 0x0B);
        print_newline();
        print_string("  Version 0.1 - Learning OS Dev", 0x0B);
        print_newline();
        print_string("  Day 13 - Timer + Uptime", 0x0B);
        print_newline();

    } else if (str_compare(input_buffer, "version")) {
        print_newline();
        print_string("  DeepithOS ", 0x0F);
        print_string("v0.1", 0x0A);
        print_newline();
        print_string("  Build: Day 13", 0x07);
        print_newline();
        print_string("  Arch:  x86 32-bit Protected Mode", 0x07);
        print_newline();
        print_string("  Shell: 9 commands", 0x07);
        print_newline();

    } else if (str_compare(input_buffer, "meminfo")) {
        print_newline();
        print_string("  Memory Layout:", 0x0E);
        print_newline();
        print_string("  Bootloader : ", 0x0F);
        print_hex(0x7C00, 0x0A);
        print_newline();
        print_string("  Kernel     : ", 0x0F);
        print_hex(0x1000, 0x0A);
        print_newline();
        print_string("  Stack      : ", 0x0F);
        print_hex(0x90000, 0x0A);
        print_newline();
        print_string("  Video RAM  : ", 0x0F);
        print_hex(0xB8000, 0x0A);
        print_newline();

    } else if (str_compare(input_buffer, "uptime")) {
        print_newline();
        print_string("  Uptime: ", 0x0F);
        print_int(timer_seconds(), 0x0A);
        print_string(" seconds", 0x0F);
        print_newline();

    } else if (str_compare(input_buffer, "reboot")) {
        print_newline();
        print_string("  Rebooting...", 0x0C);
        print_newline();
        reboot();

    } else if (str_starts_with(input_buffer, "echo ")) {
        print_newline();
        print_string("  ", 0x0F);
        print_string(input_buffer + 5, current_color);
        print_newline();

    } else if (str_starts_with(input_buffer, "color ")) {
        char code = input_buffer[6];
        if (code == '1') { current_color = 0x0A; print_newline(); print_string("  Color: Green", 0x0A); }
        else if (code == '2') { current_color = 0x0B; print_newline(); print_string("  Color: Cyan", 0x0B); }
        else if (code == '3') { current_color = 0x0C; print_newline(); print_string("  Color: Red", 0x0C); }
        else if (code == '4') { current_color = 0x0E; print_newline(); print_string("  Color: Yellow", 0x0E); }
        else if (code == '5') { current_color = 0x0F; print_newline(); print_string("  Color: White", 0x0F); }
        else { print_newline(); print_string("  Usage: color 1-5", 0x07); }
        print_newline();

    } else if (input_buffer[0] == 0) {
        // empty — do nothing

    } else {
        print_newline();
        print_string("  Unknown command. Type 'help'", 0x04);
        print_newline();
    }
}

// Handle one keypress
void shell_handle_key(char c) {
    if (c == '\n') {
        execute_command();
        buffer_clear();
        print_string("> ", 0x0E);
    } else if (c == '\b') {
        if (buffer_pos > 0) {
            buffer_pos--;
            input_buffer[buffer_pos] = 0;
        }
    } else {
        if (buffer_pos < 255) {
            input_buffer[buffer_pos] = c;
            buffer_pos++;
        }
    }
}
