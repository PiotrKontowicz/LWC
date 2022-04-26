#include "conv.h"

std::string uint8_to_hex_string(const uint8_t* v, const size_t s) {
	std::stringstream ss;

	ss << std::hex << std::setfill('0');

	for (int i = 0; i < s; i++) {
		ss << std::hex << std::setw(2) << static_cast<int>(v[i]);
	}

	return ss.str();
}

std::map<std::string, uint8_t> get_codes() {
	std::map<std::string, uint8_t> uint8_hex;

	uint8_t j = 0;
	for (int i = 0; i <= 255; i++) {
		std::string ccc = uint8_to_hex_string(&j, 1);
		std::transform(ccc.begin(), ccc.end(), ccc.begin(),
			[](unsigned char c) { return std::toupper(c); });

		uint8_hex[ccc] = j;
		j++;
	}
	return uint8_hex;
}

std::vector<uint8_t> string_to_uint8_vec(std::string value, std::map<std::string, uint8_t>& code) {
	std::vector<uint8_t> to_return;
	std::transform(value.begin(), value.end(), value.begin(), ::toupper);

	for (uint32_t i = 0; i < value.length(); i += 2) {
		std::string x = value.substr(i, 2);
		uint8_t hehehehe = code[x];
		to_return.push_back(hehehehe);
	}

	return to_return;
}