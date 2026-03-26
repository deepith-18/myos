// Forward declarations from kernel.c
void print_string(char *str, unsigned char color);
void print_newline();
void clear_screen();

// Buffer to store what user types
char input_buffer[256];
int buffer_pos = 0;

// Compare two strings
int str_compare(char *a, char *b) {
    int i = 0;
    while (a[i] && b[i]) {
        if (a[i] != b[i]) return 0;
        i++;
    }
    return a[i] == b[i];
}

// Clear the input buffer
void buffer_clear() {
    int i = 0;
    while (i < 256) {
        input_buffer[i] = 0;
        i++;
    }
    buffer_pos = 0;
}

// Handle a complete command
void execute_command() {
    if (str_compare(input_buffer, "help")) {
        print_newline();
        print_string("  Commands: help, clear, about", 0x0E);
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

    } else if (input_buffer[0] == 0) {
        // empty command, do nothing

    } else {
        print_newline();
        print_string("  Unknown command. Type 'help'", 0x04);
        print_newline();
    }
}

// Handle one keypress from kernel
void shell_handle_key(char c) {
    if (c == '\n') {
        // Enter pressed — execute command
        execute_command();
        buffer_clear();
        print_string("> ", 0x0E);

    } else if (c == '\b') {
        // Backspace — remove last char from buffer
        if (buffer_pos > 0) {
            buffer_pos--;
            input_buffer[buffer_pos] = 0;
        }

    } else {
        // Normal character — add to buffer
        if (buffer_pos < 255) {
            input_buffer[buffer_pos] = c;
            buffer_pos++;
        }
    }
}
