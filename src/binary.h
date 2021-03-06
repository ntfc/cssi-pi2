#ifndef __BINARY_H__
#define __BINARY_H__

#include <stdint.h>

#define W (uint8_t)(sizeof(uint32_t) * 8)
#define CHAR_BIT_SIZE sizeof(char) * 8 // should always be equal to 8
#define NUMBER_OF_WORDS(m) CEILING((double)m / (double)W)


uint8_t binary_hamming_weight(uint32_t n);
int16_t binary_degree(uint32_t p); // TODO: 8bytes version
uint32_t binary_reverse_number(uint32_t n);
// TODO: validate 
uint32_t binary_char_to_uint32(const unsigned char *c);
uint16_t binary_char_to_uint16(const unsigned char *c);
uint8_t binary_char_to_uint8(const unsigned char *c);
unsigned char* binary_uint32_to_char(uint32_t w, unsigned char *dst);
// TODO: estes 2 valem a pena? um simples cast devera ser suficiente
unsigned char* binary_uint16_to_char(uint16_t w, unsigned char *dst);
unsigned char* binary_uint8_to_char(uint8_t w, unsigned char *dst);
uint8_t binary_get_bit(uint32_t w, uint8_t i);

void binary_array_shift_left(uint32_t *a, uint16_t t);
void binary_array_shift_left2(const uint32_t *a, uint16_t t, uint32_t* b); // TODO: change name..
void binary_array_shift_right(uint32_t *a, uint16_t t);
#endif
