#ifndef OCCULUS_ECC_H
#define OCCULUS_ECC_H

#include "prng.h"

// Генерация публичного и закрытого ключей
Keychain *ecc_keygen(EllipticCurve *ec);

// Удаление пары ключей
void delete_keys(Keychain *kc);

// Шифрование сообщения
QByteArray ECC_Encrypt(EllipticCurve *ec, Point pk, QByteArray message);

// Расшифрование сообщения
QByteArray ECC_Decrypt(EllipticCurve *ec, mpz_t prk, QByteArray encrypted);

// Создание цифровой подписи
// ГОСТ 34.10-2012
QByteArray ECC_Sign(EllipticCurve *ec, mpz_t prk, QByteArray data);

// Проверка цифровой подписи
// ГОСТ 34.10-2012
int ECC_Check(EllipticCurve *ec, Point puk, QByteArray data, QByteArray signature);

void ecc_test();

#endif //OCCULUS_ECC_H
