#ifndef _CONSTRAINT_SEARCH_H
#define _CONSTRAINT_SEARCH_H

#include "curl.h"

struct constraint_solution {
  uint8_t restricted[STATE_LENGTH];
  int8_t *input;
  int flip_idx;
};

struct constraint_set;

struct constraint_set *generate_constraints(int flip_idx);
struct constraint_solution *search_constraints(struct constraint_set *constraints, unsigned num_threads);
#endif