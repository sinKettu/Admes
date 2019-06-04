//
// Created by sin on 06.11.18.
//
#include "ECC.h"
#include <iostream>
using namespace std;

#define ELLIPTIC_CURVES_COUNT 15

typedef unsigned char byte;
typedef struct
{
    mpz_t a;
    mpz_t b;
    mpz_t p;
    mpz_t q;

    Point G;

    bool exists = false;

} EllipticCurve;

// 15 эллиптических кривых со всеми необходимыми параметрами
// Взяты из "STANDARDS FOR EFFICIENT CRYPTOGRAPHY (SEC2)"
// p, a, b, q, X-point, Y-point
char ecc__ellipticArray[15][6][133]
        {
                // secp112r1
                {"db7c2abf62e35e668076bead208b\0", "db7c2abf62e35e668076bead2088\0", "659ef8ba043916eede8911702b22\0", "db7c2abf62e35e7628dfac6561c5\0", "409487239995a5ee76b55f9c2f09\0", "8a89ce5af8724c0a23e0e0ff77500\0"},
                // secp112r2
                {"db7c2abf62e35e668076bead208b\0", "6127c24c05f38a0aaaf65c0ef02c\0", "51def1815db5ed74fcc34c85d709\0", "36df0aafd8b8d7597ca10520d04b\0", "4ba30ab5e892b4e1649dd0928643\0", "adcd46f5882e3747def36e956e97\0"},
                // secp128r1
                {"fffffffdffffffffffffffffffffffff\0" , "fffffffdfffffffffffffffffffffffc\0", "e87579c11079f43dd824993c2cee5ed3\0", "fffffffe0000000075a30d1b9038a115\0", "161ff7528b899b2d0c28607ca52c5b86\0", "cf5ac8395bafeb13c02da292dded7a83\0"},
                // secp128r2
                {"fffffffdffffffffffffffffffffffff\0", "d6031998d1b3bbfebf59cc9bbff9aee1\0", "5eeefca380d02919dc2c6558bb6d8a5d\0", "3fffffff7fffffffbe0024720613b5a3\0", "7b6aa5d85e572983e6fb32a7cdebc140\0", "27b6916a894d3aee7106fe805fc34b44\0"},
                // secp160k1
                {"fffffffffffffffffffffffffffffffeffffac73\0", "0\0", "7\0", "100000000000000000001b8fa16dfab9aca16b6b3\0", "3b4c382ce37aa192a4019e763036f4f5dd4d7ebb\0", "938cf935318fdced6bc28286531733c3f03c4fee\0"},
                // secp160r1
                {"ffffffffffffffffffffffffffffffff7fffffff\0", "ffffffffffffffffffffffffffffffff7ffffffc\0", "1c97befc54bd7a8b65acf89f81d4d4adc565fa45\0", "100000000000000000001f4c8f927aed3ca752257\0", "4a96b5688ef573284664698968c38bb913cbfc82\0", "23a628553168947d59dcc912042351377ac5fb32\0"},
                // secp160r2
                {"fffffffffffffffffffffffffffffffeffffac73\0", "fffffffffffffffffffffffffffffffeffffac70\0", "b4e134d3fb59eb8bab57274904664d5af50388ba\0", "100000000000000000000351ee786a818f3a1a16b\0", "52dcb034293a117e1f4ff11b30f7199d3144ce6d\0", "feaffef2e331f296e071fa0df9982cfea7d43f2e\0"},
                // secp192k1
                {"fffffffffffffffffffffffffffffffffffffffeffffee37\0", "0\0", "3\0", "fffffffffffffffffffffffe26f2fc170f69466a74defd8d\0", "db4ff10ec057e9ae26b07d0280b7f4341da5d1b1eae06c7d\0", "9b2f2f6d9c5628a7844163d015be86344082aa88d95e2f9d\0"},
                // secp192r1
                {"fffffffffffffffffffffffffffffffeffffffffffffffff\0", "fffffffffffffffffffffffffffffffefffffffffffffffc\0", "64210519e59c80e70fa7e9ab72243049feb8deecc146b9b1\0", "ffffffffffffffffffffffff99def836146bc9b1b4d22831\0", "188da80eb03090f67cbf20eb43a18800f4ff0afd82ff1012\0", "7192b95ffc8da78631011ed6b24cdd573f977a11e794811\0"},
                // secp224k1
                {"fffffffffffffffffffffffffffffffffffffffffffffffeffffe56d\0", "0\0", "5\0", "10000000000000000000000000001dce8d2ec6184caf0a971769fb1f7\0", "a1455b334df099df30fc28a169a467e9e47075a90f7e650eb6b7a45c\0", "7e089fed7fba344282cafbd6f7e319f7c0b0bd59e2ca4bdb556d61a5\0"},
                // secp224r1
                {"ffffffffffffffffffffffffffffffff000000000000000000000001\0", "fffffffffffffffffffffffffffffffefffffffffffffffffffffffe\0", "b4050a850c04b3abf54132565044b0b7d7bfd8ba270b39432355ffb4\0", "ffffffffffffffffffffffffffff16a2e0b8f03e13dd29455c5c2a3d\0", "b70e0cbd6bb4bf7f321390b94a03c1d356c21122343280d6115c1d21\0", "bd376388b5f723fb4c22dfe6cd4375a05a07476444d5819985007e34\0"},
                // secp256k1
                {"fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f", "0", "7", "fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141", "79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798", "483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8"},
                // secp256r1
                {"ffffffff00000001000000000000000000000000ffffffffffffffffffffffff\0", "ffffffff00000001000000000000000000000000fffffffffffffffffffffffc\0", "5ac635d8aa3a93e7b3ebbd55769886bc651d06b0cc53b0f63bce3c3e27d2604b\0", "ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551\0", "6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296\0", "4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5\0"},
                // secp384r1
                {"fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffeffffffff0000000000000000ffffffff\0", "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffeffffffff0000000000000000fffffffc\0", "b3312fa7e23ee7e4988e056be3f82d19181d9c6efe8141120314088f5013875ac656398d8a2ed19d2a85c8edd3ec2aef\0", "ffffffffffffffffffffffffffffffffffffffffffffffffc7634d81f4372ddf581a0db248b0a77aecec196accc52973\0", "aa87ca22be8b05378eb1c71ef320ad746e1d3b628ba79b9859f741e082542a385502f25dbf55296c3a545e3872760ab7\0", "3617de4a96262c6f5d9e98bf9292dc29f8f41dbd289a147ce9da3113b5f0b8c00a60b1ce1d7e819d7a431d7c90ea0e5f\0"},
                // secp521r1
                {"1ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff\0", "1fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffc\0", "51953eb9618e1c9a1f929a21a0b68540eea2da725b99b315f3b8b489918ef109e156193951ec7e937b1652c0bd3bb1bf073573df883d2c34f1ef451fd46b503f00\0", "1fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffa51868783bf2f966b7fcc0148f709a5d03bb5c9b8899c47aebb6fb71e91386409\0", "c6858e06b70404e9cd9e3ecb662395b4429c648139053fb521f828af606b4d3dbaa14b5e77efe75928fe1dc127a2ffa8de3348b3c1856a429bf97e7e31c2e5bd66\0", "11839296a789a3bc0045c8a5fb42c7d1bd998f54449579b446817afbd17273e662c97ee72995ef42640c550b9013fad0761353c7086a272c24088be94769fd16650\0"}
        };

