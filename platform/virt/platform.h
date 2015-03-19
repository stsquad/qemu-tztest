#ifndef _PLATFORM_H
#define _PLATFORM_H

/* NORFLASH0 on Vexpress aseries (a15) is mapped from 0x08000000 to 0x0A000000
 * which is 32MB.  It is also aliased to 0x0 (to 0x2000000).
 * It is acceptable to have the text here as it is RO.
 */
#define FLASH_BASE 0x00000000               // From QEMU virt.c
#define FLASH_SIZE 0x08000000               // From QEMU virt.c
#define EL3_FLASH_BASE FLASH_BASE
#define EL1_S_FLASH_BASE (FLASH_BASE+0x10000)
#define EL1_NS_FLASH_BASE (FLASH_BASE+0x20000)
#define EL0_NS_FLASH_BASE (FLASH_BASE+0x30000)
#define EL0_S_FLASH_BASE (FLASH_BASE+0x50000)

#define RAM_BASE 0x40000000                 // From QEMU virt.c
#define RAM_SIZE (2*1024*1024*1024)         // Only 2GB needed
#define EL3_BASE_VA 0xF000000000
#define EL1_S_BASE_VA 0xC000000000
#define EL1_NS_BASE_VA 0x80000000
#define EL0_S_BASE_VA 0x00080000
#define EL0_NS_BASE_VA 0x00010000
#define EL3_RAM_BASE RAM_BASE
#define EL3_RAM_SIZE (512*1024)
#define EL1_S_RAM_BASE (RAM_BASE+0x1000000)
#define EL1_S_RAM_SIZE (512*1024)
#define EL1_NS_RAM_BASE (RAM_BASE+0x2000000)
#define EL1_NS_RAM_SIZE (512*1024)
#define EL0_NS_RAM_BASE (RAM_BASE+0x4000000)
#define EL0_NS_RAM_SIZE (512*1024)
#define VA_SIZE 48
#define TnSZ (64-VA_SIZE)

#define EL3_PGTBL_SIZE 0x40000
#define EL3_PGTBL_BASE EL3_RAM_BASE+EL3_RAM_SIZE-EL3_PGTBL_SIZE
#define EL3_STACK_SIZE 0x40000
#define EL3_STACK_BASE 0xFF00000000
#define EL1_S_PGTBL_SIZE 0x40000
#define EL1_S_PGTBL_BASE EL1_S_RAM_BASE+EL1_S_RAM_SIZE-EL1_S_PGTBL_SIZE
#define EL1_S_STACK_SIZE 0x40000
#define EL1_S_STACK_BASE 0xCF00000000
#define EL1_NS_PGTBL_SIZE 0x40000
#define EL1_NS_PGTBL_BASE EL1_NS_RAM_BASE+EL1_NS_RAM_SIZE-EL1_NS_PGTBL_SIZE
#define EL1_NS_STACK_SIZE 0x40000
#define EL1_NS_STACK_BASE 0xFFFFF000
#define EL0_STACK_SIZE 0x40000
#define EL0_S_STACK_BASE 0x80000000-0x1000
#define EL0_NS_STACK_BASE 0x70000000-0x1000

#define UART0_BASE 0x09000000

/* Taken from Linux vexpress support */
#define SYSREG_BASE 0x1c010000
#define SYSREG_CFGCTRL 0xa4
#define SYS_CFGCTRL_FUNC(n) (((n) & 0x3f) << 20)
#define SYS_CFGFUNC_RESET 5
#define SYS_CFGFUNC_SHUTDOWN 8
#define SYS_CFGCTRL_START       (1 << 31)
#define SYS_CFGCTRL_WRITE       (1 << 30)
#define SYS_SHUTDOWN    (SYS_CFGCTRL_FUNC(SYS_CFGFUNC_SHUTDOWN) | \
                         SYS_CFGCTRL_START | SYS_CFGCTRL_WRITE)
#define SYS_RESET       (SYS_CFGCTRL_FUNC(SYS_CFGFUNC_SHUTDOWN) | \
                         SYS_CFGCTRL_START | SYS_CFGCTRL_WRITE)

#define STACK_SIZE 0x4000

#define TZTEST_COMMON_HEAP_BASE 0x49000000
#define TZTEST_SHARED_HEAP_BASE 0x4A000000
#endif
