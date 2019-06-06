#include <cstring> // memcpy
#include <string> // std::swap
#include <iostream>
#include "AES.h"

typedef unsigned char byte;

byte** roundKeys;

byte pos[4][4] = {
        {2, 3, 1, 1},
        {1, 2, 3, 1},
        {1, 1, 2, 3},
        {3, 1, 1, 2}
};

byte neg[4][4] = {
        {0x0e, 0x0b, 0x0d, 0x09},
        {0x09, 0x0e, 0x0b, 0x0d},
        {0x0d, 0x09, 0x0e, 0x0b},
        {0x0b, 0x0d, 0x09, 0x0e}
};

byte InvBytes[256] = {
        0x0, 0x1, 0x8d, 0xf6, 0xcb, 0x52, 0x7b, 0xd1, 0xe8, 0x4f, 0x29, 0xc0, 0xb0, 0xe1, 0xe5, 0xc7, 
        0x74, 0xb4, 0xaa, 0x4b, 0x99, 0x2b, 0x60, 0x5f, 0x58, 0x3f, 0xfd, 0xcc, 0xff, 0x40, 0xee, 0xb2, 
        0x3a, 0x6e, 0x5a, 0xf1, 0x55, 0x4d, 0xa8, 0xc9, 0xc1, 0xa, 0x98, 0x15, 0x30, 0x44, 0xa2, 0xc2, 
        0x2c, 0x45, 0x92, 0x6c, 0xf3, 0x39, 0x66, 0x42, 0xf2, 0x35, 0x20, 0x6f, 0x77, 0xbb, 0x59, 0x19, 
        0x1d, 0xfe, 0x37, 0x67, 0x2d, 0x31, 0xf5, 0x69, 0xa7, 0x64, 0xab, 0x13, 0x54, 0x25, 0xe9, 0x9, 
        0xed, 0x5c, 0x5, 0xca, 0x4c, 0x24, 0x87, 0xbf, 0x18, 0x3e, 0x22, 0xf0, 0x51, 0xec, 0x61, 0x17, 
        0x16, 0x5e, 0xaf, 0xd3, 0x49, 0xa6, 0x36, 0x43, 0xf4, 0x47, 0x91, 0xdf, 0x33, 0x93, 0x21, 0x3b, 
        0x79, 0xb7, 0x97, 0x85, 0x10, 0xb5, 0xba, 0x3c, 0xb6, 0x70, 0xd0, 0x6, 0xa1, 0xfa, 0x81, 0x82, 
        0x83, 0x7e, 0x7f, 0x80, 0x96, 0x73, 0xbe, 0x56, 0x9b, 0x9e, 0x95, 0xd9, 0xf7, 0x2, 0xb9, 0xa4, 
        0xde, 0x6a, 0x32, 0x6d, 0xd8, 0x8a, 0x84, 0x72, 0x2a, 0x14, 0x9f, 0x88, 0xf9, 0xdc, 0x89, 0x9a, 
        0xfb, 0x7c, 0x2e, 0xc3, 0x8f, 0xb8, 0x65, 0x48, 0x26, 0xc8, 0x12, 0x4a, 0xce, 0xe7, 0xd2, 0x62, 
        0xc, 0xe0, 0x1f, 0xef, 0x11, 0x75, 0x78, 0x71, 0xa5, 0x8e, 0x76, 0x3d, 0xbd, 0xbc, 0x86, 0x57, 
        0xb, 0x28, 0x2f, 0xa3, 0xda, 0xd4, 0xe4, 0xf, 0xa9, 0x27, 0x53, 0x4, 0x1b, 0xfc, 0xac, 0xe6, 
        0x7a, 0x7, 0xae, 0x63, 0xc5, 0xdb, 0xe2, 0xea, 0x94, 0x8b, 0xc4, 0xd5, 0x9d, 0xf8, 0x90, 0x6b, 
        0xb1, 0xd, 0xd6, 0xeb, 0xc6, 0xe, 0xcf, 0xad, 0x8, 0x4e, 0xd7, 0xe3, 0x5d, 0x50, 0x1e, 0xb3, 
        0x5b, 0x23, 0x38, 0x34, 0x68, 0x46, 0x3, 0x8c, 0xdd, 0x9c, 0x7d, 0xa0, 0xcd, 0x1a, 0x41, 0x1c
};

