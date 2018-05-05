#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>
#include "constraint_search.h"
#include "curl.h"
#include "converter.h"
#define UNBOUND -2
#define SIM_ROUNDS 8

struct sbox {
  int singular:1;
  int dependent:1;
  int bound;

  union {
    struct {
      int idx_a;
      int idx_b;
    } indices;

    struct {
      struct sbox *sbox_a;
      struct sbox *sbox_b;
    } boxes;
  } sources;
};

struct list {
  struct sbox *constraint;
  struct list *next;
};

struct constraint_set {
  int flip_idx;
  int8_t bound_values[STATE_LENGTH];
  struct list *sbox_constraints;
};

struct constraint_param {
  struct constraint_set *constraints;
  volatile uint8_t term;
};

static struct list *list_append(struct list *list, struct sbox *sbox) {
  struct list *lnew = malloc(sizeof(struct list));
  lnew->constraint = sbox;
  lnew->next = list;

  return lnew;
}


static struct sbox *make_singular(int idx_a, int idx_b, int flip_idx, int8_t *bound_values) {
  struct sbox *sbx = malloc(sizeof(struct sbox));
  sbx->singular = 1;
  sbx->bound = UNBOUND;
  sbx->dependent = idx_a == flip_idx;
  sbx->sources.indices.idx_a = idx_a;
  sbx->sources.indices.idx_b = idx_b;

  if (idx_b == flip_idx) {
    assert(bound_values[idx_a] == UNBOUND);
    bound_values[idx_a] = 1;
    sbx->bound = 0;
  } else if (idx_a == flip_idx) {
    bound_values[idx_b] = 1;
  }

  return sbx;
}

static struct sbox *make_nonsingular(struct sbox *sbox_a, struct sbox *sbox_b) {
  struct sbox *sbx = malloc(sizeof(struct sbox));
  sbx->singular = 0;
  sbx->dependent = sbox_a->dependent;
  sbx->sources.boxes.sbox_a = sbox_a;
  sbx->sources.boxes.sbox_b = sbox_b;
  sbx->bound = UNBOUND;

  if (sbox_a->dependent && sbox_b->dependent)
    printf("ERROR: double dependency prevents equation generation\n");
  else if (sbox_b->dependent) {
    assert(sbox_a->bound == UNBOUND);
    sbox_a->bound = 1;
  } else if (sbox_a->dependent) {
    assert(sbox_b->bound == UNBOUND);
    sbox_b->bound = 1;
  }

  return sbx;
}

static void print_sbox(struct sbox *sbox) {
  if(sbox->singular) {
    printf("%d %d ", sbox->sources.indices.idx_a, sbox->sources.indices.idx_b);
  } else {
    printf("( ");
    print_sbox(sbox->sources.boxes.sbox_a);
    printf(") (");
    print_sbox(sbox->sources.boxes.sbox_b);
    printf(") ");
  }
}

static inline int check(struct sbox *sbox, int8_t *state) {
  if (sbox->singular) {
    return TRUTH_TABLE[state[sbox->sources.indices.idx_a] + (state[sbox->sources.indices.idx_b] << 2) + 5];
  }
  return TRUTH_TABLE[check(sbox->sources.boxes.sbox_a, state) + (check(sbox->sources.boxes.sbox_b, state) << 2) + 5];
}

static inline void mark(struct sbox *sbox, uint8_t *mk) {
  if (sbox->singular) {
    mk[sbox->sources.indices.idx_a] = 1;
    mk[sbox->sources.indices.idx_b] = 1;
  } else{
    mark(sbox->sources.boxes.sbox_a, mk);
    mark(sbox->sources.boxes.sbox_b, mk);
  }
}

