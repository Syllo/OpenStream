#ifndef RUNTIME_TASKS_INFO_H
#define RUNTIME_TASKS_INFO_H

#include "config.h"
#include "task_fusion.h"

#if TASK_INFO_USE_HASHTABLE
#include "khash.h"
#endif

#include <assert.h>
#include <stdint.h>


#define likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)

typedef void (*work_fn_p)(void *);

struct task_type_info {
  work_fn_p task_ptr;
  uintmax_t instances_executed;
  unsigned sliding_win_start, sliding_win_end;
  double task_runtime_sliding_window[TASK_INFO_SLIDING_WIN_SIZE];
  float sum_exec_time;
  WSTREAM_FUSE_TASKS_FIELDS
};

void init_task_info_for_ptr(work_fn_p ptr, struct task_type_info *tti);

void task_info_add_exec_time(struct task_type_info *info, double time);

inline unsigned
amount_in_slidling_window(const struct task_type_info *const info) {
  if (info->sliding_win_start > info->sliding_win_end) {
    return info->sliding_win_end + TASK_INFO_SLIDING_WIN_SIZE -
           info->sliding_win_start;
  } else {
    return info->sliding_win_end - info->sliding_win_start;
  }
}

inline float task_info_avg_exec_time(const struct task_type_info *const info) {
  unsigned amount = amount_in_slidling_window(info);
  assert(amount != 0);
  return info->sum_exec_time / amount;
}

#if TASK_INFO_USE_HASHTABLE

KHASH_DECLARE(mapTaskTypeToInfo, void *, struct task_type_info)

typedef khash_t(mapTaskTypeToInfo) * map_tasktype_to_info_p;

inline struct task_type_info *get_tti_from_work_fn(map_tasktype_to_info_p map,
                                                   work_fn_p fn) {
  khint_t index = kh_get_mapTaskTypeToInfo(map, fn);
  if (index == kh_end(map))
    return NULL;
  else {
    return &kh_value(map, index);
  }
}

#else
typedef struct {
  struct task_type_info *task_type_array;
  size_t amount_in_map;
  size_t map_array_size;
} map_tasktype_to_info_s, *map_tasktype_to_info_p; // tti

inline struct task_type_info *get_tti_from_work_fn(map_tasktype_to_info_p map,
                                                   work_fn_p fn) {
  for (size_t i = 0; i < map->amount_in_map; ++i) {
    if (map->task_type_array[i].task_ptr == fn)
      return &map->task_type_array[i];
  }
  return NULL;
}

#endif // TASK_INFO_USE_HASHTABLE

map_tasktype_to_info_p alloc_tti_map();

struct task_type_info *create_maping_for_function(map_tasktype_to_info_p map,
                                                  work_fn_p fn);

void free_tti_map(map_tasktype_to_info_p map);

#endif // RUNTIME_TASKS_INFO_H