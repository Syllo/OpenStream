#ifndef TASK_FUSION_H__
#define TASK_FUSION_H__

struct wstream_fused_macro_task_loop {
  unsigned max_tasks_to_fuse;
  unsigned num_tasks_fused;
  struct wstream_df_frame **task_frames;
};

#if WSTREAM_FUSE_MACRO_TASK_LOOP

#define num_default_fuse_task 5
#define FUSE_INFO_MACRO_TASK_FIELDS                                            \
  unsigned best_amount_to_fuse;                                                \
  struct wstream_fused_macro_task_loop fused_frames;

#else // !WSTREAM_FUSE_MACRO_TASK_LOOP

#define FUSE_INFO_MACRO_TASK_FIELDS

#endif // WSTREAM_FUSE_MACRO_TASK_LOOP

struct task_fuse_info {
  FUSE_INFO_MACRO_TASK_FIELDS
};

struct wstream_df_frame *alloc_frame_for_fused_task(void);

struct wstream_fused_macro_task_loop *
wstream_fused_macro_task_loop_location_in_frame(void *frame_ptr);

struct wstream_df_frame **alloc_task_frame_array(unsigned size_array);

void init_task_fuse_info_fields(struct task_fuse_info *fuse_info);

#if WSTREAM_FUSE_TASKS
#define WSTREAM_FUSE_TASKS_FIELDS struct task_fuse_info fuse_info;
#define WSTREAM_FUSE_TASKS_INIT_FIELDS(ptr) init_task_fuse_info_fields(ptr);
#else
#define TASK_FUSION_RUNTIME_FIELDS
#define WSTREAM_FUSE_TASKS_INIT_FIELDS(struct_prt)
#endif

#endif // TASK_FUSION_H__