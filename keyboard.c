#include "ports.h" // Ensure this includes your port_read/inb definitions

#define KEYBOARD_PORT 0x60

// CORRECTED: Perfect 1-to-1 index matching for IBM Scan Code Set 1
char scancode_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0,
  '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0, '*',   0, ' '
};

// CORRECTED: Fixed shifted map alignment 
char scancode_shift_map[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
  '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',   0,
  '|',  'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',   0, '*',   0, ' '
};

// Shift, caps lock, and keyboard focus states
int shift_pressed = 0;
int caps_lock = 0;
int keyboard_focus = 0; // 0 = Shell/Kernel, 1 = Snake Game

unsigned char port_read(unsigned short port) {
    unsigned char result;
    __asm__("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

int keyboard_has_key() {
    return port_read(0x64) & 0x01;
}

void keyboard_handler() {}

char keyboard_read() {
    if (!keyboard_has_key()) return 0;

    unsigned char scancode = port_read(KEYBOARD_PORT);

    // Key release events (bit 7 set)
    if (scancode & 0x80) {
        unsigned char released = scancode & 0x7F;
        if (released == 0x2A || released == 0x36) {
            shift_pressed = 0;
        }
        return 0;
    }

    // Left/Right shift press
    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = 1;
        return 0;
    }

    // Caps Lock toggle
    if (scancode == 0x3A) {
        caps_lock = !caps_lock;
        return 0;
    }

    // Up arrow
    if (scancode == 0x48) return 0x01;

    // Down arrow
    if (scancode == 0x50) return 0x02;

    // Standard character map lookup bounded to 128 keys
    if (scancode < 128) {
        char c;
        if (shift_pressed) {
            c = scancode_shift_map[scancode];
        } else {
            c = scancode_map[scancode];
        }

        // Apply caps lock logic to alphabetical letters
        if (caps_lock && c >= 'a' && c <= 'z') c = c - 32;
        if (caps_lock && c >= 'A' && c <= 'Z' && !shift_pressed) c = c + 32;

        return c;
    }

    return 0;
}
