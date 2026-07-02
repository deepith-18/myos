// Clock display for OS
extern unsigned char *vga;

// Forward Declarations & System Prototypes
void timer_sec_part();
void timer_min_part();
void timer_hour_part();
unsigned int timer_get_ticks();
unsigned int timer_seconds();
unsigned int timer_minutes();
unsigned int timer_hours();
char keyboard_read();

void clock_draw_char(int x, int y, char c, unsigned char color);
void clock_draw_digit(int x, int y, int n, unsigned char color);
void clock_draw_time(int x, int y, unsigned char color);
void clock_run();
void clock_update_corner();

// Draw a digit at x,y in VGA
void clock_draw_char(int x, int y, char c, unsigned char color) {
    int pos = y * 80 + x;
    vga[pos * 2]     = c;
    vga[pos * 2 + 1] = color;
}

void clock_draw_digit(int x, int y, int n, unsigned char color) {
    clock_draw_char(x, y, '0' + n, color);
}

// Draw time HH:MM:SS at position x,y
void clock_draw_time(int x, int y, unsigned char color) {
    unsigned int s = timer_seconds();
    unsigned int m = timer_minutes();
    unsigned int h = timer_hours();

    int hh = h % 24;
    int mm = m % 60;
    int ss = s % 60;

    clock_draw_digit(x,     y, hh / 10, color);
    clock_draw_digit(x + 1, y, hh % 10, color);
    clock_draw_char (x + 2, y, ':', color);
    clock_draw_digit(x + 3, y, mm / 10, color);
    clock_draw_digit(x + 4, y, mm % 10, color);
    clock_draw_char (x + 5, y, ':', color);
    clock_draw_digit(x + 6, y, ss / 10, color);
    clock_draw_digit(x + 7, y, ss % 10, color);
}

// Show live clock until Q pressed
void clock_run() {
    // Clear screen cleanly
    int i, j;
    for (i = 0; i < 25; i++) {
        for (j = 0; j < 80; j++) {
            vga[(i * 80 + j) * 2]     = ' ';
            vga[(i * 80 + j) * 2 + 1] = 0x0F;
        }
    }

    // Draw top and bottom alignment borders
    for (j = 0; j < 80; j++) {
        vga[j * 2] = '='; vga[j * 2 + 1] = 0x08;
        vga[(24 * 80 + j) * 2] = '='; vga[(24 * 80 + j) * 2 + 1] = 0x08;
    }

    // Center Title 
    char *title = "DeepithOS Clock";
    int tx = 32;
    for (i = 0; title[i]; i++) {
        clock_draw_char(tx + i, 0, title[i], 0x0B);
    }

    // Dynamic Uptime Setup (Row 8)
    char *uptime_label = "Uptime since boot: ";
    int lx = 25;
    for (i = 0; uptime_label[i]; i++) {
        clock_draw_char(lx + i, 8, uptime_label[i], 0x07);
    }

    // Centered Quit Command (Row 20)
    char *quit_label = "Press Q to return to shell";
    int qx = 27;
    for (i = 0; quit_label[i]; i++) {
        clock_draw_char(qx + i, 20, quit_label[i], 0x07);
    }

    int running = 1;
    unsigned int counter = 0;
    unsigned int last_recorded_second = 0xFFFFFFFF; // Tracks when a real second updates

    while (running) {
        counter++;
        // We still use a small loop delay to avoid blasting the CPU/VRAM continuously
        if (counter >= 100000) { 
            counter = 0;

            // FIX: Read the actual continuous running seconds from your PIT timer hardware
            unsigned int total_seconds = timer_seconds(); 

            // Only redraw when the system time actually rolls over to a new second
            if (total_seconds != last_recorded_second) {
                last_recorded_second = total_seconds;

                unsigned int ss = total_seconds % 60;
                unsigned int mm = (total_seconds / 60) % 60;
                unsigned int hh = (total_seconds / 3600) % 24;

                // Centering math for tight formatting (8 characters total width)
                int cx = 36; 
                int cy = 12;

                // Render continuous main clock columns matching global kernel uptime
                clock_draw_char(cx,     cy, '0' + hh/10, 0x0A);
                clock_draw_char(cx + 1, cy, '0' + hh%10, 0x0A);
                clock_draw_char(cx + 2, cy, ':', 0x0E);
                clock_draw_char(cx + 3, cy, '0' + mm/10, 0x0A);
                clock_draw_char(cx + 4, cy, '0' + mm%10, 0x0A);
                clock_draw_char(cx + 5, cy, ':', 0x0E);
                clock_draw_char(cx + 6, cy, '0' + ss/10, 0x0C);
                clock_draw_char(cx + 7, cy, '0' + ss%10, 0x0C);

                // Print counter inline with label on row 8
                int ux = lx + 19; 
                int uy = 8;
                clock_draw_char(ux,     uy, '0' + hh/10, 0x0B);
                clock_draw_char(ux + 1, uy, '0' + hh%10, 0x0B);
                clock_draw_char(ux + 2, uy, 'h', 0x07);
                clock_draw_char(ux + 4, uy, '0' + mm/10, 0x0B);
                clock_draw_char(ux + 5, uy, '0' + mm%10, 0x0B);
                clock_draw_char(ux + 6, uy, 'm', 0x07);
                clock_draw_char(ux + 8, uy, '0' + ss/10, 0x0B);
                clock_draw_char(ux + 9, uy, '0' + ss%10, 0x0B);
                clock_draw_char(ux + 10, uy, 's', 0x07);

                // Column-synchronized colon flasher
                if (ss % 2 == 0) {
                    clock_draw_char(cx + 2, cy, ':', 0x0E);
                    clock_draw_char(cx + 5, cy, ':', 0x0E);
                } else {
                    clock_draw_char(cx + 2, cy, ' ', 0x0F);
                    clock_draw_char(cx + 5, cy, ' ', 0x0F);
                }
            }
        }

        char c = keyboard_read();
        if (c == 'q' || c == 'Q') running = 0;
    }
}

// Draw clock in top right corner (called from kernel main loop)
void clock_update_corner() {
    clock_draw_time(71, 0, 0x08);
}
