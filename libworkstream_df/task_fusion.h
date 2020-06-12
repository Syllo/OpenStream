#ifndef TASK_FUSION_H__
#define TASK_FUSION_H__

struct wstream_fused_macro_task_loop {
  unsigned max_tasks_to_fuse;
  unsigned num_tasks_fused;
  void **task_frames;
};

void exec_macro_task_loop(struct wstream_fused_macro_task_loop *fused);

#endif // TASK_FUSION_H__