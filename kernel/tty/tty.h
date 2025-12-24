 

#ifndef ICE_TTY_H
#define ICE_TTY_H

#include "../types.h"

 
#define TTY_SCHEME_DEFAULT  1
#define TTY_SCHEME_DARK     2
#define TTY_SCHEME_LIGHT    3
#define TTY_SCHEME_MONO     4

 
void tty_init(void);

 
void tty_set_color_scheme(int scheme);

 
int tty_get_color_scheme(void);

 
void tty_puts(const char *s);

 
void tty_printf(const char *format, ...);

 
int tty_getline(char *buffer, int max_len);

 
void tty_clear(void);

 
void tty_set_prompt(const char *prompt);

 
void tty_print_prompt(void);

#endif  
