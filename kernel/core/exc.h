 

#ifndef ICE_EXC_H
#define ICE_EXC_H

#include "../types.h"

 
#define EXC_MAGIC 0x43584549

 
#define EXC_VERSION 1

 
#define EXC_FLAG_NONE   0x00
#define EXC_FLAG_KRNL   0x01     
#define EXC_FLAG_HIDDEN 0x02     

 
typedef enum {
    EXC_TYPE_NATIVE = 0,         
    EXC_TYPE_SCRIPT,             
} exc_type_t;

 
typedef struct __attribute__((packed)) {
    u32 magic;                   
    u8  version;                 
    u8  type;                    
    u8  flags;                   
    u8  reserved;
    
    u32 exec_id;                 
    u32 entry_point;             
    u32 code_size;               
    u32 data_size;               
    u32 bss_size;                
    u32 stack_size;              
    
    char name[32];               
} exc_header_t;

 
typedef struct {
    exec_id_t id;
    char path[64];
    char name[32];
    exc_type_t type;
    u8 flags;
    bool valid;
    u32 entry_point;
    u32 load_addr;
} exc_entry_t;

 
#define MAX_EXECUTABLES 256

 
void exc_init(void);

 
exec_id_t exc_register(const char *path, const char *name, u8 flags);

 
exc_entry_t* exc_find(exec_id_t id);

 
int exc_get_count(void);

 
void exc_list(void (*callback)(exc_entry_t *entry));

 
u32 exc_load(exec_id_t id);

#endif  
