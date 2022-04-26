#include "test.h"
#include "xoodoo.h"
#include <cassert>
#include <string.h>
#include <fstream>


void test_xoodoo() {
	xoodoo_bitset x;

	for (int i = 0; i < 384; i++) {
		x.permute();
	}

	std::array<uint8_t, 48> data = x.state_to_array();
	uint8_t result[] = { 0xb0, 0xfa, 0x04, 0xfe, 0xce, 0xd8, 0xd5, 0x42,
		0xe7, 0x2e, 0xc6, 0x29, 0xcf, 0xe5, 0x7a, 0x2a,
		0xa3, 0xeb, 0x36, 0xea, 0x0a, 0x9e, 0x64, 0x14,
		0x1b, 0x52, 0x12, 0xfe, 0x69, 0xff, 0x2e, 0xfe,
		0xa5, 0x6c, 0x82, 0xf1, 0xe0, 0x41, 0x4c, 0xfc,
		0x4f, 0x39, 0x97, 0x15, 0xaf, 0x2f, 0x09, 0xeb };

	for (int i = 0; i < 48; i++) {
		assert(result[i] == data[i]);
	}
}

std::vector<std::map<std::string, std::vector<uint8_t>>> parse_hash(std::string path) {

	std::vector<std::map<std::string, std::vector<uint8_t>>> to_return;

	std::fstream file(path);

	std::string msg_line;
	std::string msg_new_line;

	std::string md_line;
	std::string md_new_line;

	std::map<std::string, uint8_t> code = get_codes();

	if (file.is_open()) {
		while (std::getline(file, msg_line)) {
			if (msg_line.find("msg") != std::string::npos) {

				msg_new_line = msg_line.substr(16, (msg_line.length() - 18));

				std::getline(file, md_line);
				md_new_line = md_line.substr(15, (md_line.length() - 16));

				std::map<std::string, std::vector<uint8_t>> tmp = {
					{"msg", string_to_uint8_vec(msg_new_line, code)},
					{"md", string_to_uint8_vec(md_new_line, code)}
				};

				to_return.push_back(tmp);
			}
		}
	}
	else {
		std::cout << "File " << path << " not found!\n";
	}

	return to_return;
}

void test_hash() {
	std::vector<std::map<std::string, std::vector<uint8_t>>> data = parse_hash("vectors/hash.json");

	for (int jj = 0; jj < data.size(); jj++) {
		std::vector<uint8_t> msg = data[jj]["msg"];
		std::vector<uint8_t>  md = data[jj]["md"];

		xoodyak x;
		x.absorb(msg.begin(), msg.size());
		std::vector<uint8_t> tag(32);
		tag = x.squeeze(32);

		assert(tag.size() == md.size());
		assert(tag == md);
	}
}

std::vector<std::map<std::string, std::vector<uint8_t>>> parse_aead(std::string path) {
	std::vector<std::map<std::string, std::vector<uint8_t>>> to_return;

	std::fstream file(path);

	std::string key;
	std::string nonce;
	std::string pt;
	std::string ad;
	std::string ct;

	std::map<std::string, uint8_t> code = get_codes();

	if (file.is_open()) {
		while (std::getline(file, key)) {
			if (key.find("key") != std::string::npos) {

				key = key.substr(16, (key.length() - 18));

				std::getline(file, nonce);
				nonce = nonce.substr(18, nonce.length() - 20);

				std::getline(file, pt);
				pt = pt.substr(15, pt.length() - 17);

				std::getline(file, ad);
				ad = ad.substr(15, ad.length() - 17);

				std::getline(file, ct);
				ct = ct.substr(15, ct.length() - 16);

				std::map<std::string, std::vector<uint8_t>> tmp = {
					{"key", string_to_uint8_vec(key, code)},
					{"nonce", string_to_uint8_vec(nonce, code)},
					{"pt", string_to_uint8_vec(pt, code)},
					{"ad", string_to_uint8_vec(ad, code)},
					{"ct", string_to_uint8_vec(ct, code)},
				};

				to_return.push_back(tmp);
			}
		}
	}
	else {
		std::cout << "File " << path << " not found!\n";
	}

	return to_return;
}

void test_aead() {
	std::vector<std::map<std::string, std::vector<uint8_t>>> data = parse_aead("vectors/aead.json");
	uint64_t i;
	for (i = 0; i < data.size(); i++) {
		std::vector<uint8_t> key = data[i]["key"];
		std::vector<uint8_t> nonce = data[i]["nonce"];
		std::vector<uint8_t> pt = data[i]["pt"];
		std::vector<uint8_t> ad = data[i]["ad"];
		std::vector<uint8_t> ct = data[i]["ct"];

		std::vector<uint8_t> x;

		xoodyak xod(key.begin(), key.size(), x.begin(), 0, x.begin(), 0);

		xod.absorb(nonce.begin(), nonce.size());
		xod.absorb(ad.begin(), ad.size());
		std::vector<uint8_t> new_ct(pt.size());
		new_ct = xod.enctypt(pt.begin(), pt.size());

		std::vector<uint8_t> tag(16);
		tag = xod.squeeze(16);

		new_ct.insert(new_ct.end(), tag.begin(), tag.end());

		assert(new_ct == ct);
	}
}

void test_xoodyak() {
	std::clog << "Test xoodoo...\n";
	test_xoodoo();
	std::clog << "Xoodoo test done.\n";

	std::clog << "Test Xoodyak hash...\n";
	test_hash();
	std::clog << "Xoodyak hash test done.\n";

	std::clog << "Test Xoodyak aead...\n";
	test_aead();
	std::clog << "Xoodyak aead test done.\n";
}