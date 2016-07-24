#ifndef _IO_H
#define _IO_H

#include <stdint.h>

// read a byte from the specified IO port
static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    asm volatile ("inb %1, %0"
                 : "=a" (value)
                 : "dN" (port));
    // the in instruction takes the value from the port address in either %dx or
    // an immediate 8 bit, and writes it to either %al, %ax, or %eax
    return value;
}

// write a byte to the specified IO port
static inline void outb(uint8_t value, uint16_t port) {
    asm volatile ("outb %0, %1"
                  :
                  : "a" (value), "dN" (port));
    // the out instruction takes the value from %al, %ax, or %eax, and writes it 
    // to the port address in either %dx, or an immediate 8 bit
}

#endif // _IO_H
