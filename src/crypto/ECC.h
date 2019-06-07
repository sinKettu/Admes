#include "prng.h"

#ifndef OCCULUS_ECC_H
#define OCCULUS_ECC_H

// Генерация публичного и закрытого ключей
Keychain *ecc_keygen(EllipticCurve *ec);

// Шифрование сообщения
int ECC_Encrypt(EllipticCurve *ec, Point pk, unsigned char *message, unsigned int m_len, ECC_encrypted_data *data);

// Расшифрование сообщения
int ECC_Decrypt(EllipticCurve *ec, mpz_t prk, ECC_encrypted_data data, unsigned char **message, unsigned int &m_len);

// Создание цифровой подписи
// ГОСТ 34.10-2012
int ECC_Sign(EllipticCurve *ec, mpz_t prk, unsigned char *message, unsigned int m_len, ECC_signature *signature);

// Проверка цифровой подписи
// ГОСТ 34.10-2012
int ecc_check_sign(EllipticCurve *ec, Point pk, ECC_signature *signature, unsigned char* hash, int h_len);

void ecc_test();

#endif //OCCULUS_ECC_H
