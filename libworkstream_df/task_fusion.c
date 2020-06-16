#include <stdalign.h>

#include "alloc.h"
#include "config.h"
#include "task_fusion.h"
#include "wstream_df.h"

#ifdef WSTREAM_FUSE_TASKS

struct wstream_fused_macro_task_loop *
wstream_fused_macro_task_loop_location_in_frame(void *frame_ptr) {
  char *fp = (char *)frame_ptr;
  return align_up(fp + sizeof(wstream_df_frame_t), alignof(struct wstream_fused_macro_task_loop));
}

struct wstream_df_frame *frame_pointer_for_fused_task(workfn_ptr_type work) {
  void *new_frame = __builtin_ia32_tcreate(0,
                                           sizeof(wstream_df_frame_t) +
                                               alignof(struct wstream_fused_macro_task_loop) +
                                               sizeof(struct wstream_fused_macro_task_loop),
                                           work, false);
  wstream_df_frame_p fused_task_frame = (wstream_df_frame_p)new_frame;
  fused_task_frame->work_fn = work;
  return fused_task_frame;
}

void exec_macro_task_loop(void *frame) {
  struct wstream_fused_macro_task_loop *fused =
      wstream_fused_macro_task_loop_location_in_frame(frame);
  for (unsigned i = 0; i < fused->num_tasks_fused; ++i) {
    wstream_df_frame_p fp = (wstream_df_frame_p)fused->task_frames[i];
    exec_work_frame(fp);
  }
}

HASHMAP_FUNCS_CREATE(fused_tl, workfn_ptr_type, struct wstream_fused_macro_task_loop)

#endif // def WSTREAM_FUSE_TASKS