// Структура - текущая эллиптическая кривая
// Выбирается в функции ecc_init_params
// Параметры берутся из списка ellipticArray
EllipticCurve current_elliptic_curve;
Point ecc_public_key;
mpz_t ecc_private_key;
bool puk_inited = false;
bool prk_inited = false;

void current_EC_out()
{
    cout << "Curve:" << endl;
    cout << "p:\t"; mpz_out_str(stdout, 10, current_elliptic_curve.p); cout << endl;
    cout << "a:\t"; mpz_out_str(stdout, 10, current_elliptic_curve.a); cout << endl;
    cout << "b:\t"; mpz_out_str(stdout, 10, current_elliptic_curve.b); cout << endl;
    cout << "q:\t"; mpz_out_str(stdout, 10, current_elliptic_curve.q); cout << endl;
    cout << "Gx:\t"; mpz_out_str(stdout, 10, current_elliptic_curve.G.x); cout << endl;
    cout << "Gy:\t"; mpz_out_str(stdout, 10, current_elliptic_curve.G.y); cout << endl;
    cout << endl;
}

Point point_copy(Point in)
{
    Point out;
    mpz_init_set(out.x, in.x);
    mpz_init_set(out.y, in.y);

    return out;
}

Point point_copy(Point in, Point &
out)
{
    mpz_set(out.x, in.x);
    mpz_set(out.y, in.y);
}

