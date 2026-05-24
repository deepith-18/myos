// System info
char *cpu_get_vendor();
unsigned int cpu_get_family();
unsigned int cpu_get_model();
int proc_count();
unsigned int timer_get_ticks();

// Process manager
int proc_spawn(char *name);
int proc_kill(unsigned int pid);
struct process { unsigned int pid; char name[32]; unsigned int state; unsigned int ticks; };
struct process *proc_get(int index);

// Filesystem declarations
int fs_create(char *name);
int fs_write(char *name, char *data);
char *fs_read(char *name);
int fs_delete(char *name);
int fs_append(char *name, char *data);
int fs_rename(char *old_name, char *new_name);
struct file { char name[16]; char data[256]; unsigned int size; unsigned int used; };
struct file *fs_get(int index);

// Kernel declarations
void print_string(char *str, unsigned char color);
void print_newline();
void print_int(int num, unsigned char color);
void print_hex(unsigned int num, unsigned char color);
void clear_screen();
void mem_stats(unsigned int *free_bytes, unsigned int *used_bytes);
unsigned int timer_seconds();

// Input buffer
char input_buffer[256];
int buffer_pos = 0;
unsigned char current_color = 0x0F;

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
        print_string("  Commands:", 0x0E);
        print_newline();
        print_string("  help           show this list", 0x0F); print_newline();
        print_string("  about          about this OS", 0x0F); print_newline();
        print_string("  clear          clear screen", 0x0F); print_newline();
        print_string("  reboot         restart OS", 0x0F); print_newline();
        print_string("  echo <text>    print text", 0x0F); print_newline();
        print_string("  color <n>      change color", 0x0F); print_newline();
        print_string("  meminfo        memory layout", 0x0F); print_newline();
        print_string("  version        OS version", 0x0F); print_newline();
        print_string("  uptime         seconds running", 0x0F); print_newline();
        print_string("  ls             list files", 0x0F); print_newline();
        print_string("  create <n>     create file", 0x0F); print_newline();
        print_string("  write <n> <t>  write file", 0x0F); print_newline();
        print_string("  read <n>       read file", 0x0F); print_newline();
        print_string("  rm <n>         delete file", 0x0F); print_newline();
        print_string("  append <n> <t> append file", 0x0F); print_newline();
        print_string("  rename <o> <n> rename file", 0x0F); print_newline();
        print_string("  ps             list processes", 0x0F); print_newline();
        print_string("  spawn <name>   create process", 0x0F); print_newline();
        print_string("  kill <pid>     kill process", 0x0F); print_newline();
        print_string("  sysinfo        full system info", 0x0F);
        print_newline();

    } else if (str_compare(input_buffer, "clear")) {
        clear_screen();
        print_string("DeepithOS v0.1", 0x0B);
        print_newline();
        print_newline();

    } else if (str_compare(input_buffer, "about")) {
        print_newline();
        print_string("  DeepithOS - Built by Deepith", 0x0B); print_newline();
        print_string("  Version 0.1 - Learning OS Dev", 0x0B); print_newline();
        print_string("  Day 17 - Process Manager", 0x0B); print_newline();

    } else if (str_compare(input_buffer, "version")) {
        print_newline();
        print_string("  DeepithOS v0.1", 0x0A); print_newline();
        print_string("  Build: Day 17", 0x07); print_newline();
        print_string("  Arch:  x86 32-bit Protected Mode", 0x07); print_newline();
        print_string("  Shell: 19 commands", 0x07); print_newline();

    } else if (str_compare(input_buffer, "meminfo")) {
        print_newline();
        print_string("  Memory Layout:", 0x0E); print_newline();
        print_string("  Bootloader : ", 0x0F); print_hex(0x7C00, 0x0A); print_newline();
        print_string("  Kernel     : ", 0x0F); print_hex(0x1000, 0x0A); print_newline();
        print_string("  Stack      : ", 0x0F); print_hex(0x90000, 0x0A); print_newline();
        print_string("  Video RAM  : ", 0x0F); print_hex(0xB8000, 0x0A); print_newline();
        print_string("  Heap Start : ", 0x0F); print_hex(0x200000, 0x0A); print_newline();
        unsigned int f = 0, u = 0;
        mem_stats(&f, &u);
        print_string("  Heap Free  : ", 0x0F); print_int(f, 0x0A);
        print_string(" bytes", 0x0F); print_newline();
        print_string("  Heap Used  : ", 0x0F); print_int(u, 0x0C);
        print_string(" bytes", 0x0F); print_newline();

    } else if (str_compare(input_buffer, "uptime")) {
        print_newline();
        print_string("  Uptime: ", 0x0F);
        print_int(timer_seconds(), 0x0A);
        print_string(" seconds", 0x0F); print_newline();

    } else if (str_compare(input_buffer, "reboot")) {
        print_newline();
        print_string("  Rebooting...", 0x0C); print_newline();
        reboot();

    } else if (str_starts_with(input_buffer, "echo ")) {
        print_newline();
        print_string("  ", 0x0F);
        print_string(input_buffer + 5, current_color); print_newline();

    } else if (str_starts_with(input_buffer, "color ")) {
        char code = input_buffer[6];
        if (code == '1') { current_color = 0x0A; print_newline(); print_string("  Color: Green", 0x0A); }
        else if (code == '2') { current_color = 0x0B; print_newline(); print_string("  Color: Cyan", 0x0B); }
        else if (code == '3') { current_color = 0x0C; print_newline(); print_string("  Color: Red", 0x0C); }
        else if (code == '4') { current_color = 0x0E; print_newline(); print_string("  Color: Yellow", 0x0E); }
        else if (code == '5') { current_color = 0x0F; print_newline(); print_string("  Color: White", 0x0F); }
        else { print_newline(); print_string("  Usage: color 1-5", 0x07); }
        print_newline();

    } else if (str_compare(input_buffer, "ls")) {
        print_newline();
        print_string("  Files:", 0x0E); print_newline();
        int i = 0; int found = 0;
        while (i < 16) {
            struct file *f = fs_get(i);
            if (f->used) {
                print_string("  - ", 0x0F);
                print_string(f->name, 0x0A);
                print_string(" (", 0x07);
                print_int(f->size, 0x07);
                print_string(" bytes)", 0x07); print_newline();
                found++;
            }
            i++;
        }
        if (!found) { print_string("  No files yet", 0x07); print_newline(); }

    } else if (str_starts_with(input_buffer, "create ")) {
        char *name = input_buffer + 7;
        int result = fs_create(name);
        print_newline();
        if (result >= 0) { print_string("  File created: ", 0x0A); print_string(name, 0x0A); }
        else if (result == -1) { print_string("  Error: already exists", 0x0C); }
        else { print_string("  Error: filesystem full", 0x0C); }
        print_newline();

    } else if (str_starts_with(input_buffer, "write ")) {
        char *rest = input_buffer + 6;
        int i = 0;
        while (rest[i] && rest[i] != ' ') i++;
        rest[i] = 0;
        char *name = rest; char *data = rest + i + 1;
        int result = fs_write(name, data);
        print_newline();
        if (result == 0) { print_string("  Written to: ", 0x0A); print_string(name, 0x0A); }
        else { print_string("  Error: file not found", 0x0C); }
        print_newline();

    } else if (str_starts_with(input_buffer, "read ")) {
        char *name = input_buffer + 5;
        char *data = fs_read(name);
        print_newline();
        if (data) { print_string("  ", 0x0F); print_string(data, 0x0B); }
        else { print_string("  Error: file not found", 0x0C); }
        print_newline();

    } else if (str_starts_with(input_buffer, "rm ")) {
        char *name = input_buffer + 3;
        int result = fs_delete(name);
        print_newline();
        if (result == 0) { print_string("  Deleted: ", 0x0A); print_string(name, 0x0A); }
        else { print_string("  Error: file not found", 0x0C); }
        print_newline();

    } else if (str_starts_with(input_buffer, "append ")) {
        char *rest = input_buffer + 7;
        int i = 0;
        while (rest[i] && rest[i] != ' ') i++;
        rest[i] = 0;
        char *name = rest; char *data = rest + i + 1;
        int result = fs_append(name, data);
        print_newline();
        if (result == 0) { print_string("  Appended to: ", 0x0A); print_string(name, 0x0A); }
        else { print_string("  Error: file not found", 0x0C); }
        print_newline();

    } else if (str_starts_with(input_buffer, "rename ")) {
        char *rest = input_buffer + 7;
        int i = 0;
        while (rest[i] && rest[i] != ' ') i++;
        rest[i] = 0;
        char *old_name = rest; char *new_name = rest + i + 1;
        int result = fs_rename(old_name, new_name);
        print_newline();
        if (result == 0) { print_string("  Renamed to: ", 0x0A); print_string(new_name, 0x0A); }
        else if (result == -2) { print_string("  Error: name taken", 0x0C); }
        else { print_string("  Error: file not found", 0x0C); }
        print_newline();

    } else if (str_compare(input_buffer, "ps")) {
        print_newline();
        print_string("  PID  STATE    TICKS  NAME", 0x0E); print_newline();
        int i = 0;
        while (i < 8) {
            struct process *p = proc_get(i);
            if (p->state != 0) {
                print_string("  ", 0x0F);
                print_int(p->pid, 0x0A);
                print_string("    ", 0x0F);
                if (p->state == 1) print_string("RUNNING  ", 0x0A);
                if (p->state == 2) print_string("STOPPED  ", 0x0C);
                print_int(p->ticks, 0x07);
                print_string("    ", 0x0F);
                print_string(p->name, 0x0B); print_newline();
            }
            i++;
        }

    } else if (str_starts_with(input_buffer, "spawn ")) {
        char *name = input_buffer + 6;
        int pid = proc_spawn(name);
        print_newline();
        if (pid > 0) {
            print_string("  Spawned: ", 0x0A);
            print_string(name, 0x0A);
            print_string(" [PID ", 0x07);
            print_int(pid, 0x07);
            print_string("]", 0x07);
        } else {
            print_string("  Error: process table full", 0x0C);
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
        if (result == 0) { print_string("  Killed PID: ", 0x0A); print_int(pid, 0x0A); }
        else if (result == -2) { print_string("  Error: cannot kill kernel", 0x0C); }
        else { print_string("  Error: process not found", 0x0C); }
        print_newline();

    } else if (str_compare(input_buffer, "sysinfo")) {
        print_newline();
        print_string("  DeepithOS System Information", 0x0E);
        print_newline();
        print_string("  ─────────────────────────────", 0x07);
        print_newline();
        print_string("  OS      : DeepithOS v0.1", 0x0F);
        print_newline();
        print_string("  Arch    : x86 32-bit Protected Mode", 0x0F);
        print_newline();
        print_string("  CPU     : ", 0x0F);
        print_string(cpu_get_vendor(), 0x0A);
        print_newline();
        print_string("  Family  : ", 0x0F);
        print_int(cpu_get_family(), 0x0A);
        print_string("  Model: ", 0x0F);
        print_int(cpu_get_model(), 0x0A);
        print_newline();
        print_string("  Uptime  : ", 0x0F);
        print_int(timer_seconds(), 0x0A);
        print_string(" seconds", 0x0F);
        print_newline();
        print_string("  Procs   : ", 0x0F);
        print_int(proc_count(), 0x0A);
        print_string(" running", 0x0F);
        print_newline();
        unsigned int f = 0, u = 0;
        mem_stats(&f, &u);
        print_string("  Heap    : ", 0x0F);
        print_int(f, 0x0A);
        print_string(" free / ", 0x0F);
        print_int(u, 0x0C);
        print_string(" used", 0x0F);
        print_newline();
        print_string("  ─────────────────────────────", 0x07);
        print_newline();
    
    }else if (input_buffer[0] == 0) {
        // empty

    } else {
        print_newline();
        print_string("  Unknown command. Type 'help'", 0x04);
        print_newline();
    }
}

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
