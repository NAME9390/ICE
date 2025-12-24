 

#ifndef ICE_MPM_H
#define ICE_MPM_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <time.h>

#include "../../EXC/format/exc.h"

 

 
typedef uint32_t exec_id_t;
#define EXEC_ID_INVALID 0
#define EXEC_ID_FORMAT "#%08X"

 
typedef uint32_t ice_pid_t;
#define ICE_PID_INVALID 0

 

typedef enum {
    PROC_STATE_OFF = 0,      
    PROC_STATE_ON,           
    PROC_STATE_PAUSED,       
    PROC_STATE_ZOMBIE        
} proc_state_t;

 

#define EXC_FLAG_NONE   0x00
#define EXC_FLAG_KRNL   0x01     
#define EXC_FLAG_HIDDEN 0x02     

 

 

typedef enum {
     
    API_PROCESS_LIST = 100,
    API_PROCESS_KILL,
    API_PROCESS_RESTART,
    API_PROCESS_INFO,
    
     
    API_EXEC_RUN = 200,
    API_EXEC_REGISTER,
    
     
    API_MEMORY_ALLOC = 300,
    API_MEMORY_FREE,
    API_MEMORY_INFO,
    
     
    API_TTY_BIND = 400,
    API_TTY_UNBIND,
    API_TTY_WRITE,
    API_TTY_READ,
    API_TTY_COLOR,
    
     
    API_FS_READ = 500,
    API_FS_WRITE,
    API_FS_LIST,
    API_FS_EXISTS,
} api_request_type_t;

 

typedef enum {
    CALLER_KERNEL = 0,       
    CALLER_PM,               
    CALLER_GPM,              
} caller_type_t;

 

 
#define MAX_PATH_LEN 256
#define MAX_PROCESSES 64
#define MAX_EXECUTABLES 1024
#define MAX_ERROR_MSG 128

 
typedef struct {
    api_request_type_t type;
    caller_type_t caller;
    union {
         
        struct {
            exec_id_t exec_id;
        } process;
        
         
        struct {
            exec_id_t exec_id;
            char path[MAX_PATH_LEN];
            uint8_t flags;
        } exec;
        
         
        struct {
            ice_pid_t pid;
            size_t size;
        } memory;
        
         
        struct {
            ice_pid_t pid;
            int tty_id;
            int color_scheme;
            char buffer[1024];
        } tty;
        
         
        struct {
            char path[MAX_PATH_LEN];
        } fs;
    } params;
} mpm_request_t;

 
typedef enum {
    MPM_OK = 0,
    MPM_ERR_INVALID_REQUEST,
    MPM_ERR_UNAUTHORIZED,
    MPM_ERR_NOT_FOUND,
    MPM_ERR_ALREADY_EXISTS,
    MPM_ERR_NO_MEMORY,
    MPM_ERR_INVALID_STATE,
    MPM_ERR_IO_ERROR,
    MPM_ERR_INVALID_FORMAT,
    MPM_ERR_REGISTRY_FULL,
} mpm_error_t;

 
typedef struct {
    ice_pid_t pid;
    exec_id_t exec_id;
    proc_state_t state;
    size_t memory_used;
    int tty_id;
} process_info_t;

 
typedef struct {
    mpm_error_t error;
    char error_msg[MAX_ERROR_MSG];
    union {
         
        struct {
            int count;
            process_info_t processes[MAX_PROCESSES];
        } process_list;
        
         
        process_info_t process_info;
        
         
        struct {
            exec_id_t exec_id;
            ice_pid_t pid;
        } exec;
        
         
        struct {
            size_t total;
            size_t used;
            size_t free;
        } memory_info;
        
         
        bool success;
    } data;
} mpm_response_t;

 

typedef struct {
    exec_id_t id;
    char path[MAX_PATH_LEN];
    char name[64];
    exc_type_t type;
    uint8_t flags;
    bool valid;
} registry_entry_t;

 

 
int mpm_init(void);

 
void mpm_shutdown(void);

 
mpm_response_t mpm_process_request(const mpm_request_t *request);

 
bool mpm_authorize(api_request_type_t type, caller_type_t caller);

 
int mpm_validate_system(void);

 
const char* mpm_error_string(mpm_error_t error);

 
time_t mpm_get_uptime(void);

 
int mpm_get_color_scheme(void);

 
size_t mpm_get_registry_count(void);

 
const registry_entry_t* mpm_get_registry(void);

#endif  
