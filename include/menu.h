/**
 * @file menu.h
 * @brief Hierarchical OLED menu system with navigation and page handling.
 * @defgroup MENU Menu System
 * @ingroup UI
 * @{
 */

#ifndef INCLUDE_MENU_H
#define INCLUDE_MENU_H

#include "io.h"
#include <avr/io.h>
#include <stdint.h>

/**
 * @brief Menu page identifiers
 */
enum page_id {
  PAGE_WELCOME,
  PAGE_PLAY_GAME,
  PAGE_HIGH_SCORES,
  PAGE_SETTINGS,
  PAGE_ADJUST_BRIGHTNESS,
  PAGE_CALIBRATE_JOYSTICK,
  PAGE_GAME_OVER,
};

// Represents a menu item in the menu system. Each menu item can either:
// - perform an action (start a game, show high scores, open settings etc)
// - open a submenu (chid menu) that has additional menu items
/**
 * @brief Menu item struct
 */
struct menu_item {
  const char *name;          /**< Name of menu item */
  enum page_id target_page;  /**< Target page when selected (optional) */
  struct menu_item *submenu; /**< Pointer to submenu items (optional) */
  uint8_t submenu_length;    /**< Number of items in submenu */
};

// Represents the current menu configuration and state.
// Each menu configuration struct pointes to an OLED device and contains
// information about the current menu state. The current menu state may then
// change on navigation and selection of menu items.
/**
 * @brief Menu configuration and state
 */
struct menu_cfg {
  struct io_oled_device *oled; /**< Pointer to OLED device */

  struct menu_item *items;   /**< Currently active menu items */
  uint8_t length;            /**< Number of items in current menu */
  int8_t cursor_pos;         /**< Current cursor position in the menu */
  enum page_id current_page; /**< Current page identifier */

  // Root (main) menu reference for returning back to main menu from submenus
  struct menu_item *root_items; /**< Pointer to root menu */
  uint8_t root_length;          /**< Length of root menu */

  // Parent menu reference for returning back to parent menu from submenus
  struct menu_item *parent_menu; /**< Pointer to parent menu */
  uint8_t parent_length;         /**< Length of parent menu */
};

/**
 * @brief Initializes menu and OLED display
 * @param menu Pointer to menu configuration struct
 * @return 0 on success
 */
int menu_init(struct menu_cfg *menu);

/**
 * @brief Dispatches to the current page display function
 * @param menu Pointer to menu configuration struct
 * @return 0 on success
 */
int page_dispatch(struct menu_cfg *menu);

/* Individual page display functions */
int welcome_display(struct menu_cfg *menu);
int play_game_display(struct menu_cfg *menu);
int high_scores_display(struct menu_cfg *menu);
int settings_display(struct menu_cfg *menu);
int brightness_display(struct menu_cfg *menu);
int calibrate_joystick_display(struct menu_cfg *menu);
int game_over_display(struct menu_cfg *menu);

/* Cursor functions */
int draw_cursor(struct menu_cfg *menu);
int cursor_update(struct menu_cfg *menu, struct io_avr_buttons *btn);
int page_select(struct menu_cfg *menu, struct io_avr_buttons *btn);
int set_page(struct menu_cfg *menu, enum page_id page);
int page_dispatch(struct menu_cfg *menu);
int page_back(struct menu_cfg *menu, struct io_avr_buttons *btn);

/**
 * @brief Handles menu logic (navigation, selection, page timers)
 * @param menu Pointer to menu configuration
 * @param btn Pointer to button input struct
 * @return 0 on success
 */
int menu_handler(struct menu_cfg *menu, struct io_avr_buttons *btn);

#endif // INCLUDE_MENU_H

/** @} */
