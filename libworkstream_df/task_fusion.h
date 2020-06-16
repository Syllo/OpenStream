#ifndef TASK_FUSION_H__
#define TASK_FUSION_H__

#if WSTREAM_FUSE_TASKS

#include "hashmap.h"

#define num_default_fuse_task 3

struct wstream_fused_macro_task_loop {
  unsigned max_tasks_to_fuse;
  unsigned num_tasks_fused;
  void **task_frames;
};

typedef void (*workfn_ptr_type)(void *);
HASHMAP_FUNCS_DECLARE(fused_tl, workfn_ptr_type, struct wstream_fused_macro_task_loop)

struct wstream_df_frame *frame_pointer_for_fused_task(workfn_ptr_type work);

struct wstream_fused_macro_task_loop *
wstream_fused_macro_task_loop_location_in_frame(void *frame_ptr);

void exec_macro_task_loop(void *fused);

#endif // WSTREAM_FUSE_TASKS
#endif // TASK_FUSION_H__