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
  srand(time(NULL));

  int num_threads = 4;

  if (argc > 1)
    for (char **arg = argv + 1; *arg != NULL; arg ++) {
      if (!strcmp(*arg, "-t")) {
        if (*(++arg) != NULL) {
          char *end;
          num_threads = strtol(*arg, &end, 0);
          if (*end != '\0')
            usage();
        } else {
          usage();
        }
      } else {
        usage();
      }
    }

  printf("Using %d threads\n", num_threads);

  while (1) {
    struct constraint_set *constraints;
    struct constraint_solution *solution;
    int flip_idx = rand() % 243;

    printf("\nPhase 0:\n");
    /* Create phase 1 constraints */
    constraints = generate_constraints(flip_idx);


    printf("\nPhase 1:\n");
    /* Solve phase 1 constraints */
    solution = search_constraints(constraints, num_threads);

    printf("\nPhase 2:\n");
    /* Brute force phase 2 */
    collision_search(solution, num_threads);
  }

  return 0;
}
