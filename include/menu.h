#ifndef INCLUDE_MENU_H
#define INCLUDE_MENU_H

#include <avr/io.h>
#include <stdint.h>
#include "io.h"


/* struct page{
    const char *title;      // Title of the page, e.g., "Welcome", "High Scores"
    const char **items;     // Array of menu item strings
    uint8_t item_count;     // Number of items in the menu array
} */

enum page_id{
    PAGE_WELCOME,
    PAGE_PLAY_GAME,
    PAGE_HIGH_SCORES,
    PAGE_SETTINGS
};

struct menu_cfg{
    struct io_oled_device *oled;    // Pointer to the OLED device
    const char **items;             // Array of menu item strings
    uint8_t length;                 // Number of items in the menu
    int8_t cursor_pos;              // Current position of cursor in the menu
    enum page_id current_page;      // Current page identifier
};

int menu_init(struct menu_cfg *menu);

int page_dispatch(struct menu_cfg *menu);
int welcome_display(struct menu_cfg *menu);
int play_game_display(struct menu_cfg *menu);
int high_scores_display(struct menu_cfg *menu);
int settings_display(struct menu_cfg *menu);

int draw_cursor(struct menu_cfg *menu);
int cursor_update(struct menu_cfg *menu, struct io_avr_buttons *btn);
int page_select(struct menu_cfg *menu, struct io_avr_buttons *btn);
int set_page(struct menu_cfg *menu, enum page_id page);
int page_dispatch(struct menu_cfg *menu);
int page_back(struct menu_cfg *menu, struct io_avr_buttons *btn);
int menu_handler(struct menu_cfg *menu, struct io_avr_buttons *btn);

#endif // ICLUDE_MENU_H