#include <stdalign.h>

#include "alloc.h"
#include "config.h"
#include "khash.h"
#include "numa.h"
#include "task_fusion.h"
#include "wstream_df.h"

#ifdef WSTREAM_FUSE_TASKS

extern __thread wstream_df_thread_p current_thread;

static void exec_macro_task_loop(void *task_frame_ptr) {
  struct wstream_fused_macro_task_loop *fused =
      wstream_fused_macro_task_loop_location_in_frame(task_frame_ptr);
  fprintf(stderr, "Executing fused task with %u fused tasks\n",
          fused->num_tasks_fused);
  for (unsigned i = 0; i < fused->num_tasks_fused; ++i) {
    wstream_df_frame_p fp = (wstream_df_frame_p)fused->task_frames[i];
    exec_work_frame(fp);
  }
  wstream_df_frame_p frame = (wstream_df_frame_p)task_frame_ptr;
  unsigned id = slab_allocator_of(fused->task_frames);
  wstream_df_numa_node_p node = numa_node_by_id(id);
  slab_free(&node->slab_cache, fused->task_frames);
  id = slab_allocator_of(frame);
  node = numa_node_by_id(id);
  slab_free(&node->slab_cache, frame);
}

struct wstream_df_frame *alloc_frame_for_fused_task() {
  // fprintf(stderr, "Allocating fused frame\n");
  wstream_df_frame_p new_frame =
      slab_alloc(current_thread, current_thread->slab_cache,
                 sizeof(wstream_df_frame_t) +
                     alignof(struct wstream_fused_macro_task_loop) +
                     sizeof(struct wstream_fused_macro_task_loop));
  memset(new_frame, 0, sizeof(wstream_df_frame_t));
  new_frame->work_fn = exec_macro_task_loop;

  return new_frame;
}

struct wstream_fused_macro_task_loop *
wstream_fused_macro_task_loop_location_in_frame(void *frame_ptr) {
  char *fp = (char *)frame_ptr;
  return align_up(fp + sizeof(wstream_df_frame_t),
                  alignof(struct wstream_fused_macro_task_loop));
}

struct wstream_df_frame **alloc_task_frame_array(unsigned size_array) {
  return slab_alloc(current_thread, current_thread->slab_cache,
                    size_array * sizeof(struct wstream_df_frame *));
}

void init_task_fuse_info_fields(struct task_fuse_info *fuse_info) {
#if WSTREAM_FUSE_MACRO_TASK_LOOP
  fuse_info->best_amount_to_fuse = num_default_fuse_task;
  fuse_info->fused_frames.max_tasks_to_fuse = 0;
  fuse_info->fused_frames.num_tasks_fused = 0;
  fuse_info->fused_frames.task_frames = NULL;
#endif
}

#endif // def WSTREAM_FUSE_TASKS