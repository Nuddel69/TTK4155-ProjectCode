#include <avr/io.h>

#include "menu.h"
#include "utils.h"
#include "io.h"

#define BIG_FONT &OLED_FONT_8x8
#define MEDIUM_FONT &OLED_FONT_5x7

int menu_init(struct menu_cfg *menu){
    int status = 0;

    menu->length = 2;
    menu->cursor_pos = 0;

    io_oled_init(menu->oled);
    menu_display(menu);
    
    return status;
}

int menu_display(struct menu_cfg *menu){
    int status = 0;

    char welcome_txt[] = "Welcome";

    io_oled_home(menu->oled);
    io_oled_print_with_font(menu->oled, BIG_FONT, welcome_txt);

    io_oled_pos(menu->oled, 1, 0);
    io_oled_print_with_font(menu->oled, MEDIUM_FONT, "-------------");

    for (uint8_t i = 0; i < menu->length; i++) {
        io_oled_pos(menu->oled, 2 + i, 0);
        if (i == menu->cursor_pos) {
            io_oled_print_with_font(menu->oled, MEDIUM_FONT, "> ");
        } else {
            io_oled_print_with_font(menu->oled, MEDIUM_FONT, "  ");
        }
        io_oled_print_with_font(menu->oled, MEDIUM_FONT, menu->items[i]);
    }
    
    return status;
}

int menu_update(int8_t direction){
    int status = 0;
    
    return status;
}