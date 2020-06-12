#include "config.h"
#include "task_fusion.h"
#include "wstream_df.h"

#ifdef WSTREAM_FUSE_TASKS

void exec_macro_task_loop(struct wstream_fused_macro_task_loop *fused) {
    for (unsigned i = 0; i < fused->num_tasks_fused; ++i) {
        wstream_df_frame_p fp = (wstream_df_frame_p) fused->task_frames[i];
        exec_work_frame(fp);
    }
}

#endif // def WSTREAM_FUSE_TASKS