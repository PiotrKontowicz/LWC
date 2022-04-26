#include "xoodoo.h"

void xoodoo::permute() {
    std::array<uint32_t, 12> s = { 0 };

    size_t j = 0;

    for (size_t i = 0; i < 12; i++) {
        s[i] = this->bytes[j] << 0;
        j += 1;
        s[i] |= this->bytes[j] << 8;
        j += 1;
        s[i] |= this->bytes[j] << 16;
        j += 1;
        s[i] |= this->bytes[j] << 24;
        j += 1;
    }
    std::array<uint32_t, 12> round_constants = { 0x058, 0x038, 0x3C0, 0x0D0, 0x120, 0x014, 0x060, 0x02C, 0x380, 0x0F0, 0x1A0, 0x012 };

    for (const auto& rc : round_constants) {

        std::array<uint32_t, 4> e = { 0 };


        for (size_t i = 0; i < 4; i++) {
            uint32_t r = s[i] ^ s[i + 4] ^ s[i + 8];
            e[i] = (r >> 18) | (r << 14) & 0xFFFFFFFF;
            r = e[i];
            e[i] ^= (r >> 9) | (r << 23) & 0xFFFFFFFF;
        }
        for (int i = 0; i < 12; i++) {
            s[i] ^= e[(i - 1) & 3];
        }

        std::swap(s[7], s[4]);
        std::swap(s[7], s[5]);
        std::swap(s[7], s[6]);
        s[0] ^= rc;

        for (size_t i = 0; i < 4; i++) {
            uint32_t a = s[i];
            uint32_t b = s[i + 4];
            uint32_t r = s[i + 8];
            uint32_t c = (r >> 21) | (r << 11) & 0xFFFFFFFF;
            r = (b & ~a) ^ c;
            s[i + 8] = (r >> 24) | (r << 8) & 0xFFFFFFFF;
            r = (a & ~c) ^ b;
            s[i + 4] = (r >> 31) | (r << 1) & 0xFFFFFFFF;
            s[i] ^= c & ~b;
        }

        std::swap(s[8], s[10]);
        std::swap(s[9], s[11]);
    }

    j = 0;

    for (size_t i = 0; i < 12; i++) {
        this->bytes[j] = (s[i] >> 0) & 0xFF;
        j += 1;
        this->bytes[j] = (s[i] >> 8) & 0xFF;
        j += 1;
        this->bytes[j] = (s[i] >> 16) & 0xFF;
        j += 1;
        this->bytes[j] = (s[i] >> 24) & 0xFF;
        j += 1;
    }
}