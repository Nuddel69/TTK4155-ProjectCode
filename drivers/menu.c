#include <avr/io.h>

#include "menu.h"
#include "utils.h"
#include "io.h"

#define BIG_FONT &OLED_FONT_8x8
#define MEDIUM_FONT &OLED_FONT_5x7
#define SMALL_FONT &OLED_FONT_4x6

/**
 * \brief Initializes the menu struct and OLED display
 * \param[in] menu The menu configuration struct
 * \return Errno.
 */
int menu_init(struct menu_cfg *menu) {
    int status = 0;

    io_oled_init(menu->oled);
    page_dispatch(menu);

    return status;
}

/**
 * \brief Displays the welcome page
 * \param[in] menu The menu configuration struct
 * \return Errno.
 */
int welcome_display(struct menu_cfg *menu) {
    int status = 0;

    io_oled_clear_all(menu->oled);

    io_oled_home(menu->oled);
    io_oled_print_with_font(menu->oled, MEDIUM_FONT, "Welcome");

    io_oled_pos(menu->oled, 1, 0);
    //io_oled_print_with_font(menu->oled, MEDIUM_FONT, "-------------");
    io_oled_print_with_font(menu->oled, MEDIUM_FONT, "---------");

    draw_cursor(menu);

    // Printing menu items
    for (uint8_t i = 0; i < menu->length; i++) {
        io_oled_clear_line(menu->oled, 2 + i);
        io_oled_pos(menu->oled, 2 + i, 10);
        io_oled_print_with_font(menu->oled, SMALL_FONT, menu->items[i].name);
    }
    
    return status;
}

/**
 * \brief Displays the play game page
 * \param[in] menu The menu configuration struct
 * \return Errno.
 */
int play_game_display(struct menu_cfg *menu) {
    int status = 0;

    io_oled_clear_all(menu->oled);

    io_oled_home(menu->oled);
    io_oled_print_with_font(menu->oled, MEDIUM_FONT, "Game in progress..");

    return status;
}

/**
 * \brief Displays the high scores page
 * \param[in] menu The menu configuration struct
 * \return Errno.
 */
int high_scores_display(struct menu_cfg *menu) {
    int status = 0;

    io_oled_clear_all(menu->oled);

    io_oled_home(menu->oled);
    io_oled_print_with_font(menu->oled, MEDIUM_FONT, "High scores");

    return status;
}

/**
 * \brief Displays the settings page
 * \param[in] menu The menu configuration struct
 * \return Errno.
 */
int settings_display(struct menu_cfg *menu) {
    int status = 0;

    io_oled_clear_all(menu->oled);

    io_oled_home(menu->oled);
    io_oled_print_with_font(menu->oled, MEDIUM_FONT, "Settings");

    io_oled_pos(menu->oled, 1, 0);
    io_oled_print_with_font(menu->oled, MEDIUM_FONT, "-------------");

    draw_cursor(menu);

    // Printing menu items
    for (uint8_t i = 0; i < menu->length; i++) {
        io_oled_clear_line(menu->oled, 2 + i);
        io_oled_pos(menu->oled, 2 + i, 10);
        io_oled_print_with_font(menu->oled, SMALL_FONT, menu->items[i].name);
    }

    return status;
}

int brightness_display(struct menu_cfg *menu){
    int status = 0;

    io_oled_clear_all(menu->oled);

    io_oled_home(menu->oled);
    io_oled_print_with_font(menu->oled, SMALL_FONT, "Adjusting brightness..");

    return status;
}

int calibrate_joystick_display(struct menu_cfg *menu) {
    int status = 0;

    io_oled_clear_all(menu->oled);

    io_oled_home(menu->oled);
    io_oled_print_with_font(menu->oled, SMALL_FONT, "Calibrating joystick..");

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
            // Clearing the cursor position
            for (uint8_t j = 0; j < 6; j++) {
                io_oled_write_data(menu->oled, 0x00);
            }
        }
    }

    return status;
}

/**
 * \brief Updates the cursor position based on nav button from the IO board. Wraps around if going out of bounds.
 * \param[in] menu The menu configuration struct
 * \param[in] btn The button states struct
 * \return Errno.
 */