byte SBox[256] = {
       99, 124, 119, 123, 242, 107, 111, 197, 48, 1, 103, 43, 254, 215, 171, 118, 202, 130, 201, 125, 250, 89, 71, 240, 173,
       212, 162, 175, 156, 164, 114, 192, 183, 253, 147, 38, 54, 63, 247, 204, 52, 165, 229, 241, 113, 216, 49, 21, 4, 199,
       35, 195, 24, 150, 5, 154, 7, 18, 128, 226, 235, 39, 178, 117, 9, 131, 44, 26, 27, 110, 90, 160, 82, 59, 214, 179, 41,
       227, 47, 132, 83, 209, 0, 237, 32, 252, 177, 91, 106, 203, 190, 57, 74, 76, 88, 207, 208, 239, 170, 251, 67, 77, 51,
       133, 69, 249, 2, 127, 80, 60, 159, 168, 81, 163, 64, 143, 146, 157, 56, 245, 188, 182, 218, 33, 16, 255, 243, 210,
       205, 12, 19, 236, 95, 151, 68, 23, 196, 167, 126, 61, 100, 93, 25, 115, 96, 129, 79, 220, 34, 42, 144, 136, 70,
       238, 184, 20, 222, 94, 11, 219, 224, 50, 58, 10, 73, 6, 36, 92, 194, 211, 172, 98, 145, 149, 228, 121, 231, 200,
       55, 109, 141, 213, 78, 169, 108, 86, 244, 234, 101, 122, 174, 8, 186, 120, 37, 46, 28, 166, 180, 198, 232, 221,
       116, 31, 75, 189, 139, 138, 112, 62, 181, 102, 72, 3, 246, 14, 97, 53, 87, 185, 134, 193, 29, 158, 225, 248,
       152, 17, 105, 217, 142, 148, 155, 30, 135, 233, 206, 85, 40, 223, 140, 161, 137, 13, 191, 230, 66, 104, 65,
       153, 45, 15, 176, 84, 187, 22
};

byte InvSBox[256] = {
        82, 9, 106, 213, 48, 54, 165, 56, 191, 64, 163, 158, 129, 243, 215, 251, 124, 227, 57, 130, 155, 47, 255,
        135, 52, 142, 67, 68, 196, 222, 233, 203, 84, 123, 148, 50, 166, 194, 35, 61, 238, 76, 149, 11, 66, 250, 195,
        78, 8, 46, 161, 102, 40, 217, 36, 178, 118, 91, 162, 73, 109, 139, 209, 37, 114, 248, 246, 100, 134, 104,
        152, 22, 212, 164, 92, 204, 93, 101, 182, 146, 108, 112, 72, 80, 253, 237, 185, 218, 94, 21, 70, 87, 167,
        141, 157, 132, 144, 216, 171, 0, 140, 188, 211, 10, 247, 228, 88, 5, 184, 179, 69, 6, 208, 44, 30, 143, 202,
        63, 15, 2, 193, 175, 189, 3, 1, 19, 138, 107, 58, 145, 17, 65, 79, 103, 220, 234, 151, 242, 207, 206, 240,
        180, 230, 115, 150, 172, 116, 34, 231, 173, 53, 133, 226, 249, 55, 232, 28, 117, 223, 110, 71, 241, 26, 113,
        29, 41, 197, 137, 111, 183, 98, 14, 170, 24, 190, 27, 252, 86, 62, 75, 198, 210, 121, 32, 154, 219, 192, 254,
        120, 205, 90, 244, 31, 221, 168, 51, 136, 7, 199, 49, 177, 18, 16, 89, 39, 128, 236, 95, 96, 81, 127, 169,
        25, 181, 74, 13, 45, 229, 122, 159, 147, 201, 156, 239, 160, 224, 59, 77, 174, 42, 245, 176, 200, 235, 187,
        60, 131, 83, 153, 97, 23, 43, 4, 126, 186, 119, 214, 38, 225, 105, 20, 99, 85, 33, 12, 125
};

byte Rcon[11][4] = {
        {0x00, 0x00, 0x00, 0x00},
        {0x01, 0x00, 0x00, 0x00},
        {0x02, 0x00, 0x00, 0x00},
        {0x04, 0x00, 0x00, 0x00},
        {0x08, 0x00, 0x00, 0x00},
        {0x10, 0x00, 0x00, 0x00},
        {0x20, 0x00, 0x00, 0x00},
        {0x40, 0x00, 0x00, 0x00},
        {0x80, 0x00, 0x00, 0x00},
        {0x1b, 0x00, 0x00, 0x00},
        {0x36, 0x00, 0x00, 0x00}
};

