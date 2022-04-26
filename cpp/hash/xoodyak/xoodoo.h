#pragma once
#include <cstdint>
#include <array>
#include <iomanip>
#include <iostream>

#include <bitset>
#include <string>

class xoodoo
{
public:
	std::array<uint8_t, 48> bytes = { 0 };
	void permute();
};
