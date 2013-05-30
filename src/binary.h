#ifndef __BINARY_H__
#define __BINARY_H__

#include <stdint.h>

#define W (uint8_t)(sizeof(uint32_t) * 8)
#define CHAR_BIT_SIZE sizeof(char) * 8 // should always be equal to 8
#define NUMBER_OF_WORDS(m) CEILING((double)m / (double)W)

uint8_t binary_hamming_weight(const uint32_t n);
uint16_t binary_degree(const uint32_t* p, uint32_t t); // TODO: 8bytes version
uint32_t binary_reverse_number(uint32_t n);
uint32_t binary_char_to_uint(const unsigned char *c); // TODO: 8bytes version
unsigned char* binary_uint32_to_char(const uint32_t w, unsigned char *dst);
unsigned char* binary_uint8_to_char(const uint8_t w, unsigned char *dst);
uint8_t binary_get_bit(uint32_t w, uint8_t i); // TODO: 8bytes version
#endif
