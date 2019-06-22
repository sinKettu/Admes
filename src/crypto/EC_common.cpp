#include "EC_common.h"

typedef unsigned char byte;

const unsigned char listLength = 6;

// 6 elliptic curves from "STANDARDS FOR EFFICIENT CRYPTOGRAPHY (SEC2)"
// Order: p (EC prime), a, b, q (EC group order), 
// X-point, Y-point (abscissa and ordinate of generator)
char ec_list[listLength][6][133] {
    // secp128r1
    {"fffffffdffffffffffffffffffffffff\0" , "fffffffdfffffffffffffffffffffffc\0", "e87579c11079f43dd824993c2cee5ed3\0", "fffffffe0000000075a30d1b9038a115\0", "161ff7528b899b2d0c28607ca52c5b86\0", "cf5ac8395bafeb13c02da292dded7a83\0"},
    // secp128r2
    {"fffffffdffffffffffffffffffffffff\0", "d6031998d1b3bbfebf59cc9bbff9aee1\0", "5eeefca380d02919dc2c6558bb6d8a5d\0", "3fffffff7fffffffbe0024720613b5a3\0", "7b6aa5d85e572983e6fb32a7cdebc140\0", "27b6916a894d3aee7106fe805fc34b44\0"},
    // secp256k1
    {"fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f", "0", "7", "fffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141", "79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798", "483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8"},
    // secp256r1
    {"ffffffff00000001000000000000000000000000ffffffffffffffffffffffff\0", "ffffffff00000001000000000000000000000000fffffffffffffffffffffffc\0", "5ac635d8aa3a93e7b3ebbd55769886bc651d06b0cc53b0f63bce3c3e27d2604b\0", "ffffffff00000000ffffffffffffffffbce6faada7179e84f3b9cac2fc632551\0", "6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296\0", "4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5\0"},
    // secp384r1
    {"fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffeffffffff0000000000000000ffffffff\0", "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffeffffffff0000000000000000fffffffc\0", "b3312fa7e23ee7e4988e056be3f82d19181d9c6efe8141120314088f5013875ac656398d8a2ed19d2a85c8edd3ec2aef\0", "ffffffffffffffffffffffffffffffffffffffffffffffffc7634d81f4372ddf581a0db248b0a77aecec196accc52973\0", "aa87ca22be8b05378eb1c71ef320ad746e1d3b628ba79b9859f741e082542a385502f25dbf55296c3a545e3872760ab7\0", "3617de4a96262c6f5d9e98bf9292dc29f8f41dbd289a147ce9da3113b5f0b8c00a60b1ce1d7e819d7a431d7c90ea0e5f\0"},
    // secp521r1
    {"1ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff\0", "1fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffc\0", "51953eb9618e1c9a1f929a21a0b68540eea2da725b99b315f3b8b489918ef109e156193951ec7e937b1652c0bd3bb1bf073573df883d2c34f1ef451fd46b503f00\0", "1fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffa51868783bf2f966b7fcc0148f709a5d03bb5c9b8899c47aebb6fb71e91386409\0", "c6858e06b70404e9cd9e3ecb662395b4429c648139053fb521f828af606b4d3dbaa14b5e77efe75928fe1dc127a2ffa8de3348b3c1856a429bf97e7e31c2e5bd66\0", "11839296a789a3bc0045c8a5fb42c7d1bd998f54449579b446817afbd17273e662c97ee72995ef42640c550b9013fad0761353c7086a272c24088be94769fd16650\0"}        
};

unsigned int prime_lengthes[6] {
    // secp128r1 
    128,
    // secp128r2
    128,
    // secp256k1
    256,
    // secp256r1
    256,
    // secp384r1
    384,
    // secp521r1
    521
};

EllipticCurve *ec_init(unsigned char num)
{
    if (num >= listLength)
        return nullptr;

    EllipticCurve *ec = new EllipticCurve();
    mpz_init_set_str(ec->p, ec_list[num][0], 16);
    mpz_init_set_str(ec->a, ec_list[num][1], 16);
    mpz_init_set_str(ec->b, ec_list[num][2], 16);
    mpz_init_set_str(ec->q, ec_list[num][3], 16);
    mpz_init_set_str(ec->G.x, ec_list[num][4], 16);
    mpz_init_set_str(ec->G.y, ec_list[num][5], 16);

    ec->p_len = prime_lengthes[num];

    return ec;
}

