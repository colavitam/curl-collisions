#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include "curl.h"
#include "converter.h"
#define STATE_SIZE 729
#define SIM_ROUNDS 8
#define NUM_THREADS 72
#define BREAK_ROUNDS 20

const int TRUTH[3][3] = { {1, 1, -1}, {0,-1,1}, {-1,0,0} };
static volatile int term = 0;

struct sbox {
  int singular:1;
  int dependent:1;
  int bound;
#define UNBOUND -2

  int idx_a;
  int idx_b;

  struct sbox *sbox_a;
  struct sbox *sbox_b;
};

struct list {
  struct sbox *constraint;
  struct list *next;
};

struct list *list_append(struct list *list, struct sbox *sbox) {
  struct list *lnew = malloc(sizeof(struct list));
  lnew->constraint = sbox;
  lnew->next = list;

  return lnew;
}

static int bound_values[STATE_SIZE];


struct sbox *make_singular(int idx_a, int idx_b, int flip_idx) {
  struct sbox *sbx = malloc(sizeof(struct sbox));
  sbx->singular = 1;
  sbx->bound = UNBOUND;
  sbx->dependent = idx_a == flip_idx;
  sbx->idx_a = idx_a;
  sbx->idx_b = idx_b;

  if (idx_b == flip_idx) {
    // Bind the former index
    // assert(bound_values[idx_a] == UNBOUND);
    bound_values[idx_a] = 1;
    sbx->bound = 0;
  } else if (idx_a == flip_idx) {
    bound_values[idx_b] = 1;
  }

  return sbx;
}

