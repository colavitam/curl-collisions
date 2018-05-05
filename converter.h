// adopted from https://github.com/iotaledger/ccurl/blob/master/src/lib/util/converter.c
#ifndef _CONVERTER_H
#define _CONVERTER_H

#include <stdio.h>
#include <stdlib.h>

#define HASH_LENGTH 243
#define TRYTE_SPACE 27
#define MIN_TRYTE_VALUE -13
#define MAX_TRYTE_VALUE 13
#define RADIX 3
#define MAX_TRIT_VALUE (RADIX - 1) / 2
#define MIN_TRIT_VALUE -MAX_TRIT_VALUE
#define TRITS_IN_BYTE 5
#define TRITS_IN_TRYTE 3
#define TRYTE_STRING "9ABCDEFGHIJKLMNOPQRSTUVWXYZ"

char long_value(int8_t* const trits, const int offset, const int size);
char* bytes_from_trits(int8_t* const trits, const int offset, const int size);
void getTrits(const char* bytes, int bytelength, int8_t* const trits, int length);
void trits_from_trytes(const char* trytes, int length, int8_t* trits);
void copyTrits(char const value, char* const destination, const int offset, const int size);
void trytes_from_trits(int8_t* const trits, const int size, char* trytes);
char tryteValue(int8_t* const trits, const int offset);
void init_converter();

#endif
