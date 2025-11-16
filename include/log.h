#ifndef INCLUDE_INCLUDE_LOG_H_
#define INCLUDE_INCLUDE_LOG_H_

#include "uart.h"
#include "utils.h"

#define LOG_LEVEL_NONE 0
#define LOG_LEVEL_CRITICAL 1
#define LOG_LEVEL_ERROR 2
#define LOG_LEVEL_INFO 3

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_INFO
#endif

static const char log_prefix_critical[] PROGMEM = "CRITICAL: ";
static const char log_prefix_info[] PROGMEM = "INF: ";
static const char log_prefix_error[] PROGMEM = "ERR: ";
static const char log_colon_space[] PROGMEM = ": ";
static const char log_newline[] PROGMEM = "\r\n";

// UART transmit from progmem
static inline void _log_print_P(PGM_P p) {
  char c;
  while ((c = pgm_read_byte(p++)) != 0) {
    USART_Transmit(c);
  }
}

#if LOG_LEVEL > LOG_LEVEL_NONE
#define LOG_MODULE_DEFINE(name)                                                \
  static const char _log_module_name[] PROGMEM = name
#else
#define LOG_MODULE_DEFINE(name)
#endif

#if LOG_LEVEL >= LOG_LEVEL_CRITICAL
#define LOG_CRITICAL(msg)                                                      \
  do {                                                                         \
    _log_print_P(log_prefix_critical);                                         \
    _log_print_P(_log_module_name);                                            \
    _log_print_P(log_colon_space);                                             \
    USART_SendString(msg);                                                     \
    _log_print_P(log_newline);                                                 \
  } while (0)
#else
#define LOG_CRITICAL(msg) ((void)0)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
#define LOG_INF(msg)                                                           \
  do {                                                                         \
    _log_print_P(log_prefix_info);                                             \
    _log_print_P(_log_module_name);                                            \
    _log_print_P(log_colon_space);                                             \
    USART_SendString(msg);                                                     \
    _log_print_P(log_newline);                                                 \
  } while (0)
#else
#define LOG_INF(msg) ((void)0)
#endif

#if LOG_LEVEL >= LOG_LEVEL_ERROR
#define LOG_ERR(msg, status)                                                   \
  do {                                                                         \
    _log_print_P(log_prefix_error);                                            \
    _log_print_P(_log_module_name);                                            \
    _log_print_P(log_colon_space);                                             \
    USART_SendString(msg);                                                     \
    _log_print_P(log_newline);                                                 \
    STATUS_ASSERT(status);                                                     \
  } while (0)
#else
#define LOG_ERR(msg, status) ((void)0)
#endif

#endif // INCLUDE_LOG_H_
