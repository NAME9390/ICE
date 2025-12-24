 

#include "gdt.h"

 
 
static gdt_entry_t gdt_entries[6] = {
    {0, 0, 0, 0, 0, 0},       
    {0xFFFF, 0, 0, 0x9A, 0xCF, 0},  
    {0xFFFF, 0, 0, 0x92, 0xCF, 0},  
    {0xFFFF, 0, 0, 0xFA, 0xCF, 0},  
    {0xFFFF, 0, 0, 0xF2, 0xCF, 0},  
    {0, 0, 0, 0, 0, 0}        
};
static gdt_ptr_t   gdt_ptr = {0, 0};
static tss_t       tss_entry = {0};

 
static void gdt_set_gate(int num, u32 base, u32 limit, u8 access, u8 gran) {
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    gdt_entries[num].granularity |= (gran & 0xF0);
    gdt_entries[num].access      = access;
}

void gdt_init(void) {
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 6) - 1;
    gdt_ptr.base  = (u32)&gdt_entries;

     
    gdt_set_gate(0, 0, 0, 0, 0);

     
     
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

     
     
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

     
     
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);

     
     
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

     
     
    for (u32 i = 0; i < sizeof(tss_entry); i++) {
        ((u8*)&tss_entry)[i] = 0;
    }
    tss_entry.ss0 = 0x10;   
    tss_entry.esp0 = 0;
    
     
    u32 tss_base = (u32)&tss_entry;
    u32 tss_limit = sizeof(tss_entry);
    
     
    gdt_set_gate(5, tss_base, tss_limit, 0x89, 0x00);

     
    gdt_flush((u32)&gdt_ptr);
    
     
    tss_flush();
}

void gdt_set_kernel_stack(u32 stack) {
    tss_entry.esp0 = stack;
}
