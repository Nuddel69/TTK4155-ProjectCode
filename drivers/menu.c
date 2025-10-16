#include <avr/io.h>

#include "menu.h"
#include "io.h"

static const char *menu_items["Start game", "Settings"]

int menu_init(struct menu_cfg *menu){
    int status = 0;

    io_oled_init(menu->oled);
    menu_display();
    
    return status;
}

int menu_display(struct menu_cfg *menu, const struct oled_font *font){
    int status = 0;

    char welcome_txt = "Welcome to the Ping Pong Game";

    io_oled_home(menu->oled);
    io_oled_print_with_font(menu->oled, font, welcome_txt);

    io_oled_pos(1,0);
    io_oled_print_with_font(menu->oled, font, "-------------")

    for (uint8_t i = 0; i < menu->length; i++) {
        io_oled_pos(menu->oled, 2 + i, 0);
        if (i == menu->cursor_pos) {
            io_oled_print_with_font(menu->oled, font, "> ");
        } else {
            oled_print(menu->oled, font, "  ")
        }
        io_oled_print_with_font(menu->oled, font, menu->items[i]);
    }
    
    return status;
}

int menu_update(int8_t direction){
    int status = 0;
    
    return status;
}