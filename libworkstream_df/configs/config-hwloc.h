#ifndef CONFIG_H
#define CONFIG_H

#include "../arch.h"
#include <assert.h>
#include "../hwloc-support.h"

//#define _WSTREAM_DF_DEBUG 1
//#define _PRINT_STATS
//#define _WS_NO_YIELD_SPIN
#define HWLOC_VERBOSE

#define WSTREAM_DF_DEQUE_LOG_SIZE 8
#define WSTREAM_STACK_SIZE 1 << 16

#define WQUEUE_PROFILE 0
//#define MATRIX_PROFILE "wqueue_matrix.out"

#define PUSH_MIN_MEM_LEVEL 1
#define PUSH_MIN_FRAME_SIZE (64 * 1024)
#define PUSH_MIN_REL_FRAME_SIZE 1.3
#define NUM_PUSH_SLOTS 0
#define ALLOW_PUSHES (NUM_PUSH_SLOTS > 0)

#define NUM_PUSH_REORDER_SLOTS 0
#define ALLOW_PUSH_REORDER (ALLOW_PUSHES && NUM_PUSH_REORDER_SLOTS > 0)

#define DEFERRED_ALLOC

#define CACHE_LAST_STEAL_VICTIM 0

#define MAX_WQEVENT_SAMPLES 0
#define TRACE_RT_INIT_STATE
/* #define TRACE_DATA_READS */
#define ALLOW_WQEVENT_SAMPLING (MAX_WQEVENT_SAMPLES > 0)
#define MAX_WQEVENT_PARAVER_CYCLES -1
#define NUM_WQEVENT_TASKHIST_BINS 1000

#define WQEVENT_SAMPLING_OUTFILE "events.ost"
#define WQEVENT_SAMPLING_PARFILE "parallelism.gpdata"
#define WQEVENT_SAMPLING_TASKHISTFILE "task_histogram.gpdata"
#define WQEVENT_SAMPLING_TASKLENGTHFILE "task_length.gpdata"

//#define USE_BROADCAST_TABLES

//#define WS_PAPI_PROFILE
//#define WS_PAPI_MULTIPLEX

//#ifdef WS_PAPI_PROFILE
//#define WS_PAPI_NUM_EVENTS 2
//#define WS_PAPI_EVENTS { "PAPI_L1_DCM", "PAPI_L2_DCM" }
//#define MEM_CACHE_MISS_POS 0 /* Use L1_DCM as cache miss indicator */

// /* #define TRACE_PAPI_COUNTERS */
//#endif

/* Description of the memory hierarchy */
extern unsigned levels_in_machine_hierarchy;

#ifndef IN_GCC
#include <string.h>

#ifdef WS_PAPI_PROFILE
#define mem_cache_misses(th) ((th)->papi_counters[MEM_CACHE_MISS_POS])
#else
#define mem_cache_misses(th) 0
#endif

#endif /* IN_GCC */

#endif