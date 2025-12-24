 

#ifndef ICE_APPS_H
#define ICE_APPS_H

#include "../types.h"

 
typedef int (*app_main_t)(int argc, char **argv);

 
typedef struct {
    const char *name;
    const char *description;
    app_main_t main;
    bool requires_admin;
} builtin_app_t;

 
void apps_init(void);

 
int apps_run(const char *name, int argc, char **argv);

 
builtin_app_t* apps_find(const char *name);

 
void apps_list(void);

 
int app_cat(int argc, char **argv);
int app_echo(int argc, char **argv);
int app_iced(int argc, char **argv);
int app_ls(int argc, char **argv);
int app_mkdir(int argc, char **argv);
int app_rm(int argc, char **argv);
int app_pwd(int argc, char **argv);
int app_whoami(int argc, char **argv);
int app_users(int argc, char **argv);
int app_adduser(int argc, char **argv);
int app_passwd(int argc, char **argv);
int app_reboot(int argc, char **argv);
int app_shutdown(int argc, char **argv);
int app_date(int argc, char **argv);
int app_hexdump(int argc, char **argv);
int app_ip(int argc, char **argv);
int app_ping(int argc, char **argv);
int app_touch(int argc, char **argv);
int app_mkdir(int argc, char **argv);
int app_head(int argc, char **argv);
int app_tail(int argc, char **argv);
int app_wc(int argc, char **argv);
int app_env(int argc, char **argv);

#endif  
