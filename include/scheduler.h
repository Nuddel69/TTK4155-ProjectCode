#ifndef INCLUDE_INCLUDE_SCHEDULER_H_
#define INCLUDE_INCLUDE_SCHEDULER_H_

#include <stdint.h>

struct task {
  uint32_t task_period;
  uint32_t elapsed_time;
  int (*taskfn)(void);
};

struct scheduler_cfg {
  uint8_t task_num;
  uint32_t largest_task_interval;
};

#endif // INCLUDE_INCLUDE_SCHEDULER_H_