// Удалить и не позориться
// Получение ординаты ЭК по абсциссе ЭК
void point_expand(mpz_t &x, mpz_t &y)
{
    mpz_t tmp;
    mpz_init(tmp);
    mpz_mul(tmp, x, x);
    mpz_mul(tmp, tmp, x);
    mpz_set(y, tmp);

    mpz_mul(tmp, x, current_elliptic_curve.a);
    mpz_add(y, y, tmp);

    mpz_add(y, y, current_elliptic_curve.b);
    mpz_sqrt(y, y);
    mpz_mod(y, y, current_elliptic_curve.p);

    mpz_clear(tmp);
}

// Сложение точек эллиптической кривой
// result = p1 + p2
void sum(Point left, Point right, Point &result)
{
    Point p1, p2;
    mpz_init(p1.x);
    mpz_init(p1.y);
    mpz_init(p2.x);
    mpz_init(p2.y);
    point_copy(left, p1);
    point_copy(right, p2);

    if (! mpz_cmp_ui(left.x, 0) && ! mpz_cmp_ui(left.y, 0))
    {
        mpz_set(result.x, p2.x);
        mpz_set(result.y, p2.y);

    }
    else if (! mpz_cmp_ui(right.x, 0) && ! mpz_cmp_ui(right.y, 0))
    {
        mpz_set(result.x, p1.x);
        mpz_set(result.y, p1.y);
    }
    else if (mpz_cmp(p1.x, p2.x) != 0)
    {
        mpz_t tmp, g, s;
        mpz_init(tmp);
        mpz_init(g);
        mpz_init(s);
        mpz_sub(tmp, p2.x, p1.x);

        mpz_gcdext(g, s, tmp, tmp, current_elliptic_curve.p);
        if (mpz_cmp_ui(s, 0) < 0)
            mpz_add(s, s, current_elliptic_curve.p);

        mpz_sub(tmp, p2.y, p1.y);
        mpz_mul(tmp, tmp, s);
        mpz_mod(tmp, tmp, current_elliptic_curve.p);

        // X result
        mpz_mul(result.x, tmp, tmp);
        mpz_sub(result.x, result.x, p1.x);
        mpz_sub(result.x, result.x, p2.x);
        mpz_mod(result.x, result.x, current_elliptic_curve.p);

        // Y result
        mpz_sub(result.y, p1.x, result.x);
        mpz_mul(result.y, result.y, tmp);
        mpz_sub(result.y, result.y, p1.y);
        mpz_mod(result.y, result.y, current_elliptic_curve.p);

        mpz_clear(tmp);
        mpz_clear(g);
        mpz_clear(s);
    }
    else if (mpz_cmp(p1.x, p2.x) == 0 && mpz_cmp(p1.y, p2.y) == 0 && mpz_cmp_ui(p1.y, 0) != 0)
    {
        mpz_t tmp, s, g, t;
        mpz_init(tmp);
        mpz_init(g);
        mpz_init(s);
        mpz_init(t);

        mpz_mul_ui(tmp, p1.y, 2);
        mpz_gcdext(g, s, t, tmp, current_elliptic_curve.p);
        if (mpz_cmp_ui(s, 0) < 0)
            mpz_add(s, s, current_elliptic_curve.p);

        mpz_mul_ui(tmp, p1.x, 3);
        mpz_mul(tmp, tmp, p1.x);
        mpz_add(tmp, tmp, current_elliptic_curve.a);
        mpz_mul(tmp, tmp, s);
        mpz_mod(tmp, tmp, current_elliptic_curve.p);

        // X result
        mpz_mul_ui(s, p1.x, 2);
        mpz_mul(result.x, tmp, tmp);
        mpz_sub(result.x, result.x, s);
        mpz_mod(result.x, result.x, current_elliptic_curve.p);

        // Y result
        mpz_sub(result.y, p1.x, result.x);
        mpz_mul(result.y, result.y, tmp);
        mpz_sub(result.y, result.y, p1.y);
        mpz_mod(result.y, result.y, current_elliptic_curve.p);

        mpz_clear(tmp);
        mpz_clear(g);
        mpz_clear(s);
    }
    else
    {
        mpz_sub(p2.y, current_elliptic_curve.p, p2.y);
        if (!mpz_cmp(p1.x, p2.x) && !mpz_cmp(p1.y, p2.y))
        {
            mpz_set_ui(result.x, 0);
            mpz_set_ui(result.y, 0);
        }
    }

    mpz_clear(p1.x);
    mpz_clear(p1.y);
    mpz_clear(p2.x);
    mpz_clear(p2.y);
}

