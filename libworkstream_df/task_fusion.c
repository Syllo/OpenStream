#include <stdalign.h>

#include "config.h"
#include "task_fusion.h"
#include "wstream_df.h"
#include "alloc.h"

#ifdef WSTREAM_FUSE_TASKS

struct wstream_fused_macro_task_loop *
wstream_fused_macro_task_loop_location_in_frame(void *frame_ptr) {
  char *fp = (char *)frame_ptr;
  return align_up(fp + sizeof(wstream_df_frame_t),
                  alignof(struct wstream_fused_macro_task_loop));
}

void exec_macro_task_loop(void *frame) {
  struct wstream_fused_macro_task_loop *fused = wstream_fused_macro_task_loop_location_in_frame(frame);
  for (unsigned i = 0; i < fused->num_tasks_fused; ++i) {
    wstream_df_frame_p fp = (wstream_df_frame_p)fused->task_frames[i];
    exec_work_frame(fp);
  }
}

#endif // def WSTREAM_FUSE_TASKS