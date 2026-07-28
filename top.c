// Live system monitor for DeepithOS
extern unsigned char *vga;
extern int cursor;

void print_string(char *str, unsigned char color);
void print_char(char c, unsigned char color);
void print_newline();
void print_int(int num, unsigned char color);
void clear_screen();
char keyboard_read();

unsigned int timer_get_ticks();
unsigned int timer_seconds();
void mem_stats(unsigned int *free_bytes, unsigned int *used_bytes);
int proc_count();

struct process { unsigned int pid; char name[32]; unsigned int state; unsigned int ticks; };
struct process *proc_get(int index);

// Draw char directly to VGA
void top_draw(int x, int y, char c, unsigned char color) {
    int pos = y * 80 + x;
    vga[pos * 2]     = c;
    vga[pos * 2 + 1] = color;
}

// Draw string directly to VGA
void top_draw_str(int x, int y, char *str, unsigned char color) {
    int i = 0;
    while (str[i] && x + i < 80) {
        top_draw(x + i, y, str[i], color);
        i++;
    }
}

// Draw integer directly to VGA
void top_draw_int(int x, int y, unsigned int num, unsigned char color) {
    char digits[12];
    int i = 0;
    if (num == 0) {
        top_draw(x, y, '0', color);
        return;
    }
    while (num > 0) {
        digits[i++] = '0' + (num % 10);
        num /= 10;
    }
    int col = x;
    while (i > 0) {
        top_draw(col++, y, digits[--i], color);
    }
}

// Draw a full line of dashes
void top_draw_line(int y, unsigned char color) {
    int x;
    for (x = 0; x < 80; x++) {
        top_draw(x, y, '-', color);
    }
}

// Draw a full line of equals
void top_draw_header(int y, unsigned char color) {
    int x;
    for (x = 0; x < 80; x++) {
        top_draw(x, y, '=', color);
    }
}

// Clear a line
void top_clear_line(int y) {
    int x;
    for (x = 0; x < 80; x++) {
        top_draw(x, y, ' ', 0x0F);
    }
}

void top_run() {
    clear_screen();

    int running = 1;
    unsigned int last_update = 0;

    // Draw static parts
    top_draw_header(0, 0x08);
    top_draw_str(20, 0, "  DeepithOS System Monitor  ", 0x0B);
    top_draw_header(1, 0x08);

    top_draw_str(2, 3,  "  Uptime     :", 0x07);
    top_draw_str(2, 4,  "  CPU Ticks  :", 0x07);
    top_draw_str(2, 5,  "  Heap Free  :", 0x07);
    top_draw_str(2, 6,  "  Heap Used  :", 0x07);
    top_draw_str(2, 7,  "  Processes  :", 0x07);

    top_draw_line(8, 0x08);

    top_draw_str(2,  9, "PID", 0x0E);
    top_draw_str(8,  9, "STATE  ", 0x0E);
    top_draw_str(18, 9, "TICKS", 0x0E);
    top_draw_str(28, 9, "NAME", 0x0E);

    top_draw_line(10, 0x08);
    top_draw_line(22, 0x08);
    top_draw_str(2, 23, "  Press Q to exit  |  Updates every second", 0x07);
    top_draw_header(24, 0x08);

    while (running) {
        unsigned int now = timer_get_ticks();

        // Update every ~18 ticks (1 second) or on first run
        if (now - last_update >= 18 || last_update == 0) {
            last_update = now;

            // Update uptime
            top_clear_line(3);
            top_draw_str(2, 3, "  Uptime     :", 0x07);
            top_draw_int(18, 3, timer_seconds(), 0x0A);
            top_draw_str(26, 3, "seconds", 0x07);

            // Update ticks
            top_clear_line(4);
            top_draw_str(2, 4, "  CPU Ticks  :", 0x07);
            top_draw_int(18, 4, timer_get_ticks(), 0x0A);

            // Update memory
            unsigned int f = 0, u = 0;
            mem_stats(&f, &u);

            top_clear_line(5);
            top_draw_str(2, 5, "  Heap Free  :", 0x07);
            top_draw_int(18, 5, f, 0x0A);
            top_draw_str(26, 5, "bytes", 0x07);

            top_clear_line(6);
            top_draw_str(2, 6, "  Heap Used  :", 0x07);
            top_draw_int(18, 6, u, 0x0C);
            top_draw_str(26, 6, "bytes", 0x07);

            // Update process count
            top_clear_line(7);
            top_draw_str(2, 7, "  Processes  :", 0x07);
            top_draw_int(18, 7, proc_count(), 0x0A);
            top_draw_str(20, 7, "running", 0x07);

            // Update process list
            int i;
            for (i = 0; i < 8; i++) {
                int row = 11 + i;
                top_clear_line(row);

                struct process *p = proc_get(i);
                if (p->state != 0) {
                    // PID
                    top_draw_int(2, row, p->pid, 0x0A);

                    // State
                    if (p->state == 1)
                        top_draw_str(8, row, "RUNNING", 0x0A);
                    else
                        top_draw_str(8, row, "STOPPED", 0x0C);

                    // Ticks
                    top_draw_int(18, row, p->ticks, 0x07);

                    // Name
                    top_draw_str(28, row, p->name, 0x0B);
                }
            }

            // Update timestamp in header
            top_clear_line(1);
            top_draw_header(1, 0x08);
            top_draw_str(2, 1, "  Tick:", 0x08);
            top_draw_int(9, 1, now, 0x08);
        }

        // Check for Q
        char c = keyboard_read();
        if (c == 'q' || c == 'Q') running = 0;
    }

    clear_screen();
    cursor = 0;
}