static void *search_thread(void *param) {
  struct constraint_param *constraint_param = (struct constraint_param*) param;

  /* Low entropy RNG seed */
  unsigned int seed = (uint64_t) &constraint_param ^ time(NULL);

  /* Loop until a thread is successful */
  while (!constraint_param->term) {
    int8_t state[STATE_LENGTH];
    int8_t temp[STATE_LENGTH];
    int8_t input[HASH_LENGTH];

    /* Generate a random input and clear the state */
    for (int i = 0; i < HASH_LENGTH; i ++)
      input[i] = rand_r(&seed) % 3 - 1;
    memset(state, 0, STATE_LENGTH);
    memset(temp, 0, STATE_LENGTH);

    /* Hash the random input */
    absorb(input, 0, HASH_LENGTH, state, temp);

    // TODO: check this...
    if (state[constraint_param->constraints->flip_idx] == -1)
      goto next;

    /* Verify that output satisfies positional constraints */
    for (int i = 0; i < STATE_LENGTH; i ++)
      if (constraint_param->constraints->bound_values[i] != UNBOUND)
        if (state[i] != constraint_param->constraints->bound_values[i])
          goto next;

    /* Verify that output satisfies S-box constraints */
    struct list *list = constraint_param->constraints->sbox_constraints;
    while (list != NULL) {
      if (check(list->constraint, state) != list->constraint->bound)
        goto next;
      list = list->next;
    }

    /* Constraints satisfied! Print an alert and return the input */
    printf("Constraints satisfied!\n");

    char buffer[256];
    trytes_from_trits(input, HASH_LENGTH, buffer);
    printf("Fixed Prefix: %s\n", buffer);
    trytes_from_trits(state, HASH_LENGTH, buffer);
    printf("Mutable Suffix: %s\n", buffer);
    constraint_param->term = 1;
    int8_t *out = malloc(HASH_LENGTH * 2);
    memcpy(out, input, HASH_LENGTH);
    memcpy(out + HASH_LENGTH, state, HASH_LENGTH);
    return out;

    next:;
  }

  return NULL;
}

struct constraint_set *generate_constraints(int flip_idx) {
  struct constraint_set *constraints = malloc(sizeof(struct constraint_set));
  constraints->flip_idx = flip_idx;

  /* Intiailize all state positions to be unbound */
  for (int i = 0; i < STATE_LENGTH; i ++)
    constraints->bound_values[i] = UNBOUND;

  struct sbox **sboxes[SIM_ROUNDS];
  sboxes[0] = calloc(STATE_LENGTH, sizeof(struct sbox*));
  int a = 0;

  /* Generate the initial round of S-boxes, referencing indexes in the initial state */
  for (int i = 0; i < STATE_LENGTH; i ++) {
    sboxes[0][i] = make_singular(a, a + (a < 365 ? 364 : -365), flip_idx, constraints->bound_values);
    a += a < 365 ? 364 : -365;
  }

  /* Generate subsequent rounds of S-boxes, referencing the prior state */
  for (int i = 1; i < SIM_ROUNDS; i ++) {
    sboxes[i] = calloc(STATE_LENGTH, sizeof(struct sbox*));
    a = 0;

    for (int j = 0; j < STATE_LENGTH; j ++) {
      sboxes[i][j] = make_nonsingular(sboxes[i - 1][a], sboxes[i - 1][a + (a < 365 ? 364 : -365)]);
      a += a < 365 ? 364 : -365;
    }
  }

  int constraint_count = 0;
  constraints->sbox_constraints = NULL;

  /* Chain constraints into a list */
  for (int i = 0; i < SIM_ROUNDS; i ++)
    for (int j = 0; j < STATE_LENGTH; j ++)
      if (sboxes[i][j]->bound != UNBOUND) {
        constraints->sbox_constraints = list_append(constraints->sbox_constraints, sboxes[i][j]);
        constraint_count ++;
      }

  printf("%d S-box constraints generated.\n", constraint_count);

  return constraints;
}

struct constraint_solution *search_constraints(struct constraint_set *constraints, unsigned num_threads) {
  struct constraint_solution *solution = malloc(sizeof(struct constraint_solution));

  solution->flip_idx = constraints->flip_idx;

  /* Mark restricted indices in the state */
  for (int i = 0; i < STATE_LENGTH; i ++)
    if (constraints->bound_values[i] != UNBOUND)
      solution->restricted[i] = 1;
    else
      solution->restricted[i] = 0;

  struct list *list = constraints->sbox_constraints;
  while (list != NULL) {
    mark(list->constraint, solution->restricted);
    list = list->next;
  }

  /* Spawn search threads to satisfy constraints */
  struct constraint_param params;
  params.constraints = constraints;
  params.term = 0;

  pthread_t *pts = malloc(sizeof(pthread_t) * num_threads);
  for (int i = 0; i < num_threads; i ++) {
    pthread_create(&pts[i], NULL, search_thread, &params);
  }

  for (int i = 0; i < num_threads; i ++) {
    void *result;
    pthread_join(pts[i], &result);
    if (result != NULL)
      solution->input = (int8_t*) result;
  }

  return solution;
}
