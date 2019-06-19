#include <ctime>
#include <iostream>
#include <cmath>
#include <mpir.h>
#include <vector>
#include <chrono>

#include "prng.h"

#define NANO_NOW() std::chrono::high_resolution_clock::now()
#define NANO_DIST(t1, t2) std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count()

typedef std::chrono::time_point<std::chrono::high_resolution_clock> ticks;

EllipticCurve *epng_ec;
bool epng_inited = false;
mpz_t _seed;

void typing_random(mpz_t result)
{
    ticks t1 = NANO_NOW();
    long tmp_si;
    mpz_t tmp;
    mpz_init(tmp);
    mpz_set_ui(result, 0);
    std::cout << "Press <ENTER> for 4 times" <<  std::endl;

    for (int i = 0; i < 4; i++)
    {
        ticks t2 = NANO_NOW();
        tmp_si = NANO_DIST(t1, t2);
        t1 = NANO_NOW();
        tmp_si *= clock() ^ tmp_si;
        tmp_si ^= rand();
        std::cout << i + 1 << "\t";
        std::cin.get();

        mpz_set_ui(tmp, tmp_si);
        mpz_mul_2exp(tmp, tmp, 32 * i);
        mpz_add(result, tmp, result);
    }

    std::cout << std::endl << std::endl;

    mpz_clear(tmp);
}

// Initializing of EC
void EPNG_init(int n, mpz_t seed)
{
    if (epng_inited)
        return;

    epng_ec = ec_init(n);
    if (epng_ec == nullptr)
        return;

    mpz_mod(seed, seed, epng_ec->p);
    mpz_init_set(_seed, seed);

    epng_inited = true;
}

void EPNG_delete()
{
    mpz_clear(_seed);
    ec_deinit(epng_ec);
    epng_inited = false;
}

// Getting one PRN according to initializing
bool EPNG_get(mpz_t &result)
{
    if (epng_inited)
    {
        Point next;
        mpz_init(next.x);
        mpz_init(next.y);

        pntMul(epng_ec, epng_ec->G, _seed, next);
		
		mpz_xor(result, next.x, next.y);
		mpz_add_ui(_seed, next.y, 1);

        mpz_clear(next.x);
        mpz_clear(next.y);

        return true;
    }

    return false;
}

bool EPNG_inited()
{
    return epng_inited;
}
