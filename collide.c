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

  printf("Using %d threads\n", num_threads);
  struct constraint_set *constraints[HASH_LENGTH];

  /* Create phase 1 constraints */
  printf("\nPhase 0:\n");
  for (int i = 0; i < HASH_LENGTH; i ++)
    constraints[i] = generate_constraints(i);

  printf("Generated %d constraint sets.\n", HASH_LENGTH);

  while (1) {
    struct constraint_solution *solution;

    /* Solve phase 1 constraints */
    printf("\nPhase 1:\n");
    solution = search_constraints(constraints, num_threads);

    /* Brute force phase 2 */
    printf("\nPhase 2:\n");
    collision_search(solution, num_threads);
  }

  return 0;
}
