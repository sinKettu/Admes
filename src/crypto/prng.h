#pragma once
//
// Created by sin on 24.03.18.
//

#ifndef OCCULUS_PRNG_H
#define OCCULUS_PRNG_H

#endif //OCCULUS_PRNG_H

#if defined(_WIN32) || defined(_WIN64)
    #include "libs/mpir/mpir.h"
#else
    #include <mpir.h>
#endif

struct Point
{
    mpz_t x;
    mpz_t y;
};
//
// Генератор псевдо-случайных чисел
// Предположительно, криптостойкий
//
    void EPNG_init(int, mpz_t);
    bool EPNG_inited();
    bool EPNG_get(mpz_t);
    void EPNG_delete();
//
void simple_numbers_list(int n=100000);
void typing_random(mpz_t);
void get_primary_rand(unsigned short, mpz_t, mpz_t, mpz_t, bool );
