#define _POSIX_C_SOURCE 200112L
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <getopt.h>
#include <numaif.h>
#include "../common/common.h"
#include "cholesky_common.h"

#include <mkl_cblas.h>
#include <mkl_lapack.h>

#define _WITH_OUTPUT 0

#include <unistd.h>

static int interleave_data(void* p, size_t size)
{
	unsigned long nodemask = 0;
	unsigned long pagemask = 0xFFF;
	unsigned long maxnode = numa_max_node()+1;

	for(int i = 0; i < maxnode; i++)
		nodemask = (nodemask << 1) | 1;

	if(mbind((void*)((long)p & ~(pagemask)), size, MPOL_INTERLEAVE, &nodemask, maxnode, MPOL_MF_MOVE) != 0) {
		fprintf(stderr, "mbind error:\n");
		perror("mbind");
		return 1;
	}

	return 0;
}

int
main(int argc, char *argv[])
{
  int option;
  int i, iter;
  int size;

  int N = 4096;

  int numiters = 10;

  FILE *res_file = NULL;
  FILE *in_file = NULL;

  int padding = 64;
  int padding_elements = padding / sizeof(double);

  double * data;
  int nfo;
  char upper = 'U';

  while ((option = getopt(argc, argv, "n:s:r:i:o:p:h")) != -1)
    {
      switch(option)
	{
	case 'n':
	  N = atoi(optarg);
	  break;
	case 's':
	  N = 1 << atoi(optarg);
	  break;
	case 'r':
	  numiters = atoi (optarg);
	  break;
	case 'i':
	  in_file = fopen(optarg, "r");
	  break;
	case 'o':
	  res_file = fopen(optarg, "w");
	  break;
	case 'p':
	  padding = atoi(optarg);

	  if(padding % sizeof(double)) {
	    fprintf(stderr, "Padding must be a multiple of sizeof(double) (%lu).\n",
		    sizeof(double));
	    exit(1);
	  }

	  padding_elements = padding / sizeof(double);
	  break;
	case 'h':
	  printf("Usage: %s [option]...\n\n"
		 "Options:\n"
		 "  -n <size>                    Number of colums of the square matrix, default is %d\n"
		 "  -s <power>                   Set the number of colums of the square matrix to 1 << <power>\n"
		 "  -r <number of iterations>    Number of repetitions of the execution\n"
		 "  -i <input file>              Read matrix data from an input file\n"
		 "  -o <output file>             Write matrix data to an output file\n"
		 "  -p <padding>                 Padding at the end of aline of the global matrix. Should be\n"
		 "                               equal to the size of one cache line, e.g. 64.\n",
		 argv[0], N);
	  exit(0);
	  break;
	case '?':
	  fprintf(stderr, "Run %s -h for usage.\n", argv[0]);
	  exit(1);
	  break;
	}
    }

  if(optind != argc) {
	  fprintf(stderr, "Too many arguments. Run %s -h for usage.\n", argv[0]);
	  exit(1);
  }

  size = N * N;

  struct timeval *sstart = (struct timeval *) malloc (numiters * sizeof (struct timeval));
  struct timeval *send = (struct timeval *) malloc (numiters * sizeof (struct timeval));

   if (posix_memalign ((void **) &data, 64, size * sizeof (double) + N * padding))
    {
      printf ("Out of memory.\n");
      exit (1);
    }

   interleave_data(data, size * sizeof (double) + N * padding);

  // Generate random numbers or read from file
  if (in_file == NULL)
    {
      int seed[4] = {1092, 43, 77, 1};
      int sp = 1;
      dlarnv_(&sp, seed, &size, data);

      // Also allow saving data sessions
      if (res_file != NULL)
	fwrite (data, sizeof (double), size, res_file);
    }
  else
    {
      fread (data, sizeof(double), size, in_file);
    }

  // Ensure matrix is definite positive
  for(i = 0; i < N; ++i)
    data[i*N + i] += N;

  matrix_add_padding(data, N, padding_elements);

  for (iter = 0; iter < numiters; ++iter)
    {
      double * seq_data;
      if (posix_memalign ((void **) &seq_data, 64, size * sizeof (double) + N * padding))
	{
	  printf ("Out of memory.\n");
	  exit (1);
	}

      interleave_data(seq_data, size * sizeof (double) + N * padding);

      memcpy (seq_data, data, size * sizeof (double) + N * padding);

      gettimeofday (&sstart[iter], NULL);
      int Npad = N + padding_elements;
      dpotrf_(&upper, &N, seq_data, &Npad, &nfo);
      gettimeofday (&send[iter], NULL);

      free (seq_data);
    }

  double seq_time = 0;
  for (iter = 0; iter < numiters; ++iter)
    {
      seq_time += tdiff (&send[iter], &sstart[iter]);
    }

  printf ("%.5f \n", seq_time);

  return 0;
}





