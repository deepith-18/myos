void editor_run(char *fname);
void snake_game();
char *cpu_get_vendor();
unsigned int cpu_get_family();
unsigned int cpu_get_model();
int proc_count();
unsigned int timer_get_ticks();

int proc_spawn(char *name);
int proc_kill(unsigned int pid);
struct process { unsigned int pid; char name[32]; unsigned int state; unsigned int ticks; };
struct process *proc_get(int index);

int fs_create(char *name);
int fs_write(char *name, char *data);
char *fs_read(char *name);
int fs_delete(char *name);
int fs_append(char *name, char *data);
int fs_rename(char *old_name, char *new_name);
struct file { char name[16]; char data[256]; unsigned int size; unsigned int used; };
struct file *fs_get(int index);

void print_string(char *str, unsigned char color);
void print_char(char c, unsigned char color);
void print_newline();
void print_int(int num, unsigned char color);
void print_hex(unsigned int num, unsigned char color);
void clear_screen();
void backspace();
void mem_stats(unsigned int *free_bytes, unsigned int *used_bytes);
unsigned int timer_seconds();

char input_buffer[256];
int buffer_pos = 0;
unsigned char current_color = 0x0F;

// Day 28 Global Theme Variables
unsigned char theme_text = 0x0F;
unsigned char theme_accent = 0x0B;
unsigned char theme_success = 0x0A;
unsigned char theme_error = 0x0C;
unsigned char theme_prompt = 0x0E;

char history[8][256];
int history_count = 0;
int history_index = -1;

int str_compare(char *a, char *b) {
    int i = 0;
    while (a[i] && b[i]) {
        if (a[i] != b[i]) return 0;
        i++;
    }
    return a[i] == b[i];
}

int str_starts_with(char *str, char *prefix) {
    int i = 0;
    while (prefix[i]) {
        if (str[i] != prefix[i]) return 0;
        i++;
    }
    return 1;
}

void buffer_clear() {
    int i = 0;
    while (i < 256) { input_buffer[i] = 0; i++; }
    buffer_pos = 0;
}

void history_add() {
    if (input_buffer[0] == 0) return;
    int i = 0;
    while (i < 256) {
        history[history_count % 8][i] = input_buffer[i];
        i++;
    }
    history_count++;
    history_index = -1;
}

void line_replace(char *text) {
    while (buffer_pos > 0) {
        backspace();
        buffer_pos--;
    }
    buffer_clear();
    int i = 0;
    while (text[i]) {
        print_char(text[i], theme_text);
        input_buffer[i] = text[i];
        i++;
    }
    buffer_pos = i;
}

void reboot() {
    unsigned char good = 0x02;
    while (good & 0x02) {
        __asm__("inb $0x64, %0" : "=a"(good));
    }
    __asm__("outb %0, $0x64" : : "a"((unsigned char)0xFE));
}

