 

#ifndef ICE_PMM_H
#define ICE_PMM_H

#include "../types.h"

 
#define PAGE_SIZE 4096

 
void pmm_init(void *mboot_info);

 
phys_addr_t pmm_alloc_page(void);

 
void pmm_free_page(phys_addr_t addr);

 
u32 pmm_get_total_memory(void);
u32 pmm_get_free_memory(void);

#endif  
