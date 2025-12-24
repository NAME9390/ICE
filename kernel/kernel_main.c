#include "fs/ext2.h"
 

#include "core/mpm.h"
#include "drivers/vga.h"
#include "cpu/gdt.h"
#include "cpu/idt.h"
#include "drivers/pic.h"
#include "drivers/pit.h"
#include "drivers/keyboard.h"
#include "mm/pmm.h"
#include "tty/tty.h"

 
#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002

 
void kernel_main(uint32_t magic, void *mboot_info) {
     
    vga_init();
    vga_puts("VGA Initialized.\n");
    
     
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        vga_puts("ERROR: Not booted by Multiboot loader!\n");
        
    }
    
     
    vga_puts("[BOOT] Loading GDT... ");
    gdt_init();
    vga_puts("OK\n");
    
     
    vga_puts("[BOOT] Loading IDT... ");
    idt_init();
    vga_puts("OK\n");
    
     
    vga_puts("[BOOT] Initializing PIC... ");
    pic_init();
    vga_puts("OK\n");
    
     
    vga_puts("[BOOT] Initializing memory... ");
    pmm_init(mboot_info);
    vga_puts("OK\n");
    
     
    vga_puts("[BOOT] Initializing timer... ");
    pit_init(100);   
    vga_puts("OK\n");
    
     
    vga_puts("[BOOT] Initializing keyboard... ");
    keyboard_init();
    vga_puts("OK\n");
    
     
    vga_puts("[BOOT] Initializing TTY... ");
    tty_init();
    vga_puts("OK\n");

     
    vga_puts("[BOOT] Mounting Filesystem (EXT2)... ");
    if (ext2_init() == 0) {
        vga_puts("OK\n");
    } else {
        vga_puts("FAILED (Check disk)\n");
    }
    
     
    
     
    vga_puts("[BOOT] Enabling interrupts... ");
    __asm__ volatile ("sti");
    vga_puts("OK\n");
    
    vga_puts("\n");
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_puts("MPM Kernel ready.\n");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_puts("Type 'help' for available commands.\n\n");
    
     
    mpm_init();
    mpm_shell();
    
     
    for (;;) {
        __asm__ volatile ("hlt");
    }
}
