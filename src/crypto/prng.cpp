//
// Created by sin on 26.03.18.
//
#include <ctime>
#include <iostream>
#include <cmath>
#include <mpir.h>
#include <vector>
#include <chrono>
//#include <ncurses.h>

#include "prng.h"

using namespace std;
typedef chrono::time_point<chrono::high_resolution_clock> ticks;

vector<int> sn_lst;

Point general;
bool epng_inited = false;
mpz_t _seed;
mpz_t p, a, b, q;
char ellipticArray[15][6][133]
        {
                {"db7c2abf62e35e668076bead208b\0", "db7c2abf62e35e668076bead2088\0", "659ef8ba043916eede8911702b22\0", "db7c2abf62e35e7628dfac6561c5\0", "409487239995a5ee76b55f9c2f09\0", "8a89ce5af8724c0a23e0e0ff77500\0"},
                {"db7c2abf62e35e668076bead208b\0", "6127c24c05f38a0aaaf65c0ef02c\0", "51def1815db5ed74fcc34c85d709\0", "36df0aafd8b8d7597ca10520d04b\0", "4ba30ab5e892b4e1649dd0928643\0", "adcd46f5882e3747def36e956e97\0"},
                {"fffffffdffffffffffffffffffffffff\0" , "fffffffdfffffffffffffffffffffffc\0", "e87579c11079f43dd824993c2cee5ed3\0", "fffffffe0000000075a30d1b9038a115\0", "161ff7528b899b2d0c28607ca52c5b86\0", "cf5ac8395bafeb13c02da292dded7a83\0"},
                {"fffffffdffffffffffffffffffffffff\0", "d6031998d1b3bbfebf59cc9bbff9aee1\0", "5eeefca380d02919dc2c6558bb6d8a5d\0", "3fffffff7fffffffbe0024720613b5a3\0", "7b6aa5d85e572983e6fb32a7cdebc140\0", "27b6916a894d3aee7106fe805fc34b44\0"},
                {"fffffffffffffffffffffffffffffffeffffac73\0", "0\0", "7\0", "100000000000000000001b8fa16dfab9aca16b6b3\0", "3b4c382ce37aa192a4019e763036f4f5dd4d7ebb\0", "938cf935318fdced6bc28286531733c3f03c4fee\0"},
                {"ffffffffffffffffffffffffffffffff7fffffff\0", "ffffffffffffffffffffffffffffffff7ffffffc\0", "1c97befc54bd7a8b65acf89f81d4d4adc565fa45\0", "100000000000000000001f4c8f927aed3ca752257\0", "4a96b5688ef573284664698968c38bb913cbfc82\0", "23a628553168947d59dcc912042351377ac5fb32\0"},
                {"fffffffffffffffffffffffffffffffeffffac73\0", "fffffffffffffffffffffffffffffffeffffac70\0", "b4e134d3fb59eb8bab57274904664d5af50388ba\0", "100000000000000000000351ee786a818f3a1a16b\0", "52dcb034293a117e1f4ff11b30f7199d3144ce6d\0", "feaffef2e331f296e071fa0df9982cfea7d43f2e\0"},
                {"fffffffffffffffffffffffffffffffffffffffeffffee37\0", "0\0", "3\0", "fffffffffffffffffffffffe26f2fc170f69466a74defd8d\0", "db4ff10ec057e9ae26b07d0280b7f4341da5d1b1eae06c7d\0", "9b2f2f6d9c5628a7844163d015be86344082aa88d95e2f9d\0"},
                {"fffffffffffffffffffffffffffffffeffffffffffffffff\0", "fffffffffffffffffffffffffffffffefffffffffffffffc\0", "64210519e59c80e70fa7e9ab72243049feb8deecc146b9b1\0", "ffffffffffffffffffffffff99def836146bc9b1b4d22831\0", "188da80eb03090f67cbf20eb43a18800f4ff0afd82ff1012\0", "7192b95ffc8da78631011ed6b24cdd573f977a11e794811\0"},
                {"fffffffffffffffffffffffffffffffffffffffffffffffeffffe56d\0", "0\0", "5\0", "10000000000000000000000000001dce8d2ec6184caf0a971769fb1f7\0", "a1455b334df099df30fc28a169a467e9e47075a90f7e650eb6b7a45c\0", "7e089fed7fba344282cafbd6f7e319f7c0b0bd59e2ca4bdb556d61a5\0"},
                {"ffffffffffffffffffffffffffffffff000000000000000000000001\0", "fffffffffffffffffffffffffffffffefffffffffffffffffffffffe\0", "b4050a850c04b3abf54132565044b0b7d7bfd8ba270b39432355ffb4\0", "ffffffffffffffffffffffffffff16a2e0b8f03e13dd29455c5c2a3d\0", "b70e0cbd6bb4bf7f321390b94a03c1d356c21122343280d6115c1d21\0", "bd376388b5f723fb4c22dfe6cd4375a05a07476444d5819985007e34\0"},
                {"fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f\0", "0\0", "7\0", "fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141\0", "79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798\0", "483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8\0"},
                {"ffffffff00000001000000000000000000000000ffffffffffffffffffffffff\0", "ffffffff00000001000000000000000000000000fffffffffffffffffffffffc\0", "5ac635d8aa3a93e7b3ebbd55769886bc651d06b0cc53b0f63bce3c3e27d2604b\0", "ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551\0", "6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296\0", "4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5\0"},
                {"fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffeffffffff0000000000000000ffffffff\0", "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffeffffffff0000000000000000fffffffc\0", "b3312fa7e23ee7e4988e056be3f82d19181d9c6efe8141120314088f5013875ac656398d8a2ed19d2a85c8edd3ec2aef\0", "ffffffffffffffffffffffffffffffffffffffffffffffffc7634d81f4372ddf581a0db248b0a77aecec196accc52973\0", "aa87ca22be8b05378eb1c71ef320ad746e1d3b628ba79b9859f741e082542a385502f25dbf55296c3a545e3872760ab7\0", "3617de4a96262c6f5d9e98bf9292dc29f8f41dbd289a147ce9da3113b5f0b8c00a60b1ce1d7e819d7a431d7c90ea0e5f\0"},
                {"1ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff\0", "1fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffc\0", "51953eb9618e1c9a1f929a21a0b68540eea2da725b99b315f3b8b489918ef109e156193951ec7e937b1652c0bd3bb1bf073573df883d2c34f1ef451fd46b503f00\0", "1fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffa51868783bf2f966b7fcc0148f709a5d03bb5c9b8899c47aebb6fb71e91386409\0", "c6858e06b70404e9cd9e3ecb662395b4429c648139053fb521f828af606b4d3dbaa14b5e77efe75928fe1dc127a2ffa8de3348b3c1856a429bf97e7e31c2e5bd66\0", "11839296a789a3bc0045c8a5fb42c7d1bd998f54449579b446817afbd17273e662c97ee72995ef42640c550b9013fad0761353c7086a272c24088be94769fd16650\0"}
        };