// Умножение точки ЭК на число
// result = k*pnt
// O(logN)
void mul(Point left, mpz_t right, Point &result)
{
    mpz_t tmp, k;
    Point pnt;
    mpz_init(k);
    mpz_init(pnt.x);
    mpz_init(pnt.y);
    mpz_init_set_ui(tmp, 1);
    mpz_and(tmp, right, tmp);
    if (mpz_cmp_ui(tmp, 0) == 0)
    {
        mpz_set_ui(result.x, 0);
        mpz_set_ui(result.y, 0);
    }
    else
    {
        mpz_set(result.x, left.x);
        mpz_set(result.y, left.y);
    }

    mpz_div_ui(k, right, 2);
    point_copy(left, pnt);

    while (mpz_cmp_ui(k, 0) > 0)
    {
        sum(pnt, pnt, pnt);
        mpz_set_ui(tmp, 1);
        mpz_and(tmp, k, tmp);
        if (mpz_cmp_ui(tmp, 0) != 0)
            sum(result, pnt, result);

        mpz_div_ui(k, k, 2);
    }
}

int ecc_init_params(byte num)
{
    if (num >= ELLIPTIC_CURVES_COUNT)
        return 1;

    if (current_elliptic_curve.exists)
        return 2;

    mpz_init_set_str(current_elliptic_curve.p, ecc__ellipticArray[num][0], 16);
    mpz_init_set_str(current_elliptic_curve.a, ecc__ellipticArray[num][1], 16);
    mpz_init_set_str(current_elliptic_curve.b, ecc__ellipticArray[num][2], 16);
    mpz_init_set_str(current_elliptic_curve.q, ecc__ellipticArray[num][3], 16);
    mpz_init_set_str(current_elliptic_curve.G.x, ecc__ellipticArray[num][4], 16);
    mpz_init_set_str(current_elliptic_curve.G.y, ecc__ellipticArray[num][5], 16);
    current_elliptic_curve.exists = true;

    return 0;
}

int ecc_delete_params()
{
    if (! current_elliptic_curve.exists)
        return 1;

    mpz_clear(current_elliptic_curve.p);
    mpz_clear(current_elliptic_curve.a);
    mpz_clear(current_elliptic_curve.b);
    mpz_clear(current_elliptic_curve.q);
    mpz_clear(current_elliptic_curve.G.x);
    mpz_clear(current_elliptic_curve.G.y);
    current_elliptic_curve.exists = false;

    return 0;
}

void ecc_mpz_to_cstr(mpz_t a, byte** b, int &len)
{
    len = 0;
    mpz_t tmp;  mpz_init_set_ui(tmp, 1);
    mpz_t xff;  mpz_init_set_ui(xff, 0xff);

    while (mpz_cmp(tmp, a) != 0)
    {
        len++;
        mpz_mod_2exp(tmp, a, len * 8);
    }

    *b = new byte[len];

    for (int i = len - 1; i >= 0; i--)
    {
        mpz_and(tmp, a, xff);
        (*b)[i] = static_cast<byte>(mpz_get_ui(tmp));
        mpz_div_2exp(a, a, 8);
    }

    mpz_clear(tmp);
    mpz_clear(xff);
}

