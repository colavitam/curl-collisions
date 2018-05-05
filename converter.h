/*
 * Copyright (c) 2017 IOTA AS, IOTA Foundation & Developers (https://iotatoken.com)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Adopted from https://github.com/iotaledger/ccurl/blob/master/src/lib/util/converter.c
 */

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
