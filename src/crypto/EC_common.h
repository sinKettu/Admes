#include <mpir.h>

#define SECP128R1 0
#define SECP128R2 1
#define SECP256K1 2
#define SECP256R1 3
#define SECP384R1 4
#define SECP521R1 5

// Elliptic curve point
struct Point
{
    mpz_t x;
    mpz_t y;
};

struct EllipticCurve
{
    mpz_t a;            // Coefficient
    mpz_t b;            // Coefficient
    mpz_t p;            // Prime
    mpz_t q;            // Order
    unsigned int p_len; // Prime length in bits
    Point G;            // Group generator
};

// Public and private ECC keys
struct Keychain
{
    Point PublicKey;
    mpz_t PrivateKey;
};

// Data encrypted by ECC
struct ECC_encrypted_data
{
    unsigned int s_len_x;
    unsigned char *side_x;
    unsigned int s_len_y;
    unsigned char *side_y;

    unsigned int g_len_x;
    unsigned char *general_x;
    unsigned int g_len_y;
    unsigned char *general_y;
};

// Signature based on ECC
struct ECC_signature
{
    unsigned int r_len;
    unsigned char* r;
    unsigned int s_len;
    unsigned char* s;
};

EllipticCurve *ec_init(unsigned char num);

Point pnt_init();

void ec_deinit(EllipticCurve *ec);

void pnt_deinit(Point pnt);

void pntcpy(Point src, Point &dst);

bool pntExpand(EllipticCurve *ec, mpz_t x, mpz_t &y);

void pntSum(EllipticCurve *ec, Point left, Point right, Point &result);

void pntMul(EllipticCurve *ec, Point pnt, mpz_t k, Point &result);

void ecc_cstr_to_mpz(unsigned char* a, unsigned int len, mpz_t &b);

void ecc_mpz_to_cstr(mpz_t a, unsigned char** b, unsigned int &len);