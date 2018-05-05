#ifndef _CURL_H
#define _CURL_H

#include <stdint.h>

#define HASH_LENGTH     243
#define STATE_LENGTH    (3*HASH_LENGTH)
#define HALF_LENGTH     (STATE_LENGTH/2)

#define NROUNDS 27

extern int8_t TRUTH_TABLE[11];

void absorb(const int8_t *input, int32_t ioffset, int32_t ilength, int8_t *state, int8_t *temp);
void absorb_rounds(const int8_t *input, int32_t ioffset, int32_t ilength, int8_t *state, int8_t *temp, unsigned rounds);
void squeeze(int8_t *output, int32_t ooffset, int32_t olength, int8_t *state, int8_t *temp);
void hash(const int8_t *input, int32_t ilength, int8_t *output, int32_t olength, int8_t *state, int8_t *temp);

#endif