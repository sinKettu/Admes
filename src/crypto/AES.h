#include <QByteArray>

QByteArray AES_ECB_Encrypt(QByteArray buffer, char *key, unsigned int key_len);

QByteArray AES_ECB_Decrypt(QByteArray encrypted, char *key, unsigned int key_len);

QByteArray AES_CBC_Encrypt(QByteArray buffer, char *key, unsigned int key_len, unsigned char *iv);

QByteArray AES_CBC_Decrypt(QByteArray encrypted, char *key, unsigned int key_len, unsigned char *iv);

// void AES_test();

// TODO:
//      'RoundKeysInstall' function for optimizing