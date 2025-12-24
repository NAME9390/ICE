 

#ifndef ICE_EXC_FORMAT_H
#define ICE_EXC_FORMAT_H

#include <stdint.h>

 

 
#define EXC_MAGIC 0x00454349

 
#define EXC_VERSION_MAJOR 1
#define EXC_VERSION_MINOR 0

 
typedef enum {
    EXC_TYPE_NATIVE = 0,     
    EXC_TYPE_PYTHON,         
} exc_type_t;

 
typedef struct __attribute__((packed)) {
    uint32_t magic;              
    uint8_t  version_major;      
    uint8_t  version_minor;      
    uint8_t  type;               
    uint8_t  flags;              
    
    uint32_t exec_id;            
    uint32_t entry_offset;       
    uint32_t code_size;          
    uint32_t metadata_offset;    
    uint32_t metadata_size;      
    
    uint64_t created_time;       
    uint64_t modified_time;      
    
    char     name[20];           
} exc_header_t;

_Static_assert(sizeof(exc_header_t) == 64, "exc_header_t must be 64 bytes");

 
typedef enum {
    META_SOURCE_PATH = 1,        
    META_COMPILER,               
    META_RUNTIME,                
    META_DEPENDENCIES,           
    META_AUTHOR,                 
    META_DESCRIPTION,            
} metadata_type_t;

 
typedef struct __attribute__((packed)) {
    uint8_t  type;               
    uint16_t length;             
     
} metadata_entry_t;

 

 
int exc_validate_header(const exc_header_t *header);

 
int exc_create_from_source(const char *source_path, const char *output_path,
                           uint8_t flags, uint32_t exec_id);

 
int exc_read_header(const char *path, exc_header_t *header);

 
int exc_is_valid(const char *path);

 
const char* exc_get_name(const exc_header_t *header);

 
const char* exc_type_string(uint8_t type);

#endif  
