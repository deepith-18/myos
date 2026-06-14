// Simple text editor for DeepithOS
#define SCREEN_W 80
#define SCREEN_H 25
#define MAX_LINES 20
#define MAX_LINE_LEN 78

// Kernel functions
void print_string(char *str, unsigned char color);
void print_char(char c, unsigned char color);
void print_newline();
void clear_screen();
void update_cursor();

// Keyboard
char keyboard_read();

// Filesystem
int fs_create(char *name);
int fs_write(char *name, char *data);
char *fs_read(char *name);

// VGA direct
extern unsigned char *vga;
extern int cursor;

void draw_char(int x, int y, char c, unsigned char color) {
    int pos = y * SCREEN_W + x;
    vga[pos * 2]     = c;
    vga[pos * 2 + 1] = color;
}

void draw_string(int x, int y, char *str, unsigned char color) {
    int i = 0;
    while (str[i] && x + i < SCREEN_W) {
        draw_char(x + i, y, str[i], color);
        i++;
    }
}

void clear_line(int y, unsigned char color) {
    int x;
    for (x = 0; x < SCREEN_W; x++) {
        draw_char(x, y, ' ', color);
    }
}

// Editor state
char lines[MAX_LINES][MAX_LINE_LEN];
int line_count;
int cur_line;
int cur_col;
char filename[16];

int ed_strlen(char *str) {
    int i = 0;
    while (str[i]) i++;
    return i;
}

void ed_strcpy(char *dst, char *src, int max) {
    int i = 0;
    while (src[i] && i < max - 1) { dst[i] = src[i]; i++; }
    dst[i] = 0;
}

// Draw the editor screen
void draw_editor() {
    int i;

    // Draw title bar
    clear_line(0, 0x70);
    draw_string(2, 0, "DeepithOS Editor - ", 0x70);
    draw_string(21, 0, filename, 0x70);
    draw_string(50, 0, "S=Save  Q=Quit", 0x70);

    // Draw lines
    for (i = 0; i < MAX_LINES; i++) {
        clear_line(i + 1, 0x0F);
        // Line number
        draw_char(0, i + 1, '0' + (i / 10), 0x08);
        draw_char(1, i + 1, '0' + (i % 10), 0x08);
        draw_char(2, i + 1, '|', 0x08);
        // Line content
        if (i < line_count) {
            draw_string(3, i + 1, lines[i], 0x0F);
        }
    }

    // Status bar
    clear_line(SCREEN_H - 1, 0x70);
    draw_string(2, SCREEN_H - 1, "Line:", 0x70);
    draw_char(7, SCREEN_H - 1, '0' + cur_line / 10, 0x70);
    draw_char(8, SCREEN_H - 1, '0' + cur_line % 10, 0x70);
    draw_string(10, SCREEN_H - 1, "Col:", 0x70);
    draw_char(14, SCREEN_H - 1, '0' + cur_col / 10, 0x70);
    draw_char(15, SCREEN_H - 1, '0' + cur_col % 10, 0x70);

    // Set hardware cursor
    cursor = (cur_line + 1) * SCREEN_W + cur_col + 3;
    update_cursor();
}

// Save file
void editor_save() {
    // Combine all lines into one buffer with newline markers
    char buf[256];
    int pos = 0;
    int i;
    for (i = 0; i < line_count && pos < 250; i++) {
        int j = 0;
        while (lines[i][j] && pos < 250) {
            buf[pos++] = lines[i][j++];
        }
        if (i < line_count - 1 && pos < 250) {
            buf[pos++] = ' ';
        }
    }
    buf[pos] = 0;

    // Try write first, create if not exists
    if (fs_write(filename, buf) != 0) {
        fs_create(filename);
        fs_write(filename, buf);
    }

    // Show saved message in status bar
    clear_line(SCREEN_H - 1, 0x70);
    draw_string(2, SCREEN_H - 1, "Saved!", 0x2F);
}

void editor_run(char *fname) {
    // Init
    int i, j;
    for (i = 0; i < MAX_LINES; i++) {
        for (j = 0; j < MAX_LINE_LEN; j++) {
            lines[i][j] = 0;
        }
    }
    line_count = 1;
    cur_line = 0;
    cur_col = 0;

    // Copy filename
    ed_strcpy(filename, fname, 16);

    // Load existing file if exists
    char *existing = fs_read(fname);
    if (existing) {
        int pos = 0;
        int ln = 0;
        int col = 0;
        while (existing[pos] && ln < MAX_LINES) {
            if (existing[pos] == ' ' && ln < MAX_LINES - 1) {
                lines[ln][col] = 0;
                ln++;
                col = 0;
            } else if (col < MAX_LINE_LEN - 1) {
                lines[ln][col++] = existing[pos];
            }
            pos++;
        }
        lines[ln][col] = 0;
        line_count = ln + 1;
    }

    clear_screen();
    draw_editor();

    // Editor loop
    int running = 1;
    while (running) {
        char c = keyboard_read();
        if (c == 0) continue;

        if (c == 'q' || c == 'Q') {
            running = 0;

        } else if (c == 's' || c == 'S') {
            editor_save();

        } else if (c == 0x01) {
            // Up arrow
            if (cur_line > 0) {
                cur_line--;
                int len = ed_strlen(lines[cur_line]);
                if (cur_col > len) cur_col = len;
            }

        } else if (c == 0x02) {
            // Down arrow
            if (cur_line < line_count - 1) {
                cur_line++;
                int len = ed_strlen(lines[cur_line]);
                if (cur_col > len) cur_col = len;
            }

        } else if (c == '\b') {
            // Backspace
            if (cur_col > 0) {
                int len = ed_strlen(lines[cur_line]);
                int k;
                for (k = cur_col - 1; k < len; k++) {
                    lines[cur_line][k] = lines[cur_line][k + 1];
                }
                cur_col--;
            }

        } else if (c == '\n') {
            // Enter - new line
            if (line_count < MAX_LINES) {
                int k;
                for (k = line_count; k > cur_line + 1; k--) {
                    ed_strcpy(lines[k], lines[k-1], MAX_LINE_LEN);
                }
                // Clear new line
                for (k = 0; k < MAX_LINE_LEN; k++) lines[cur_line+1][k] = 0;
                line_count++;
                cur_line++;
                cur_col = 0;
            }

        } else if (c >= 32 && c < 127) {
            // Normal character
            int len = ed_strlen(lines[cur_line]);
            if (cur_col < MAX_LINE_LEN - 1) {
                // Shift characters right
                int k;
                for (k = len; k > cur_col; k--) {
                    lines[cur_line][k] = lines[cur_line][k-1];
                }
                lines[cur_line][cur_col] = c;
                cur_col++;
            }
        }

        draw_editor();
    }

    clear_screen();
}