int ecc_cstr_to_mpz(byte* a, int len, mpz_t b)
{
    mpz_t tmp;
    mpz_init(tmp);
    mpz_set_ui(b, 0);

    for (int i = 0; i < len; i++)
    {
        mpz_set_ui(tmp, a[len - i - 1]);
        mpz_mul_2exp(tmp, tmp, i * 8);
        mpz_add(b, b, tmp);
    }

    mpz_clear(tmp);

    if (mpz_cmp(current_elliptic_curve.p, b) < 0)
        return 1;
    else
        return 0;
}

void ecc_keygen()
{
    mpz_init(ecc_private_key);
    mpz_init(ecc_public_key.x);
    mpz_init(ecc_public_key.y);
    if (! EPNG_inited())
    {
        typing_random(ecc_private_key);
        EPNG_init(14, ecc_private_key);
    }

    EPNG_get(ecc_private_key);
    mpz_t tmp;
    mpz_init_set(tmp, ecc_private_key);

    mul(point_copy(current_elliptic_curve.G), tmp, ecc_public_key);
    prk_inited = puk_inited = true;
}

int ecc_encrypt(unsigned char* message, int m_len, ecc_encrypted_data &data)
{
    if (!puk_inited)
        return 1;

    mpz_t tmp;
    mpz_init(tmp);
    if (ecc_cstr_to_mpz(message, m_len, tmp))
        return 2;

    // Представление сообщения в виде точки на ЭК

    Point mes;
    mpz_init_set(mes.x, tmp);
    mpz_init(mes.y);

    // Y^2 = X^3 + a*X + b
    point_expand(mes.x, mes.y);

    if (! EPNG_inited())
    {
        typing_random(tmp);
        EPNG_init(14, tmp);
    }

    // Получение первой (side) части зашифрованного сообщения

    EPNG_get(tmp);
    Point tmp_point;
    mpz_init(tmp_point.x);
    mpz_init(tmp_point.y);
    mul(current_elliptic_curve.G, tmp, tmp_point);
    ecc_mpz_to_cstr(tmp_point.x, &data.side_x, data.s_len_x);
    ecc_mpz_to_cstr(tmp_point.y, &data.side_y, data.s_len_y);

    // Получение второй (general) части зашифрованного сообщения

    mul(ecc_public_key, tmp, tmp_point);
    sum(mes, tmp_point, tmp_point);
    ecc_mpz_to_cstr(tmp_point.x, &data.general_x, data.g_len_x);
    ecc_mpz_to_cstr(tmp_point.y, &data.general_y, data.g_len_y);

    mpz_clear(tmp);
    mpz_clear(tmp_point.x);
    mpz_clear(tmp_point.y);

    return 0;
}

int ecc_decrypt(ecc_encrypted_data data, unsigned char** message, int &m_len)
{
    if (! prk_inited)
        return 1;

    if (!current_elliptic_curve.exists)
        return 2;

    // Загрузка первой (side) части ключа

    Point side;
    mpz_init(side.x);
    mpz_init(side.y);
    ecc_cstr_to_mpz(data.side_x, data.s_len_x, side.x);
    ecc_cstr_to_mpz(data.side_y, data.s_len_y, side.y);

    // Загрузка второй (general) части ключа

    Point general;
    mpz_init(general.x);
    mpz_init(general.y);
    ecc_cstr_to_mpz(data.general_x, data.g_len_x, general.x);
    ecc_cstr_to_mpz(data.general_y, data.g_len_y, general.y);

    // Расшифрование

    mul(side, ecc_private_key, side);
    mpz_sub(side.y, current_elliptic_curve.p, side.y);
    sum(general, side, general);

    ecc_mpz_to_cstr(general.x, message, m_len);
    mpz_clear(side.y);
    mpz_clear(side.x);
    mpz_clear(general.y);
    mpz_clear(general.x);

    return 0;
}

