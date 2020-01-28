include ../Makefile.config

TESTS = lbm \
				lbm-omp \
				lbm-tiled

CFLAGS =  -Wall -g -std=gnu11 -Og  -ffast-math $(PROFILER_CFLAGS)
LDFLAGS = -L$(LIB_DIR) -lm -lrt # Do not link all test cases with Wstream_Df as it launches worker threads even in seq. comp. for now.

all: $(TESTS)

lbm: d2q9.c main.c
	$(PLAIN_GCC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LD_RPATH_FLAGS) -fno-openmp

lbm-tiled: d2q9-tiled.c main.c
	$(PLAIN_GCC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LD_RPATH_FLAGS) -fno-openmp

lbm-omp: d2q9.c main.c
	$(PLAIN_GCC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LD_RPATH_FLAGS) -fopenmp


clean:
	rm -f $(TESTS) *~ *.c.* *.s *.raw *.out *.txt \
		lbm lbm-omp
