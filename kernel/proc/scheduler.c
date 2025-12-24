 

#include "scheduler.h"
#include "../drivers/vga.h"
#include "../mm/pmm.h"

 
static pcb_t process_table[MAX_PROCESSES];
static ice_pid_t next_pid = 1;
static int current_process = -1;
static int process_count = 0;

 
#define DEFAULT_TIMESLICE 10

 
static void strncpy_s(char *dest, const char *src, int n) {
    int i;
    for (i = 0; i < n - 1 && src[i]; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
}

void scheduler_init(void) {
     
    for (int i = 0; i < MAX_PROCESSES; i++) {
        process_table[i].state = PROC_STATE_FREE;
        process_table[i].pid = 0;
    }
    
    next_pid = 1;
    current_process = -1;
    process_count = 0;
}

ice_pid_t scheduler_create_process(const char *name, u32 entry_point) {
     
    int slot = -1;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].state == PROC_STATE_FREE) {
            slot = i;
            break;
        }
    }
    
    if (slot < 0) {
        return 0;   
    }
    
    pcb_t *proc = &process_table[slot];
    
     
    proc->pid = next_pid++;
    proc->exec_id = proc->pid;   
    proc->state = PROC_STATE_READY;
    strncpy_s(proc->name, name, sizeof(proc->name));
    
     
    proc->kernel_stack = pmm_alloc_page();
    if (!proc->kernel_stack) {
        return 0;   
    }
    
     
    proc->context.eip = entry_point;
    proc->context.esp = proc->kernel_stack + PAGE_SIZE - 16;
    proc->context.eflags = 0x202;   
    proc->context.cr3 = 0;   
    
    proc->memory_used = PAGE_SIZE;
    proc->tty_id = 0;
    proc->timeslice = DEFAULT_TIMESLICE;
    proc->ticks_remaining = DEFAULT_TIMESLICE;
    
    process_count++;
    
    return proc->pid;
}

void scheduler_kill_process(ice_pid_t pid) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].pid == pid && process_table[i].state != PROC_STATE_FREE) {
             
            if (process_table[i].kernel_stack) {
                pmm_free_page(process_table[i].kernel_stack);
            }
            
            process_table[i].state = PROC_STATE_FREE;
            process_table[i].pid = 0;
            process_count--;
            
             
            if (i == current_process) {
                current_process = -1;
            }
            
            return;
        }
    }
}

void scheduler_tick(void) {
    if (current_process < 0) return;
    
    pcb_t *proc = &process_table[current_process];
    if (proc->state == PROC_STATE_RUNNING) {
        proc->ticks_remaining--;
        
        if (proc->ticks_remaining == 0) {
             
            proc->state = PROC_STATE_READY;
            proc->ticks_remaining = proc->timeslice;
            scheduler_yield();
        }
    }
}

void scheduler_yield(void) {
    if (process_count == 0) return;
    
     
    int start = (current_process + 1) % MAX_PROCESSES;
    int i = start;
    
    do {
        if (process_table[i].state == PROC_STATE_READY) {
             
            if (current_process >= 0 && 
                process_table[current_process].state == PROC_STATE_RUNNING) {
                process_table[current_process].state = PROC_STATE_READY;
            }
            
            current_process = i;
            process_table[i].state = PROC_STATE_RUNNING;
            return;
        }
        i = (i + 1) % MAX_PROCESSES;
    } while (i != start);
}

pcb_t* scheduler_get_current(void) {
    if (current_process < 0) return 0;
    return &process_table[current_process];
}

pcb_t* scheduler_get_process(ice_pid_t pid) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].pid == pid && process_table[i].state != PROC_STATE_FREE) {
            return &process_table[i];
        }
    }
    return 0;
}

int scheduler_get_process_count(void) {
    return process_count;
}

void scheduler_list_processes(void (*callback)(pcb_t *proc)) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].state != PROC_STATE_FREE) {
            callback(&process_table[i]);
        }
    }
}