byte xTime[256] = {
	0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 
	46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88,
	90, 92, 94, 96, 98, 100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122, 124, 126,
	128, 130, 132, 134, 136, 138, 140, 142, 144, 146, 148, 150, 152, 154, 156, 158, 160,
	162, 164, 166, 168, 170, 172, 174, 176, 178, 180, 182, 184, 186, 188, 190, 192, 194,
	196, 198, 200, 202, 204, 206, 208, 210, 212, 214, 216, 218, 220, 222, 224, 226, 228,
	230, 232, 234, 236, 238, 240, 242, 244, 246, 248, 250, 252, 254, 27, 25, 31, 29, 19,
	17, 23, 21, 11, 9, 15, 13, 3, 1, 7, 5, 59, 57, 63, 61, 51, 49, 55, 53, 43, 41, 47, 45,
	35, 33, 39, 37, 91, 89, 95, 93, 83, 81, 87, 85, 75, 73, 79, 77, 67, 65, 71, 69, 123,
	121, 127, 125, 115, 113, 119, 117, 107, 105, 111, 109, 99, 97, 103, 101, 155, 153, 159, 
	157, 147, 145, 151, 149, 139, 137, 143, 141, 131, 129, 135, 133, 187, 185, 191, 189,
	179, 177, 183, 181, 171, 169, 175, 173, 163, 161, 167, 165, 219, 217, 223, 221, 211, 209,
	215, 213, 203, 201, 207, 205, 195, 193, 199, 197, 251, 249, 255, 253, 243, 241, 247,
	245, 235, 233, 239, 237, 227, 225, 231, 229
};

// Pocedure for encrypt...
void xorWords(byte* a, byte* b, int len)
{
    for (byte i = 0; i < len; i++)
    {
        *(a + i) = *(b + i) ^ *(a + i);
    }
}

void xorWords(byte* a, byte* b, byte* result)
{
    result[0] = a[0] ^ b[0];
    result[1] = a[1] ^ b[1];
    result[2] = a[2] ^ b[2];
    result[3] = a[3] ^ b[3];
}

byte mul(byte a, byte b)
{
    byte deg = 0;
    byte result = 0;
    byte tmp = 0;

    while (b > 0)
    {
        if(b & 1)
        {
            tmp = a;
            for (int i = 0; i < deg; i++)
            {
                tmp = xTime[tmp];
            }
            result ^= tmp;
        }
        deg++;
        b >>= 1;
    }

    return result;
}

void subBytes(byte* a)
{
    a[0] = SBox[a[0]];
    a[1] = SBox[a[1]];
    a[2] = SBox[a[2]];
    a[3] = SBox[a[3]];
}

void rotBytes(byte* a)
{
    std::swap(a[0], a[3]);
    std::swap(a[0], a[2]);
    std::swap(a[0], a[1]);
}

void KeyExpansion(byte* key, short nb, short nr, short nk, byte** w)
{
    for (int i = 0; i < nk; i++)
    {
        w[i][0] = key[4 * i];
        w[i][1] = key[4 * i + 1];
        w[i][2] = key[4 * i + 2];
        w[i][3] = key[4 * i + 3];
    }

    byte *temp = new byte[4];

    for (int i = nk; i < nb * (nr + 1); i++)
    {
        temp[0] = w[i - 1][0];
        temp[1] = w[i - 1][1];
        temp[2] = w[i - 1][2];
        temp[3] = w[i - 1][3];

        if (! (i % nk))
        {
            rotBytes(temp);
            subBytes(temp);
            xorWords(temp, Rcon[i / nk], temp);
        }
        else if(nk > 6 && i % nk == 4)
        {
            subBytes(temp);
        }

        xorWords(temp, w[i - nk], temp);

        w[i][0] = temp[0];
        w[i][1] = temp[1];
        w[i][2] = temp[2];
        w[i][3] = temp[3];
    }
}

