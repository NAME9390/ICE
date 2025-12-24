



BITS 32


MULTIBOOT_MAGIC         equ 0x1BADB002
MULTIBOOT_ALIGN         equ 1<<0            
MULTIBOOT_MEMINFO       equ 1<<1            
MULTIBOOT_FLAGS         equ MULTIBOOT_ALIGN | MULTIBOOT_MEMINFO
MULTIBOOT_CHECKSUM      equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)


KERNEL_STACK_SIZE       equ 16384

section .multiboot
align 4
multiboot_header:
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd MULTIBOOT_CHECKSUM

section .bss
align 16
stack_bottom:
    resb KERNEL_STACK_SIZE
stack_top:

section .text
global _start
extern kernel_main

_start:
    
    mov dword [0xB8000], 0x2F4B2F4F

    
    cli

    
    mov esp, stack_top

    
    push 0
    popf

    
    push ebx
    
    push eax

    
    call kernel_main

    
.halt:
    cli
    hlt
    jmp .halt

global gdt_flush
gdt_flush:
    mov eax, [esp+4]    
    lgdt [eax]          

    mov ax, 0x10        
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    jmp 0x08:.flush     
.flush:
    ret

global tss_flush
tss_flush:
    mov ax, 0x28        
    ltr ax              
    ret
