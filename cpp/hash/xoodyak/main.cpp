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
	
#endif
}