void ecc_test()
{
    mpz_t a, b;
    mpz_init_set_str(a, "123456789123456789123456789123456789", 10);
    mpz_init_set_str(b, "555555555555555555555555555555555557", 10);

    Point res;
    mpz_init(res.x);
    mpz_init(res.y);

    mul(current_elliptic_curve.G, a, res);
    mul(res, b, res);
    mpz_out_str(stdout, 10, res.x); cout << endl;
    mpz_out_str(stdout, 10, res.y); cout << endl;

    mul(current_elliptic_curve.G, b, res);
    mul(res, a, res);
    mpz_out_str(stdout, 10, res.x); cout << endl;
    mpz_out_str(stdout, 10, res.y); cout << endl;
}

int ecc_make_sign(unsigned char* message, int m_len, ecc_signature &signature)
{
    if (! current_elliptic_curve.exists)
        return 1;

    if (! prk_inited)
        return 2;

    // Шаг 1, 2

    mpz_t cur;
    mpz_init(cur);
    ecc_cstr_to_mpz(message, m_len, cur);
    mpz_mod(cur, cur, current_elliptic_curve.q);
    if (mpz_cmp_ui(cur, 0) == 0)
        mpz_set_ui(cur, 1);

    // Шаг 3

    mpz_t tmp, r, s;
    mpz_init_set_ui(r, 0);
    mpz_init_set_ui(s, 0);
    mpz_init_set_ui(tmp, 0);
    while (mpz_cmp_ui(r, 0) == 0 || mpz_cmp_ui(s, 0) == 0)
    {
        if (! EPNG_inited())
        {
            typing_random(tmp);
            EPNG_init(14, tmp);
        }

        EPNG_get(tmp);
        mpz_mod(tmp, tmp, current_elliptic_curve.q);

    // Шаг 4

        Point tmp_point;
        mpz_init(tmp_point.x);
        mpz_init(tmp_point.y);
        mul(current_elliptic_curve.G, tmp, tmp_point);
        mpz_mod(r, tmp_point.x, current_elliptic_curve.q);

    // Шаг 5

        mpz_mul(s, r, ecc_private_key);
        mpz_mul(tmp, tmp, cur);
        mpz_add(s, s, tmp);
        mpz_mod(s, s, current_elliptic_curve.q);

        mpz_clear(tmp_point.x);
        mpz_clear(tmp_point.y);
    }

    // Шаг 6

    ecc_mpz_to_cstr(r, &signature.r, signature.r_len);
    ecc_mpz_to_cstr(s, &signature.s, signature.s_len);

    mpz_clear(tmp);
    mpz_clear(r);
    mpz_clear(s);

    return 0;
}

int ecc_check_sign(ecc_signature signature, byte* hash, int h_len)
{
    if (! current_elliptic_curve.exists)
        return 2;
    if (! puk_inited)
        return 3;

    // Шаг 1

    mpz_t r, s;
    mpz_init(r);
    mpz_init(s);
    ecc_cstr_to_mpz(signature.r, signature.r_len, r);
    ecc_cstr_to_mpz(signature.s, signature.s_len, s);

    // Шаг 2

    mpz_t cur;
    mpz_init(cur);
    ecc_cstr_to_mpz(hash, h_len, cur);
    mpz_mod(cur, cur, current_elliptic_curve.q);
    if (mpz_cmp_ui(cur, 0) == 0)
        mpz_set_ui(cur, 1);

    // Шаг 4

    mpz_invert(cur, cur, current_elliptic_curve.q);

    // Шаг 5, 6

    mpz_t z;
    mpz_init(z);
    mpz_mul(z, s, cur);
    mpz_mod(z, z, current_elliptic_curve.q);
    Point C, tmp_point;
    mpz_init(C.x);
    mpz_init(C.y);
    mpz_init(tmp_point.x);
    mpz_init(tmp_point.y);
    mul(current_elliptic_curve.G, z, C);
    mpz_mul(z, r, cur);
    mpz_mod(z, z, current_elliptic_curve.q);
    mpz_sub(z, current_elliptic_curve.q, z);
    mul(ecc_public_key, z, tmp_point);
    sum(C, tmp_point, C);

    mpz_mod(z, C.x, current_elliptic_curve.q);

    return mpz_cmp(z, r) == 0 ? 0 : 1;
}