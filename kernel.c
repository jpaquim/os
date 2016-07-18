#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// hardware text mode color constants
typedef enum vga_color {
    COLOR_BLACK         = 0,
    COLOR_BLUE          = 1,
    COLOR_GREEN         = 2,
    COLOR_CYAN          = 3,
    COLOR_RED           = 4,
    COLOR_MAGENTA       = 5,
    COLOR_BROWN         = 6,
    COLOR_LIGHT_GREY    = 7,
    COLOR_DARK_GREY     = 8,
    COLOR_LIGHT_BLUE    = 9,
    COLOR_LIGHT_GREEN   = 10,
    COLOR_LIGHT_CYAN    = 11,
    COLOR_LIGHT_RED     = 12,
    COLOR_LIGHT_MAGENTA = 13,
    COLOR_LIGHT_BROWN   = 14,
    COLOR_WHITE         = 15
} vga_color;

// function declarations
void kernel_main(void);
void term_initialize(void);
void term_writestring(const char *data);
void term_putchar(char c);
void term_scrolldown(void);
void term_putentryat(char c, uint8_t color, size_t x, size_t y);
void term_setcolor(uint8_t color);
uint16_t make_vgaentry(char c, uint8_t color);
uint8_t make_color(vga_color fg, vga_color bg);
size_t strlen(const char *str);

// constants
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

// global variables
size_t term_row;
size_t term_column;
uint8_t term_color;
uint16_t *term_buffer;

// kernel entry point
void kernel_main(void) {
    // Initialize terminal interface
    term_initialize();

    term_writestring("This line will scroll off the screen.\n");
    term_writestring("This one too.\n");
    term_writestring("As well as this one.\n");

    term_writestring("Hello kernel world,\n");

    term_writestring("This line will be partially overwritten\r");
    for (size_t i = 0; i < 20; ++i) {
        term_writestring("This line repeats 20 times.\n");
    }

    term_writestring("Printed 25 lines, the screen starts scrolling\n");

    term_writestring("Scrolling once\n");
    term_writestring("Scrolling twice\n");
    term_writestring("Scrolling thrice\n");
    term_writestring("Scrolled!");
}

// initializes the terminal global variables, leaves the screen blank
void term_initialize(void) {
    term_row = 0;
    term_column = 0;
    term_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    term_buffer = (uint16_t *) 0xB8000; // VGA text buffer address
    // fill screen with spaces
    for (size_t y = 0; y < VGA_HEIGHT; ++y) {
        for (size_t x = 0; x < VGA_WIDTH; ++x) {
            term_putentryat(' ', term_color, x, y);
        }
    }
}

// prints a string to the terminal
void term_writestring(const char *data) {
    while (*data) {
        term_putchar(*data++);
    }
    // busy waiting, to view individual lines as they appear
    for (size_t i = 0; i < 1e7; ++i)
        ;
}

// prints a character to the terminal
void term_putchar(char c) {
    switch (c) {
        case '\n': // newline
            ++term_row;
            term_column = 0;
            break;
        case '\r': // carriage return
            term_column = 0;
            break;
        default:
            term_putentryat(c, term_color, term_column, term_row);
            ++term_column;
            break;
    }
    // past the far right of the screen, move cursor to the next line
    if (term_column == VGA_WIDTH) {
        term_column = 0;
        ++term_row;
    }
    // past the bottom of the screen, scroll down, leave cursor in the last line
    if (term_row == VGA_HEIGHT) {
        term_scrolldown();
        --term_row;
    }
}

// macro, gives the linear index into the VGA buffer corresponding to an (x, y)
// position
#define IND(x, y) (y)*VGA_WIDTH+(x)

// scrolls the terminal down a single line of text
void term_scrolldown(void) {
    // copy every line to the one above
    for (size_t y = 0; y < VGA_HEIGHT-1; ++y) {
        for (size_t x = 0; x < VGA_WIDTH; ++x) {
            term_buffer[IND(x, y)] = term_buffer[IND(x, y+1)];
        }
    }
    // fill last line with spaces
    for (size_t x = 0; x < VGA_WIDTH; ++x) {
        term_putentryat(' ', term_color, x, VGA_HEIGHT-1);
    }
}

// puts a char with the given color at the specified terminal location
void term_putentryat(char c, uint8_t color, size_t x, size_t y) {
    term_buffer[IND(x, y)] = make_vgaentry(c, color);
}

// sets the color of the text
void term_setcolor(uint8_t color) {
    term_color = color;
}

// creates a vga byte pair from the given character and VGA color byte
uint16_t make_vgaentry(char c, uint8_t color) {
    uint16_t c16 = c;
    uint16_t color16 = color;
    return color16 << 8 | c16;
}

// creates the VGA text attribute byte from the specified foreground and
// background colors
uint8_t make_color(vga_color fg, vga_color bg) {
    return bg << 4 | fg;
}

// returns the length of a null terminated string
size_t strlen(const char *str) {
    size_t len = 0;
    while (str[len]) {
        ++len;
    }
    return len;
}
