#include <io.h>
#include <screen.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// function declarations
void kernel_main(void);
size_t strlen(const char *str);

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
    term_movecursor(0, 0);
}

// returns the length of a null terminated string
size_t strlen(const char *str) {
    size_t len = 0;
    while (str[len]) {
        ++len;
    }
    return len;
}
