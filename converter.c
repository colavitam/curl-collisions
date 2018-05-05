#include <string.h>
#include "converter.h"

char BYTE_TO_TRITS_MAPPINGS
    [HASH_LENGTH][TRITS_IN_BYTE];

char TRYTE_TO_TRITS_MAPPINGS
    [TRYTE_SPACE][TRITS_IN_TRYTE];

static const char* TRYTE_ALPHABET = TRYTE_STRING;

char long_value(int8_t* const trits, const int offset, const int size) {
  int i;

  char value = 0;
  for (i = size; i-- > 0;) {
    value = value * RADIX + trits[offset + i];
  }
  return value;
}

char* bytes_from_trits(int8_t* const trits, const int offset, const int size) {
  int i, j;
  int length =
      (size + TRITS_IN_BYTE - 1) / TRITS_IN_BYTE;
  char* bytes = (char*)malloc(sizeof(char) * length);
  for (i = 0; i < length; i++) {

    char value = 0;
    for (j = (size - i * TRITS_IN_BYTE) < 5
                     ? (size - i * TRITS_IN_BYTE)
                     : TRITS_IN_BYTE;
         j-- > 0;) {
      value = value * RADIX + trits[offset + i * TRITS_IN_BYTE + j];
    }
    bytes[i] = (char)value;
  }

  return bytes;
}

void getTrits(const char* bytes, int bytelength, int8_t* const trits,
              int length) {
  int i;

  int offset = 0;
  for (i = 0; i < bytelength && offset < length; i++) {
    memcpy(
        trits + offset,
        BYTE_TO_TRITS_MAPPINGS
            [bytes[i] < 0
                 ? (bytes[i] +
                    HASH_LENGTH /* length of what? first? BYTE_TO_TRITS_MAPPINGS.length */)
                 : bytes[i]],
        sizeof(char) * (length - offset < TRITS_IN_BYTE
                              ? (length - offset)
                              : TRITS_IN_BYTE));
    offset += TRITS_IN_BYTE;
  }
  while (offset < length) {
    trits[offset++] = 0;
  }
}

void trits_from_trytes(const char* trytes, int length, int8_t* trits) {
  int i;
  for (i = 0; i < length; i++) {
    memcpy(trits + i * TRITS_IN_TRYTE,
           TRYTE_TO_TRITS_MAPPINGS[strchr(TRYTE_ALPHABET, trytes[i]) -
                                   TRYTE_ALPHABET],
           sizeof(char) * TRITS_IN_TRYTE);
  }
}

void copyTrits(char const value, char* const destination, const int offset,
               const int size) {
  int i;

  char absoluteValue = value < 0 ? -value : value;
  for (i = 0; i < size; i++) {

    int remainder = (int)(absoluteValue % RADIX);
    absoluteValue /= RADIX;
    if (remainder > MAX_TRIT_VALUE) {

      remainder = MIN_TRIT_VALUE;
      absoluteValue++;
    }
    destination[offset + i] = remainder;
  }

  if (value < 0) {
    for (i = 0; i < size; i++) {
      destination[offset + i] = -destination[offset + i];
    }
  }
}

void trytes_from_trits(int8_t* const trits, const int size, char* trytes) {
  int i;
  const int length =
      (size + TRITS_IN_TRYTE - 1) / TRITS_IN_TRYTE;
  trytes[length] = '\0';

  for (i = 0; i < length; i++) {
    char j = trits[i * 3] + trits[i * 3 + 1] * 3 +
               trits[i * 3 + 2] * 9;
    if (j < 0) {
      j += 27;
    }
    trytes[i] = TRYTE_ALPHABET[(size_t)j];
  }
}

char tryteValue(int8_t* const trits, const int offset) {
  return trits[offset] + trits[offset + 1] * 3 + trits[offset + 2] * 9;
}

static void increment(int8_t* trits, int size) {
  int i;
  for (i = 0; i < size; i++) {
    if (++trits[i] > MAX_TRIT_VALUE) {
      trits[i] = MIN_TRIT_VALUE;
    } else {
      break;
    }
  }
}

void init_converter (void) __attribute__ ((constructor));
void init_converter() {
  int i;
  int8_t trits[TRITS_IN_BYTE];
  memset(trits, 0, TRITS_IN_BYTE * sizeof(int8_t));
  for (i = 0; i < HASH_LENGTH; i++) {
    memcpy(&(BYTE_TO_TRITS_MAPPINGS[i]), trits,
           TRITS_IN_BYTE * sizeof(char));
    increment(trits, TRITS_IN_BYTE);
  }
  for (i = 0; i < TRYTE_SPACE; i++) {
    memcpy(&(TRYTE_TO_TRITS_MAPPINGS[i]), trits,
           TRITS_IN_TRYTE * sizeof(int8_t));
    increment(trits, TRITS_IN_TRYTE);
  }
}