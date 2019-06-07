//
// Created by sin on 06.11.18.
//
#include "ECC.h"

Keychain *ecc_keygen(EllipticCurve *ec)
{   
    if (! EPNG_inited())
    {
        return nullptr;
    }

    Keychain *kc = new Keychain(); 
    mpz_init(kc->PrivateKey);
    mpz_init(kc->PublicKey.x);
    mpz_init(kc->PublicKey.y);

    EPNG_get(kc->PrivateKey);
    mpz_mod(kc->PrivateKey, kc->PrivateKey, ec->p);
    pntMul(ec, ec->G, kc->PrivateKey, kc->PublicKey);

    return kc;
}

int _encrypt(EllipticCurve *ec, Point pk, unsigned char *message, unsigned int m_len, ECC_encrypted_data *data)
{
    if (! EPNG_inited())
    {
        return 0;
    }

    Point mes;
    mpz_init(mes.x);
    mpz_init(mes.y);
    ecc_cstr_to_mpz(message, m_len, mes.x);

    if (mpz_cmp(mes.x, ec->p) > 0)
    {
        mpz_clears(mes.x, mes.y, NULL);
        return 0;
    }

    // Представление сообщения в виде точки на ЭК

    // Y^2 = X^3 + a*X + b
    if (!pntExpand(ec, mes.x, mes.y))
    {
        mpz_clear(mes.x);
        mpz_clear(mes.y);
        return 0;
    }

    // Получение первой (side) части зашифрованного сообщения

    mpz_t tmp;
    mpz_init(tmp);
    EPNG_get(tmp);
    Point tmp_point;
    mpz_init(tmp_point.x);
    mpz_init(tmp_point.y);
    pntMul(ec, ec->G, tmp, tmp_point);
    ecc_mpz_to_cstr(tmp_point.x, &data->side_x, data->s_len_x);
    ecc_mpz_to_cstr(tmp_point.y, &data->side_y, data->s_len_y);

    // Получение второй (general) части зашифрованного сообщения

    pntMul(ec, pk, tmp, tmp_point);
    pntSum(ec, mes, tmp_point, tmp_point);
    ecc_mpz_to_cstr(tmp_point.x, &data->general_x, data->g_len_x);
    ecc_mpz_to_cstr(tmp_point.y, &data->general_y, data->g_len_y);

    mpz_clear(tmp);
    mpz_clear(tmp_point.x);
    mpz_clear(tmp_point.y);

    return 1;
}

QByteArray ECC_Encrypt(EllipticCurve *ec, Point pk, QByteArray message)
{
    QByteArray result;
    unsigned int step = ec->p_len / 8 - 3;
    unsigned char *mid = new unsigned char[step + 2];
    ECC_encrypted_data *data = new ECC_encrypted_data();
    for (unsigned int offset = 0; offset < message.length(); offset += step)
    {
        unsigned int mid_len = qMin(step, message.length() - offset);
        QByteArray qmid = message.mid(offset, mid_len);
        unsigned short appendix = 0x0000;
        memcpy(mid, qmid.data(), mid_len);
        *reinterpret_cast<unsigned short *>(mid + step) = appendix;
        while (!_encrypt(ec, pk, mid, step + 2, data))
        {
            appendix++;
            *reinterpret_cast<unsigned short *>(mid + step) = appendix;
            if (!appendix)
            {
                result.clear();
                delete data;
                delete[] mid;
                return result;
            }
        }
        
        unsigned int data_len = 16;
        data_len += data->g_len_y + data->g_len_x + data->s_len_x + data->s_len_y;
        result.append(reinterpret_cast<char *>(data), data_len);

        delete[] data->general_x;
        delete[] data->general_y;
        delete[] data->side_x;
        delete[] data->side_y;
    }

    delete[] mid;
    delete data;
    return result;
}

int ECC_Decrypt(EllipticCurve *ec, mpz_t prk, ECC_encrypted_data *data, unsigned char **message, unsigned int &m_len)
{
    // Загрузка первой (side) части ключа

    Point side;
    mpz_init(side.x);
    mpz_init(side.y);
    ecc_cstr_to_mpz(data->side_x, data->s_len_x, side.x);
    ecc_cstr_to_mpz(data->side_y, data->s_len_y, side.y);

    // Загрузка второй (general) части ключа

    Point general;
    mpz_init(general.x);
    mpz_init(general.y);
    ecc_cstr_to_mpz(data->general_x, data->g_len_x, general.x);
    ecc_cstr_to_mpz(data->general_y, data->g_len_y, general.y);

    // Расшифрование

    pntMul(ec, side, prk, side);
    mpz_sub(side.y, ec->p, side.y);
    pntSum(ec, general, side, general);

    ecc_mpz_to_cstr(general.x, message, m_len);
    mpz_clear(side.y);
    mpz_clear(side.x);
    mpz_clear(general.y);
    mpz_clear(general.x);

    return 0;
}

