#include <io.h>
#include <screen.h>

// constants
static const size_t VGA_HEIGHT = 25;
static const size_t VGA_WIDTH = 80;

// global variables
size_t term_row;
size_t term_column;
uint8_t term_color;
uint16_t *term_buffer;

// initializes the terminal global variables, leaves the screen blank
void term_initialize(void) {
    term_row = 0;
    term_column = 0;
    term_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    term_buffer = (uint16_t *) 0xB8000; // VGA text buffer address
    // fill screen with spaces
    for (size_t i = 0; i < VGA_HEIGHT; ++i) {
        for (size_t j = 0; j < VGA_WIDTH; ++j) {
            term_putentryat(' ', term_color, i, j);
        }
    }
}

// prints a string to the terminal
void term_writestring(const char *data) {
    while (*data) {
        term_putchar(*data++);
    }
    // busy waiting, to view individual lines as they appear
    // for (size_t i = 0; i < 1e7; ++i)
    //     ;
}

void term_putnum(int n) {
    int div = n / 10;
    if (div != 0) {
        term_putnum(div);
    }
    term_putchar('0' + n % 10);
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
            term_putentryat(c, term_color, term_row, term_column);
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

// gives the linear index into the VGA buffer corresponding to a certain (i, j)
// position
static inline size_t index(size_t i, size_t j) {
    return VGA_WIDTH * i + j;
}

// scrolls the terminal down a single line of text
void term_scrolldown(void) {
    // copy every line to the one above
    for (size_t i = 0; i < VGA_HEIGHT-1; ++i) {
        for (size_t j = 0; j < VGA_WIDTH; ++j) {
            term_buffer[index(i, j)] = term_buffer[index(i+1, j)];
        }
    }
    // fill last line with spaces
    for (size_t j = 0; j < VGA_WIDTH; ++j) {
        term_putentryat(' ', term_color, VGA_HEIGHT-1, j);
    }
}

// puts a char with the given color at the specified terminal location
void term_putentryat(char c, uint8_t color, size_t i, size_t j) {
    term_buffer[index(i, j)] = make_vgaentry(c, color);
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

// variable names are based on documentation from the FreeVGA Project

// VGA I/O ports
static const uint16_t CRTC_ADDR_PORT = 0x03D4;
static const uint16_t CRTC_DATA_PORT = 0x03D5;

// VGA CRTC registers
static const uint8_t CURSOR_LOCATION_HI_REG = 0x0E;
static const uint8_t CURSOR_LOCATION_LO_REG = 0x0F;

// moves the cursor to the specified location
void term_movecursor(size_t i, size_t j) {
    uint16_t cursor_location = index(i, j);
    // write the high byte
    outb(CURSOR_LOCATION_HI_REG, CRTC_ADDR_PORT);
    outb(cursor_location >> 8, CRTC_DATA_PORT);
    // write the low byte
    outb(CURSOR_LOCATION_LO_REG, CRTC_ADDR_PORT);
    outb(cursor_location & 0xFF, CRTC_DATA_PORT);
}
