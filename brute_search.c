#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include <stdint.h>
#include "brute_search.h"
#include "curl.h"
#include "converter.h"
#define BREAK_ROUNDS 20

struct collide_param {
  struct constraint_solution *solution;
  int8_t *initial_state;
  volatile int cnt;
};

static void *collide_thread(void *param) {
  struct collide_param *parms = (struct collide_param*) param;

  /* Low entropy RNG seed */
  unsigned int seed = (uint64_t) &parms ^ time(NULL);

  /* Initialize state stores */
  int8_t input[HASH_LENGTH];
  int8_t temp[STATE_LENGTH];
  int8_t state[STATE_LENGTH];
  int8_t s1[STATE_LENGTH];
  char buffer1[256];
  char buffer2[256];

  /* Loop until 4 candidate collisions are found across threads */
  while (parms->cnt < 4) {
    /* Initialize input to the result of the constraint solver */
    memcpy(input, parms->solution->input + HASH_LENGTH, HASH_LENGTH);

    /* Randomize the portions of the input that are unconstrained */
    for (int i = 0; i < HASH_LENGTH; i ++)
      if (!parms->solution->restricted[i])
        input[i] = (rand_r(&seed) % 3) - 1;

    /* Set the target flippable index to 0 */
    input[parms->solution->flip_idx] = 0;

    /* Run the requested number of rounds and save the resultant state */
    memcpy(state, parms->initial_state, STATE_LENGTH);
    absorb_rounds(input, 0, HASH_LENGTH, state, temp, BREAK_ROUNDS);
    memcpy(s1, state, STATE_LENGTH);

    /* Set the target flippable index to 1 */
    input[parms->solution->flip_idx] = 1;

    /* Run the requested number of rounds */
    memcpy(state, parms->initial_state, STATE_LENGTH);
    absorb_rounds(input, 0, HASH_LENGTH, state, temp, BREAK_ROUNDS);

    /* Count the number of differential between the states */
    int diffs = 0;
    for (int i = 0; i < STATE_LENGTH; i ++)
      if (s1[i] != state[i])
          diffs ++;

    /* Move on if more than one differential */
    if (diffs != 1)
      goto next;

    /* Collision candidate found! Print an alert */
    input[parms->solution->flip_idx] = 0;
    trytes_from_trits(parms->solution->input, HASH_LENGTH, buffer1);
    trytes_from_trits(input, HASH_LENGTH, buffer2);
    printf("%s%s\n", buffer1, buffer2);
    input[parms->solution->flip_idx] = 1;
    trytes_from_trits(input, HASH_LENGTH, buffer2);
    printf("%s%s\n", buffer1, buffer2);

    /* Run the hashing algorithm on the candidate collision and store the results */
    memcpy(state, parms->initial_state, STATE_LENGTH);
    input[parms->solution->flip_idx] = 0;
    absorb(input, 0, HASH_LENGTH, state, temp);
    memcpy(s1, state, STATE_LENGTH);
    memcpy(state, parms->initial_state, STATE_LENGTH);
    input[parms->solution->flip_idx] = 1;
    absorb(input, 0, HASH_LENGTH, state, temp);

    /* Count the number of differentials (and their positions) between the two digests */
    int hash_diffs = 0;
    int state_diffs = 0;
    for (int i = 0; i < STATE_LENGTH; i ++)
      if (s1[i] != state[i]) {
        if (i < HASH_LENGTH)
          hash_diffs ++;
        else
          state_diffs ++;
      }

    /* Determine the type of collision */
    if (hash_diffs == 0) {
      printf("Basic collision: hash = ");
      trytes_from_trits(s1, HASH_LENGTH, buffer2);
      printf("%s\n\n", buffer2);
    } else if (state_diffs == 0) {
      printf("Full-state collision prefix\n\n");
    } else {
      printf("Dud collision\n\n");
    }

    parms->cnt ++;

    next:;
  }

  return NULL;
}

void collision_search(struct constraint_solution *solution, unsigned num_threads) {
  int8_t initial_state[STATE_LENGTH];
  int8_t temp[STATE_LENGTH];
  memset(initial_state, 0, sizeof(initial_state));
  absorb(solution->input, 0, HASH_LENGTH, initial_state, temp);

  struct collide_param parm = {
    .solution = solution,
    .initial_state = initial_state,
    .cnt = 0
  };

  /* Spawn threads to perform the full collision search */

  pthread_t *threads = malloc(sizeof(pthread_t) * num_threads);
  for (int i = 0; i < num_threads; i ++)
    pthread_create(&threads[i], NULL, collide_thread, (void*)&parm);

  for (int i = 0; i < num_threads; i ++)
    pthread_join(threads[i], NULL);
}