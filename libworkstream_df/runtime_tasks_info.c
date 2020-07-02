#include "runtime_tasks_info.h"
#include "alloc.h"
#include "config.h"
#include "wstream_df.h"

extern __thread wstream_df_thread_p current_thread;

#define next_in_window(N) ((N + 1) % TASK_INFO_SLIDING_WIN_SIZE)

void init_task_info_for_ptr(work_fn_p ptr, struct task_type_info *tti) {
  tti->task_ptr = ptr;
  tti->instances_executed = 0;
  tti->sliding_win_end = 0;
  tti->sliding_win_start = 0;
  tti->sum_exec_time = 0.f;
}

void task_info_add_exec_time(struct task_type_info *info, double time) {
  unsigned next_to_end = next_in_window(info->sliding_win_end);
  if (likely(next_to_end == info->sliding_win_start)) {
    info->sum_exec_time -=
        info->task_runtime_sliding_window[info->sliding_win_start];
    info->task_runtime_sliding_window[info->sliding_win_start] = time;
    info->sliding_win_start = next_in_window(info->sliding_win_start);
  } else {
    info->task_runtime_sliding_window[info->sliding_win_end] = time;
  }
  info->sliding_win_end = next_to_end;
  info->sum_exec_time += time;
}

extern inline unsigned
amount_in_slidling_window(const struct task_type_info *const info);

extern inline float task_info_avg_exec_time(const struct task_type_info *const info);

#if TASK_INFO_USE_HASHTABLE

map_tasktype_to_info_p alloc_tti_map() { return kh_init_mapTaskTypeToInfo(); }

#include <assert.h>
#include <tgmath.h>
#undef kcalloc
#define kcalloc(N, Z)                                                          \
  memset(slab_alloc(current_thread, current_thread->slab_cache, N *Z), 0, N *Z)
#undef kmalloc
#define kmalloc(Z) slab_alloc(current_thread, current_thread->slab_cache, Z)
#undef kfree
#define kfree(P) slab_free(current_thread->slab_cache, P)
#undef krealloc
#define krealloc(P, N) slab_realloc(current_thread->slab_cache, P, N)

// Assume 64 bit for now
static_assert(sizeof(void *) == 8);
#define hash_pointer_func(ptr)                                                 \
  kh_int64_hash_func((uint64_t)ptr >> (size_t)(log2(sizeof(void *))))
#define hash_pointer_equal(a, b) ((a) == (b))
__KHASH_IMPL(mapTaskTypeToInfo, , void *, struct task_type_info, 1,
             hash_pointer_func, hash_pointer_equal)

struct task_type_info *create_maping_for_function(map_tasktype_to_info_p map,
                                                  work_fn_p fn) {
  int retVal;
  khint_t index = kh_put_mapTaskTypeToInfo(map, fn, &retVal);
  assert(retVal != 0 && retVal != -1); // Do not insert twice the same key
  struct task_type_info *tti = &kh_value(map, index);
  init_task_info_for_ptr(fn, tti);
  return tti;
}

#else

#define map_size_increment 16

map_tasktype_to_info_p alloc_tti_map() {
  map_tasktype_to_info_p map =
      slab_alloc(NULL, current_thread->slab_cache, sizeof(*map));
  map->amount_in_map = 0;
  map->map_array_size = 0;
  map->task_type_array = NULL;
  return map;
}

struct task_type_info *create_maping_for_function(map_tasktype_to_info_p map,
                                                  work_fn_p fn) {
  if (map->amount_in_map == map->map_array_size) {
    map->task_type_array =
        slab_realloc(current_thread->slab_cache, map->task_type_array,
                     (map->map_array_size + map_size_increment) *
                         sizeof(*map->task_type_array));
    map->map_array_size += map_size_increment;
  }
  init_task_info_for_ptr(fn, &map->task_type_array[map->amount_in_map]);
  map->amount_in_map++;
  return &map->task_type_array[map->amount_in_map];
}

#undef increment_size_increase

#endif // TASK_INFO_USE_HASHTABLE
