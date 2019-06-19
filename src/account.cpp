#include "account.h"
#include "common.h"
#include <QFile>

bool logged = false;
EllipticCurve *currentEC;
Keychain *currentECKeys;
QByteArray masterKey;
QString userName;

QByteArray UserDataToAESKey(QString login, QString password)
{
    QString tmp = login + password;
    if (EPNG_inited())
    {
        mpz_t saved;
        mpz_init(saved);
        EPNG_get(saved);
        EPNG_get(saved);
        EPNG_get(saved);
        EPNG_get(saved);

        EPNG_delete();
        mpz_t seed;
        mpz_init(seed);
        ecc_cstr_to_mpz(reinterpret_cast<unsigned char *>(tmp.data()), tmp.length(), seed);
        EPNG_init(SECP128R2, seed);

        mpz_t _key;
        mpz_init(_key);
        EPNG_get(_key);
        unsigned char *char_key;
        unsigned int l;
        ecc_mpz_to_cstr(_key, &char_key, l);
        QByteArray aes_key = QByteArray(reinterpret_cast<char *>(char_key), l);
        mpz_clears(seed, _key, NULL);
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

        mpz_t _key;
        mpz_init(_key);
        EPNG_get(_key);
        unsigned char *char_key;
        unsigned int l;
        ecc_mpz_to_cstr(_key, &char_key, l);
        QByteArray aes_key = QByteArray(reinterpret_cast<char *>(char_key), l);
        mpz_clears(seed, _key, NULL);
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
    QFile fout("./config/users/" + login);
    if (fout.exists())
    {
        std::cout << prefix << "User exists\n";
        return false;
    }

    if (!EPNG_inited())
    {
        mpz_t seed;
        mpz_init(seed);
        typing_random(seed);
        EPNG_init(SECP128R2, seed);
        mpz_clear(seed);
    }

    int ec_id = SECP521R1;
    EllipticCurve *new_ec = ec_init(ec_id);
    Keychain *new_kc = ecc_keygen(new_ec);
    QByteArray byteEccKeys = ecc_keys_to_qba(new_kc);
    QByteArray aesKey = UserDataToAESKey(login, password);
    QByteArray encEccKeys = AES_ECB_Encrypt(byteEccKeys, aesKey.data(), aesKey.length());
    if (encEccKeys.isEmpty())
    {
        ec_deinit(new_ec);
        delete_keys(new_kc);
        return false;
    }

    QByteArray encAESKey = ECC_Encrypt(new_ec, new_kc->PublicKey, aesKey);
    if (encAESKey.isEmpty())
    {
        ec_deinit(new_ec);
        delete_keys(new_kc);
        return false;
    }

    QByteArray userInfo;
    userInfo.append(reinterpret_cast<char *>(&ec_id), 4);
    ec_id = encAESKey.length();
    userInfo.append(reinterpret_cast<char *>(&ec_id), 4);
    ec_id = encEccKeys.length();
    userInfo.append(reinterpret_cast<char *>(&ec_id), 4);
    userInfo.append(encAESKey);
    userInfo.append(encEccKeys);

    if (!fout.open(QIODevice::WriteOnly))
    {
        ec_deinit(new_ec);
        delete_keys(new_kc);
        std::cout << prefix << "Couldn't read user file\n";
        return false;
    }

    fout.write(userInfo);
    fout.close();
    ec_deinit(new_ec);
    delete_keys(new_kc);

    return true;
}

bool LogIn(QString login, QString password)
{
    QFile fin("./config/users/" + login);
    if (!(fin.exists() && fin.open(QIODevice::ReadOnly)))
    {
        return false;
    }

    int ec_id = *reinterpret_cast<int *>(fin.read(4).data());
    int encAKLen = *reinterpret_cast<int *>(fin.read(4).data());
    int encEKLen = *reinterpret_cast<int *>(fin.read(4).data());
    
    QByteArray encAESKey = fin.read(encAKLen);
    if (encAESKey.length() != encAKLen)
    {
        fin.close();
        return false;
    }

    QByteArray encEKeys = fin.read(encEKLen);
    if (encEKeys.length() != encEKLen)
    {
        fin.close();
        return false;
    }

    fin.close();
    QByteArray masterKey = UserDataToAESKey(login, password);
    QByteArray byteEKeys = AES_ECB_Decrypt(encEKeys, masterKey.data(), masterKey.length());
    if (byteEKeys.isEmpty())
    {
        return false;
    }

    currentECKeys = qba_to_ecc_keys(byteEKeys);
    if (currentECKeys == nullptr)
    {
        return false;
    }
    currentEC = ec_init(ec_id);

    QByteArray decAESKey = ECC_Decrypt(currentEC, currentECKeys->PrivateKey, encAESKey);
    if (decAESKey == masterKey)
    {
        logged = true;
        userName = login;
        return true;
    }
    else
    {
        ec_deinit(currentEC);
        delete_keys(currentECKeys);
        masterKey.clear();
        return false;
    }
}

bool IsLoggedIn()
{
    return logged;
}