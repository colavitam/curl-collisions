#include <stdint.h>
#include <string.h>
#include <assert.h>

#define HASH_LENGTH     243
#define STATE_LENGTH    (3*HASH_LENGTH)
#define HALF_LENGTH     (STATE_LENGTH/2)

#define NROUNDS 27

static char TRUTH_TABLE[11] = {1, 0, -1, 2, 1, -1, 0, 2, -1, 1, 0};  // magic numbers


// here's where the spongy magic happens
static void transform(char *state, char *temp, int rounds) {
    for(int32_t r = 0; r < rounds; r++) {
        int32_t t = 0;
        int32_t p = 0;
        memcpy(temp, state, STATE_LENGTH * sizeof(char));
        for(int32_t s = 0; s < STATE_LENGTH; s++) {
            p = t;
            if(t < 365) {
                t += 364;
            }
            else {
                t -= 365;
            }
            state[s] = TRUTH_TABLE[temp[p] + (temp[t] << 2) + 5];
        }
    }
}


// absorbs into the sponge
// `input` is the string to be absorbed, starting at offset `ioffset` with remaining length `ilength`
// `state` is the sponge state buffer
//      - expects to be zero-initialized or continue from a previous state
// `temp` is a temporary buffer
//      - no expectations about contents

void absorb(const char *input, int32_t ioffset, int32_t ilength, char *state, char *temp) {
    do {
        memcpy(state, &input[ioffset], (ilength < HASH_LENGTH ? ilength : HASH_LENGTH) * sizeof(char));
        transform(state, temp, NROUNDS);
        ioffset += HASH_LENGTH;
    }
    while((ilength -= HASH_LENGTH) > 0);
}


// absorbs into the sponge
// `input` is the string to be absorbed, starting at offset `ioffset` with remaining length `ilength`
// `state` is the sponge state buffer
//      - expects to be zero-initialized or continue from a previous state
// `temp` is a temporary buffer
//      - no expectations about contents

void absorb_rounds(const char *input, int32_t ioffset, int32_t ilength, char *state, char *temp, int rounds) {
    do {
        memcpy(state, &input[ioffset], (ilength < HASH_LENGTH ? ilength : HASH_LENGTH) * sizeof(char));
        transform(state, temp, rounds);
        ioffset += HASH_LENGTH;
    }
    while((ilength -= HASH_LENGTH) > 0);
}


// squeezes the sponge
// `output` is the buffer to be written to, starting at offset `ooffset` with remaining length `olength`
// `state` is the sponge state buffer
//      - expects to be the result of an earlier absorb
// `temp` is a temporary buffer
//      - no expectations about contents

void squeeze(char *output, int32_t ooffset, int32_t olength, char *state, char *temp) {
    do {
        memcpy(&output[ooffset], state, (olength < HASH_LENGTH ? olength : HASH_LENGTH) * sizeof(char));
        transform(state, temp, NROUNDS);
        ooffset += HASH_LENGTH;
    }
    while((olength -= HASH_LENGTH) > 0);
}


// does a complete Curl hash
// `input` is the string to be hashed, with length `ilength`
// `output` is the location to store the hash, with length `olength` (usually HASH_LENGTH)
// `state` is a temporary buffer with STATE_LENGTH members
// `scratch` is another temporary buffer with STATE_LENGTH members

void hash(const char *input, int32_t ilength, char *output, int32_t olength, char *state, char *temp) {

    assert(input);
    assert(output);
    assert(state);
    assert(temp);
    
    assert(ilength > 0);
    assert(olength > 0);
    
    memset(state, 0, STATE_LENGTH * sizeof(char));
    
    absorb(input, 0, ilength, state, temp);
    squeeze(output, 0, olength, state, temp);
}
