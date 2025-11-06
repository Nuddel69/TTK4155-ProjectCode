#include <avr/io.h>

#include "menu.h"
#include "utils.h"
#include "io.h"

#define BIG_FONT &OLED_FONT_8x8
#define MEDIUM_FONT &OLED_FONT_5x7
#define SMALL_FONT &OLED_FONT_4x6

// TODOS:
// - Add submenus
// -- generalize menu items to a struct as a child to a parent struct?
// -- generalize item lists and allow nested menus
// - Implement settings page options such as joystick calibration, brightness adjustment etc
// - Get rid of hardcoded values for menu item positions (rows & columns)
// - Printing high scores on high scores page

/**
 * \brief Initializes the menu struct and OLED display
 * \param[in] menu The menu configuration struct
 * \return Errno.
 */
int menu_init(struct menu_cfg *menu) {
    int status = 0;

    menu->cursor_pos = 0;
    menu->current_page = PAGE_WELCOME;

    io_oled_init(menu->oled);
    //io_oled_blink(menu->oled, 2);         // Blink once to indicate ready
    page_dispatch(menu);                    // Initialize with loading the welcome page

    return status;
}

/**
 * \brief Displays the welcome page
 * \param[in] menu The menu configuration struct
 * \return Errno.
 */
int welcome_display(struct menu_cfg *menu) {
    int status = 0;

    io_oled_clear_line(menu->oled, 0);
    io_oled_home(menu->oled);
    io_oled_print_with_font(menu->oled, MEDIUM_FONT, "Welcome");

    io_oled_clear_line(menu->oled, 1);
    io_oled_pos(menu->oled, 1, 0);
    io_oled_print_with_font(menu->oled, MEDIUM_FONT, "-------------");

    // Draws the cursor. Only redraws the cursor when the joystick is moved
    draw_cursor(menu);

    // Printing menu items
    for (uint8_t i = 0; i < menu->length; i++) {
        io_oled_clear_line(menu->oled, 2 + i);
        io_oled_pos(menu->oled, 2 + i, 10);
        io_oled_print_with_font(menu->oled, SMALL_FONT, menu->items[i]);
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

    io_oled_clear_line(menu->oled,0);
    io_oled_home(menu->oled);
    io_oled_print_with_font(menu->oled, MEDIUM_FONT, "Play game");

    return status;
}

/**
 * \brief Displays the high scores page
 * \param[in] menu The menu configuration struct
 * \return Errno.
 */
int high_scores_display(struct menu_cfg *menu) {
    int status = 0;

    io_oled_clear_line(menu->oled,0);
    io_oled_home(menu->oled);
    io_oled_print_with_font(menu->oled, MEDIUM_FONT, "High scores");

    io_oled_pos(menu->oled, 2, 0);

    return status;
}

/**
 * \brief Displays the settings page
 * \param[in] menu The menu configuration struct
 * \return Errno.
 */
int settings_display(struct menu_cfg *menu) {
    int status = 0;

    io_oled_clear_line(menu->oled,0);
    io_oled_home(menu->oled);
    io_oled_print_with_font(menu->oled, MEDIUM_FONT, "Settings");

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
            for (uint8_t i = 0; i < 6; i++) {
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

    // TODO: 
    // Implement modulus for wrapping around the cursor position
    // Implement a delay/debounce to avoid fast cursor movement if this is a problem

    if (btn->ND) { // NAV DOWN
        menu->cursor_pos++;
        
        if (menu->cursor_pos >= menu->length) {
            menu->cursor_pos = 0;
        }
    }
    else if (btn->NU) { // NAV UP
        menu->cursor_pos--;

        if (menu->cursor_pos < 0) {
            menu->cursor_pos = menu->length - 1;
        }
    }

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
    default:
        menu->current_page = PAGE_WELCOME;
        return welcome_display(menu);
    }

    return status;
}

/**
 * \brief Goes back to the welcome page if not already there based on left button from the IO board
 * \param[in] menu The menu configuration struct
 * \param[in] btn The button states struct
 * \return Errno.
 */
int page_back(struct menu_cfg *menu, struct io_avr_buttons *btn) {
    int status = 0;

    if (btn->L7) {
        if (menu->current_page != PAGE_WELCOME) {
            return set_page(menu, PAGE_WELCOME);
        }
    }
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

    // Only allow cursor movement and page selection if we're on the welcome page, if so:
    if (menu->current_page == PAGE_WELCOME) {
        cursor_update(menu, btn);
        draw_cursor(menu);
        page_select(menu, btn);
    } else {
        page_back(menu, btn);
    }

    return status;
}