Point General()
{
    return general;
}

Point p_copy(Point pnt)
{
    Point newPoint;
    mpz_init_set(newPoint.x, pnt.x);
    mpz_init_set(newPoint.y, pnt.y);

    return newPoint;
}


// List of simple numbers between 2 and n
// inline
void simple_numbers_list(int n)
{
    sn_lst.clear();
    sn_lst.push_back(2);
    bool flag = false;
    for (int i = 3; i <= n; i += 2)
    {
        if (i > 10 && i % 5 == 0)
            continue;

        for (int j = 0; j < sn_lst.size(); j++)
        {
            if (sn_lst[j] * sn_lst[j] - 1 > i)
            {
                sn_lst.push_back(i);
                break;
            }

            if (i % sn_lst[j] == 0)
            {
                flag = true;
                break;
            }
        }

        if (!flag)
        {
            flag = false;
            sn_lst.push_back(i);
        }
    }
}

void typing_random(mpz_t result)
{
    ticks t1 = chrono::high_resolution_clock::now();
    long tmp_si;
    mpz_t tmp;
    mpz_init(tmp);
    mpz_set_ui(result, 0);
    cout << "Press <ENTER> for 4 times" <<  endl;

    for (int i = 0; i < 4; i++)
    {
        ticks t2 = chrono::high_resolution_clock::now();
        tmp_si = chrono::duration_cast<chrono::nanoseconds>(t2 - t1).count();
        t1 = chrono::high_resolution_clock::now();
        tmp_si *= clock() ^ tmp_si;
        tmp_si ^= rand();
        cout << i + 1 << "\t";
        cin.get();

        mpz_set_ui(tmp, tmp_si);
        mpz_mul_2exp(tmp, tmp, 32 * i);
        mpz_add(result, tmp, result);
    }

    cout << endl << endl;
}