void ec_deinit(EllipticCurve *ec)
{
    mpz_clears(ec->p, ec->a, ec->b, ec->q, ec->G.x, ec->G.y, NULL);
    delete ec;
}

void pntcpy(Point src, Point &dst)
{
    mpz_set(dst.x, src.x);
    mpz_set(dst.y, src.y);
}

bool pntcmp(Point a, Point b)
{
    return mpz_cmp(a.x, b.x) | mpz_cmp(a.y, b.y);
}

bool sqrt_modulo_p(mpz_t a, mpz_t p, mpz_t &res)
{
    mpz_t ax, bx, b, t, n;
    mpz_init_set(n, a);
    mpz_init(ax);
    mpz_init(bx);
    mpz_init(b);
    mpz_init(t);

    // (n|p) != -1
    mpz_sub_ui(ax, p, 1);
    mpz_div_ui(ax, ax, 2);
    mpz_powm(ax, n, ax, p);

    if (mpz_legendre(n, p) == -1)
    {
        mpz_set_ui(res, 0);
        mpz_clears(ax, bx, b, t, n, NULL);
        return false;
    }

    // (b|p) == -1
    gmp_randstate_t state;
    gmp_randinit_mt(state);
    mpz_urandomm(b, state, p);
    while (mpz_legendre(b, p) != -1)
        mpz_urandomm(b, state, p);
    gmp_randclear(state);

    // p-1 = (2^s) * t
    mpz_sub_ui(ax, p, 1);
    mpz_set_ui(bx, 1);
    mpz_and(t, ax, bx);
    unsigned int s = 0;
    while (mpz_cmp_ui(t, 0) == 0)
    {
        s++;
        mpz_div_2exp(ax, ax, 1);
        mpz_and(t, ax, bx);
    }
    mpz_set(t, ax);
    
    // b = b^t (mod p)
    mpz_powm(b, b, t, p);

    // res= n^((t+1)/2) (mod p)
    mpz_add_ui(res, t, 1);
    mpz_div_ui(res, res, 2);
    mpz_powm(res, n, res, p);

    for (unsigned int i = 1; i < s; i++)
    {
        // exp = 2^(s - i - 1)
        mpz_set_ui(bx, 2);
        mpz_powm_ui(bx, bx, s - i - 1, p);

        // base = (r^2 * a^-1)
        if (!mpz_invert(ax, n, p))
        {
            mpz_clears(ax, bx, b, t, n, NULL);
            mpz_set_ui(res, 0);
            return false;
        }
        mpz_mul(ax, ax, res);
        mpz_mul(ax, ax, res);

        // d = base ^ exp (mod p)
        mpz_powm(ax, ax, bx, p);
        mpz_sub_ui(bx, p, 1);

        // if d = -1 (mod p)
        if (mpz_cmp(ax, bx) == 0)
        {
            // res = res * b (mod p)
            mpz_mul(res, res, b);
            mpz_mod(res, res, p);
        }
    }

    mpz_clears(ax, bx, b, t, n, NULL);
    return true;
}

// Y^2 = X^3 + a*X + b
bool pntExpand(EllipticCurve *ec, mpz_t x, mpz_t &y)
{
    mpz_t tmp;
    mpz_init(tmp);
    mpz_pow_ui(y, x, 3);

    mpz_mul(tmp, x, ec->a);
    mpz_add(y, y, tmp);

    mpz_add(y, y, ec->b);

    mpz_mod(y, y, ec->p);
    mpz_clear(tmp);
    return sqrt_modulo_p(y, ec->p, y);
}

