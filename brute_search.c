#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include "brute_search.h"
#include "curl.h"
#include "converter.h"
#define BREAK_ROUNDS 12

struct collide_param {
  struct constraint_solution *solution;
  int8_t *initial_state;
  volatile int cnt;
};

void *collide_thread(void *param) {
  struct collide_param *parms = (struct collide_param*) param;

  unsigned int seed = (uint64_t) &parms ^ time(NULL);
  int count = 0;
  int8_t input[HASH_LENGTH];
  int8_t temp[STATE_LENGTH];
  int8_t state[STATE_LENGTH];
  int8_t s1[STATE_LENGTH];
  int8_t s2[STATE_LENGTH];
  while (parms->cnt < 4) {
    count ++;
    memcpy(input, parms->solution->input + HASH_LENGTH, HASH_LENGTH);
    memcpy(state, parms->initial_state, STATE_LENGTH);
    for (int i = 0; i < HASH_LENGTH; i ++)
      if (!parms->solution->restricted[i])
        input[i] = (rand_r(&seed) % 3) - 1;

    input[parms->solution->flip_idx] = 0;
    absorb_rounds(input, 0, HASH_LENGTH, state, temp, BREAK_ROUNDS);
    memcpy(s1, state, STATE_LENGTH);
    memcpy(state, parms->initial_state, STATE_LENGTH);
    input[parms->solution->flip_idx] = 1;
    absorb_rounds(input, 0, HASH_LENGTH, state, temp, BREAK_ROUNDS);
    memcpy(s2, state, STATE_LENGTH);
    int diffs = 0;
    for (int i = 0; i < STATE_LENGTH; i ++)
      if (s1[i] != s2[i])
          diffs ++;

    if (diffs != 1)
      goto next;

    char buffer1[256];
    char buffer2[256];
    input[parms->solution->flip_idx] = 0;
    trytes_from_trits(parms->solution->input, HASH_LENGTH, buffer1);
    trytes_from_trits(input, HASH_LENGTH, buffer2);
    printf("%s%s\n", buffer1, buffer2);
    input[parms->solution->flip_idx] = 1;
    trytes_from_trits(input, HASH_LENGTH, buffer2);
    printf("%s%s\n", buffer1, buffer2);

    memcpy(state, parms->initial_state, STATE_LENGTH);
    input[parms->solution->flip_idx] = 0;
    absorb(input, 0, HASH_LENGTH, state, temp);
    memcpy(s1, state, STATE_LENGTH);
    memcpy(state, parms->initial_state, STATE_LENGTH);
    input[parms->solution->flip_idx] = 1;
    absorb(input, 0, HASH_LENGTH, state, temp);
    memcpy(s2, state, STATE_LENGTH);
    int hash_diffs = 0;
    int state_diffs = 0;
    for (int i = 0; i < STATE_LENGTH; i ++)
      if (s1[i] != s2[i]) {
        if (i < HASH_LENGTH)
          hash_diffs ++;
        else
          state_diffs ++;
      }
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

    next:
    (void) param;
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

  pthread_t *threads = malloc(sizeof(pthread_t) * num_threads);
  for (int i = 0; i < num_threads; i ++)
    pthread_create(&threads[i], NULL, collide_thread, (void*)&parm);

  for (int i = 0; i < num_threads; i ++)
    pthread_join(threads[i], NULL);
}