// Summing points on elliptic curve
void _plus(Point p1, Point p2, Point *result)
{
    mpz_t zero;
    mpz_init(zero);
    mpz_set_ui(zero, 0);

    if (mpz_cmp(p1.x, zero) == 0 && mpz_cmp(p1.y, zero) == 0)
    {
        mpz_set((*result).x, p2.x);
        mpz_set((*result).y, p2.y);

        mpz_clear(zero);

        return;
    }
    if (mpz_cmp(p2.x, zero)==0 && mpz_cmp(p2.y, zero) == 0)
    {
        mpz_set((*result).x, p1.x);
        mpz_set((*result).y, p1.y);

        mpz_clear(zero);

        return;
    }

    if (mpz_cmp(p1.x, p2.x) != 0)
    {
        mpz_t g, s, t, temp;
        mpz_init(temp);
        mpz_sub(temp, p2.x,p1.x);
        mpz_init(g); mpz_init(s); mpz_init(t);
        mpz_gcdext(g, s, t, temp, p);

        mpz_clear(g);
        mpz_clear(t);

        if (mpz_cmp(s, zero) < 0)
        {
            mpz_add(s, s, p);
        }

        mpz_sub(temp, p2.y, p1.y);
        mpz_mul(temp, temp, s);
        mpz_mod(temp, temp, p); //Модуль может быть отрицательным!!!

        mpz_mul((*result).x, temp, temp);
        mpz_sub((*result).x, (*result).x, p1.x);
        mpz_sub((*result).x, (*result).x, p2.x);
        mpz_mod((*result).x, (*result).x, p);

        mpz_sub((*result).y, p1.x, (*result).x);
        mpz_mul((*result).y, (*result).y, temp);
        mpz_sub((*result).y, (*result).y, p1.y);
        mpz_mod((*result).y, (*result).y, p);

        mpz_clear(temp);
        mpz_clear(s);
        mpz_clear(zero);

        return;
    }
    if (mpz_cmp(p1.x, p2.x) == 0 && mpz_cmp(p1.y, p2.y) == 0 && mpz_cmp(p1.y, zero) != 0)
    {
        mpz_t g, s, temp;
        mpz_init_set_ui(temp, 2);
        mpz_mul(temp, p1.y, temp);
        mpz_init(g);
        mpz_init(s);
        mpz_gcdext(g, s, temp, temp, p);
        mpz_clear(g);

        if (mpz_cmp(s, zero) < 0)
        {
            mpz_add(s, s, p);
        }

        mpz_set_ui(temp, 3);
        mpz_mul(temp, p1.x, p1.x);
        mpz_add(temp, temp, a);
        mpz_mul(temp, temp, s);
        mpz_mod(temp, temp, p);

        mpz_mul((*result).x, temp, temp);
        mpz_sub((*result).x, (*result).x, p1.x);
        mpz_sub((*result).x, (*result).x, p1.x);
        mpz_mod((*result).x, (*result).x, p);

        mpz_sub((*result).y, p1.x, (*result).x);
        mpz_mul((*result).y, temp, (*result).y);
        mpz_sub((*result).y,(*result).y, p1.y);
        mpz_mod((*result).y, (*result).y, p);

        mpz_clear(temp);
        mpz_clear(s);
        mpz_clear(zero);

        return;
    }

    mpz_sub((*result).y, p, p2.y);

    if(mpz_cmp(p1.x, p2.x) == 0 && mpz_cmp((*result).y, p1.y) == 0)
    {
        mpz_set((*result).x, zero);
        mpz_set((*result).y, zero);
        mpz_clear(zero);
    }

}

