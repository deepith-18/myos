#define SCREEN_W 80
#define SCREEN_H 25
#define MAX_SNAKE 100

void print_string(char *str, unsigned char color);
void print_char(char c, unsigned char color);
void print_newline();
void clear_screen();
void update_cursor();

char keyboard_read();
unsigned int timer_get_ticks();

extern unsigned char *vga;
extern int cursor;

void draw_char(int x, int y, char c, unsigned char color) {
    int pos = y * SCREEN_W + x;
    vga[pos * 2]     = c;
    vga[pos * 2 + 1] = color;
}

char get_char(int x, int y) {
    int pos = y * SCREEN_W + x;
    return vga[pos * 2];
}

unsigned int rand_seed = 12345;
unsigned int rand_next() {
    rand_seed = rand_seed * 1103515245 + 12345;
    return (rand_seed >> 16) & 0x7FFF;
}

int snake_x[MAX_SNAKE];
int snake_y[MAX_SNAKE];
int snake_len;
int dir; // 0=right 1=left 2=up 3=down

int food_x;
int food_y;
int score;

// Reads directly from PS/2 Status Register safely inside local context
unsigned char snake_check_port() {
    unsigned char result;
    __asm__("inb $0x64, %0" : "=a"(result));
    return result & 0x01;
}

void place_food() {
    int x, y;
    do {
        x = 2 + (rand_next() % (SCREEN_W - 4));
        y = 2 + (rand_next() % (SCREEN_H - 4));
    } while (get_char(x, y) != ' ');
    food_x = x;
    food_y = y;
    draw_char(food_x, food_y, '*', 0x0C);
}

void draw_border() {
    int x, y;
    for (x = 0; x < SCREEN_W; x++) {
        draw_char(x, 0, '-', 0x07);
        draw_char(x, SCREEN_H - 2, '-', 0x07);
    }
    for (y = 0; y < SCREEN_H - 1; y++) {
        draw_char(0, y, '|', 0x07);
        draw_char(SCREEN_W - 1, y, '|', 0x07);
    }
}

void draw_score() {
    draw_char(2, SCREEN_H - 1, 'S', 0x0E);
    draw_char(3, SCREEN_H - 1, 'c', 0x0E);
    draw_char(4, SCREEN_H - 1, 'o', 0x0E);
    draw_char(5, SCREEN_H - 1, 'r', 0x0E);
    draw_char(6, SCREEN_H - 1, 'e', 0x0E);
    draw_char(7, SCREEN_H - 1, ':', 0x0E);
    draw_char(8, SCREEN_H - 1, ' ', 0x0E);

    int s = score;
    if (s == 0) {
        draw_char(9, SCREEN_H - 1, '0', 0x0A);
    } else {
        char digits[6];
        int i = 0;
        while (s > 0) { digits[i++] = '0' + (s % 10); s /= 10; }
        int col = 9;
        while (i > 0) { draw_char(col++, SCREEN_H - 1, digits[--i], 0x0A); }
    }
}

void wait_ticks(unsigned int n) {
    unsigned int start = timer_get_ticks();
    while (timer_get_ticks() - start < n);
}

void snake_game() {
    clear_screen();

    snake_len = 3;
    snake_x[0] = 20; snake_y[0] = 12;
    snake_x[1] = 19; snake_y[1] = 12;
    snake_x[2] = 18; snake_y[2] = 12;
    dir = 0;  
    score = 0;

    draw_border();

    draw_char(snake_x[0], snake_y[0], '@', 0x0A);
    draw_char(snake_x[1], snake_y[1], 'o', 0x0A);
    draw_char(snake_x[2], snake_y[2], 'o', 0x0A);

    place_food();
    draw_score();

    draw_char(30, SCREEN_H - 1, 'W', 0x07);
    draw_char(31, SCREEN_H - 1, 'A', 0x07);
    draw_char(32, SCREEN_H - 1, 'S', 0x07);
    draw_char(33, SCREEN_H - 1, 'D', 0x07);
    draw_char(34, SCREEN_H - 1, '=', 0x07);
    draw_char(35, SCREEN_H - 1, 'm', 0x07);
    draw_char(36, SCREEN_H - 1, 'o', 0x07);
    draw_char(37, SCREEN_H - 1, 'v', 0x07);
    draw_char(38, SCREEN_H - 1, 'e', 0x07);

    while (1) {
        // ONLY call keyboard_read if a key is genuinely waiting at the hardware interface
        if (snake_check_port()) {
            char c = keyboard_read();
            if (c == 'w' || c == 'W') { if (dir != 3) dir = 2; }
            if (c == 's' || c == 'S') { if (dir != 2) dir = 3; }
            if (c == 'a' || c == 'A') { if (dir != 0) dir = 1; }
            if (c == 'd' || c == 'D') { if (dir != 1) dir = 0; }
            if (c == 'q' || c == 'Q') break;  
        }

        // Snake keeps sliding dynamically even when no key is touched!
        int new_x = snake_x[0];
        int new_y = snake_y[0];
        if (dir == 0) new_x++;
        if (dir == 1) new_x--;
        if (dir == 2) new_y--;
        if (dir == 3) new_y++;

        if (new_x <= 0 || new_x >= SCREEN_W - 1 || new_y <= 0 || new_y >= SCREEN_H - 2) {
            break; 
        }

        char hit = get_char(new_x, new_y);
        if (hit == 'o' || hit == '@') break;

        int ate = (new_x == food_x && new_y == food_y);
        int tail_x = snake_x[snake_len - 1];
        int tail_y = snake_y[snake_len - 1];

        int i;
        for (i = snake_len - 1; i > 0; i--) {
            snake_x[i] = snake_x[i - 1];
            snake_y[i] = snake_y[i - 1];
        }
        snake_x[0] = new_x;
        snake_y[0] = new_y;

        if (!ate) {
            draw_char(tail_x, tail_y, ' ', 0x0F);
        } else {
            if (snake_len < MAX_SNAKE) {
                snake_x[snake_len] = tail_x;
                snake_y[snake_len] = tail_y;
                snake_len++;
            }
            score += 10;
            draw_score();
            place_food();
        }

        draw_char(snake_x[1], snake_y[1], 'o', 0x0A);
        draw_char(snake_x[0], snake_y[0], '@', 0x0B);

        unsigned int speed = 8;
        if (score > 50)  speed = 6;
        if (score > 100) speed = 4;
        wait_ticks(speed);
    }

    clear_screen();
    cursor = 0;
    print_string("   GAME OVER!", 0x0C);
    print_newline();
    print_string("   Score: ", 0x0F);
    int s = score;
    if (s == 0) {
        print_char('0', 0x0A);
    } else {
        char digits[6];
        int i = 0;
        while (s > 0) { digits[i++] = '0' + (s % 10); s /= 10; }
        while (i > 0) print_char(digits[--i], 0x0A);
    }
    print_newline();
    print_string("   Press any key to return...", 0x07);
    print_newline();

    while (1) {
        if (snake_check_port()) {
            keyboard_read();
            break;
        }
    }
}
