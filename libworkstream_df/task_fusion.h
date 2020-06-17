#ifndef TASK_FUSION_H__
#define TASK_FUSION_H__

#if WSTREAM_FUSE_TASKS

#include "hashmap.h"

#define num_default_fuse_task 3

typedef void (*workfn_ptr_type)(void *);

struct wstream_fused_macro_task_loop {
  unsigned max_tasks_to_fuse;
  unsigned num_tasks_fused;
  struct wstream_df_frame **task_frames;
};

struct wstream_task_type_fuse_info {
  workfn_ptr_type task_work_fn;
  uintmax_t encountered_times;
  unsigned best_amount_to_fuse;
  struct wstream_fused_macro_task_loop fused_frames;
};

struct wstream_df_frame *alloc_frame_for_fused_task(void);

struct wstream_fused_macro_task_loop *
wstream_fused_macro_task_loop_location_in_frame(void *frame_ptr);

struct wstream_task_type_fuse_info *alloc_task_type_fuse_info(workfn_ptr_type task_work_fn);

struct wstream_df_frame **alloc_task_frame_array(unsigned size_array);

HASHMAP_FUNCS_DECLARE(fused_tl, workfn_ptr_type, struct wstream_task_type_fuse_info)

#endif // WSTREAM_FUSE_TASKS
#endif // TASK_FUSION_H__