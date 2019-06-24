#ifndef OCCULUS_PRNG_H
#define OCCULUS_PRNG_H

#if defined(_WIN32) || defined(_WIN64)
    #include "libs/mpir/mpir.h"
#elif MPIR_H
    #include <mpir.h>
#else
    #include <gmp.h>
#endif

#include "EC_common.h"

//
// PRNG based on ECC
//
void EPNG_init(int, mpz_t seed);
bool EPNG_inited();
bool EPNG_get(mpz_t &);
void EPNG_delete();
//
// random number based on time of typing
//
void typing_random(mpz_t result);

#endif //OCCULUS_PRNG_H