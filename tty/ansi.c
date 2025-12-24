 

#include <stdio.h>
#include "tty.h"

 

 
void ansi_bold(void) {
    printf("\033[1m");
}

void ansi_dim(void) {
    printf("\033[2m");
}

void ansi_italic(void) {
    printf("\033[3m");
}

void ansi_underline(void) {
    printf("\033[4m");
}

void ansi_blink(void) {
    printf("\033[5m");
}

void ansi_reverse(void) {
    printf("\033[7m");
}

void ansi_hidden(void) {
    printf("\033[8m");
}

void ansi_strikethrough(void) {
    printf("\033[9m");
}

void ansi_reset(void) {
    printf("\033[0m");
}

 

void ansi_draw_box(int row, int col, int width, int height) {
     
    printf("\033[%d;%dH┌", row, col);
    
     
    for (int i = 1; i < width - 1; i++) {
        printf("─");
    }
    printf("┐");
    
     
    for (int i = 1; i < height - 1; i++) {
        printf("\033[%d;%dH│", row + i, col);
        printf("\033[%d;%dH│", row + i, col + width - 1);
    }
    
     
    printf("\033[%d;%dH└", row + height - 1, col);
    
     
    for (int i = 1; i < width - 1; i++) {
        printf("─");
    }
    printf("┘");
    
    fflush(stdout);
}

 

void ansi_progress_bar(int row, int col, int width, int percent) {
    printf("\033[%d;%dH[", row, col);
    
    int filled = (width - 2) * percent / 100;
    for (int i = 0; i < width - 2; i++) {
        if (i < filled) {
            printf("█");
        } else {
            printf("░");
        }
    }
    
    printf("] %3d%%", percent);
    fflush(stdout);
}
