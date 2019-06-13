#include "ECC.h"

Keychain *ecc_keygen(EllipticCurve *ec)
{   
    if (! EPNG_inited())
    {
        return nullptr;
    }

    Keychain *kc = new Keychain(); 
    mpz_init(kc->PrivateKey);
    kc->PublicKey = pnt_init();

    EPNG_get(kc->PrivateKey);
    mpz_mod(kc->PrivateKey, kc->PrivateKey, ec->p);
    pntMul(ec, ec->G, kc->PrivateKey, kc->PublicKey);

    return kc;
}

void delete_keys(Keychain *kc)
{
    pnt_deinit(kc->PublicKey);
    mpz_clear(kc->PrivateKey);
    delete kc;
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
        *reinterpret_cast<unsigned short *>(mid + mid_len) = appendix;
        while (!_encrypt(ec, pk, mid, mid_len + 2, data))
        {
            appendix++;
            *reinterpret_cast<unsigned short *>(mid + mid_len) = appendix;
            if (!appendix)
            {
                result.clear();
                delete data;
                delete[] mid;
                return result;
            }
        }

        result.append(reinterpret_cast<char *>(&(data->g_len_x)), 4);
        result.append(reinterpret_cast<char *>(data->general_x), data->g_len_x);

        result.append(reinterpret_cast<char *>(&(data->g_len_y)), 4);
        result.append(reinterpret_cast<char *>(data->general_y), data->g_len_y);

        result.append(reinterpret_cast<char *>(&(data->s_len_x)), 4);
        result.append(reinterpret_cast<char *>(data->side_x), data->s_len_x);

        result.append(reinterpret_cast<char *>(&(data->s_len_y)), 4);
        result.append(reinterpret_cast<char *>(data->side_y), data->s_len_y);

        delete[] data->general_x;
        delete[] data->general_y;
        delete[] data->side_x;
        delete[] data->side_y;
    }

    delete[] mid;
    delete data;
    return result;
}

int _decrypt(EllipticCurve *ec, mpz_t prk, ECC_encrypted_data *data, unsigned char **message, unsigned int &m_len)
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

    return 1;
}

int byte_array_to_enc_data(char *array, int len, ECC_encrypted_data *data)
{
    // set general_x
    int offset = 0;
    data->g_len_x = *reinterpret_cast<unsigned int *>(array + offset);
    offset += 4;
    if (offset >= len)
    {
        return 0;
    }
    data->general_x = new unsigned char[data->g_len_x];
    memcpy(data->general_x, array + offset, data->g_len_x);
    offset += data->g_len_x;
    if (offset >= len)
    {
        delete[] data->general_x;
        return 0;
    }

    // set general_y
    data->g_len_y = *reinterpret_cast<unsigned int *>(array + offset);
    offset += 4;
    if (offset >= len)
    {
        delete[] data->general_x;
        return 0;
    }
    data->general_y = new unsigned char[data->g_len_y];
    memcpy(data->general_y, array + offset, data->g_len_y);
    offset += data->g_len_y;
    if (offset >= len)
    {
        delete[] data->general_x;
        delete[] data->general_y;
        return 0;
    }

    // set side_x
    data->s_len_x = *reinterpret_cast<unsigned int *>(array + offset);
    offset += 4;
    if (offset >= len)
    {
        delete[] data->general_x;
        delete[] data->general_y;
        return 0;
    }
    data->side_x = new unsigned char[data->s_len_x];
    memcpy(data->side_x, array + offset, data->s_len_x);
    offset += data->s_len_x;
    if (offset >= len)
    {
        delete[] data->side_x;
        delete[] data->general_x;
        delete[] data->general_y;
        return 0;
    }

    // set side_y
    data->s_len_y = *reinterpret_cast<unsigned int *>(array + offset);
    offset += 4;
    if (offset >= len)
    {
        delete[] data->side_x;
        delete[] data->general_x;
        delete[] data->general_y;
        return 0;
    }
    data->side_y = new unsigned char[data->s_len_y];
    memcpy(data->side_y, array + offset, data->s_len_y);
    offset += data->s_len_y;
    if (offset > len)
    {
        delete[] data->side_x;
        delete[] data->side_y;
        delete[] data->general_x;
        delete[] data->general_y;
        return 0;
    }

    return offset;
}

