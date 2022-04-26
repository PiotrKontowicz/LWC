#include "xoodyak.h"

inline size_t get_min(size_t a, size_t b) {
    if (a < b) {
        return a;
    }
    return b;
}

inline void rates::hash() {
    this->rate_absorb = this->rate_HASH;
    this->rate_squeeze = this->rate_HASH;
}

inline void rates::keyed() {
    this->rate_absorb = this->rate_INPUT;
    this->rate_squeeze = this->rate_OUTPUT;
}

rates::rates() {
    this->rate_absorb = 1;
    this->rate_squeeze = 1;
}

xoodyak::xoodyak() {
    this->rate.hash();
}

xoodyak::xoodyak(std::vector<uint8_t>::iterator key, size_t key_len, std::vector<uint8_t>::iterator ID, size_t id_len, std::vector<uint8_t>::iterator cnt, size_t cnt_len) {
    this->mode = KEYED;
    this->rate.keyed();
    size_t len = key_len + id_len + 1;
    std::vector<uint8_t> buffer(len);

    if (key_len > 0)
        std::copy(key, key + key_len, buffer.begin());

    if (id_len > 0)
        std::copy(ID, ID + id_len, buffer.end());

    buffer[len - 1] = id_len & 0xFF;

    if (len <= this->rate.rate_INPUT)
        assert(1, 0);

    this->absorb_any(buffer.begin(), len, this->rate.rate_absorb, AB_KEY);

    if (cnt_len > 0)
        this->absorb_any(cnt, cnt_len, 1, ZERO);
}


void xoodyak::absorb_any(std::vector<uint8_t>::iterator input, size_t in_len, size_t rate, uint8_t down_flag) {
    do {
        if (this->phase != UP)
            this->up(input, 0, ZERO);

        size_t block_len = get_min(in_len, rate);

        this->down(input, block_len, down_flag);
        in_len -= block_len;
        input += block_len;
        down_flag = ZERO;
    } while (in_len != 0);
}

std::vector<uint8_t> xoodyak::crypt(std::vector<uint8_t>::iterator input, size_t in_len, bool decrypt) {
    uint8_t flag = CRYPT;
    std::vector<uint8_t> out(in_len);
    std::vector<uint8_t>::iterator output = out.begin();

    do {
        size_t block_size = get_min(in_len, this->rate.rate_OUTPUT);
        this->up(input, 0, flag);
        flag = ZERO;

        for (int i = 0; i < block_size; i++)
            *(output + i) = input[i] ^ this->xodo.bytes[i];

        if (decrypt)
            this->down(output, block_size, ZERO);
        else
            this->down(input, block_size, ZERO);

        output += block_size;
        input += block_size;
        in_len -= block_size;
    } while (in_len != 0);

    return out;
}

std::vector<uint8_t> xoodyak::squeeze_any(std::vector<uint8_t>::iterator output, size_t out_len, uint8_t up_flag) {
    std::vector<uint8_t> out;
    size_t block_len = get_min(out_len, this->rate.rate_squeeze);

    this->up(output, block_len, up_flag);
    out.insert(out.end(), output, output + block_len);

    output += block_len;
    out_len -= block_len;

    while (out_len) {
        block_len = get_min(out_len, this->rate.rate_squeeze);
        this->down(output, 0, ZERO);
        this->up(output, block_len, ZERO);
        out.insert(out.end(), output, output + block_len);
        output += block_len;
        out_len -= block_len;
    }
    return out;
}

void xoodyak::down(std::vector<uint8_t>::iterator block, size_t count, uint8_t flag) {
    this->phase = DOWN;
    for (int i = 0; i < count; i++)
        this->xodo.bytes[i] ^= *(block + i);

    this->xodo.bytes[count] ^= 0x01;

    if (this->mode == HASH)
        this->xodo.bytes[47] ^= flag & 0x01;
    else
        this->xodo.bytes[47] ^= flag;
}

void xoodyak::up(std::vector<uint8_t>::iterator block, size_t count, uint8_t flag) {
    this->phase = UP;
    if (this->mode != HASH)
        this->xodo.bytes[47] ^= flag;
    this->xodo.permute();
    for (int i = 0; i < count; i++)
        *(block + i) = this->xodo.bytes[i];
}

std::vector<uint8_t> xoodyak::squeeze_key(size_t output_size)
{
    std::vector<uint8_t> tmp(output_size);
    return this->squeeze_any(tmp.begin(), output_size, SQUEEZE_KEY);
}

std::vector<uint8_t> xoodyak::enctypt(std::vector<uint8_t>::iterator plain_text, size_t pt_len) {
    return this->crypt(plain_text, pt_len, false);
}

std::vector<uint8_t> xoodyak::decrypt(std::vector<uint8_t>::iterator ct, size_t ct_len) {
    std::vector<uint8_t> pt(ct_len);
    return this->crypt(ct, ct_len, true);
}

std::vector<uint8_t> xoodyak::squeeze(size_t output_size) {
    std::vector<uint8_t> to_return(output_size);
    return this->squeeze_any(to_return.begin(), output_size, SQUEEZE);
}

void xoodyak::absorb(std::vector<uint8_t>::iterator input, size_t in_size) {
    this->absorb_any(input, in_size, this->rate.rate_absorb, ABSORB);
}

void xoodyak::ratchet() {
    std::vector<uint8_t> buffer = { this->rate.rate_RATCHET };
    this->squeeze_any(buffer.begin(), 1, RATCHET);
    this->absorb_any(buffer.begin(), 1, this->rate.rate_RATCHET, ZERO);
}