// Multipling point and number on elliptic curve. Binomial
void _binmul(Point pnt, mpz_t k, Point &result)
{
    mpz_t temp;
    mpz_init_set_ui(temp, 1);
    mpz_and(temp, k, temp);

    if (mpz_cmp_si(temp, 0) == 0)
    {
        mpz_set(result.x, pnt.x);
        mpz_set(result.y, pnt.y);
    }
    else
    {
        mpz_set_si(result.x, 0);
        mpz_set_si(result.y, 0);
    }

    mpz_div_ui(k, k , 2);
    while (mpz_cmp_si(k, 0) != 0)
    {
        _plus(p_copy(pnt), p_copy(pnt), &pnt);
        mpz_mod_ui(temp, k, 2);

        if(mpz_cmp_si(temp, 1) == 0)
        {
            _plus(p_copy(result), p_copy(pnt), &result);
        }

        mpz_div_ui(k, k, 2);
    }

    mpz_clear(temp);
}

// Initializing of EC
void EPNG_init(int n, mpz_t seed)
{
    if (epng_inited)
        EPNG_delete();

    mpz_init(p); mpz_init(a); mpz_init(b); mpz_init(q); mpz_init(general.x); mpz_init(general.y);
    mpz_set_str(p,  ellipticArray[n][0], 16);
    mpz_set_str(a,  ellipticArray[n][1], 16);
    mpz_set_str(b,  ellipticArray[n][2], 16);
    mpz_set_str(q,  ellipticArray[n][3], 16);

    mpz_set_str(general.x, ellipticArray[n][4], 16);
    mpz_set_str(general.y, ellipticArray[n][5], 16);

    mpz_mod(seed, seed, p);
    mpz_init_set(_seed, seed);

    epng_inited = true;
}

void EPNG_delete()
{
    mpz_clear(p);
    mpz_clear(a);
    mpz_clear(b);
    mpz_clear(q);
    mpz_clear(general.x);
    mpz_clear(general.y);

    epng_inited = false;
}

void _point_out(Point pnt)
{
    mpz_out_str(stdout, 16, pnt.x); cout << endl;
    mpz_out_str(stdout, 16, pnt.y); cout << endl;
}

// Getting one PRN according to initializing
bool EPNG_get(mpz_t result)
{
    if (epng_inited)
    {
        Point next;
        mpz_init(next.x);
        mpz_init(next.y);

        _binmul(p_copy(general), _seed, next);
		
		mpz_xor(result, next.x, next.y);
		mpz_set(_seed, next.y);

        mpz_clear(next.x);
        mpz_clear(next.y);

        return false;
    }

    return true;
}

bool EPNG_inited()
{
    return epng_inited;
}

void get_primary_root(mpz_t root, mpz_t fut, mpz_t s)
{
    EPNG_get(root);
    mpz_mod(root, root, fut);
    mpz_t tmp;
    mpz_init(tmp);

    while (mpz_cmp_ui(root, 0) > 0)
    {
        bool flag = true;
        mpz_powm(tmp, root, s, fut);

        if (mpz_cmp_ui(tmp, 1) != 0)
        {
            for (int i = 0; i < sn_lst.size(); i++)
            {
                mpz_set_ui(tmp, sn_lst[i]);
                mpz_mul(tmp, tmp, tmp);

                if (mpz_cmp(tmp, s) > 0)
                {
                    break;
                }

                mpz_mod_ui(tmp, s, sn_lst[i]);
                if (mpz_cmp_ui(tmp, 0) == 0)
                {
                    mpz_sub_ui(tmp, fut, 1);
                    mpz_div_ui(tmp, tmp, sn_lst[i]);
                    mpz_powm(tmp, root, tmp, fut);

                    if (mpz_cmp_ui(tmp, 1) == 0)
                    {
                        flag = false;
                        break;
                    }
                }
            }

            if (flag)
                break;
        }

        mpz_sub_ui(root, root, 1);
    }
}

