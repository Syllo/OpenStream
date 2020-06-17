#include <stdalign.h>

#include "alloc.h"
#include "config.h"
#include "task_fusion.h"
#include "wstream_df.h"

#ifdef WSTREAM_FUSE_TASKS

extern __thread wstream_df_thread_p current_thread;

static void exec_macro_task_loop(void *task_frame_ptr) {
  struct wstream_fused_macro_task_loop *fused =
      wstream_fused_macro_task_loop_location_in_frame(task_frame_ptr);
  fprintf(stderr, "Executing fused task with %u fused tasks\n", fused->num_tasks_fused);
  for (unsigned i = 0; i < fused->num_tasks_fused; ++i) {
    wstream_df_frame_p fp = (wstream_df_frame_p)fused->task_frames[i];
    exec_work_frame(fp);
  }
  wstream_df_frame_p frame = (wstream_df_frame_p)task_frame_ptr;
  slab_free(current_thread->slab_cache, fused->task_frames);
  __built_in_wstream_df_dec_frame_ref(frame, 1);
}

struct wstream_df_frame *alloc_frame_for_fused_task() {
  // fprintf(stderr, "Allocating fused frame\n");
  void *new_frame = __builtin_ia32_tcreate(0,
                                           sizeof(wstream_df_frame_t) +
                                               alignof(struct wstream_fused_macro_task_loop) +
                                               sizeof(struct wstream_fused_macro_task_loop),
                                           exec_macro_task_loop, false);
  return new_frame;
}

struct wstream_fused_macro_task_loop *
wstream_fused_macro_task_loop_location_in_frame(void *frame_ptr) {
  char *fp = (char *)frame_ptr;
  return align_up(fp + sizeof(wstream_df_frame_t), alignof(struct wstream_fused_macro_task_loop));
}

struct wstream_task_type_fuse_info *alloc_task_type_fuse_info(workfn_ptr_type task_work_fn) {
  struct wstream_task_type_fuse_info *new_fuse_info =
      slab_alloc(current_thread, current_thread->slab_cache, sizeof(*new_fuse_info));
  new_fuse_info->task_work_fn = task_work_fn;
  new_fuse_info->best_amount_to_fuse = num_default_fuse_task;
  new_fuse_info->encountered_times = 0;
  new_fuse_info->fused_frames.max_tasks_to_fuse = num_default_fuse_task;
  new_fuse_info->fused_frames.num_tasks_fused = 0;
  new_fuse_info->fused_frames.task_frames = alloc_task_frame_array(num_default_fuse_task);
  return new_fuse_info;
}

struct wstream_df_frame **alloc_task_frame_array(unsigned size_array) {
  return slab_alloc(current_thread, current_thread->slab_cache,
                    size_array * sizeof(struct wstream_df_frame *));
}

HASHMAP_FUNCS_CREATE(fused_tl, workfn_ptr_type, struct wstream_task_type_fuse_info)

#endif // def WSTREAM_FUSE_TASKS