// make without temporary variable
void addRoundKey(byte** st, byte round, byte** w)
{
    byte** roundKey;
    roundKey = new byte*[4];
    roundKey[0] = new byte[4];
    roundKey[1] = new byte[4];
    roundKey[2] = new byte[4];
    roundKey[3] = new byte[4];

    for (int i = round * 4; i < (round + 1) * 4; i++)
    {
        roundKey[i - round * 4][0] =  w[i][0];
        roundKey[i - round * 4][1] =  w[i][1];
        roundKey[i - round * 4][2] =  w[i][2];
        roundKey[i - round * 4][3] =  w[i][3];
    }

    for (int i = 0; i < 4; i++)
    {
        xorWords(st[i], roundKey[i], st[i]);
        delete[] roundKey[i];
    }
    delete[] roundKey;
}

void shiftRows(byte** state, byte nk)
{
    rotBytes(state[1]);

    for (byte i = 2; i < 4; i++)
    {
        if ((i + nk / 8) % 4 == 2)
        {
            std::swap(state[i][0], state[i][2]);
            std::swap(state[i][1], state[i][3]);
        }
        else if((i + nk / 8) % 4 == 3)
        {
            std::swap(state[i][0], state[i][3]);
            std::swap(state[i][3], state[i][1]);
            std::swap(state[i][3], state[i][2]);
        }
        else if ((i + nk / 8) % 4 == 1)
        {
            rotBytes(state[i]);
        }
    }
}

void subBytes(byte** a)
{
    subBytes(a[0]);
    subBytes(a[1]);
    subBytes(a[2]);
    subBytes(a[3]);
}

void mixColumns(byte** state)
{
    byte* tmp = new byte[4];
    for (byte i = 0; i < 4; i++)
    {
        tmp[0] = state[0][i];
        tmp[1] = state[1][i];
        tmp[2] = state[2][i];
        tmp[3] = state[3][i];

        for (byte j = 0; j < 4; j++)
        {
            state[j][i] = 0;

            for (byte k = 0; k < 4; k++)
            {
                state[j][i] ^= mul(pos[j][k], tmp[k]);
            }
        }
    }
}

void invShiftRows(byte** state, byte nk)
{
    std::swap(state[1][0], state[1][3]);
    std::swap(state[1][3], state[1][1]);
    std::swap(state[1][3], state[1][2]);

    for (byte i = 2; i < 4; i++)
    {
        if ((i + nk / 8) % 4 == 2)
        {
            std::swap(state[i][0], state[i][2]);
            std::swap(state[i][1], state[i][3]);
        }
        else if((i + nk / 8) % 4 == 3)
        {
            rotBytes(state[i]);
        }
        else if ((i + nk / 8) % 4 == 1)
        {
            std::swap(state[i][0], state[i][3]);
            std::swap(state[i][3], state[i][1]);
            std::swap(state[i][3], state[i][2]);
        }
    }
}

void invSubBytes(byte** state)
{
    for (byte i = 0; i < 4; i++)
    {
        state[i][0] = InvSBox[state[i][0]];
        state[i][1] = InvSBox[state[i][1]];
        state[i][2] = InvSBox[state[i][2]];
        state[i][3] = InvSBox[state[i][3]];
    }
}

void invMixColumns(byte** state)
{
    byte* tmp = new byte[4];
    for (byte i = 0; i < 4; i++)
    {
        tmp[0] = state[0][i];
        tmp[1] = state[1][i];
        tmp[2] = state[2][i];
        tmp[3] = state[3][i];

        for (byte j = 0; j < 4; j++)
        {
            state[j][i] = 0;

            for (byte k = 0; k < 4; k++)
            {
                state[j][i] ^= mul(neg[j][k], tmp[k]);
            }
        }
    }
}

void _cipher(byte* mes, byte nr, byte nk)
{
    byte** result = new byte*[4];
    for (byte i = 0; i < 4; i++)
    {
        result[i] = new byte[4];
        result[i][0] = mes[i];
        result[i][1] = mes[i + 4];
        result[i][2] = mes[i + 8];
        result[i][3] = mes[i + 12];
    }

    addRoundKey(result, 0, roundKeys);

    for (byte round = 1; round < nr; round++)
    {
        subBytes(result);
        shiftRows(result, nk);
        mixColumns(result);
        addRoundKey(result, round, roundKeys);
    }

    subBytes(result);
    shiftRows(result, nk);
    addRoundKey(result, nr, roundKeys);

    for (byte i = 0; i < 4; i++)
    {
        mes[i] = result[i][0];
        mes[i + 4] = result[i][1];
        mes[i + 8] = result[i][2];
        mes[i + 12] = result[i][3];
        delete[] result[i];
    }
    delete[] result;
}