void get_primary_rand(unsigned short from /* 2^from */, mpz_t fut_simple, mpz_t cur_simple, mpz_t root, bool needRoot)
{
    mpz_t tmp;
    mpz_init(tmp);
    long tmp_si;

    // Инициализация ГПСЧ на ЕК,
    // если это еще не сделано

    if (! epng_inited)
    {
        mpz_t seed;
        mpz_init_set_ui(seed, 0);
        ticks t1 = chrono::high_resolution_clock::now();

        for (int i = 0; i < 16; i++)
        {
            ticks t2 = chrono::high_resolution_clock::now();
            tmp_si = chrono::duration_cast<chrono::nanoseconds>(t2 - t1).count();
            t1 = chrono::high_resolution_clock::now();
            mpz_set_ui(tmp, tmp_si);
            mpz_mul_2exp(tmp, tmp, 8 * i);
            mpz_xor(seed, seed, tmp);
        }

        EPNG_init(13, seed);
        mpz_clear(seed);
    }

    // Заполнить список предпосчитанных простых чисел,
    // если он пуст

    if (sn_lst.empty())
    {
        simple_numbers_list();
    }

    // Если не имеется простого числа, подсчитанного ранее,
    // будет взято случайное значение из списка предпосчитанных

    if (mpz_cmp_ui(cur_simple, 0) == 0)
    {
        EPNG_get(tmp);
        mpz_mod_ui(tmp, tmp, sn_lst.size());
        tmp_si = static_cast<int>(mpz_get_si(tmp));
        mpz_set_ui(cur_simple, sn_lst[tmp_si]);
        mpz_set_ui(tmp, 1);
        mpz_mul_2exp(tmp, tmp, from);

        while (mpz_cmp(cur_simple, tmp) > 0 || mpz_cmp_si(cur_simple, 2) == 0)
        {
            EPNG_get(tmp);
            mpz_mod_ui(tmp, tmp, sn_lst.size());
            tmp_si = static_cast<int>(mpz_get_si(tmp));
            mpz_set_ui(cur_simple, sn_lst[tmp_si]);
            mpz_set_ui(tmp, 1);
            mpz_mul_2exp(tmp, tmp, from);
        }
    }

    // Генерация и тестирование следующего
    // возможного простого числа, согласно
    // малой теореме Ферма

    mpz_set_ui(fut_simple, 0);
    mpz_t s;        // random even-numbered
    mpz_init(s);
    EPNG_get(s);
    mpz_mod_ui(tmp, s, 2);
    if (mpz_cmp_ui(tmp, 0) != 0)
        mpz_add_ui(s, s, 7);

    while (mpz_cmp_ui(fut_simple, 0) == 0)
    {
        mpz_add_ui(s, s, 2);
        mpz_mul(fut_simple, cur_simple, s);
        mpz_add_ui(fut_simple, fut_simple, 1);
        //
        //  Проверка полученного числа на делимость на одно
        //  из предпосчитанных чисел. На данный момент не актуально
        //
        //  for (tmp_si = 0; tmp_si < sn_lst.size(); tmp_si++)
        //  {
        //      mpz_mod_ui(tmp, fut_simple, sn_lst[tmp_si]);
        //      if (mpz_cmp_ui(tmp, 0) == 0 && mpz_cmp_ui(fut_simple, sn_lst[tmp_si]))
        //      {
        //          mpz_set_ui(fut_simple, 0);
        //          break;
        //      }
        //  }
        //
        //                        Вместо проверки по списку используется алг. Миллера-Рабина
        //
        if (mpz_cmp_ui(fut_simple, 0) != 0 && mpz_probab_prime_p(fut_simple, 6) == 0)
        {
            mpz_set_ui(fut_simple, 0);
        }

        if (mpz_cmp_ui(fut_simple, 0) != 0)
        {
            mpz_t test_value;
            mpz_init(test_value);
            for (tmp_si = 0; tmp_si < 6; tmp_si++)
            {
                EPNG_get(test_value);
                mpz_sub_ui(tmp, fut_simple, 1);
                mpz_powm(tmp, test_value, tmp, fut_simple);

                if (mpz_cmp_ui(tmp, 1) != 0)
                {
                    mpz_set_ui(fut_simple, 0);
                    break;
                }
            }

            mpz_clear(test_value);
        }
    }

    mpz_set_ui(tmp, 1);
    mpz_mul_2exp(tmp, tmp, from);
    if (mpz_cmp(fut_simple, tmp) < 0)
    {
        mpz_set(cur_simple, fut_simple);
        get_primary_rand(from, fut_simple, cur_simple, root, needRoot);
    }

    else
    {
        // Генерация примитивного корня, если надо

        if (needRoot)
            get_primary_root(root, fut_simple, s);
    }

}