int cursor_update(struct menu_cfg *menu, struct io_avr_buttons *btn) {
    int status = 0;
    
    static uint8_t prev_ND = 0;
    static uint8_t prev_NU = 0;

    // TODO: 
    // Implement modulus for wrapping around the cursor position

    // --- NAV DOWN ---
    if (btn->ND && !prev_ND) {
        menu->cursor_pos++;
        
        if (menu->cursor_pos >= menu->length) {
            menu->cursor_pos = 0;
        }
    }
    // --- NAV UP ---
    else if (btn->NU && !prev_NU) {
        menu->cursor_pos--;

        if (menu->cursor_pos < 0) {
            menu->cursor_pos = menu->length - 1;
        }
    }

    prev_ND = btn->ND;
    prev_NU = btn->NU;

    return 0;
}

/**
 * \brief Selects the current menu item based on nav button from the IO board
 * \param[in] menu The menu configuration struct
 * \param[in] btn The button states struct
 * \return Errno.
 */
int page_select(struct menu_cfg *menu, struct io_avr_buttons *btn) {
    int status = 0;
    static uint8_t prev_NR = 0;
    
    if(btn->NR && !prev_NR) {
        // Get currently selected menu item
        // - Q: why do I need an ampersand here?
        struct menu_item *selected_item = &menu->items[menu->cursor_pos];

        // Save current menu as parent before moving anywhere
        menu->parent_menu = menu->items;
        menu->parent_length = menu->length;

        // --- Case 1: The selected item has a submenu ---
        if(selected_item->submenu != NULL) {
            // Set submenu info
            menu->items = selected_item->submenu;
            menu->length = selected_item->submenu_length;
            menu->cursor_pos = 0;

            // Go to the requested page
            return set_page(menu, selected_item->target_page);
        }

        // --- Case 2: Leaf page ---
        else {
            return set_page(menu, selected_item->target_page);
        }
    }

    prev_NR = btn->NR;

    return status;
}

/**
 * \brief Sets the current page field in the menu struct to a specified page and calls the page_dispatch function
 * \param[in] menu The menu configuration struct
 * \param[in] page The page to set
 * \return Errno.
 */
int set_page(struct menu_cfg *menu, enum page_id page) {
    int status = 0;

    // Checking if the requested page is the current page. If so, do nothing.
    if (menu->current_page == page) {
        return status;
    }

    menu->current_page = page;
    page_dispatch(menu);
    
    return status;
}

/**
 * \brief Dispatches to the appropriate page display function based on the current page field in the menu struct
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
    case PAGE_ADJUST_BRIGHTNESS:
        return brightness_display(menu);
    case PAGE_CALIBRATE_JOYSTICK:
        return calibrate_joystick_display(menu);
    default:
        menu->current_page = PAGE_WELCOME;
        return welcome_display(menu);
    }

    return status;
}

int page_back(struct menu_cfg *menu, struct io_avr_buttons *btn) {
    int status = 0;
    static uint8_t prev_NL = 0;

    if(btn->NL && !prev_NL) {

        // If we have a parent menu, go back to that
        if(menu->parent_menu != NULL) {
            // Restore parent
            menu->items = menu->parent_menu;
            menu->length = menu->parent_length;
            menu->cursor_pos = 0;

            // If we just went back to the root, clear parent pointers
            if(menu->items == menu->root_items) {
                menu->parent_menu = NULL;
                menu->parent_length = 0;
                return(set_page(menu, PAGE_WELCOME));
            }

            // Otherwise, go to parents page (e.g. Brightness -> Settings)
            menu->parent_menu = menu->root_items;
            menu->parent_length = menu->root_length;
            return(set_page(menu, PAGE_SETTINGS));
        }
    }

    prev_NL = btn->NL;

    return status;
}

/**
 * \brief Handles menu logic based on current page and button inputs from the IO board
 * \param[in] menu The menu configuration struct
 * \param[in] btn The button states struct
 * \return Errno.
 */
int menu_handler(struct menu_cfg *menu, struct io_avr_buttons *btn) {
    int status = 0;

    // Only allow cursor movement and page selection if we're on welcome page or settings page
    if (menu->current_page == PAGE_WELCOME || menu->current_page == PAGE_SETTINGS) {
        cursor_update(menu, btn);
        draw_cursor(menu);
        page_select(menu, btn);
    }

    page_back(menu, btn);

    return status;
}