#define KEYBOARD_PORT 0x60

char scancode_map[] = {
    0, 0, '1','2','3','4','5','6','7','8','9','0','-','=', 0,
    0, 'q','w','e','r','t','y','u','i','o','p','[',']', 0,
    0, 'a','s','d','f','g','h','j','k','l',';','\'','`',
    0, '\\','z','x','c','v','b','n','m',',','.','/', 0,
    '*', 0, ' '
};

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
    if (scancode & 0x80) return 0;

    // Backspace
    if (scancode == 0x0E) return '\b';

    // Enter
    if (scancode == 0x1C) return '\n';

    // Tab
    if (scancode == 0x0F) return '\t';

    // Up arrow → return special code 0x01
    if (scancode == 0x48) return 0x01;

    // Down arrow → return special code 0x02
    if (scancode == 0x50) return 0x02;

    if (scancode < sizeof(scancode_map)) {
        return scancode_map[scancode];
    }
    return 0;
}
