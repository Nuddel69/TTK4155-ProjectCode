#ifndef INCLUDE_MENU_H
#define INCLUDE_MENU_H

#include <avr/io.h>
#include <stdint.h>
#include "io.h"

struct menu_cfg{
    struct io_oled_device *oled;
    const char **items;
    uint8_t length;
    int8_t cursor_pos;
};

int menu_init(struct menu_cfg *menu);
int menu_display(struct menu_cfg *menu);
//int menu_update(int8_t direction);

#endif // ICLUDE_MENU_H