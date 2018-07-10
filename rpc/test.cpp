#include "Any.h"

#include <vector>
#include <iostream>
#include <string>
#include <functional>

#include "RPCClient.h"

int main() {
	CRPCClient c;
	std::vector<CAny> vec;

	c._ParseParam(vec, 100, 'c', 3.1514, std::string("15024aaa"));
	int a = 0;
	a++;
}
