 

#ifndef ICE_TTY_H
#define ICE_TTY_H

 
#define TTY_COLOR_DEFAULT   1   
#define TTY_COLOR_DARK      2   
#define TTY_COLOR_LIGHT     3   
#define TTY_COLOR_MONO      4   

 
int tty_init(void);

 
void tty_shutdown(void);

 
int tty_set_color_scheme(int scheme);

 
int tty_get_color_scheme(void);

 
void tty_clear(void);

 
int tty_printf(const char *format, ...) __attribute__((format(printf, 1, 2)));

 
int tty_printf_color(int fg, int bg, const char *format, ...) 
    __attribute__((format(printf, 3, 4)));

 
#define TTY_BLACK   0
#define TTY_RED     1
#define TTY_GREEN   2
#define TTY_YELLOW  3
#define TTY_BLUE    4
#define TTY_MAGENTA 5
#define TTY_CYAN    6
#define TTY_WHITE   7

 
void tty_reset_color(void);

 
void tty_set_cursor(int row, int col);

 
void tty_cursor_visible(int visible);

#endif  