struct sbox *make_nonsingular(struct sbox *sbox_a, struct sbox *sbox_b) {
  struct sbox *sbx = malloc(sizeof(struct sbox));
  sbx->singular = 0;
  sbx->dependent = sbox_a->dependent;
  sbx->sbox_a = sbox_a;
  sbx->sbox_b = sbox_b;
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

void print_sbox(struct sbox *sbox) {
  if(sbox->singular) {
    printf("%d %d ", sbox->idx_a, sbox->idx_b);
  } else {
    printf("( ");
    print_sbox(sbox->sbox_a);
    printf(") ");
    printf("( ");
    print_sbox(sbox->sbox_b);
    printf(") ");
  }
}

static struct list *glist = NULL;

static inline int check(struct sbox *sbox, char *state) {
  if (sbox->singular) {
    return TRUTH[state[sbox->idx_a] + 1][state[sbox->idx_b] + 1];
  }
  return TRUTH[check(sbox->sbox_a, state) + 1][check(sbox->sbox_b, state) + 1];
}

static inline void mark(struct sbox *sbox, char *mk) {
  if (sbox->singular) {
    mk[sbox->idx_a] = 1;
    mk[sbox->idx_b] = 1;
  } else{
    mark(sbox->sbox_a, mk);
    mark(sbox->sbox_b, mk);
  }
}

void *search_thread(void *param) {
  int flip_idx = *(int *) param;

  unsigned int seed = (uint64_t) &flip_idx ^ time(NULL);

  while (!term) {
    char state[729];
    char temp[729];
    char input[243];

    for (int i = 0; i < 243; i ++) {
      input[i] = rand_r(&seed) % 3 - 1;
    }
    memset(state, 0, 729);
    memset(temp, 0, 729);

    absorb(input, 0, 243, state, temp);

    // TODO: check this...
    if (state[flip_idx] == -1)
      goto next;

    for (int i = 0; i < STATE_SIZE; i ++)
      if (bound_values[i] != UNBOUND)
        if (state[i] != bound_values[i]) {
          // printf("Mismatch at %d (expected %i saw %i)\n",i,bound_values[i], state[i]);
          goto next;
        }

    struct list *list = glist;
    while (list != NULL) {
      if (check(list->constraint, state) != list->constraint->bound)
        goto next;
      list = list->next;
    }

    printf("M1:\n");
    for (int i = 0; i < 243; i ++) {
      printf("%d, ", input[i]);
    }
    printf("\nM2:\n");
    for (int i = 0; i < 243; i ++) {
      printf("%d, ", state[i]);
    }
    printf("\n");
    term = 1;
    char *out = malloc(243 * 2);
    memcpy(out, input, 243);
    memcpy(out + 243, state, 243);
    return out;

    next:
    (void) param;
  }

  return NULL;
}

struct collide_param {
  char *inp;
  char *inp2;
  int flip_idx;
  char *restricted;
  char *initial_state;
  volatile int cnt;
};

void *collide_thread(void *param) {
  struct collide_param *parms = (struct collide_param*) param;

  unsigned int seed = (uint64_t) &parms ^ time(NULL);
  // printf("Enter\n");
  int count = 0;
  char input[243];
  char temp[729];
  char state[729];
  char s1[729];
  char s2[729];
  while (parms->cnt < 4) {
    count ++;
    memcpy(input, parms->inp2, 243);
    memcpy(state, parms->initial_state, STATE_SIZE);
    for (int i = 0; i < 243; i ++) {
      if (!parms->restricted[i])
        input[i] = (rand_r(&seed) % 3) - 1;
    }

    input[parms->flip_idx] = 0;
    absorb_rounds(input, 0, 243, state, temp, BREAK_ROUNDS);
    memcpy(s1, state, 729);
    memcpy(state, parms->initial_state, STATE_SIZE);
    input[parms->flip_idx] = 1;
    absorb_rounds(input, 0, 243, state, temp, BREAK_ROUNDS);
    memcpy(s2, state, 729);
    int diffs = 0;
    for (int i = 0; i < 729; i ++)
      if (s1[i] != s2[i]) {
        // if (!pos_bitmap[i]) {
          // printf("Difference at %d\n", i);
          // goto next;
          diffs ++;
        // }
      }
    // printf("%d diffs\n", diffs);
    // if (diffs < 10)
      // printf("Diffs: %d\n", diffs);
    if (diffs != 1)
      goto next;

    // printf("Successful value:\n");
    // for (int i = 0; i < 243; i ++) {
    //   printf("%d, ", input[i]);
    // }

    char buffer1[256];
    char buffer2[256];
    input[parms->flip_idx] = 0;
    trytes_from_trits(parms->inp, 243, buffer1);
    trytes_from_trits(input, 243, buffer2);
    printf("%s%s\n", buffer1, buffer2);
    input[parms->flip_idx] = 1;
    trytes_from_trits(input, 243, buffer2);
    printf("%s%s\n", buffer1, buffer2);

    memcpy(state, parms->initial_state, STATE_SIZE);
    input[parms->flip_idx] = 0;
    absorb(input, 0, 243, state, temp);
    memcpy(s1, state, 729);
    memcpy(state, parms->initial_state, STATE_SIZE);
    input[parms->flip_idx] = 1;
    absorb(input, 0, 243, state, temp);
    memcpy(s2, state, 729);
    int hash_diffs, state_diffs = 0;
    for (int i = 0; i < 729; i ++)
      if (s1[i] != s2[i]) {
        if (i < 243)
          hash_diffs ++;
        else
          state_diffs ++;
      }
    if (hash_diffs == 0) {
      printf("Basic collision: ");
      trytes_from_trits(s1, 243, buffer2);
      printf("%s\n", buffer2);
    } else if (state_diffs == 0) {
      printf("Full collision!\n");
    } else {
      printf("Dud collision\n");
    }

    parms->cnt ++;

    next:
    // if (count % 100000 == 0)
    //   printf("%d\n", count);
    // printf("Failure\n");
    (void) param;
  }

  return NULL;
}

void collision_search(char *inp, char *inp2, int flip_idx, char *restricted) {
  char initial_state[STATE_SIZE];
  char temp[729];
  memset(initial_state, 0, sizeof(initial_state));
  absorb(inp, 0, 243, initial_state, temp);

  struct collide_param parm = {
    .inp = inp,
    .inp2 = inp2,
    .flip_idx = flip_idx,
    .restricted = restricted,
    .initial_state = initial_state,
    .cnt = 0
  };

  pthread_t threads[NUM_THREADS];
  for (int i = 0; i < NUM_THREADS; i ++) {
    pthread_create(&threads[i], NULL, collide_thread, (void*)&parm);
  }
  for (int i = 0; i < NUM_THREADS; i ++) {
    pthread_join(threads[i], NULL);
  }
}

int main(int argc, char **argv) {
  srand(time(NULL));

  while (1) {
    for (int i = 0; i < STATE_SIZE; i ++)
      bound_values[i] = UNBOUND;

    term = 0;

    int flip_idx = rand() % 243;

    struct sbox **sboxes[SIM_ROUNDS];
    sboxes[0] = calloc(STATE_SIZE, sizeof(struct sbox*));
    int a = 0;

    for (int i = 0; i < STATE_SIZE; i ++) {
      sboxes[0][i] = make_singular(a, a + (a < 365 ? 364 : -365), flip_idx);
      a += a < 365 ? 364 : -365;
    }

    for (int i = 1; i < SIM_ROUNDS; i ++) {
      sboxes[i] = calloc(STATE_SIZE, sizeof(struct sbox*));
      
      a = 0;

      for (int j = 0; j < STATE_SIZE; j ++) {
        sboxes[i][j] = make_nonsingular(sboxes[i - 1][a], sboxes[i - 1][a + (a < 365 ? 364 : -365)]);
        a += a < 365 ? 364 : -365;
      }
    }

    // printf("Computed constraints for %d rounds:\n", SIM_ROUNDS);

    // for (int i = 0; i < STATE_SIZE; i ++) {
    //   if (bound_values[i] != UNBOUND) {
    //     printf("%d -> %d\n", i, bound_values[i]);
    //   }
    // }

    glist = NULL;

    for (int i = 0; i < SIM_ROUNDS; i ++) {
      for (int j = 0; j < STATE_SIZE; j ++) {
        if (sboxes[i][j]->bound != UNBOUND) {
          // print_sbox(sboxes[i][j]);
          glist = list_append(glist, sboxes[i][j]);
          // printf("-> %d (%d %d) \n", sboxes[i][j]->bound, i, j);
        }
      }
    }

    char restricted[STATE_SIZE];
    memset(restricted, 0, STATE_SIZE);
    for (int i = 0; i < STATE_SIZE; i ++)
      if (bound_values[i] != UNBOUND)
        restricted[i] = 1;

    struct list *list = glist;
      while (list != NULL) {
        mark(list->constraint, restricted);
        list = list->next;
      }

    // printf("Mutable indices:\n");
    // for (int i = 0; i < 243; i ++) {
    //   if (!restricted[i])
    //     printf("%d ", i);
    // }
    // printf("\n");

    printf("Beginning initialization search for bit %d...\n", flip_idx);

    pthread_t pts[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i ++) {
      pthread_create(&pts[i], NULL, search_thread, &flip_idx);
    }
    for (int i = 0; i < NUM_THREADS; i ++) {
      void *result;
      pthread_join(pts[i], &result);
      if (result != NULL) {
        printf("Beginning collision search...\n");
        collision_search((char *)result, result + 243, flip_idx, restricted);
      }
    }
  }

  return 0;
}
