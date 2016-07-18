# fields of Multiboot header - u32
#   'magic' - magic number identifying the header
#   'flags' - features that the kernel requests from the bootloader
#     - bit 0 - align loaded modules on page (4K) boundaries
#     - bit 1 - provide information on available memory on the mem_* fields, and
#               memory map on the mmap_* fields of the Multiboot information
#               structure.
# 'checksum' - verifies the validity of the Multiboot header
# if the kernel is in a.out format, rather than ELF, additional fields have to
# be set

MAGIC    = 0x1BADB002
ALIGN    = 1 << 0
MEMINFO  = 1 << 1
FLAGS    = ALIGN | MEMINFO
CHECKSUM = -(MAGIC + FLAGS)

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

# section flags:
#   a - section is allocatable
#   w - section is writable
# section type:
#   @nobits - section does not contain data
.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 16384 # 16 KiB
stack_top:

.section .text
.global _start
.type _start, @function
_start:
    mov $stack_top, %esp
    
    call kernel_main
    
    cli
1:	hlt
    jmp 1b # jump backwards to local label 1:

.size _start, . - _start
