#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include "constraint_search.h"
#include "brute_search.h"

static void usage(void) {
  printf("Usage: ./collide [-t NUM_THREADS]\n");
  exit(1);
}

int main(int argc, char **argv) {
  unsigned num_threads = 4;

  /* Seed RNG for index selection */
  srand(time(NULL));

  /* Process NUM_THREADS parameters */
  if (argc > 1)
    for (char **arg = argv + 1; *arg != NULL; arg ++) {
      if (!strcmp(*arg, "-t")) {
        if (*(++arg) != NULL) {
          char *end;
          num_threads = strtoul(*arg, &end, 0);
          if (*end != '\0')
            usage();
        } else {
          usage();
        }
      } else {
        usage();
      }
    }

  if (num_threads == 0 || num_threads > 2048) {
    printf("Error: NUM_THREADS is outside the range [0, 2048]\n");
    exit(1);
  }

  printf("Using %d threads\n\n", num_threads);
  struct constraint_set *constraints[HASH_LENGTH];

  for (unsigned p = 0; p < HASH_LENGTH; p ++) {
    printf("Measuring for position %d\n", p);

    struct timespec start, end;

    clock_gettime(CLOCK_REALTIME, &start);

    constraints[p] = generate_constraints(p);

    struct constraint_solution *solution;

    /* Solve phase 1 constraints */
    solution = search_constraints(constraints, num_threads, p);

    /* Brute force phase 2 */
    collision_search(solution, num_threads);

    clock_gettime(CLOCK_REALTIME, &end);

    double elapsed = end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec) / 1000000000.0;

    printf("Time elapsed: %f seconds\n", elapsed);
  }

  return 0;
}
