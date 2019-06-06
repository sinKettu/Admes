bool AES_ECB_Encrypt(unsigned char* input_buffer, uint32_t ib_len, 
                     unsigned char** output_buffer, uint32_t &ob_len,
                     unsigned char* key, uint32_t k_len);

bool AES_ECB_Decrypt(unsigned char *input_buffer, uint32_t ib_len,
                     unsigned char **output_buffer, uint32_t &ob_len,
                     unsigned char *key, uint32_t k_len);

bool AES_CBC_Encrypt(unsigned char *input_buffer, uint32_t ib_len,
                     unsigned char **output_buffer, uint32_t &ob_len,
                     unsigned char *key, uint32_t k_len,
                     unsigned char *iv);

bool AES_CBC_Decrypt(unsigned char *input_buffer, uint32_t ib_len,
                     unsigned char **output_buffer, uint32_t &ob_len,
                     unsigned char *key, uint32_t k_len,
                     unsigned char *iv);
