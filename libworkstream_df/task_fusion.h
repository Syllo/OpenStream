#ifndef TASK_FUSION_H__
#define TASK_FUSION_H__

#define num_default_fuse_task 3

struct wstream_fused_macro_task_loop {
  unsigned max_tasks_to_fuse;
  unsigned num_tasks_fused;
  void **task_frames;
};

struct wstream_fused_macro_task_loop *
wstream_fused_macro_task_loop_location_in_frame(void *frame_ptr);

void exec_macro_task_loop(void *fused);

#endif // TASK_FUSION_H__