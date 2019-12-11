#ifndef CONFIG_H
#define CONFIG_H

 /*********************** OpenStream Runtime Configuration ***********************/

/*
 * The stack size of the workers.
 */

#define WSTREAM_STACK_SIZE 1 << 16

/*
 * The log2 of the initial size of the work queue, i.e., each worker starts with a 2^(WSTREAM_DF_DEQUE_LOG_SIZE) empty queue.
 */

#define WSTREAM_DF_DEQUE_LOG_SIZE 8

/*
 * When a worker did not successfully steal some work from any other workers,
 * the worker will relinquish the CPU and the thread is placed at the end of
 * the scheduler queue.
 * Activating this option will activate active polling for new task without stealing the 
 */

// #define WS_NO_YIELD_SPIN

 /*********************** OpenStream Debug Options ***********************/

/*
 * Make the slab allocator more verbose by printing wasted memory and numa
 * mapping warnings.
 * This may have a performance impact.
 */

// #define SLAB_ALLOCATOR_VERBOSE

/*
 * Make HWLOC print information about the hardware and worker placement. This
 * has no performance impact on the program other than the initialisation in
 * the pre_main function.
 */

#define HWLOC_VERBOSE


 /*********************** OpenStream Profiling Options ***********************/

/*
 * Profile the work queues.
 */
#define WQUEUE_PROFILE 1

/*
 * MATRIX_PROFILE profiles the amount of information exchanged between the the
 * worker threads through the streams. The information is dumped inside the
 * specified file in a matrix form (line worker id to column worker id).
 * REQUIRES WQUEUE_PROFILE
 */

// #define MATRIX_PROFILE "wqueue_matrix.out"

/*
 * Use linux getrusage function to gather resource usage for running threads.
 * This includes:
 *   - User CPU time
 *   - Major page faults (page to be allocated to the process)
 *   - Minor page faults (page already in memory but not mapped into the process)
 *   - The maximum resident set size (peak RAM usage of the process)
 *   - The number of involuntary context switches (e.g. kernel scheduler intervention)
 */

// #define PROFILE_RUSAGE

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

#ifndef IN_GCC
#include <string.h>

#ifdef WS_PAPI_PROFILE
#define mem_cache_misses(th) ((th)->papi_counters[MEM_CACHE_MISS_POS])
#else // !defined(WS_PAPI_PROFILE)
#define mem_cache_misses(th) 0
#endif // !defined(WS_PAPI_PROFILE)

#endif /* IN_GCC */

/*
 * Some configuration checks
 */
#if defined(UNIFORM_MEMORY_ACCESS) && MAX_NUMA_NODES != 1
#error "UNIFORM_MEMORY_ACCESS defined, but MAX_NUMA_NODES != 1"
#endif

#if defined(MATRIX_PROFILE) && !WQUEUE_PROFILE
#error "MATRIX_PROFILE defined, but WQUEUE_PROFILE != 1"
#endif // defined(MATRIX_PROFILE) && !defined(WQUEUE_PROFILE)

#endif