// ASCII Art Generator for DeepithOS
void print_string(char *str, unsigned char color);
void print_newline();
void print_char(char c, unsigned char color);

// Each letter is 5 rows tall, 6 chars wide
// 1 = filled, 0 = space
// Letters A-Z and space

char font[27][5][6] = {
    // A
    {" ### ",
     "#   #",
     "#####",
     "#   #",
     "#   #"},
    // B
    {"#### ",
     "#   #",
     "#### ",
     "#   #",
     "#### "},
    // C
    {" ####",
     "#    ",
     "#    ",
     "#    ",
     " ####"},
    // D
    {"#### ",
     "#   #",
     "#   #",
     "#   #",
     "#### "},
    // E
    {"#####",
     "#    ",
     "###  ",
     "#    ",
     "#####"},
    // F
    {"#####",
     "#    ",
     "###  ",
     "#    ",
     "#    "},
    // G
    {" ####",
     "#    ",
     "#  ##",
     "#   #",
     " ####"},
    // H
    {"#   #",
     "#   #",
     "#####",
     "#   #",
     "#   #"},
    // I
    {"#####",
     "  #  ",
     "  #  ",
     "  #  ",
     "#####"},
    // J
    {"#####",
     "   # ",
     "   # ",
     "#  # ",
     " ##  "},
    // K
    {"#   #",
     "#  # ",
     "###  ",
     "#  # ",
     "#   #"},
    // L
    {"#    ",
     "#    ",
     "#    ",
     "#    ",
     "#####"},
    // M
    {"#   #",
     "## ##",
     "# # #",
     "#   #",
     "#   #"},
    // N
    {"#   #",
     "##  #",
     "# # #",
     "#  ##",
     "#   #"},
    // O
    {" ### ",
     "#   #",
     "#   #",
     "#   #",
     " ### "},
    // P
    {"#### ",
     "#   #",
     "#### ",
     "#    ",
     "#    "},
    // Q
    {" ### ",
     "#   #",
     "#   #",
     "#  ##",
     " ####"},
    // R
    {"#### ",
     "#   #",
     "#### ",
     "#  # ",
     "#   #"},
    // S
    {" ####",
     "#    ",
     " ### ",
     "    #",
     "#### "},
    // T
    {"#####",
     "  #  ",
     "  #  ",
     "  #  ",
     "  #  "},
    // U
    {"#   #",
     "#   #",
     "#   #",
     "#   #",
     " ### "},
    // V
    {"#   #",
     "#   #",
     "#   #",
     " # # ",
     "  #  "},
    // W
    {"#   #",
     "#   #",
     "# # #",
     "## ##",
     "#   #"},
    // X
    {"#   #",
     " # # ",
     "  #  ",
     " # # ",
     "#   #"},
    // Y
    {"#   #",
     " # # ",
     "  #  ",
     "  #  ",
     "  #  "},
    // Z
    {"#####",
     "   # ",
     "  #  ",
     " #   ",
     "#####"},
    // Space
    {"     ",
     "     ",
     "     ",
     "     ",
     "     "}
};

// Get font index for a character
int get_font_index(char c) {
    if (c >= 'a' && c <= 'z') return c - 'a';
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c == ' ') return 26;
    return 26; // default to space
}

// Print text in big ASCII art letters
void art_print(char *text, unsigned char color) {
    int row;
    // Print each of the 5 rows
    for (row = 0; row < 5; row++) {
        print_string("  ", color);
        // Print this row for each character
        int i = 0;
        while (text[i] && i < 12) {  // max 12 chars
            int idx = get_font_index(text[i]);
            // Print each character of the font row
            int col;
            for (col = 0; col < 5; col++) {
                char fc = font[idx][row][col];
                if (fc == '#') {
                    print_char('#', color);
                } else {
                    print_char(' ', color);
                }
            }
            print_char(' ', color);  // space between letters
            i++;
        }
        print_newline();
    }
}
