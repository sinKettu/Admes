#include "account.h"
#include <QFile>

bool logged = false;

QByteArray UserDataToAESKey(QString login, QString password)
{
    QString tmp = login + password;
    if (EPNG_inited())
    {
        mpz_t saved;
        mpz_init(saved);
        EPNG_get(saved);

        EPNG_delete();
        mpz_t seed;
        mpz_init(seed);
        ecc_cstr_to_mpz(reinterpret_cast<unsigned char *>(tmp.data()), tmp.length(), seed);
        EPNG_init(SECP128R2, seed);

        mpz_t mpz_key;
        mpz_init(mpz_key);
        EPNG_get(mpz_key);
        unsigned char *char_key;
        unsigned int l;
        ecc_mpz_to_cstr(mpz_key, &char_key, l);
        QByteArray aes_key = QByteArray(reinterpret_cast<char *>(char_key), l);
        mpz_clears(seed, mpz_key, NULL);
        delete[] char_key;

        if (aes_key.length() < 16)
        {
            char *tmp = new char[16 - aes_key.length()];
            memset(tmp, 0, 16 - aes_key.length());
            aes_key.append(tmp, 16 - aes_key.length());
            delete[] tmp;
        }
        else if (aes_key.length() > 16)
        {
            aes_key = aes_key.mid(0, 16);
        }

        EPNG_delete();
        EPNG_init(SECP128R2, saved);
        mpz_clear(saved);
        return aes_key;
    }
    else
    {
        mpz_t seed;
        mpz_init(seed);
        ecc_cstr_to_mpz(reinterpret_cast<unsigned char *>(tmp.data()), tmp.length(), seed);
        EPNG_init(SECP128R2, seed);

        mpz_t mpz_key;
        mpz_init(mpz_key);
        EPNG_get(mpz_key);
        unsigned char *char_key;
        unsigned int l;
        ecc_mpz_to_cstr(mpz_key, &char_key, l);
        QByteArray aes_key = QByteArray(reinterpret_cast<char *>(char_key), l);
        mpz_clears(seed, mpz_key, NULL);
        delete[] char_key;

        if (aes_key.length() < 16)
        {
            char *tmp = new char[16 - aes_key.length()];
            memset(tmp, 0, 16 - aes_key.length());
            aes_key.append(tmp, 16 - aes_key.length());
            delete[] tmp;
        }
        else if (aes_key.length() > 16)
        {
            aes_key = aes_key.mid(0, 16);
        }

        EPNG_delete();
        return aes_key;
    }
}

bool CreateAccount(QString login, QString password)
{
    if (!EPNG_inited())
    {
        mpz_t seed;
        mpz_init(seed);
        typing_random(seed);
        EPNG_init(SECP128R2, seed);
        mpz_clear(seed);
    }

    EllipticCurve *new_ec = ec_init(SECP521R1);
    Keychain *new_kc = ecc_keygen(new_ec);
    QByteArray byteEcKey = ecc_keys_to_qba(new_kc);
    QByteArray aesKey = UserDataToAESKey(login, password);

    QFile fout("config/users");
}