 

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#include "tty.h"

 

static struct {
    int initialized;
    int color_scheme;
    int current_fg;
    int current_bg;
} tty_state = {0};

 
static const int color_schemes[5][2] = {
    {TTY_WHITE, TTY_BLACK},      
    {TTY_WHITE, TTY_BLACK},      
    {TTY_GREEN, TTY_BLACK},      
    {TTY_BLACK, TTY_WHITE},      
    {TTY_WHITE, TTY_BLACK},      
};

 

int tty_init(void) {
    if (tty_state.initialized) return 0;
    
    tty_state.color_scheme = TTY_COLOR_DEFAULT;
    tty_state.current_fg = color_schemes[1][0];
    tty_state.current_bg = color_schemes[1][1];
    tty_state.initialized = 1;
    
     
    tty_reset_color();
    
    return 0;
}

void tty_shutdown(void) {
    if (!tty_state.initialized) return;
    
    tty_reset_color();
    tty_state.initialized = 0;
}

 

int tty_set_color_scheme(int scheme) {
    if (scheme < 1 || scheme > 4) return -1;
    
    tty_state.color_scheme = scheme;
    tty_state.current_fg = color_schemes[scheme][0];
    tty_state.current_bg = color_schemes[scheme][1];
    
     
    printf("\033[%d;%dm", 30 + tty_state.current_fg, 40 + tty_state.current_bg);
    fflush(stdout);
    
    return 0;
}

int tty_get_color_scheme(void) {
    return tty_state.color_scheme;
}

void tty_reset_color(void) {
    printf("\033[0m");
    fflush(stdout);
}

 

void tty_clear(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
}

void tty_set_cursor(int row, int col) {
    printf("\033[%d;%dH", row, col);
    fflush(stdout);
}

void tty_cursor_visible(int visible) {
    if (visible) {
        printf("\033[?25h");
    } else {
        printf("\033[?25l");
    }
    fflush(stdout);
}

 

int tty_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    int result = vprintf(format, args);
    va_end(args);
    fflush(stdout);
    return result;
}

int tty_printf_color(int fg, int bg, const char *format, ...) {
     
    printf("\033[%d;%dm", 30 + fg, 40 + bg);
    
     
    va_list args;
    va_start(args, format);
    int result = vprintf(format, args);
    va_end(args);
    
     
    printf("\033[%d;%dm", 30 + tty_state.current_fg, 40 + tty_state.current_bg);
    fflush(stdout);
    
    return result;
}
