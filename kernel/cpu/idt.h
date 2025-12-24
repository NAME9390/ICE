 

#ifndef ICE_IDT_H
#define ICE_IDT_H

#include "../types.h"

 
typedef struct __attribute__((packed)) {
    u16 offset_low;
    u16 selector;
    u8  zero;
    u8  type_attr;
    u16 offset_high;
} idt_entry_t;

 
typedef struct __attribute__((packed)) {
    u16 limit;
    u32 base;
} idt_ptr_t;

 
typedef struct {
    u32 ds;
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
    u32 int_no, err_code;
    u32 eip, cs, eflags, useresp, ss;
} interrupt_frame_t;

 
typedef void (*interrupt_handler_t)(interrupt_frame_t *frame);

 
void idt_init(void);

 
void idt_register_handler(u8 n, interrupt_handler_t handler);

#endif  
