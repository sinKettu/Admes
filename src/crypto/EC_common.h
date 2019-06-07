#include <mpir.h>

// Elliptic curve point
struct Point
{
    mpz_t x;
    mpz_t y;
};

struct EllipticCurve
{
    mpz_t a; // Coefficient
    mpz_t b; // Coefficient
    mpz_t p; // Prime
    mpz_t q; // Order
    Point G; // Group generator
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
    int r_len;
    unsigned char* r;
    int s_len;
    unsigned char* s;
};

bool ec_init(EllipticCurve *ec, unsigned char num);

void ec_deinit(EllipticCurve *ec);

void pntcpy(Point src, Point dst);

bool pntExpand(EllipticCurve *ec, mpz_t x, mpz_t y);

void pntSum(EllipticCurve *ec, Point left, Point right, Point result);

void pntMul(EllipticCurve *ec, Point pnt, mpz_t k, Point result);