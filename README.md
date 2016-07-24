# OS
A toy operating system written in C, still unnamed.

## Building
The OS binary image is built using an i686-elf cross-toolchain (instructions [here](http://wiki.osdev.org/GCC_Cross-Compiler)), and being tested in QEMU. The included Makefile takes care of compiling and linking the individual components, and firing up QEMU for testing.

## Next steps
- [ ] Setup the IDT, implement basic interrupt and exception handling.
- [ ] Write PS/2 and keyboard drivers.
- [ ] Switch from VGA text mode output to framebuffer graphics.
- [ ] Implement paging, and switch to a higher-half kernel.
