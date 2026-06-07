#include "ports.h"

#define SCREEN_W 80
#define SCREEN_H 25
#define MAX_SNAKE 150

extern unsigned char *vga;
extern int cursor;

void clear_screen();
void print_string(char *str, unsigned char color);
void print_newline();
char keyboard_read();
unsigned int timer_get_ticks();

int snake_x[MAX_SNAKE];
int snake_y[MAX_SNAKE];
int snake_len;
int snake_dir; 
int food_x, food_y;
int game_score;
unsigned int s_rand = 0xACE1u;

void s_draw(int x, int y, char c, unsigned char color) {
    if (x < 0 || x >= SCREEN_W || y < 0 || y >= SCREEN_H) return;
    vga[(y * SCREEN_W + x) * 2] = c;
    vga[(y * SCREEN_W + x) * 2 + 1] = color;
}

char s_get(int x, int y) {
    if (x < 0 || x >= SCREEN_W || y < 0 || y >= SCREEN_H) return '#';
    return vga[(y * SCREEN_W + x) * 2];
}

void s_place_food() {
    s_rand = s_rand * 1103515245 + 12345;
    food_x = 2 + (s_rand % (SCREEN_W - 5));
    food_y = 2 + (s_rand % (SCREEN_H - 5));
    if (s_get(food_x, food_y) != ' ') {
        s_place_food(); 
        return;
    }
    s_draw(food_x, food_y, '*', 0x0C);
}

// SLOW DOWN FUNCTION
void s_sleep(unsigned int ticks) {
    unsigned int start = timer_get_ticks();
    
    // If your PIT timer is not working, this manual loop will run.
    // I increased the number to 40,000,000 for a slower fallback.
    if (start == 0) { 
        for(volatile int i=0; i<40000000; i++); 
        return;
    }

    // Normal Timer-based wait
    while (timer_get_ticks() - start < ticks);
}

void snake_game() {
    clear_screen();
    cursor = 0; 

    snake_len = 5;
    for(int i=0; i<snake_len; i++) {
        snake_x[i] = 20 - i;
        snake_y[i] = 12;
    }
    snake_dir = 0; // Start moving Right
    game_score = 0;

    // --- DRAW BORDERS ---
    // Horizontal walls
    for(int x=0; x < SCREEN_W; x++) {
        s_draw(x, 0, '#', 0x07);           
        s_draw(x, SCREEN_H-2, '#', 0x07);  
    }
    // Vertical walls
    for(int y=0; y < SCREEN_H-1; y++) {
        s_draw(0, y, '#', 0x07);           
        s_draw(SCREEN_W-1, y, '#', 0x07);  
    }
    
    s_place_food();

    int running = 1;
    while(running) {
        char k = keyboard_read();

        // Control logic: WASD or Arrow Keys (1=Up, 2=Down, 3=Left, 4=Right)
        if ((k == 'w' || k == 'W' || k == 1) && snake_dir != 3) snake_dir = 2; // Up
        if ((k == 's' || k == 'S' || k == 2) && snake_dir != 2) snake_dir = 3; // Down
        if ((k == 'a' || k == 'A' || k == 3) && snake_dir != 0) snake_dir = 1; // Left
        if ((k == 'd' || k == 'D' || k == 4) && snake_dir != 1) snake_dir = 0; // Right
        if (k == 'q' || k == 'Q') break;

        int nx = snake_x[0];
        int ny = snake_y[0];
        if (snake_dir == 0) nx++;
        if (snake_dir == 1) nx--;
        if (snake_dir == 2) ny--;
        if (snake_dir == 3) ny++;

        // Collision Check
        char hit = s_get(nx, ny);
        if (hit == '#' || hit == 'o') {
            running = 0;
            break;
        }

        // Eat logic
        if (nx == food_x && ny == food_y) {
            if(snake_len < MAX_SNAKE) snake_len++;
            game_score += 10;
            s_place_food();
        } else {
            // Remove tail
            s_draw(snake_x[snake_len-1], snake_y[snake_len-1], ' ', 0x07);
        }

        // Move body in array
        for(int i = snake_len-1; i > 0; i--) {
            snake_x[i] = snake_x[i-1];
            snake_y[i] = snake_y[i-1];
        }
        snake_x[0] = nx;
        snake_y[0] = ny;

        // Draw the snake
        for(int i=0; i<snake_len; i++) {
            s_draw(snake_x[i], snake_y[i], (i==0)?'@':'o', 0x0A);
        }

        // --- SPEED CONTROL ---
        // 250 ticks is usually "Normal" speed for 1000Hz PIT.
        // If it is still too fast, change 250 to 500.
        s_sleep(250); 
    }

    // End Game
    clear_screen();
    cursor = 0;
    print_string("  GAME OVER! Score: ", 0x0C);
    
    // Convert score to string
    char score_str[4];
    score_str[0] = (game_score / 100) + '0';
    score_str[1] = ((game_score / 10) % 10) + '0';
    score_str[2] = (game_score % 10) + '0';
    score_str[3] = '\0';
    print_string(score_str, 0x0A);

    print_newline();
    print_string("  Press Q to exit.", 0x0F);
    while(1) {
        char k = keyboard_read();
        if (k == 'q' || k == 'Q') break;
    }
    clear_screen();
    cursor = 0;
}