void execute_command() {
    if (str_compare(input_buffer, "help")) {
        print_newline();
        print_string("  Commands:", theme_prompt); print_newline();
        print_string("  help            show this list", theme_text); print_newline();
        print_string("  about           about this OS", theme_text); print_newline();
        print_string("  clear           clear screen", theme_text); print_newline();
        print_string("  reboot          restart OS", theme_text); print_newline();
        print_string("  echo <text>     print text", theme_text); print_newline();
        print_string("  color <n>       change color 1-5", theme_text); print_newline();
        print_string("  theme <name>    dark/matrix/ocean/fire", theme_text); print_newline();
        print_string("  meminfo         memory layout", theme_text); print_newline();
        print_string("  version         OS version", theme_text); print_newline();
        print_string("  uptime          seconds running", theme_text); print_newline();
        print_string("  ls              list files", theme_text); print_newline();
        print_string("  create <n>      create file", theme_text); print_newline();
        print_string("  write <n> <t>   write file", theme_text); print_newline();
        print_string("  read <n>        read file", theme_text); print_newline();
        print_string("  rm <n>          delete file", theme_text); print_newline();
        print_string("  append <n> <t>  append file", theme_text); print_newline();
        print_string("  rename <o> <n>  rename file", theme_text); print_newline();
        print_string("  ps              list processes", theme_text); print_newline();
        print_string("  spawn <name>    create process", theme_text); print_newline();
        print_string("  kill <pid>      kill process", theme_text); print_newline();
        print_string("  sysinfo         full system info", theme_text); print_newline();
        print_string("  calc <n> op <n> calculate math", theme_text);  print_newline();
        print_string("  snake           play snake game", theme_text);  print_newline();
        print_string("  edit <name>     open text editor", theme_text);  print_newline();

    } else if (str_compare(input_buffer, "clear")) {
        clear_screen();
        print_string("DeepithOS v0.1", theme_accent);
        print_newline();
        print_newline();

    } else if (str_compare(input_buffer, "about")) {
        print_newline();
        print_string("  DeepithOS - Built by Deepith", theme_accent); print_newline();
        print_string("  Version 0.1 - Learning OS Dev", theme_accent); print_newline();
        print_string("  Day 28 - Shell Themes Integration", theme_accent); print_newline();

    } else if (str_compare(input_buffer, "version")) {
        print_newline();
        print_string("  DeepithOS v0.1", theme_success); print_newline();
        print_string("  Build: Day 28", theme_text); print_newline();
        print_string("  Arch:  x86 32-bit Protected Mode", theme_text); print_newline();
        print_string("  Shell: 21 commands", theme_text); print_newline();

    } else if (str_compare(input_buffer, "meminfo")) {
        print_newline();
        print_string("  Memory Layout:", theme_prompt); print_newline();
        print_string("  Bootloader : ", theme_text); print_hex(0x7C00, theme_success); print_newline();
        print_string("  Kernel     : ", theme_text); print_hex(0x1000, theme_success); print_newline();
        print_string("  Stack      : ", theme_text); print_hex(0x90000, theme_success); print_newline();
        print_string("  Video RAM  : ", theme_text); print_hex(0xB8000, theme_success); print_newline();
        print_string("  Heap Start : ", theme_text); print_hex(0x200000, theme_success); print_newline();
        unsigned int f = 0, u = 0;
        mem_stats(&f, &u);
        print_string("  Heap Free  : ", theme_text); print_int(f, theme_success);
        print_string(" bytes", theme_text); print_newline();
        print_string("  Heap Used  : ", theme_text); print_int(u, theme_error);
        print_string(" bytes", theme_text); print_newline();

    } else if (str_compare(input_buffer, "uptime")) {
        print_newline();
        print_string("  Uptime: ", theme_text);
        print_int(timer_seconds(), theme_success);
        print_string(" seconds", theme_text); print_newline();

    } else if (str_compare(input_buffer, "reboot")) {
        print_newline();
        print_string("  Rebooting...", theme_error); print_newline();
        reboot();

    } else if (str_starts_with(input_buffer, "echo ")) {
        print_newline();
        print_string("  ", theme_text);
        print_string(input_buffer + 5, current_color); print_newline();

    } else if (str_starts_with(input_buffer, "color ")) {
        char code = input_buffer[6];
        if (code == '1') { current_color = 0x0A; print_newline(); print_string("  Color: Green", 0x0A); }
        // For standard themes, keeping these configurations relative to choices:
        else if (code == '2') { current_color = 0x0B; print_newline(); print_string("  Color: Cyan", 0x0B); }
        else if (code == '3') { current_color = 0x0C; print_newline(); print_string("  Color: Red", 0x0C); }
        else if (code == '4') { current_color = 0x0E; print_newline(); print_string("  Color: Yellow", 0x0E); }
        else if (code == '5') { current_color = 0x0F; print_newline(); print_string("  Color: White", 0x0F); }
        else { print_newline(); print_string("  Usage: color 1-5", theme_text); }
        print_newline();

    } else if (str_compare(input_buffer, "ls")) {
        print_newline();
        print_string("  Files:", theme_prompt); print_newline();
        int i = 0; int found = 0;
        while (i < 16) {
            struct file *f = fs_get(i);
            if (f->used) {
                print_string("  - ", theme_text);
                print_string(f->name, theme_success);
                print_string(" (", theme_text);
                print_int(f->size, theme_text);
                print_string(" bytes)", theme_text); print_newline();
                found++;
            }
            i++;
        }
        if (!found) { print_string("  No files yet", theme_text); print_newline(); }

    } else if (str_starts_with(input_buffer, "create ")) {
        char *name = input_buffer + 7;
        int result = fs_create(name);
        print_newline();
        if (result >= 0) { print_string("  File created: ", theme_success); print_string(name, theme_success); }
        else if (result == -1) { print_string("  Error: already exists", theme_error); }
        else { print_string("  Error: filesystem full", theme_error); }
        print_newline();

    } else if (str_starts_with(input_buffer, "write ")) {
        char *rest = input_buffer + 6;
        int i = 0;
        while (rest[i] && rest[i] != ' ') i++;
        rest[i] = 0;
        char *name = rest; char *data = rest + i + 1;
        int result = fs_write(name, data);
        print_newline();
        if (result == 0) { print_string("  Written to: ", theme_success); print_string(name, theme_success); }
        else { print_string("  Error: file not found", theme_error); }
        print_newline();

    } else if (str_starts_with(input_buffer, "read ")) {
        char *name = input_buffer + 5;
        char *data = fs_read(name);
        print_newline();
        if (data) { print_string("  ", theme_text); print_string(data, theme_accent); }
        else { print_string("  Error: file not found", theme_error); }
        print_newline();

    } else if (str_starts_with(input_buffer, "rm ")) {
        char *name = input_buffer + 3;
        int result = fs_delete(name);
        print_newline();
        if (result == 0) { print_string("  Deleted: ", theme_success); print_string(name, theme_success); }
        else { print_string("  Error: file not found", theme_error); }
        print_newline();

    } else if (str_starts_with(input_buffer, "append ")) {
        char *rest = input_buffer + 7;
        int i = 0;
        while (rest[i] && rest[i] != ' ') i++;
        rest[i] = 0;
        char *name = rest; char *data = rest + i + 1;
        int result = fs_append(name, data);
        print_newline();
        if (result == 0) { print_string("  Appended to: ", theme_success); print_string(name, theme_success); }
        else { print_string("  Error: file not found", theme_error); }
        print_newline();

    } else if (str_starts_with(input_buffer, "rename ")) {
        char *rest = input_buffer + 7;
        int i = 0;
        while (rest[i] && rest[i] != ' ') i++;
        rest[i] = 0;
        char *old_name = rest; char *new_name = rest + i + 1;
        int result = fs_rename(old_name, new_name);
        print_newline();
        if (result == 0) { print_string("  Renamed to: ", theme_success); print_string(new_name, theme_success); }
        else if (result == -2) { print_string("  Error: name taken", theme_error); }
        else { print_string("  Error: file not found", theme_error); }
        print_newline();

    } else if (str_compare(input_buffer, "ps")) {
        print_newline();
        print_string("  PID  STATE    TICKS  NAME", theme_prompt); print_newline();
        int i = 0;
        while (i < 8) {
            struct process *p = proc_get(i);
            if (p->state != 0) {
                print_string("  ", theme_text);
                print_int(p->pid, theme_success);
                print_string("    ", theme_text);
                if (p->state == 1) print_string("RUNNING  ", theme_success);
                if (p->state == 2) print_string("STOPPED  ", theme_error);
                print_int(p->ticks, theme_text);
                print_string("    ", theme_text);
                print_string(p->name, theme_accent); print_newline();
            }
            i++;
        }

    } else if (str_starts_with(input_buffer, "spawn ")) {
        char *name = input_buffer + 6;
        int pid = proc_spawn(name);
        print_newline();
        if (pid > 0) {
            print_string("  Spawned: ", theme_success);
            print_string(name, theme_success);
            print_string(" [PID ", theme_text);
            print_int(pid, theme_text);
            print_string("]", theme_text);
        } else {
            print_string("  Error: process table full", theme_error);
        }
        print_newline();

    } else if (str_starts_with(input_buffer, "kill ")) {
        char *pid_str = input_buffer + 5;
        unsigned int pid = 0;
        int i = 0;
        while (pid_str[i] >= '0' && pid_str[i] <= '9') {
            pid = pid * 10 + (pid_str[i] - '0');
            i++;
        }
        int result = proc_kill(pid);
        print_newline();
        if (result == 0) { print_string("  Killed PID: ", theme_success); print_int(pid, theme_success); }
        else if (result == -2) { print_string("  Error: cannot kill kernel", theme_error); }
        else { print_string("  Error: process not found", theme_error); }
        print_newline();

    } else if (str_compare(input_buffer, "sysinfo")) {
        print_newline();
        print_string("  DeepithOS System Information", theme_prompt); print_newline();
        print_string("  =============================", theme_text); print_newline();
        print_string("  OS      : DeepithOS v0.1", theme_text); print_newline();
        print_string("  Arch    : x86 32-bit Protected Mode", theme_text); print_newline();
        print_string("  CPU     : ", theme_text);
        print_string(cpu_get_vendor(), theme_success); print_newline();
        print_string("  Family  : ", theme_text);
        print_int(cpu_get_family(), theme_success);
        print_string("  Model: ", theme_text);
        print_int(cpu_get_model(), theme_success); print_newline();
        print_string("  Uptime  : ", theme_text);
        print_int(timer_seconds(), theme_success);
        print_string(" seconds", theme_text); print_newline();
        print_string("  Procs   : ", theme_text);
        print_int(proc_count(), theme_success);
        print_string(" running", theme_text); print_newline();
        unsigned int f = 0, u = 0;
        mem_stats(&f, &u);
        print_string("  Heap    : ", theme_text);
        print_int(f, theme_success);
        print_string(" free / ", theme_text);
        print_int(u, theme_error);
        print_string(" used", theme_text); print_newline();
        print_string("  =============================", theme_text); print_newline();

    } else if (str_starts_with(input_buffer, "calc ")) {
        char *expr = input_buffer + 5;
        int a = 0;
        int neg_a = 0;
        int i = 0;

        if (expr[i] == '-') { neg_a = 1; i++; }
        while (expr[i] >= '0' && expr[i] <= '9') {
            a = a * 10 + (expr[i] - '0');
            i++;
        }
        if (neg_a) a = -a;

        while (expr[i] == ' ') i++;
        char op = expr[i];
        i++;
        while (expr[i] == ' ') i++;

        int b = 0;
        int neg_b = 0;
        if (expr[i] == '-') { neg_b = 1; i++; }
        while (expr[i] >= '0' && expr[i] <= '9') {
            b = b * 10 + (expr[i] - '0');
            i++;
        }
        if (neg_b) b = -b;

        print_newline();
        if (op == '+') {
            print_string("  Result: ", theme_success);
            print_int(a + b, theme_success);
        } else if (op == '-') {
            print_string("  Result: ", theme_success);
            print_int(a - b, theme_success);
        } else if (op == '*') {
            print_string("  Result: ", theme_success);
            print_int(a * b, theme_success);
        } else if (op == '/') {
            if (b == 0) {
                print_string("  Error: divide by zero!", theme_error);
            } else {
                print_string("  Result: ", theme_success);
                print_int(a / b, theme_success);
                int rem = a % b;
                if (rem != 0) {
                    print_string("  remainder ", theme_text);
                    print_int(rem, theme_text);
                }
            }
        } else if (op == '%') {
            if (b == 0) {
                print_string("  Error: divide by zero!", theme_error);
            } else {
                print_string("  Result: ", theme_success);
                print_int(a % b, theme_success);
            }
        } else {
            print_string("  Error: unknown operator", theme_error);
            print_newline();
            print_string("  Use: calc <n> +/-/*///%  <n>", theme_text);
        }
        print_newline();

    } else if (str_compare(input_buffer, "snake")) {
        snake_game();
        clear_screen();
        print_string("================================================================================", theme_text);
        print_newline();
        print_string("         Welcome to DeepithOS v0.1 - Built by Deepith                          ", theme_accent);
        print_newline();
        print_string("         x86 32-bit Protected Mode Kernel                                       ", theme_success);
        print_newline();
        print_string("================================================================================", theme_text);
        print_newline();
        print_string(" Type 'help' to see all commands", theme_prompt);
        print_newline();
        print_string("================================================================================", theme_text);
        print_newline();
        print_string("> ", theme_prompt);

    } else if (str_starts_with(input_buffer, "edit ")) {
        char *fname = input_buffer + 5;
        editor_run(fname);
        clear_screen();
        print_string("================================================================================", theme_text);
        print_newline();
        print_string("         Welcome to DeepithOS v0.1 - Built by Deepith                          ", theme_accent);
        print_newline();
        print_string("         x86 32-bit Protected Mode Kernel                                       ", theme_success);
        print_newline();
        print_string("================================================================================", theme_text);
        print_newline();
        print_string(" Type 'help' to see all commands", theme_prompt);
        print_newline();
        print_string("================================================================================", theme_text);
        print_newline();
        print_newline();
        print_string("> ", theme_prompt);

    } else if (str_starts_with(input_buffer, "theme")) {
        char *name = input_buffer + 5;
        while (*name == ' ') name++;

        print_newline();
        if (str_compare(name, "dark") || name[0] == 0) {
            theme_text = 0x0F; theme_accent = 0x0B;
            theme_success = 0x0A; theme_error = 0x0C; theme_prompt = 0x0E;
            current_color = 0x0F;
            print_string("  Theme set: Dark", 0x0F);

        } else if (str_compare(name, "matrix")) {
            theme_text = 0x0A; theme_accent = 0x0A;
            theme_success = 0x0A; theme_error = 0x0C; theme_prompt = 0x0A;
            current_color = 0x0A;
            print_string("  Theme set: Matrix", 0x0A);

        } else if (str_compare(name, "ocean")) {
            theme_text = 0x0B; theme_accent = 0x09;
            theme_success = 0x0B; theme_error = 0x0C; theme_prompt = 0x09;
            current_color = 0x0B;
            print_string("  Theme set: Ocean", 0x0B);

        } else if (str_compare(name, "fire")) {
            theme_text = 0x0C; theme_accent = 0x0E;
            theme_success = 0x0E; theme_error = 0x04; theme_prompt = 0x0C;
            current_color = 0x0C;
            print_string("  Theme set: Fire", 0x0C);

        } else {
            print_string("  Themes: dark, matrix, ocean, fire", theme_text);
        }
        print_newline();

    } else if (input_buffer[0] == 0) {
        // empty

    } else {
        print_newline();
        print_string("  Unknown command. Type 'help'", theme_error);
        print_newline();
    }
}

