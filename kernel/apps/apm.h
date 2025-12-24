 

#ifndef ICE_APM_H
#define ICE_APM_H

#include "../types.h"

 
#define APM_MAGIC 0x4D504149

 
#define APM_VERSION 1

 
typedef enum {
    LANG_UNKNOWN = 0,
    LANG_C,
    LANG_CPP,
    LANG_PYTHON,
    LANG_ASM_X86,
    LANG_ASM_X64,
    LANG_RUST,
    LANG_HTML,
    LANG_CSS,
    LANG_JS,
    LANG_GOLANG,
    LANG_MIXED,          
    LANG_EXC,            
} apm_lang_t;

 
typedef struct __attribute__((packed)) {
    u32 magic;               
    u8  version;             
    u8  lang;                
    u8  flags;               
    u8  reserved;
    
    u32 exec_id;             
    u32 entry_offset;        
    u32 code_size;           
    u32 data_size;           
    
    char name[32];           
    char author[32];         
    char desc[40];           
    
    u32 checksum;            
} apm_header_t;

 
#define APM_FLAG_COMPRESSED  0x01
#define APM_FLAG_SIGNED      0x02
#define APM_FLAG_NATIVE      0x04
#define APM_FLAG_SCRIPT      0x08

 
typedef struct {
    u32 id;
    char name[32];
    char path[64];
    apm_lang_t lang;
    bool installed;
    u32 size;
} apm_entry_t;

 
#define MAX_PACKAGES 128

 
void apm_init(void);

 
int apm_install(const char *path);

 
int apm_setup(const char *source_path);

 
int apm_run(const char *name, int argc, char **argv);

 
void apm_list(void);

 
int apm_remove(const char *name);

 
apm_entry_t* apm_get(const char *name);

 
apm_lang_t apm_detect_lang(const char *filename);

 
const char* apm_lang_name(apm_lang_t lang);

 
int apm_compile(const char *source, const char *output);

 
int app_apm(int argc, char **argv);

#endif  