void pntSum(EllipticCurve *ec, Point left, Point right, Point &result)
{
    if (! mpz_cmp_ui(left.x, 0) && ! mpz_cmp_ui(left.y, 0))
    {
        mpz_set(result.x, right.x);
        mpz_set(result.y, right.y);
        return;

    }
    if (! mpz_cmp_ui(right.x, 0) && ! mpz_cmp_ui(right.y, 0))
    {
        mpz_set(result.x, left.x);
        mpz_set(result.y, left.y);
        return;
    }

    Point p1, p2;
    mpz_init(p1.x);
    mpz_init(p1.y);
    mpz_init(p2.x);
    mpz_init(p2.y);
    pntcpy(left, p1);
    pntcpy(right, p2);

    if (mpz_cmp(p1.x, p2.x) != 0)
    {
        mpz_t tmp, s;
        mpz_init(tmp);
        mpz_init(s);

        // Lambda
        mpz_sub(tmp, p2.x, p1.x);
        mpz_mod(tmp, tmp, ec->p);
        mpz_invert(s, tmp, ec->p);
        mpz_sub(tmp, p2.y, p1.y);
        mpz_mul(tmp, tmp, s);
        mpz_mod(tmp, tmp, ec->p);

        // X result
        mpz_mul(result.x, tmp, tmp);
        mpz_sub(result.x, result.x, p1.x);
        mpz_sub(result.x, result.x, p2.x);
        mpz_mod(result.x, result.x, ec->p);

        // Y result
        mpz_sub(result.y, p2.x, result.x);
        mpz_mul(result.y, result.y, tmp);
        mpz_sub(result.y, result.y, p2.y);
        mpz_mod(result.y, result.y, ec->p);

        mpz_clears(tmp, s, NULL);
    }
    else if (mpz_cmp(p1.x, p2.x) == 0 && mpz_cmp(p1.y, p2.y) == 0 && mpz_cmp_ui(p1.y, 0) != 0)
    {
        mpz_t tmp, s;
        mpz_init(tmp);
        mpz_init(s);

        mpz_mul_ui(s, p1.y, 2);
        mpz_invert(s, s, ec->p);
        //
        // Lambda
        //
        mpz_mul_ui(tmp, p1.x, 3);
        mpz_mul(tmp, tmp, p1.x);
        mpz_add(tmp, tmp, ec->a);
        mpz_mul(tmp, tmp, s);
        mpz_mod(tmp, tmp, ec->p);
        //
        // X result
        //
        mpz_mul_ui(s, p1.x, 2);
        mpz_mul(result.x, tmp, tmp);
        mpz_sub(result.x, result.x, s);
        mpz_mod(result.x, result.x, ec->p);
        //
        // Y result
        //
        mpz_sub(result.y, p1.x, result.x);
        mpz_mul(result.y, result.y, tmp);
        mpz_sub(result.y, result.y, p1.y);
        mpz_mod(result.y, result.y, ec->p);
        //
        mpz_clears(tmp, s, NULL);
    }
    else
    {
        mpz_sub(p2.y, ec->p, p2.y);
        if (!mpz_cmp(p1.x, p2.x) && !mpz_cmp(p1.y, p2.y))
        {
            mpz_set_ui(result.x, 0);
            mpz_set_ui(result.y, 0);
        }
        printf("!\n");
    }

    mpz_clears(p1.x, p1.y, p2.x, p2.y, NULL);
}

void pntMul(EllipticCurve *ec, Point point, mpz_t num, Point &result)
{
    Point pnt;
    mpz_t tmp, k;
    mpz_inits(tmp, k, pnt.x, pnt.y, NULL);
    mpz_set(k, num);
    pntcpy(point, pnt);

    mpz_set_ui(result.x, 0);
    mpz_set_ui(result.y, 0);

    while(mpz_cmp_ui(k, 0) > 0)
    {
        mpz_mod_ui(tmp, k, 2);
        if (mpz_cmp_ui(tmp, 1) == 0)        
            pntSum(ec, pnt, result, result);
        pntSum(ec, pnt, pnt, pnt);
        mpz_div_ui(k, k, 2);
    }

    mpz_clear(tmp);
}

void ecc_mpz_to_cstr(mpz_t n, byte** b, unsigned int &len)
{
    len = 0;
    mpz_t a; mpz_init_set(a, n);
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
    mpz_clear(a);
}

void ecc_cstr_to_mpz(unsigned char* a, unsigned int len, mpz_t &b)
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
}

Point pnt_init()
{
    Point pnt;
    mpz_init(pnt.x);
    mpz_init(pnt.y);

    return pnt;
}

void pnt_deinit(Point pnt)
{
    mpz_clears(pnt.x, pnt.y, NULL);
}

