#include <avr/io.h>

#include "menu.h"
#include "utils.h"
#include "io.h"

#define BIG_FONT &OLED_FONT_8x8
#define MEDIUM_FONT &OLED_FONT_5x7
#define SMALL_FONT &OLED_FONT_4x6

// TODOS:
// - Get rid of hardcoded values for menu item positions (rows & columns)
// - Add submenus
// -- generalize menu items to a struct as a child to a parent struct?
// --- generalize item lists and allow nested menus

int menu_init(struct menu_cfg *menu) {
    int status = 0;

    //menu->length = 2;         // Set dynamically in main instead
    menu->cursor_pos = 0;
    menu->current_page = PAGE_WELCOME;

    io_oled_init(menu->oled);
    //welcome_display(menu);    // Moved to page_dispatch
    page_dispatch(menu);        // Initialize with loading the welcome page
    
    return status;
}

int welcome_display(struct menu_cfg *menu) {
    int status = 0;

    io_oled_home(menu->oled);
    io_oled_print_with_font(menu->oled, MEDIUM_FONT, "Welcome");

    io_oled_pos(menu->oled, 1, 0);
    io_oled_print_with_font(menu->oled, MEDIUM_FONT, "-------------");

    for (uint8_t i = 0; i < menu->length; i++) {
        io_oled_pos(menu->oled, 2 + i, 2);
        io_oled_print_with_font(menu->oled, SMALL_FONT, menu->items[i]);
    }

    draw_cursor(menu); // Only redraw cursor when the joystick is moved
    
    return status;
}

int play_game_display(struct menu_cfg *menu) {
    int status = 0;

    io_oled_home(menu->oled);
    io_oled_print_with_font(menu->oled, MEDIUM_FONT, "Play game");

    return status;
}

int high_scores_display(struct menu_cfg *menu) {
    int status = 0;

    io_oled_home(menu->oled);
    io_oled_print_with_font(menu->oled, MEDIUM_FONT, "High scores");

    io_oled_pos(menu->oled, 2, 0);
    // TODO: for-loop for printing saved high scores

    return status;
}

int settings_display(struct menu_cfg *menu) {
    int status = 0;

    io_oled_home(menu->oled);
    io_oled_print_with_font(menu->oled, MEDIUM_FONT, "Settings");

    //TODO: add settings options such as joystick calibration, brightness

    return status;
}

/**
 * \brief Draws the cursor (">") at the current cursor position in the menu
 * \param[in] menu The menu configuration struct
 * \return Errno.
 */
int draw_cursor(struct menu_cfg *menu) {
    int status = 0;

    for (uint8_t i = 0; i < menu->length; i++) {
        io_oled_pos(menu->oled, 2 + i, 0);
        if (i == menu->cursor_pos) {
            io_oled_print_with_font(menu->oled, SMALL_FONT, "> ");
        } else {
            io_oled_print_with_font(menu->oled, SMALL_FONT, "  ");
        }
    }

    return status;
}

// Conteplated to call io_joystick_read_position() inside the function instead of passing direction as parameter,
// but I think this will be a better seperation(?) - seperating hardware (input readaing) from logic (cursor updating).
// Should 'direction' be a pointer? I did not do it as it is only used for reading inside the function.
int cursor_update(struct menu_cfg *menu, enum io_joystick_direction direction) {
    int status = 0;

    if (direction == DOWN) {
        menu->cursor_pos++;
        
        if (menu->cursor_pos >= menu->length) {
            menu->cursor_pos = 0;
        }
    }
    else if (direction == UP) {
        menu->cursor_pos--;

        if (menu->cursor_pos < 0) {
            menu->cursor_pos = menu->length - 1;
        }
    }

    return 0;
}

/**
 * \brief Selects the current menu item based on joystick button press
 * \param[in] menu The menu configuration struct
 * \param[in] btn The button states struct
 * \return Errno.
 */
int page_select(struct menu_cfg *menu, struct io_avr_buttons *btn) {
    int status = 0;

    if (btn->NB) {
        switch (menu->cursor_pos) {
        case 0:
            return set_page(menu, PAGE_PLAY_GAME);
        case 1:
            return set_page(menu, PAGE_HIGH_SCORES);
        case 2:
            return set_page(menu, PAGE_SETTINGS);
        default:
            return status;
        }
    }

    return status;
}

/**
 * \brief Sets the current page to the specified page and updates the display
 * \param[in] menu The menu configuration struct
 * \param[in] page The page to set
 * \return Errno.
 */
int set_page(struct menu_cfg *menu, enum page_id page) {
    int status = 0;

    // Checking if the requested page is the current page
    if (menu->current_page == page) {
        return status;
    }

    menu->current_page = page;
    page_dispatch(menu);
    
    return status;
}

/**
 * \brief Dispatches to the appropriate page display function based on the current page
 * \param[in] menu The menu configuration struct
 * \return Errno.
 */
int page_dispatch(struct menu_cfg *menu) {
    int status = 0;

    switch (menu->current_page) {
    case PAGE_WELCOME:
        return welcome_display(menu);
    case PAGE_PLAY_GAME:
        return play_game_display(menu);
    case PAGE_HIGH_SCORES:
        return high_scores_display(menu);
    case PAGE_SETTINGS:
        return settings_display(menu);
    default:
        menu->current_page = PAGE_WELCOME;
        return welcome_display(menu);
    }

    return status;
}

int page_back(struct menu_cfg *menu, struct io_avr_buttons *btn) {
    int status = 0;

    // Not sure if I can use 'left' or I need to use L1, L2, etc.
    if (btn->L7) {
        if (menu->current_page != PAGE_WELCOME) {
            return set_page(menu, PAGE_WELCOME);
        }
    }
    return status;
}