void _invCipher(byte* enc, byte nr, byte nk)
{
    byte** result = new byte*[4];
    for (byte i = 0; i < 4; i++)
    {
        result[i] = new byte[4];
        result[i][0] = enc[i];
        result[i][1] = enc[i + 4];
        result[i][2] = enc[i + 8];
        result[i][3] = enc[i + 12];
    }

    addRoundKey(result, nr, roundKeys);

    for (byte round = (byte)(nr - 1); round > 0; round--)
    {
        invShiftRows(result,nk);
        invSubBytes(result);
        addRoundKey(result, round, roundKeys);
        invMixColumns(result);
    }

    invShiftRows(result, nk);
    invSubBytes(result);
    addRoundKey(result, 0, roundKeys);

    for (byte i = 0; i < 4; i++)
    {
        enc[i] = result[i][0];
        enc[i + 4] = result[i][1];
        enc[i + 8] = result[i][2];
        enc[i + 12] = result[i][3];
        delete[] result[i];
    }
    delete[] result;
}

bool AES_ECB_Encrypt(byte* input_buffer, uint32_t ib_len, 
                     byte** output_buffer, uint32_t &ob_len,
                     byte* key, uint32_t k_len)
{
    uint32_t remainder = ib_len & 0xf;
    ob_len = remainder ? ib_len + 16 - remainder : ib_len;
    *output_buffer = new byte[ob_len];
    
    memcpy(*output_buffer, input_buffer, ib_len);
    if (remainder)
    {
        *(*output_buffer + ib_len) = 0x00;
        memset(*output_buffer + ib_len + 1, 0x01, 16 - remainder - 1);
    }
    
    byte nk, nr;
    if (k_len == 16)
    {
        nk = 4;
        nr = 10;
    }
    else if (k_len == 24)
    {
        nk = 6;
        nr = 12;
    }
    else if (k_len == 32)
    {
        nk = 8;
        nr = 14;
    }
    else 
        return false;
    
    roundKeys = new byte*[4 * (nr + 1)];
    for (byte i = 0; i < 4 * (nr + 1); i++)
        roundKeys[i] = new byte[4];
    KeyExpansion(key, 4, nr, nk, roundKeys);

    for (uint32_t offset = 0; offset < ob_len; offset += 16)
        _cipher(*output_buffer + offset, nr, nk);
    
    for (byte i = 0; i < 4 * (nr + 1); i++)
    {
        // clear
        *reinterpret_cast<uint32_t *>(roundKeys[i]) = 0xffffffff;
        delete[] roundKeys[i];
    }

    delete[] roundKeys;
    return true;
}

bool AES_ECB_Decrypt(byte *input_buffer, uint32_t ib_len,
                     byte **output_buffer, uint32_t &ob_len,
                     byte *key, uint32_t k_len)
{
    byte *tmp_buffer = new byte[ib_len];
    memcpy(tmp_buffer, input_buffer, ib_len);

    byte nk, nr;
    if (k_len == 16)
    {
        nk = 4;
        nr = 10;
    }
    else if (k_len == 24)
    {
        nk = 6;
        nr = 12;
    }
    else if (k_len == 32)
    {
        nk = 8;
        nr = 14;
    }
    else 
        return false;

    roundKeys = new byte*[4 * (nr + 1)];
    for (byte i = 0; i < 4 * (nr + 1); i++)
        roundKeys[i] = new byte[4];
    KeyExpansion(key, 4, nr, nk, roundKeys);

    for (uint32_t offset = 0; offset < ib_len; offset += 16)
        _invCipher(tmp_buffer + offset, nr, nk);
    
    ob_len = ib_len;
    for (; *(tmp_buffer + ob_len - 1) == 0x01; ob_len--) {}
    if (*(tmp_buffer + ob_len - 1))
        ob_len = ib_len;
    else
        ob_len--;

    *output_buffer = new byte[ob_len];
    memcpy(*output_buffer, tmp_buffer, ob_len);

    for (byte i = 0; i < 4 * (nr + 1); i++)
    {
        // clear
        *reinterpret_cast<uint32_t *>(roundKeys[i]) = 0xffffffff;
        delete[] roundKeys[i];
    }

    delete[] roundKeys;
    delete[] tmp_buffer;
    return true;
}

