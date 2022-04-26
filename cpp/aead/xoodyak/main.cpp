#include "xoodyak.h"
#include <iostream>
#include <string>
#include <map>

// To run tests uncomment next line.
//#define test 

#ifdef test
#include "test.h"

void run_tests()
{
	test_xoodyak();
}
#endif 

#ifndef test
#include "conv.h"
#endif 


std::vector<uint8_t> encrypt(std::vector<uint8_t> AD, std::vector<uint8_t> NONCE, std::vector<uint8_t> KEY,
	std::vector<uint8_t> pt) {

	std::vector<uint8_t> x;

	xoodyak xod(KEY.begin(), KEY.size(), x.begin(), 0, x.begin(), 0);
	xod.absorb(NONCE.begin(), NONCE.size());
	xod.absorb(AD.begin(), AD.size());
	std::vector<uint8_t> p(pt.size());
	p = xod.enctypt(pt.begin(), pt.size());

	std::vector<uint8_t> tag(16);
	tag = xod.squeeze(16);

	p.insert(
		p.end(),
		std::make_move_iterator(tag.begin()),
		std::make_move_iterator(tag.end())
	);

	return p;
}

std::vector<uint8_t> decrypt(std::vector<uint8_t> AD, std::vector<uint8_t> NONCE, std::vector<uint8_t> KEY,
	std::vector<uint8_t> ct) {
	std::vector<uint8_t> x;
	xoodyak xod(KEY.begin(), KEY.size(), x.begin(), 0, x.begin(), 0);
	xod.absorb(NONCE.begin(), NONCE.size());
	xod.absorb(AD.begin(), AD.size());
	std::vector<uint8_t> p;
	std::vector<uint8_t> c;
	for (int i = 0; i < ct.size() - 16; i++) {
		c.push_back(ct[i]);
	}
	p = xod.decrypt(c.begin(), c.size());

	std::vector<uint8_t> t(16);
	t = xod.squeeze(16);

	int j = 0;
	for (int i = ct.size() - 16; i < ct.size(); i++) {
		if (t[j] != ct[i]) {
			throw std::exception("Wrong tag.");
		}
		j = j + 1;
	}
	return p;
}

std::vector<uint8_t> string_to_vec(std::string in) {
	std::vector<uint8_t> to_return;
	std::copy(in.begin(), in.end(), std::back_inserter(to_return));
	return to_return;
}

void enc() {
	std::string ad = "";
	std::string nonce = "";
	std::string key = "";
	std::string plain = "";

	std::cout << "Enter key:\n";
	std::cin >> key;

	std::cout << "Entere nonce:\n";
	std::cin >> nonce;

	std::cout << "Enter additional data:\n";
	std::cin >> ad;

	std::cout << "Enter plain text:\n";
	std::cin >> plain;

	std::vector<uint8_t> ct = encrypt(string_to_vec(ad), string_to_vec(nonce), string_to_vec(key), string_to_vec(plain));
	std::clog << "Ciphertext:\n";
	for (auto i = ct.begin(); i != ct.end(); i++) {
		std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(*i);
	}
	std::clog << "\n";
}

void dec() {
	std::string ad = "";
	std::string nonce = "";
	std::string key = "";
	std::string cipher = "";

	std::cout << "Enter key:\n";
	std::cin >> key;

	std::cout << "Entere nonce:\n";
	std::cin >> nonce;

	std::cout << "Enter additional data:\n";
	std::cin >> ad;

	std::cout << "Enter cipher text:\n";
	std::cin >> cipher;
	std::map<std::string, uint8_t> code = get_codes();
	std::vector<uint8_t> pt = decrypt(string_to_vec(ad), string_to_vec(nonce), string_to_vec(key), string_to_uint8_vec(cipher, code));
	std::clog << "Text:\n";
	for (auto i = pt.begin(); i != pt.end(); i++) {
		std::cout << *i;
	}
	std::clog << "\n";
}


int main()
{
#ifdef test
	run_tests();
#else
	int k = 0;
	while (true) {
		std::clog << "Enter 1 to encrypt data, enter 2 to decrypt data:\n";
		std::cin >> k;
		switch (k)
		{
		case 1: {
			enc();
			break;
		}
		case 2: {
			try {
				dec();
			}
			catch (const std::exception& e) {
				std::cout << "Wrong tag, check input data.\n";
			}

			break;
		}
		default:
			break;
		}
	}
#endif
}