// Keyboard port
#define KEYBOARD_PORT 0x60

// Scancode to ASCII map (US keyboard layout)
char scancode_map[] = {
    0, 0, '1','2','3','4','5','6','7','8','9','0','-','=', 0,
    0, 'q','w','e','r','t','y','u','i','o','p','[',']', 0,
    0, 'a','s','d','f','g','h','j','k','l',';','\'','`',
    0, '\\','z','x','c','v','b','n','m',',','.','/', 0,
    '*', 0, ' '
};

// Read one byte from a hardware port
unsigned char port_read(unsigned short port) {
    unsigned char result;
    __asm__("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

// Check if keyboard has a key waiting
int keyboard_has_key() {
    unsigned char status = port_read(0x64);
    return status & 0x01;
}

// Read one character from keyboard
char keyboard_read() {
    // Wait until a key is pressed
    while (!keyboard_has_key());

    unsigned char scancode = port_read(KEYBOARD_PORT);

    // Only handle key press (not key release)
    if (scancode & 0x80) return 0;

    // Backspace scancode is 0x0E
    if (scancode == 0x0E) return '\b';

    // Enter scancode is 0x1C
    if (scancode == 0x1C) return '\n';

    // Return mapped character
    if (scancode < sizeof(scancode_map)) {
        return scancode_map[scancode];
    }

    return 0;
}
