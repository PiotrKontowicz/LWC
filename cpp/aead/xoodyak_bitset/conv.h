#pragma once
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <vector>

std::string uint8_to_hex_string(const uint8_t* v, const size_t s);
std::map<std::string, uint8_t> get_codes();
std::vector<uint8_t> string_to_uint8_vec(std::string value, std::map<std::string, uint8_t>& code);