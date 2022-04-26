#include "xoodyak.h"
#include <iostream>
#include <string>
#include <map>

// To run tests uncomment next line.
#define test 

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