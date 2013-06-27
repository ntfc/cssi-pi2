#ifndef __LAPIN_H__
#define __LAPIN_H__

#include <stdint.h>
#include "poly.h"

#define SEC_PARAM 80

typedef uint32_t *Challenge;
typedef struct s_key {
  Poly *s1, *s2;
} Key;

static uint32_t F_IRREDUCIBLE[17] = {
  0x100000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3
}; // x^532 + x + 1

static uint32_t F_PROD_REDUCIBLE[5][4] = { // = Poly[5]
  { 0x80000000, 0x0, 0x0, 0x189 }, // x^127+x^8+x^7+x^3+1
  { 0x40000000, 0x0, 0x0, 0x261 }, // x^126+x^9+x^6+x^5+1
  { 0x20000000, 0x0, 0x0, 0x291 }, // x^125+x^6+x^7+x^4+1
  { 0x04000000, 0x0, 0x0, 0x099 }, // x^122+x^7+x^4+x^3+1
  { 0x02000000, 0x0, 0x0, 0x123 }  // x^121+x^8+x^5+ x +1
};
static uint16_t F_PROD_REDUCIBLE_M[5] = {127, 126, 125, 122, 121};
static uint16_t F_PROD_REDUCIBLE_R[5][4] = {
  {8, 7, 3, 0},
  {9, 6, 5, 0},
  {9, 7, 4, 0},
  {7, 4, 3, 0},
  {8, 5, 1, 0}
};

// pre-computed f reducible
static uint32_t F_REDUCIBLE[20] = { // degree = 621
  0x02000000, 0x00000000, 0x00000000, 0x00000000, 0x153bc000,
  0x00000000, 0x00000000, 0x00000392, 0x564f0000, 0x00000000,
  0x00000000, 0x00358dfa, 0xca880000, 0x00000000, 0x00000001,
  0x3406728f, 0xce000000, 0x00000000, 0x000003b9, 0x42fa4143
};

Challenge challenge_generate(uint8_t sec_param);
void challenge_free(Challenge c);
void lapin_pimapping_reduc(const Poly *f, const Challenge c, uint8_t sec_param);
Poly* poly_pimapping_reduc(const Poly **f, uint8_t m, const Challenge c, uint8_t sec_param);
Challenge lapin_reader_step1(uint8_t sec_param);
void lapin_tag_step2(const Key *key, const Poly *f, const Challenge c, Poly **z,
                    Poly **r, double tau, uint8_t sec_param, uint32_t ***table);
int lapin_reader_step3(const Key *key, const Poly *f, const Challenge c,
                       const Poly *z, const Poly *r, double tau1,
                       uint8_t sec_param, uint32_t ***table);
Key* key_generate(const Poly *f);
void key_free(Key *k);
#endif