bool AES_CBC_Encrypt(byte *input_buffer, uint32_t ib_len,
                     byte **output_buffer, uint32_t &ob_len,
                     byte *key, uint32_t k_len,
                     byte *iv)
{
    uint32_t remainder = ib_len & 0xf;
    ob_len = remainder ? ib_len + 16 - remainder : ib_len;
    *output_buffer = new byte[ob_len];
    
    memcpy(*output_buffer, input_buffer, ib_len);
    if (remainder)
    {
        *(*output_buffer + ib_len) = 0x00;
        memset(*output_buffer + ib_len + 1, 0x01, 16 - remainder - 1);
    }
    
    byte nk, nr;
    if (k_len == 16)
    {
        nk = 4;
        nr = 10;
    }
    else if (k_len == 24)
    {
        nk = 6;
        nr = 12;
    }
    else if (k_len == 32)
    {
        nk = 8;
        nr = 14;
    }
    else 
        return false;
    
    roundKeys = new byte*[4 * (nr + 1)];
    for (byte i = 0; i < 4 * (nr + 1); i++)
        roundKeys[i] = new byte[4];
    KeyExpansion(key, 4, nr, nk, roundKeys);

    xorWords(*output_buffer, iv, 16);
    _cipher(*output_buffer, nr, nk);

    for (uint32_t offset = 16; offset < ob_len; offset += 16)
    {
        xorWords(*output_buffer + offset, *output_buffer + offset - 16, 16);
        _cipher(*output_buffer + offset, nr, nk);
    }
    
    for (byte i = 0; i < 4 * (nr + 1); i++)
    {
        // clear
        *reinterpret_cast<uint32_t *>(roundKeys[i]) = 0xffffffff;
        delete[] roundKeys[i];
    }

    delete[] roundKeys;
    return true;
}

bool AES_CBC_Decrypt(byte *input_buffer, uint32_t ib_len,
                     byte **output_buffer, uint32_t &ob_len,
                     byte *key, uint32_t k_len,
                     byte *iv)
{
    byte *tmp_buffer = new byte[ib_len];
    memcpy(tmp_buffer, input_buffer, ib_len);

    byte nk, nr;
    if (k_len == 16)
    {
        nk = 4;
        nr = 10;
    }
    else if (k_len == 24)
    {
        nk = 6;
        nr = 12;
    }
    else if (k_len == 32)
    {
        nk = 8;
        nr = 14;
    }
    else 
        return false;

    roundKeys = new byte*[4 * (nr + 1)];
    for (byte i = 0; i < 4 * (nr + 1); i++)
        roundKeys[i] = new byte[4];
    KeyExpansion(key, 4, nr, nk, roundKeys);

    _invCipher(tmp_buffer, nr, nk);
    xorWords(tmp_buffer, iv, 16);

    for (uint32_t offset = 16; offset < ib_len; offset += 16)
    {
        _invCipher(tmp_buffer + offset, nr, nk);
        xorWords(tmp_buffer + offset, input_buffer + offset - 16, 16);
    }
    
    ob_len = ib_len;
    for (; *(tmp_buffer + ob_len - 1) == 0x01; ob_len--) {}
    if (*(tmp_buffer + ob_len - 1))
        ob_len = ib_len;
    else
        ob_len--;

    *output_buffer = new byte[ob_len];
    memcpy(*output_buffer, tmp_buffer, ob_len);

    for (byte i = 0; i < 4 * (nr + 1); i++)
    {
        // clear
        *reinterpret_cast<uint32_t *>(roundKeys[i]) = 0xffffffff;
        delete[] roundKeys[i];
    }

    delete[] roundKeys;
    delete[] tmp_buffer;
    return true;
}

/*
void AES_test()
{
    std::cout << "AES-ECB test\n";
    const byte mes[33] = "6b1ae00a9b25cc0fa0e0148472\0";
    const byte key[17] = "5d201068a508d9b0";
    const byte iv[17] =  "4512baa0bd3d1fa2";
    std::cout << "Message: " << mes << std::endl;
    std::cout << "Key: " << key << std::endl;
    std::cout << "IV: " << iv << std::endl;
    byte *enc;
    uint32_t len;
    AES_CBC_Encrypt((byte*)mes, 26, &enc, len, (byte *)key, 16, (byte *)iv);
    std::cout << "Encrypted: " << enc << "\nLen: " << len << std::endl;
    byte *dec;
    uint32_t llen;
    AES_CBC_Decrypt(enc, len, &dec, llen, (byte *)key, 16, (byte *) iv);
    std::cout << "Decrypted: " << dec << "\nLen: " << llen << std::endl;
}
*/