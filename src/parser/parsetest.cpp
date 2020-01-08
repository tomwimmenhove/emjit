#include <iostream>
#include "driver.hh"

int parse_test()
{
	driver drv;
	//drv.trace_parsing = true;
	//drv.trace_scanning = true;
	return drv.parse ("src/parser/parseme.txt");
}

//int main() { return parse_test(); }
