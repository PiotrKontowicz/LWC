#include "xoodoo.h"

xoodoo_bitset::xoodoo_bitset()
{
    for (uint8_t i = 0; i < 3; i++) {
        for (uint8_t j = 0; j < 4; j++) {
            this->state[i][j] = std::bitset<32>(0);
        }
    }
}

std::array<uint8_t, 48> xoodoo_bitset::state_to_array()
{
    std::string tmp_state = "";
    for (uint8_t y = 0; y < 3; y++) {
        for (uint8_t x = 0; x < 4; x++) {
            std::string w = this->state[y][x].to_string();
            tmp_state += w;
        }
    }
    std::array<uint8_t, 48> to_return = { 0 };

    int cnt = 0;
    for (uint16_t i = 0; i < tmp_state.length(); i += 8) {
        int k;
        k = std::bitset<8>(tmp_state.substr(i, 8)).to_ullong();
        to_return[cnt] = k;
        cnt += 1;
    }

    for (int i = 0; i < 48; i += 4) {
        std::reverse(to_return.begin() + i, to_return.begin() + i + 4);
    }

    return to_return;
}

void xoodoo_bitset::permute()
{
    this->bytes_to_state();
    for (uint8_t r = 0; r < 12; r++) {
        round(r);
    }
    this->bytes = this->state_to_array();
}


void xoodoo_bitset::bytes_to_state()
{
    std::string bytes = "";
    std::array<uint8_t, 48> tmp_data = this->bytes;
    std::array<std::array<std::bitset<32>, 4>, 3> res_state;

    for (int i = 0; i < 48; i += 4) {
        std::reverse(tmp_data.begin() + i, tmp_data.begin() + i + 4);
    }

    for (int i = 0; i < 48; i++) {
        bytes += std::bitset<8>(tmp_data[i]).to_string();
    }

    int i = 0;
    for (uint8_t y = 0; y < 3; y++) {
        for (uint8_t x = 0; x < 4; x++) {
            res_state[y][x] = std::bitset<32>(bytes.substr(i, 32));
            i += 32;
        }
    }

    this->state = res_state;
}

std::array<std::bitset<32>, 4> xoodoo_bitset::cyclic_shift(uint16_t t, uint16_t v, std::array<std::bitset <32>, 4> plane)
{
    std::array<std::bitset<32>, 4> to_return = plane;

    for (uint8_t x = 0; x < 4; x++) {
        for (uint8_t z = 0; z < 32; z++) {
            int x_pos = ((x + t) % 4);
            int z_pos = ((z + v) % 32);
            to_return[x_pos][z_pos] = plane[x][z];
        }
    }
    return to_return;
}

std::array<std::bitset<32>, 4> xoodoo_bitset::xor_plane(std::array<std::bitset<32>, 4> a, std::array<std::bitset<32>, 4> b)
{
    std::array<std::bitset<32>, 4> to_return;

    for (uint8_t i = 0; i < 4; i++)
        to_return[i] = a[i] ^ b[i];

    return to_return;
}

std::array<std::bitset<32>, 4> xoodoo_bitset::xor_plane(std::array<std::bitset<32>, 4> a, std::array<std::bitset<32>, 4> b, std::array<std::bitset<32>, 4> c)
{
    std::array<std::bitset<32>, 4> to_return;

    for (uint8_t i = 0; i < 4; i++)
        to_return[i] = a[i] ^ b[i] ^ c[i];

    return to_return;
}

std::array<std::bitset<32>, 4> xoodoo_bitset::neg_plane(std::array<std::bitset<32>, 4> a)
{
    std::array<std::bitset<32>, 4> to_return;

    for (uint8_t i = 0; i < 4; i++)
        to_return[i] = ~a[i];

    return to_return;
}

std::array<std::bitset<32>, 4> xoodoo_bitset::and_plane(std::array<std::bitset<32>, 4> a, std::array<std::bitset<32>, 4> b)
{
    std::array<std::bitset<32>, 4> to_return;

    for (uint8_t i = 0; i < 4; i++)
        to_return[i] = a[i] & b[i];

    return to_return;
}

void xoodoo_bitset::theta()
{
    std::array<std::bitset<32>, 4> P = xor_plane(this->state[0], this->state[1], this->state[2]);
    std::array<std::bitset<32>, 4> E = xor_plane(cyclic_shift(1, 5, P), cyclic_shift(1, 14, P));

    for (uint8_t i = 0; i < 3; i++) {
        this->state[i] = xor_plane(this->state[i], E);
    }
}

void xoodoo_bitset::rho_west()
{
    this->state[1] = cyclic_shift(1, 0, this->state[1]);
    this->state[2] = cyclic_shift(0, 11, this->state[2]);
}

void xoodoo_bitset::iota(uint8_t i)
{
    this->state[0][0] = this->state[0][0] ^ std::bitset<32>(this->round_constants[i]);
}

void xoodoo_bitset::chi()
{
    std::array<std::bitset<32>, 4> B0 = and_plane(neg_plane(this->state[1]), this->state[2]);
    std::array<std::bitset<32>, 4> B1 = and_plane(neg_plane(this->state[2]), this->state[0]);
    std::array<std::bitset<32>, 4> B2 = and_plane(neg_plane(this->state[0]), this->state[1]);

    this->state[0] = xor_plane(this->state[0], B0);
    this->state[1] = xor_plane(this->state[1], B1);
    this->state[2] = xor_plane(this->state[2], B2);
}

void xoodoo_bitset::rho_east()
{
    this->state[1] = cyclic_shift(0, 1, this->state[1]);
    this->state[2] = cyclic_shift(2, 8, this->state[2]);
}

void xoodoo_bitset::round(uint8_t r)
{
    theta();
    rho_west();
    iota(r);
    chi();
    rho_east();
}