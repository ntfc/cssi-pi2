#ifndef __BINARY_H__
#define __BINARY_H__

#include <stdint.h>

#define W (uint8_t)(sizeof(uint32_t) * 8)
#define CHAR_BIT_SIZE sizeof(char) * 8 // should always be equal to 8
#define NUMBER_OF_WORDS(m) CEILING((double)m / (double)W)

uint8_t hamming_weight(uint32_t n);
uint32_t degree(uint32_t* p, uint32_t t);
uint32_t reverse_number(uint32_t n);
uint32_t char_to_uint(const unsigned char *c);

#endif
