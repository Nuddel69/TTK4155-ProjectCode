#ifndef INCLUDE_MENU_H
#define INCLUDE_MENU_H

#include <avr/io.h>
#include <stdint.h>
#include "io.c"

struct menu_cfg{
    struct io_oled_devive oled;
    const char **items:
    uint8_t length;
    int8_t = cursor_pos;
}

int menu_init(struct *menu_cfg);
int menu_display(struct *menu_cfg);
//int menu_update(int8_t direction);

#endif // ICLUDE_MENU_H