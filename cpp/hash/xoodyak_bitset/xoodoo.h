#pragma once
#include <cstdint>
#include <array>
#include <iomanip>
#include <iostream>

#include <bitset>
#include <string>


class xoodoo_bitset
{
public:
	std::array<uint8_t, 48> bytes = { 0 };
	xoodoo_bitset();
	std::array<std::array<std::bitset<32>, 4>, 3> state;
	std::array<uint8_t, 48> state_to_array();
	void permute();

private:
	const std::array<uint32_t, 12> round_constants = { 0x058, 0x038, 0x3C0, 0x0D0, 0x120, 0x014, 0x060, 0x02C, 0x380, 0x0F0, 0x1A0, 0x012 };

	void bytes_to_state();
	// Done
	std::array<std::bitset<32>, 4> cyclic_shift(uint16_t t, uint16_t v, std::array<std::bitset <32>, 4> plane);
	// Done
	std::array<std::bitset<32>, 4> xor_plane(std::array<std::bitset<32>, 4> a, std::array<std::bitset<32>, 4> b);
	// Done
	std::array<std::bitset<32>, 4> xor_plane(std::array<std::bitset<32>, 4> a, std::array<std::bitset<32>, 4> b, std::array<std::bitset<32>, 4> c);
	// Done
	std::array<std::bitset<32>, 4> neg_plane(std::array<std::bitset<32>, 4> a);
	// Done 
	std::array<std::bitset<32>, 4> and_plane(std::array<std::bitset<32>, 4> a, std::array<std::bitset<32>, 4> b);

	void theta();
	void rho_west();
	void iota(uint8_t i);
	void chi();
	void rho_east();

	void round(uint8_t r);
};