void shell_handle_key(char c) {
    if (c == 0) return; 

    if (c == '\n') {
        history_add();
        execute_command();
        buffer_clear();
        print_string("> ", theme_prompt);

    } else if (c == '\b') {
        if (buffer_pos > 0) {
            buffer_pos--;
            input_buffer[buffer_pos] = 0;
        }

    } else if (c == '\t') {
        char *commands[] = {
            "help", "about", "clear", "reboot", "echo",
            "color", "meminfo", "version", "uptime", "ls",
            "create", "write", "read", "rm", "append",
            "rename", "ps", "spawn", "kill", "sysinfo", "calc", "snake", "edit", "theme", 0
        };
        int i = 0;
        while (commands[i]) {
            int match = 1;
            int j = 0;
            while (j < buffer_pos) {
                if (input_buffer[j] != commands[i][j]) {
                    match = 0;
                    break;
                }
                j++;
            }
            if (match && buffer_pos > 0) {
                line_replace(commands[i]);
                break;
            }
            i++;
        }

    } else if (c == 0x01) {
        if (history_count == 0) return;
        if (history_index == -1) {
            history_index = (history_count - 1) % 8;
        } else {
            int prev = (history_index - 1 + 8) % 8;
            if (prev != history_count % 8) history_index = prev;
        }
        line_replace(history[history_index]);

    } else if (c == 0x02) {
        if (history_index == -1) return;
        history_index = (history_index + 1) % 8;
        if (history_index == history_count % 8) {
            while (buffer_pos > 0) { backspace(); buffer_pos--; }
            buffer_clear();
            history_index = -1;
        } else {
            line_replace(history[history_index]);
        }

    } else {
        if (buffer_pos < 255) {
            input_buffer[buffer_pos] = c;
            buffer_pos++;
        }
    }
}
