#include "account.h"
#include "common.h"
#include <QFile>
#include <QMap>

bool logged = false;
EllipticCurve *currentEC;
Keychain *currentECKeys;
QByteArray masterKey;
QString userName;
QMap<QString, Point> knownPeers;

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

    QFile fout_k("config/users/" + login + "_known");
    if (!fout.open(QIODevice::WriteOnly) || !fout_k.open(QIODevice::WriteOnly))
    {
        ec_deinit(new_ec);
        delete_keys(new_kc);
        std::cout << prefix << "Couldn't open user files\n";
        return false;
    }

    fout.write(userInfo);
    fout.close();
    ec_deinit(new_ec);
    delete_keys(new_kc);

    fout_k.close();

    return true;
}

bool LoadKnownPeers(QString login, QByteArray key)
{
    QFile fin("config/users/" + login + "_known");
    if (!fin.open(QIODevice::ReadOnly))
    {
        return false;
    }

    QByteArray buffer = fin.readAll();
    fin.close();
    if (buffer.length() % 16)
    {
        return false;
    }
    if (buffer.isEmpty())
    {
        knownPeers.clear();
        return true;
    }

    QByteArray data = AES_ECB_Decrypt(buffer, key.data(), key.length());
    QString name;
    QByteArray puk;

    qint32 start = 0, index = 0;
    while (index < data.length())
    {
        if (data.at(index) == 0)
        {
            name = data.mid(start, index - start);
            qint32 l = *reinterpret_cast<qint32*>(data.mid(index + 1, 4).data());
            puk = data.mid(index + 5, l);
            if (puk.length() != l)
            {
                knownPeers.clear();
                return false;
            }
                
            puk.push_front(static_cast<char>(0));
            puk.push_front(static_cast<char>(1));
            Keychain *kc = qba_to_ecc_keys(puk);
            if (kc == nullptr)
            {
                knownPeers.clear();
                return false;
            }
            
            Point a;
            mpz_inits(a.x, a.y, NULL);
            pntcpy(kc->PublicKey, a);
            knownPeers.insert(name, a);
            delete_keys(kc);

            index += 5 + l;
            start = index;
        }
        else
            index++;
    }

    if (!EPNG_inited())
    {
        mpz_t tmp;
        mpz_init(tmp);
        typing_random(tmp);
        EPNG_init(SECP128R2, tmp);
    }

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
    if (decAESKey == masterKey && LoadKnownPeers(login, masterKey))
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

bool IsPeerKnown(QString login)
{
    return knownPeers.contains(login);
}

bool NewPeer(QString login, QByteArray puk, QByteArray key)
{
    if (IsPeerKnown(login))
        return false;

    QFile finout("./config/users/" + login + "_known");
    if (!finout.open(QIODevice::ReadOnly))
    {
        return false;
    }

    QByteArray enc = finout.readAll();
    QByteArray data = AES_ECB_Decrypt(enc, key.data(), key.length());
    finout.close();

    data.append(login.toLocal8Bit());
    data.append("\0", 1);
    int tmp = puk.length();
    data.append(reinterpret_cast<char*>(&tmp), 4);
    data.append(puk);
    enc = AES_ECB_Encrypt(data, key.data(), key.length());

    if (!finout.open(QIODevice::WriteOnly))
    {
        return false;
    }

    finout.write(enc);
    finout.close();

    puk.push_front(static_cast<char>(0));
    puk.push_front(static_cast<char>(1));
    Keychain *kc = qba_to_ecc_keys(puk);

    Point a;
    mpz_inits(a.x, a.y, NULL);
    pntcpy(kc->PublicKey, a);
    knownPeers.insert(login, a);
    delete_keys(kc);
    pnt_deinit(a);

    return true;
}

QByteArray GetBytePuk()
{
    QByteArray tmp = ecc_keys_to_qba(currentECKeys);
    if (tmp == nullptr)
        return QByteArray();

    int a = *reinterpret_cast<int*>(tmp.data());
    return tmp.mid(a + 4);
}

QString GetLogin()
{
    return userName;
}

EllipticCurve *GetEC()
{
    return currentEC;
}

Keychain *GetKeys()
{
    return currentECKeys;
}

bool CheckKey(QString login, Point puk)
{
    return IsUserKnown(login) && pntcmp(puk, knownPeers[login]);
}

void RemovePeer(QString login, Point puk)
{
    if (!IsPeerKnown(login))
        return;

    QFile fout("/config/users/" + userName + "_known");
    if (!(fout.exists() && fout.open(QIODevice::WriteOnly)))
        return;

    knownPeers.remove(login);
    QMap<QString, Point>::iterator iter;
    for (iter = knownPeers.begin(); iter != knownPeers.end(); iter++)
    {
        QByteArray toWrite = login.toLocal8Bit();
        toWrite.push_back('\00');
        Keychain kc;
        mpz_init_set_ui(kc.PrivateKey, 0);
        kc.PublicKey = pnt_init();
        pntcpy(puk, kc.PublicKey);
        QByteArray key = ecc_keys_to_qba(&kc);
        if (key)
    }
}