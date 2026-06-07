#include "ports.h"

#define KEYBOARD_PORT 0x60
#define KEYBOARD_STATUS 0x64

// State variables
int shift_pressed = 0;
int caps_lock = 0;

unsigned char port_read(unsigned short port) {
    unsigned char result;
    __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void keyboard_handler() {
    // Required by IDT
}

// Normal keys
static char scancode_map[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ',
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, // 0x48 Up Arrow
    0, 0, 0,
    3, // 0x4B Left Arrow
    0,
    4, // 0x4D Right Arrow
    0, 0,
    2  // 0x50 Down Arrow
};

// Shifted keys (for symbols like !@# and capital letters)
static char scancode_shift_map[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' '
};

int keyboard_has_key() {
    return port_read(KEYBOARD_STATUS) & 0x01;
}

char keyboard_read() {
    if (!keyboard_has_key()) return 0;

    unsigned char scancode = port_read(KEYBOARD_PORT);

    // --- HANDLE KEY RELEASES (Break Codes) ---
    if (scancode & 0x80) {
        unsigned char released = scancode & 0x7F;
        if (released == 0x2A || released == 0x36) { // Left or Right Shift released
            shift_pressed = 0;
        }
        return 0;
    }

    // --- HANDLE KEY PRESSES (Make Codes) ---
    
    // Check for Shift Press
    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = 1;
        return 0;
    }

    // Check for Caps Lock toggle
    if (scancode == 0x3A) {
        caps_lock = !caps_lock;
        return 0;
    }

    // Map the character
    if (scancode < 128) {
        char c;
        if (shift_pressed) {
            c = scancode_shift_map[scancode];
        } else {
            c = scancode_map[scancode];
        }

        // Apply Caps Lock logic for alphabetical letters only
        if (caps_lock && c >= 'a' && c <= 'z') {
            if (!shift_pressed) c -= 32; // Lower to Upper
        } else if (caps_lock && c >= 'A' && c <= 'Z') {
            if (shift_pressed) c += 32;  // Shift + Caps = Lowercase
        }

        return c;
    }

    return 0;
}
