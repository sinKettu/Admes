#include <QByteArray>

QByteArray AES_ECB_Encrypt(QByteArray buffer, char *key, unsigned int key_len);

QByteArray AES_ECB_Decrypt(QByteArray encrypted, char *key, unsigned int key_len);

QByteArray AES_CBC_Encrypt(QByteArray buffer, char *key, unsigned int key_len, unsigned char *iv);

bool AES_CBC_Decrypt(unsigned char *input_buffer, uint32_t ib_len,
                     unsigned char **output_buffer, uint32_t &ob_len,
                     unsigned char *key, uint32_t k_len,
                     unsigned char *iv);
