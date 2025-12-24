 

#ifndef ICE_GDT_H
#define ICE_GDT_H

#include "../types.h"

 
typedef struct __attribute__((packed)) {
    u16 limit_low;
    u16 base_low;
    u8  base_middle;
    u8  access;
    u8  granularity;
    u8  base_high;
} gdt_entry_t;

 
typedef struct __attribute__((packed)) {
    u16 limit;
    u32 base;
} gdt_ptr_t;

 
typedef struct __attribute__((packed)) {
    u32 prev_tss;
    u32 esp0;
    u32 ss0;
    u32 esp1;
    u32 ss1;
    u32 esp2;
    u32 ss2;
    u32 cr3;
    u32 eip;
    u32 eflags;
    u32 eax;
    u32 ecx;
    u32 edx;
    u32 ebx;
    u32 esp;
    u32 ebp;
    u32 esi;
    u32 edi;
    u32 es;
    u32 cs;
    u32 ss;
    u32 ds;
    u32 fs;
    u32 gs;
    u32 ldt;
    u16 trap;
    u16 iomap_base;
} tss_t;

 
#define GDT_KERNEL_CODE 0x08
#define GDT_KERNEL_DATA 0x10
#define GDT_USER_CODE   0x18
#define GDT_USER_DATA   0x20
#define GDT_TSS         0x28

 
void gdt_init(void);

 
void gdt_set_kernel_stack(u32 stack);

 
extern void gdt_flush(u32 gdt_ptr);

 
extern void tss_flush(void);

#endif  
