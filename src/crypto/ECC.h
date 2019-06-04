#pragma once
//
// Created by sin on 06.11.18.
//
#include "prng.h" // structure Point is defined here

#ifndef OCCULUS_ECC_H
#define OCCULUS_ECC_H

// Структура - зашифрованное с помощью ЭК сообщение
// От точек ЭК берутся только иксы
struct ecc_encrypted_data
{
    unsigned char* side_x;
    unsigned char* side_y;
    unsigned char* general_x;
    unsigned char* general_y;
    int s_len_x;
    int s_len_y;
    int g_len_x;
    int g_len_y;
};

// Обозначения согласно
// ГОСТ 34.10-2012
struct ecc_signature
{
    unsigned char* r;
    unsigned char* s;
    int r_len;
    int s_len;
};

// Инициализация параметров для current_elliptic_curve
// С которой в дальнейшем будет работать алгоритм
// 0 - Успех
// 1 - слишком большое num
// 2 - параметры уже инициализованы, сначала нужно удалить кривую
int ecc_init_params(unsigned char num);

// Удаление параметров ЭК из памяти
// 0 - Успех
// 1 - параметры не инициализированы
int ecc_delete_params();

// Генерация публичного и закрытого ключей
void ecc_keygen();

// Шифрование сообщения
// 0 - Успех
// 1 - Ключ не сгенерирован/считан
// 2 - Размер сообщения большего модуля ЭК
int ecc_encrypt(unsigned char* message, int m_len, ecc_encrypted_data &data);

// Расшифрование сообщения
// 0 - Успех
// 1 - Ключ не сгенерирован/считан
// 2 - Не инициализирована ЭК
int ecc_decrypt(ecc_encrypted_data data, unsigned char** message, int &m_len);

// Создание цифровой подписи
// ГОСТ 34.10-2012
// 0 - Успех
// 1 - Не ииницилизирована ЭК
// 2 - Ключ не сгенерирован/считан
int ecc_make_sign(unsigned char* message, int m_len, ecc_signature &signature);

// Проверка цифровой подписи
// ГОСТ 34.10-2012
// 0 - Проверка удалась
// 1 - Проверка не удалась
// 2 - Не инициализирована ЭК
// 3 - Ключ не сгенерирован/считан
int ecc_check_sign(ecc_signature signature, unsigned char* hash, int h_len);

void ecc_test();

#endif //OCCULUS_ECC_H
