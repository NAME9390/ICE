 

#ifndef ICE_MPM_H
#define ICE_MPM_H

#include "../types.h"

 
typedef enum {
    PROC_STATE_OFF = 0,
    PROC_STATE_ON,
    PROC_STATE_PAUSED,
    PROC_STATE_ZOMBIE
} proc_state_t;

 
typedef struct {
    ice_pid_t pid;
    exec_id_t exec_id;
    proc_state_t state;
    u32 memory_used;
    int tty_id;
    char name[32];
} process_t;

 
typedef enum {
    MPM_OK = 0,
    MPM_ERR_INVALID_REQUEST,
    MPM_ERR_UNAUTHORIZED,
    MPM_ERR_NOT_FOUND,
    MPM_ERR_NO_MEMORY,
    MPM_ERR_INVALID_STATE,
} mpm_error_t;

 
void mpm_init(void);

 
void mpm_shell(void);

 
u32 mpm_get_uptime(void);

 
int mpm_get_process_count(void);

 
void mpm_get_memory_info(u32 *total, u32 *used, u32 *free);

#endif  
