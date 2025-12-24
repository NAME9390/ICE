 

#ifndef ICE_USER_H
#define ICE_USER_H

#include "../types.h"

 
typedef enum {
    USER_TYPE_PU = 0,        
    USER_TYPE_UPU,           
} user_type_t;

 
typedef u32 uid_t;
#define UID_INVALID 0
#define UID_ROOT    1        

 
#define MAX_USERS       16
#define MAX_USERNAME    16
#define MAX_PASSWORD    32

 
typedef struct {
    uid_t uid;
    char username[MAX_USERNAME];
    char password[MAX_PASSWORD];   
    user_type_t type;
    bool active;
    bool logged_in;
} user_t;

 
void user_init(void);

 
uid_t user_create(const char *username, const char *password, user_type_t type);

 
uid_t user_login(const char *username, const char *password);

 
void user_logout(void);

 
user_t* user_get_current(void);

 
user_t* user_get(uid_t uid);

 
bool user_is_admin(void);

 
void user_list(void (*callback)(user_t *user));

 
int user_delete(uid_t uid);

 
int user_change_password(uid_t uid, const char *old_pw, const char *new_pw);

#endif  