QByteArray ecc_keys_to_qba(Keychain *kc)
{
    unsigned int l;
    unsigned char *buf;
    QByteArray result;

    ecc_mpz_to_cstr(kc->PrivateKey, &buf, l);
    result.append(reinterpret_cast<char *>(&l), 4);
    result.append(reinterpret_cast<char *>(buf), l);
    delete[] buf;

    ecc_mpz_to_cstr(kc->PublicKey.x, &buf, l);
    result.append(reinterpret_cast<char *>(&l), 4);
    result.append(reinterpret_cast<char *>(buf), l);
    delete[] buf;

    ecc_mpz_to_cstr(kc->PublicKey.y, &buf, l);
    result.append(reinterpret_cast<char *>(&l), 4);
    result.append(reinterpret_cast<char *>(buf), l);
    delete[] buf;

    return result;
}

Keychain *qba_to_ecc_keys(QByteArray buffer)
{
    if (buffer.length() < 4)
        return nullptr;
    
    unsigned int l;
    unsigned int offset = 4;
    Keychain *kc = new Keychain();
    mpz_init(kc->PrivateKey);
    kc->PublicKey = pnt_init();

    l = *reinterpret_cast<unsigned int*>(buffer.data());
    if (offset + l < buffer.length())
    {
        ecc_cstr_to_mpz(reinterpret_cast<unsigned char*>(buffer.data()) + offset, l, kc->PrivateKey);
        offset += l;
    }
    else
    {
        pnt_deinit(kc->PublicKey);
        mpz_clear(kc->PrivateKey);
        delete kc;
        return nullptr;
    }

    if (offset + 4 < buffer.length())
    {
        l = *reinterpret_cast<unsigned int*>(buffer.data() + offset);
        offset += 4;
    }
    else
    {
        pnt_deinit(kc->PublicKey);
        mpz_clear(kc->PrivateKey);
        delete kc;
        return nullptr;
    }
    
    if (offset + l < buffer.length())
    {
        ecc_cstr_to_mpz(reinterpret_cast<unsigned char*>(buffer.data()) + offset, l, kc->PublicKey.x);
        offset += l;
    }
    else
    {
        pnt_deinit(kc->PublicKey);
        mpz_clear(kc->PrivateKey);
        delete kc;
        return nullptr;
    }

    if (offset + 4 < buffer.length())
    {
        l = *reinterpret_cast<unsigned int*>(buffer.data() + offset);
        offset += 4;
    }
    else
    {
        pnt_deinit(kc->PublicKey);
        mpz_clear(kc->PrivateKey);
        delete kc;
        return nullptr;
    }

    if (offset + l == buffer.length())
    {
        ecc_cstr_to_mpz(reinterpret_cast<unsigned char*>(buffer.data()) + offset, l, kc->PublicKey.y);
        return kc;
    }
    else
        return nullptr;
}

QByteArray ecc_puk_to_qba(Point puk)
{
    unsigned char *x, *y;
    unsigned int lx, ly;
    ecc_mpz_to_cstr(puk.x, &x, lx);
    ecc_mpz_to_cstr(puk.y, &y, ly);
    QByteArray result;
    
    result.append(reinterpret_cast<char*>(&lx), 4);
    result.append(reinterpret_cast<char*>(x), lx);
    result.append(reinterpret_cast<char*>(&ly), 4);
    result.append(reinterpret_cast<char*>(y), ly);

    delete[] x;
    delete[] y;

    return result;
}

Point ecc_qba_to_puk(QByteArray puk)
{
    unsigned int lx, ly;
    lx = *reinterpret_cast<unsigned int*>(puk.data());
    if (puk.length() > 8 + lx)
    {
        ly = *reinterpret_cast<unsigned int*>(puk.data() + 4+ lx);    
    }
    else
    {
        Point result = pnt_init();
        mpz_set_ui(result.x, 0);
        mpz_set_ui(result.y, 0);

        return result;
    }
    if (puk.length() == 8 + lx + ly)
    {
        Point result = pnt_init();
        ecc_cstr_to_mpz(reinterpret_cast<unsigned char*>(puk.data() + 4), lx, result.x);
        ecc_cstr_to_mpz(reinterpret_cast<unsigned char*>(puk.data() + 8 + lx), ly, result.y);
        return result;
    }
    else
    {
        Point result = pnt_init();
        mpz_set_ui(result.x, 0);
        mpz_set_ui(result.y, 0);

        return result;
    }
}