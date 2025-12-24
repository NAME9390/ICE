 

#ifndef ICE_CONSOLE_H
#define ICE_CONSOLE_H

#include "../types.h"

 
#define NUM_CONSOLES 4

 
typedef struct {
    u16 buffer[80 * 25];     
    int cursor_x;
    int cursor_y;
    u8 color;
    bool active;
} console_t;

 
void console_init(void);

 
void console_switch(int num);

 
int console_get_current(void);

 
void console_write(int num, const char *s);

 
void console_clear(int num);

 
void console_handle_hotkey(int fkey);

#endif  
