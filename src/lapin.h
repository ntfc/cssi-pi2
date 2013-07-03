#ifndef __LAPIN_H__
#define __LAPIN_H__

#include <stdint.h>
#include "poly.h"

#define SEC_PARAM 80

#define REDUCIBLE 1
#define IRREDUCIBLE 0

typedef uint32_t *Challenge;
typedef struct s_key {
  Poly *s1, *s2;
} Key;

// how to alloc this: http://stackoverflow.com/questions/9691404/how-to-initialize-const-in-a-struct-in-c-with-malloc
typedef struct s_lapin {
  const double tau, tau2;
  const uint16_t sec_param;
  const uint16_t n;
  Key *key;
  const uint8_t reduc; // 0 = irreducible, 1 = reducible
  const union mod_poly {
    Poly *normal;
    PolyCRT *crt; // if reduc=1, irrec and reduc are set. otherwise, only irre is set
  } f;
} Lapin;

static uint32_t F_IRREDUCIBLE[17] = {
  0x100000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3
}; // x^532 + x + 1

// declare x^532 + x + 1
// why the &(Poly)? see http://stackoverflow.com/a/11709950/1975046
static Poly *f_irreducible = &(Poly){.n_words = 17, .vec = F_IRREDUCIBLE};

static uint32_t F_PROD_REDUCIBLE[5][4] = { // = Poly[5]
  { 0x80000000, 0x0, 0x0, 0x189 }, // x^127+x^8+x^7+x^3+1
  { 0x40000000, 0x0, 0x0, 0x261 }, // x^126+x^9+x^6+x^5+1
  { 0x20000000, 0x0, 0x0, 0x291 }, // x^125+x^6+x^7+x^4+1
  { 0x04000000, 0x0, 0x0, 0x099 }, // x^122+x^7+x^4+x^3+1
  { 0x02000000, 0x0, 0x0, 0x123 }  // x^121+x^8+x^5+ x +1
};

//static const Poly *f_reducible_crt_1 = &(Poly){.m = 127, .t = 4, .s = 1, .vec = F_PROD_REDUCIBLE[0]};
static Poly *f_reducible_crt_vec[5] = {
  &(Poly){.n_words = 4, .vec = F_PROD_REDUCIBLE[0]},
  &(Poly){.n_words = 4, .vec = F_PROD_REDUCIBLE[1]},
  &(Poly){.n_words = 4, .vec = F_PROD_REDUCIBLE[2]},
  &(Poly){.n_words = 4, .vec = F_PROD_REDUCIBLE[3]},
  &(Poly){.n_words = 4, .vec = F_PROD_REDUCIBLE[4]}
};
static PolyCRT *f_reducible_crt = &(PolyCRT){.m = 5, .crt = f_reducible_crt_vec };

// pre-computed f reducible
static uint32_t F_REDUCIBLE[20] = { // degree = 621
  0x00002000, 0x00000000, 0x00000000, 0x00000000, 0x153bc000,
  0x00000000, 0x00000000, 0x00000392, 0x564f0000, 0x00000000,
  0x00000000, 0x00358dfa, 0xca880000, 0x00000000, 0x00000001,
  0x3406728f, 0xce000000, 0x00000000, 0x000003b9, 0x42fa4143
};
// This could be used in situations where one needs to calculate the f in the reducible case
static Poly *f_reducible = &(Poly){.n_words = 20, .vec = F_REDUCIBLE};



Lapin* lapin_init(uint8_t reduc);

void lapin_end(Lapin *l);

Challenge challenge_generate(uint8_t sec_param);
void challenge_free(Challenge c);

PolyCRT* lapin_pimapping_reduc(Lapin *lapin, const Challenge c);
Poly* lapin_pimapping_irreduc(const Lapin *lapin, const Challenge c);

// writes to r and z
/*int8_t lapin_tag(const Lapin *lapin, const Challenge c, Poly **r, Poly **z);
int8_t lapin_vrfy(const Lapin *lapin, const Challenge c, const Poly *r, const Poly *z);

*/
/*Challenge lapin_reader_step1(uint8_t sec_param);
void lapin_tag_step2(const Key *key, const Poly *f, const Challenge c, Poly **z,
                    Poly **r, double tau, uint8_t sec_param, uint32_t ***table);
int lapin_reader_step3(const Key *key, const Poly *f, const Challenge c,
                       const Poly *z, const Poly *r, double tau1,
                       uint8_t sec_param, uint32_t ***table);*/
Key* key_generate(const Poly *f);
void key_free(Key *k);
#endif
