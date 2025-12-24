 

#ifndef ICE_KEYBOARD_H
#define ICE_KEYBOARD_H

#include "../types.h"

 
#define KB_DATA_PORT    0x60
#define KB_STATUS_PORT  0x64
#define KB_COMMAND_PORT 0x64

 
#define KEY_ESCAPE      0x01
#define KEY_BACKSPACE   0x0E
#define KEY_TAB         0x0F
#define KEY_ENTER       0x1C
#define KEY_LCTRL       0x1D
#define KEY_LSHIFT      0x2A
#define KEY_RSHIFT      0x36
#define KEY_LALT        0x38
#define KEY_CAPS        0x3A
#define KEY_F1          0x3B
#define KEY_F2          0x3C
#define KEY_F3          0x3D
#define KEY_F4          0x3E
#define KEY_UP          0x48
#define KEY_LEFT        0x4B
#define KEY_RIGHT       0x4D
#define KEY_DOWN        0x50

 
void keyboard_init(void);

 
char keyboard_getc(void);

 
bool keyboard_available(void);

 
char keyboard_read(void);

 
int keyboard_getline(char *buffer, int max_len);

#endif  
