#define SCREEN_W 80
#define SCREEN_H 25
#define MAX_SNAKE 150

extern unsigned char *vga;
extern int cursor;

void clear_screen();
void print_string(char *str, unsigned char color);
void print_newline();
void print_char(char c, unsigned char color);
char keyboard_read();

int snake_x[MAX_SNAKE];
int snake_y[MAX_SNAKE];
int snake_len;
int snake_dir;
int food_x, food_y;
int game_score;
unsigned int s_rand = 0xACE1;

void s_draw(int x, int y, char c, unsigned char color) {
    if (x < 0 || x >= SCREEN_W || y < 0 || y >= SCREEN_H) return;
    vga[(y * SCREEN_W + x) * 2]     = c;
    vga[(y * SCREEN_W + x) * 2 + 1] = color;
}

char s_get(int x, int y) {
    if (x < 0 || x >= SCREEN_W || y < 0 || y >= SCREEN_H) return '#';
    return vga[(y * SCREEN_W + x) * 2];
}

void s_place_food() {
    s_rand = s_rand * 1103515245 + 12345;
    food_x = 2 + ((s_rand >> 8) % (SCREEN_W - 5));
    food_y = 2 + ((s_rand >> 4) % (SCREEN_H - 5));
    if (s_get(food_x, food_y) != ' ') {
        s_rand += 7;
        food_x = 2 + ((s_rand >> 8) % (SCREEN_W - 5));
        food_y = 2 + ((s_rand >> 4) % (SCREEN_H - 5));
    }
    s_draw(food_x, food_y, '*', 0x0C);
}

void s_sleep() {
    unsigned int i;
    for (i = 0; i < 8000000; i++) {
        __asm__("nop");
    }
}

void snake_game() {
    clear_screen();
    cursor = 0;

    int i;
    snake_len = 5;
    for (i = 0; i < snake_len; i++) {
        snake_x[i] = 20 - i;
        snake_y[i] = 12;
        s_draw(snake_x[i], snake_y[i], (i == 0) ? '@' : 'o', 0x0A);
    }
    snake_dir = 0;
    game_score = 0;

    // Draw borders
    int x, y;
    for (x = 0; x < SCREEN_W; x++) {
        s_draw(x, 0, '#', 0x07);
        s_draw(x, SCREEN_H - 2, '#', 0x07);
    }
    for (y = 0; y < SCREEN_H - 1; y++) {
        s_draw(0, y, '#', 0x07);
        s_draw(SCREEN_W - 1, y, '#', 0x07);
    }

    // Controls hint
    s_draw(2,  SCREEN_H-1, 'W', 0x07);
    s_draw(3,  SCREEN_H-1, 'A', 0x07);
    s_draw(4,  SCREEN_H-1, 'S', 0x07);
    s_draw(5,  SCREEN_H-1, 'D', 0x07);
    s_draw(6,  SCREEN_H-1, '=', 0x07);
    s_draw(7,  SCREEN_H-1, 'm', 0x07);
    s_draw(8,  SCREEN_H-1, 'o', 0x07);
    s_draw(9,  SCREEN_H-1, 'v', 0x07);
    s_draw(10, SCREEN_H-1, 'e', 0x07);
    s_draw(12, SCREEN_H-1, 'Q', 0x07);
    s_draw(13, SCREEN_H-1, '=', 0x07);
    s_draw(14, SCREEN_H-1, 'q', 0x07);
    s_draw(15, SCREEN_H-1, 'u', 0x07);
    s_draw(16, SCREEN_H-1, 'i', 0x07);
    s_draw(17, SCREEN_H-1, 't', 0x07);

    s_place_food();

    int running = 1;
    while (running) {
        // Poll keyboard multiple times
        char k = 0;
        unsigned int p;
        for (p = 0; p < 3000; p++) {
            char key = keyboard_read();
            if (key != 0) k = key;
        }

        if ((k == 'w' || k == 'W') && snake_dir != 3) snake_dir = 2;
        if ((k == 's' || k == 'S') && snake_dir != 2) snake_dir = 3;
        if ((k == 'a' || k == 'A') && snake_dir != 0) snake_dir = 1;
        if ((k == 'd' || k == 'D') && snake_dir != 1) snake_dir = 0;
        if (k == 'q' || k == 'Q') { running = 0; continue; }

        int nx = snake_x[0];
        int ny = snake_y[0];
        if (snake_dir == 0) nx++;
        if (snake_dir == 1) nx--;
        if (snake_dir == 2) ny--;
        if (snake_dir == 3) ny++;

        char hit = s_get(nx, ny);
        if (hit == '#' || hit == 'o') {
            running = 0;
            continue;
        }

        int old_tail_x = snake_x[snake_len - 1];
        int old_tail_y = snake_y[snake_len - 1];

        for (i = snake_len - 1; i > 0; i--) {
            snake_x[i] = snake_x[i-1];
            snake_y[i] = snake_y[i-1];
        }
        snake_x[0] = nx;
        snake_y[0] = ny;

        if (snake_len > 1) s_draw(snake_x[1], snake_y[1], 'o', 0x0A);
        s_draw(snake_x[0], snake_y[0], '@', 0x0B);

        if (nx == food_x && ny == food_y) {
            if (snake_len < MAX_SNAKE) {
                snake_x[snake_len] = old_tail_x;
                snake_y[snake_len] = old_tail_y;
                snake_len++;
            }
            game_score += 10;
            s_place_food();

            // Draw score
            s_draw(30, SCREEN_H-1, 'S', 0x0E);
            s_draw(31, SCREEN_H-1, 'c', 0x0E);
            s_draw(32, SCREEN_H-1, 'o', 0x0E);
            s_draw(33, SCREEN_H-1, 'r', 0x0E);
            s_draw(34, SCREEN_H-1, 'e', 0x0E);
            s_draw(35, SCREEN_H-1, ':', 0x0E);
            s_draw(36, SCREEN_H-1, ' ', 0x0E);
            s_draw(37, SCREEN_H-1, '0' + (game_score/100)%10, 0x0A);
            s_draw(38, SCREEN_H-1, '0' + (game_score/10)%10, 0x0A);
            s_draw(39, SCREEN_H-1, '0' + game_score%10, 0x0A);
        } else {
            s_draw(old_tail_x, old_tail_y, ' ', 0x0F);
        }

        s_sleep();
    }

    // Game over
    clear_screen();
    cursor = 0;
    print_string("  GAME OVER!", 0x0C);
    print_newline();
    print_string("  Score: ", 0x0F);
    char digits[6];
    int d = 0;
    int sc = game_score;
    if (sc == 0) { print_char('0', 0x0A); }
    else {
        while (sc > 0) { digits[d++] = '0' + sc % 10; sc /= 10; }
        while (d > 0) print_char(digits[--d], 0x0A);
    }
    print_newline();
    print_string("  Press Q to exit", 0x07);
    print_newline();

    while (1) {
        char k = keyboard_read();
        if (k == 'q' || k == 'Q') break;
    }
    clear_screen();
    cursor = 0;
}
