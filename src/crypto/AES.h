bool AES_ECB_Encrypt(byte* input_buffer, uint32_t ib_len, 
                     byte** output_buffer, uint32_t &ob_len,
                     byte* key, uint32_t k_len);

bool AES_ECB_Decrypt(byte *input_buffer, uint32_t ib_len,
                     byte **output_buffer, uint32_t &ob_len,
                     byte *key, uint32_t k_len);

bool AES_CBC_Encrypt(byte *input_buffer, uint32_t ib_len,
                     byte **output_buffer, uint32_t &ob_len,
                     byte *key, uint32_t k_len,
                     byte *iv);

bool AES_CBC_Decrypt(byte *input_buffer, uint32_t ib_len,
                     byte **output_buffer, uint32_t &ob_len,
                     byte *key, uint32_t k_len,
                     byte *iv);