int ECC_Sign(EllipticCurve *ec, mpz_t prk, unsigned char *message, unsigned int m_len, ECC_signature *signature)
{
    if (!EPNG_inited())
    {
        return 0;
    }

    // Шаг 1, 2

    mpz_t cur;
    mpz_init(cur);
    ecc_cstr_to_mpz(message, m_len, cur);
    mpz_mod(cur, cur, ec->q);
    if (mpz_cmp_ui(cur, 0) == 0)
        mpz_set_ui(cur, 1);

    // Шаг 3

    mpz_t tmp, r, s;
    mpz_init_set_ui(r, 0);
    mpz_init_set_ui(s, 0);
    mpz_init_set_ui(tmp, 0);
    while (mpz_cmp_ui(r, 0) == 0 || mpz_cmp_ui(s, 0) == 0)
    {
        EPNG_get(tmp);
        mpz_mod(tmp, tmp, ec->q);

    // Шаг 4

        Point tmp_point;
        mpz_init(tmp_point.x);
        mpz_init(tmp_point.y);
        pntMul(ec, ec->G, tmp, tmp_point);
        mpz_mod(r, tmp_point.x, ec->q);

    // Шаг 5

        mpz_mul(s, r, prk);
        mpz_mul(tmp, tmp, cur);
        mpz_add(s, s, tmp);
        mpz_mod(s, s, ec->q);

        mpz_clear(tmp_point.x);
        mpz_clear(tmp_point.y);
    }

    // Шаг 6

    ecc_mpz_to_cstr(r, &(signature->r), signature->r_len);
    ecc_mpz_to_cstr(s, &(signature->s), signature->s_len);

    mpz_clear(tmp);
    mpz_clear(r);
    mpz_clear(s);
    mpz_clear(cur);

    return 1;
}

int ECC_Check(EllipticCurve *ec, Point pk, ECC_signature *signature, unsigned char* hash, int h_len)
{
    // Шаг 1

    mpz_t r, s;
    mpz_init(r);
    mpz_init(s);
    ecc_cstr_to_mpz(signature->r, signature->r_len, r);
    ecc_cstr_to_mpz(signature->s, signature->s_len, s);

    // Шаг 2

    mpz_t cur;
    mpz_init(cur);
    ecc_cstr_to_mpz(hash, h_len, cur);
    mpz_mod(cur, cur, ec->q);
    if (mpz_cmp_ui(cur, 0) == 0)
        mpz_set_ui(cur, 1);

    // Шаг 4

    mpz_invert(cur, cur, ec->q);

    // Шаг 5, 6

    mpz_t z;
    mpz_init(z);
    mpz_mul(z, s, cur);
    mpz_mod(z, z, ec->q);
    Point C, tmp_point;
    mpz_init(C.x);
    mpz_init(C.y);
    mpz_init(tmp_point.x);
    mpz_init(tmp_point.y);
    pntMul(ec, ec->G, z, C);
    mpz_mul(z, r, cur);
    mpz_mod(z, z, ec->q);
    mpz_sub(z, ec->q, z);
    pntMul(ec, pk, z, tmp_point);
    pntSum(ec, C, tmp_point, C);

    mpz_mod(z, C.x, ec->q);

    int result = mpz_cmp(z, r) == 0 ? 1 : 0;
    mpz_clears(r, s, z, cur, NULL);
    return result;
}

void ecc_test()
{
    mpz_t tmp;
    mpz_init_set_str(tmp, "2346345634123453245345", 10);
    EPNG_init(0, tmp);
    EllipticCurve *ec = ec_init(4);
    Keychain *kc = ecc_keygen(ec);

    QByteArray mes = QByteArray("HelloWo");
    QByteArray res;
    res = ECC_Encrypt(ec, kc->PublicKey, mes);

    unsigned char *dec;
    unsigned int l;
    //ECC_Decrypt(ec, kc->PrivateKey, data, &dec, l);

    printf("%s\n", dec);
    EPNG_delete();
    ec_deinit(ec);
    delete ec;
}