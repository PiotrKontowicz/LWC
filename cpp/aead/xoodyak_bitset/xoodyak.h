#pragma once
#include <cstdint>
#include <iostream>
//#include <iomanip>
#include <vector>
#include <cassert>
//#include <algorithm>
//#include <algorithm>
#include "xoodoo.h"

constexpr auto ZERO = 0x00;
constexpr auto AB_KEY = 0x02; // ABSORB KEY
constexpr auto ABSORB = 0x03;
constexpr auto RATCHET = 0x10;
constexpr auto SQUEEZE_KEY = 0x20;
constexpr auto SQUEEZE = 0x40;
constexpr auto CRYPT = 0x80;
constexpr auto HASH = 0;
constexpr auto KEYED = 1;

constexpr auto UP = 0;
constexpr auto DOWN = 1;

class rates
{
public:
    rates();
    void hash();
    void keyed();

    uint8_t rate_HASH = 16;
    uint8_t rate_INPUT = 44;
    uint8_t rate_OUTPUT = 24;
    uint8_t rate_RATCHET = 16;

    uint8_t rate_absorb;
    uint8_t rate_squeeze;
};

class xoodyak
{
private:
    bool mode = HASH;
    rates rate;
    bool phase = UP;
    xoodoo_bitset xodo;
public:
    xoodyak();
    xoodyak(std::vector<uint8_t>::iterator key, size_t key_len, std::vector<uint8_t>::iterator ID, size_t id_len, std::vector<uint8_t>::iterator cnt, size_t cnt_len);
    std::vector<uint8_t> enctypt(std::vector<uint8_t>::iterator plain_text, size_t pt_len);
    std::vector<uint8_t> decrypt(std::vector<uint8_t>::iterator ct, size_t ct_len);
    std::vector<uint8_t> squeeze(size_t output_size);
    void absorb(std::vector<uint8_t>::iterator input, size_t in_size);
    void ratchet();
private:
    void absorb_any(std::vector<uint8_t>::iterator input, size_t in_len, size_t rate, uint8_t down_flag);
    std::vector<uint8_t> crypt(std::vector<uint8_t>::iterator input, size_t in_len, bool decrypt);
    std::vector<uint8_t> squeeze_any(std::vector<uint8_t>::iterator output, size_t out_len, uint8_t up_flag);
    void down(std::vector<uint8_t>::iterator block, size_t count, uint8_t flag);
    void up(std::vector<uint8_t>::iterator block, size_t count, uint8_t flag);
    std::vector<uint8_t> squeeze_key(size_t output_size);
};