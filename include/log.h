#ifndef INCLUDE_INCLUDE_LOG_H_
#define INCLUDE_INCLUDE_LOG_H_

#include "uart.h"
#include "utils.h"

#define LOG_LEVEL_NONE 0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_INFO 2

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_INFO
#endif

#if LOG_LEVEL > LOG_LEVEL_NONE
#define LOG_MODULE_DEFINE(module_name)                                         \
  static char *_log_module_name = module_name;
#else
#define LOG_MODULE_DEFINE(module_name) ;
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
#define LOG_INF(inf_string)                                                    \
  USART_SendString("INF: ");                                                   \
  USART_SendString(_log_module_name);                                          \
  USART_SendString(": ");                                                      \
  USART_SendString(inf_string);                                                \
  USART_endl();
#else
#define LOG_INF(inf_string) (void(0))
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
#define LOG_ERR(err_string, status)                                            \
  USART_SendString("ERR: ");                                                   \
  USART_SendString(_log_module_name);                                          \
  USART_SendString(": ");                                                      \
  USART_SendString(err_string);                                                \
  USART_endl();                                                                \
  STATUS_ASSERT(status)
#else
#define LOG_ERR(err_string, status) (void(0))
#endif

#endif // INCLUDE_INCLUDE_LOG_H_