QByteArray ECC_Decrypt(EllipticCurve *ec, mpz_t prk, QByteArray encrypted)
{
    QByteArray result;
    unsigned int step = ec->p_len / 8 - 3;
    unsigned int offset = 0;
    char *enc = encrypted.data();
    ECC_encrypted_data *data = new ECC_encrypted_data();
    while (offset < encrypted.length())
    {
        int local_offset = byte_array_to_enc_data(enc + offset, encrypted.length() - offset, data);
        if (!local_offset)
        {
            delete data;
            result.clear();
            return result;
        }

        offset += local_offset;
        unsigned char *mes;
        unsigned int len;
        _decrypt(ec, prk, data, &mes, len);
        result.append(reinterpret_cast<char*>(mes), len - 2);

        delete[] mes;
        delete[] data->side_x;
        delete[] data->side_y;
        delete[] data->general_x;
        delete[] data->general_y;
    }

    delete data;
    return result;
}

int _sign(EllipticCurve *ec, mpz_t prk, unsigned char *message, unsigned int m_len, ECC_signature *signature)
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

QByteArray ECC_Sign(EllipticCurve *ec, mpz_t prk, QByteArray data)
{
    if (data.length() / 8 >= ec->p_len)
        return QByteArray();

    ECC_signature *s = new ECC_signature();
    QByteArray sign;
    unsigned char *d = reinterpret_cast<unsigned char *>(data.data());
    _sign(ec, prk, d, data.length(), s);

    sign.append(reinterpret_cast<char *>(&(s->r_len)), 4);
    sign.append(reinterpret_cast<char *>(s->r), s->r_len);
    sign.append(reinterpret_cast<char *>(&(s->s_len)), 4);
    sign.append(reinterpret_cast<char *>(s->s), s->s_len);

    return sign;
}

int _check(EllipticCurve *ec, Point pk, ECC_signature *signature, unsigned char* hash, int h_len)
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

int ECC_Check(EllipticCurve *ec, Point puk, QByteArray data, QByteArray signature)
{
    if (data.length() / 8 >= ec->p_len)
        return -1;

    unsigned char *tmp = reinterpret_cast<unsigned char *>(signature.data());
    ECC_signature *s = new ECC_signature();
    
    s->r_len = *reinterpret_cast<unsigned int *>(tmp);
    if (4 + s->r_len >= signature.length())
    {
        delete s;
        return -1;
    }
    s->r = new unsigned char[s->r_len];
    memcpy(s->r, tmp + 4, s->r_len);

    s->s_len = *reinterpret_cast<unsigned int*>(tmp + 4 + s->r_len);
    if (8 + s->r_len + s->s_len > signature.length())
    {
        delete[] s->r;
        delete s;
        return -1;
    }
    s->s = new unsigned char[s->s_len];
    memcpy(s->s, tmp + 4 + s->r_len + 4, s->s_len);

    return _check(ec, puk, s, reinterpret_cast<unsigned char *>(data.data()), data.length());
}

void ecc_test()
{
    mpz_t tmp;
    mpz_init_set_str(tmp, "2346345634123453245345", 10);
    EPNG_init(0, tmp);
    EllipticCurve *ec = ec_init(4);
    Keychain *kc = ecc_keygen(ec);

    QByteArray hash = QByteArray("1234567890");
    QByteArray sign;
    sign = ECC_Sign(ec, kc->PrivateKey, hash);
    hash.append('0');
    int a = ECC_Check(ec, kc->PublicKey, hash, sign);

    EPNG_delete();
    ec_deinit(ec);
    delete_keys(kc);
}