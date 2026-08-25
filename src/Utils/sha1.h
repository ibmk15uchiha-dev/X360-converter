#ifndef _XGD_SHA1_H_
#define _XGD_SHA1_H_

#include <cstdint>
#include <cstring>

#define SHA_DIGEST_LENGTH 20

namespace XGD_SHA1 
{
    inline uint32_t rol(uint32_t value, size_t bits) { return (value << bits) | (value >> (32 - bits)); }

    inline void transform(uint32_t state[5], const uint8_t buffer[64]) 
    {
        uint32_t block[80];
        for (int i = 0; i < 16; ++i) {
            block[i] = (buffer[i * 4] << 24) | (buffer[i * 4 + 1] << 16) | (buffer[i * 4 + 2] << 8) | (buffer[i * 4 + 3]);
        }
        for (int i = 16; i < 80; ++i) {
            block[i] = rol(block[i - 3] ^ block[i - 8] ^ block[i - 14] ^ block[i - 16], 1);
        }

        uint32_t a = state[0], b = state[1], c = state[2], d = state[3], e = state[4];

        for (int i = 0; i < 80; ++i) {
            uint32_t f, k;
            if (i < 20) {
                f = (b & c) | ((~b) & d);
                k = 0x5A827999;
            } else if (i < 40) {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1;
            } else if (i < 60) {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDC;
            } else {
                f = b ^ c ^ d;
                k = 0xCA62C1D6;
            }
            uint32_t temp = rol(a, 5) + f + e + k + block[i];
            e = d;
            d = c;
            c = rol(b, 30);
            b = a;
            a = temp;
        }

        state[0] += a;
        state[1] += b;
        state[2] += c;
        state[3] += d;
        state[4] += e;
    }

    inline void compute(const uint8_t* data, size_t len, uint8_t hash[SHA_DIGEST_LENGTH]) 
    {
        uint32_t state[5] = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 };
        uint64_t bit_len = len * 8;

        while (len >= 64) {
            transform(state, data);
            data += 64;
            len -= 64;
        }

        uint8_t buffer[64] = {0};
        std::memcpy(buffer, data, len);
        buffer[len] = 0x80;

        if (len >= 56) {
            transform(state, buffer);
            std::memset(buffer, 0, 64);
        }

        for (int i = 0; i < 8; ++i) {
            buffer[63 - i] = static_cast<uint8_t>(bit_len >> (i * 8));
        }
        transform(state, buffer);

        for (int i = 0; i < 5; ++i) {
            hash[i * 4]     = static_cast<uint8_t>(state[i] >> 24);
            hash[i * 4 + 1] = static_cast<uint8_t>(state[i] >> 16);
            hash[i * 4 + 2] = static_cast<uint8_t>(state[i] >> 8);
            hash[i * 4 + 3] = static_cast<uint8_t>(state[i]);
        }
    }
}

#endif // _XGD_SHA1_H_
