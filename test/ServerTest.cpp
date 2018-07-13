#include <iostream>
#include "RPCServer.h"
#include "Any.h"

std::vector<CAny> Add1(std::vector<CAny> param) {
	int res = any_cast<int>(param[0])  + any_cast<int>(param[1]);
	std::vector<CAny> ret;
	ret.push_back(CAny(res));
	return ret;
}

int main() {
	CRPCServer server;
	server.Init(4);
	server.RegisterFunc("Add1", "i(ii)", Add1);
	server.Start(8951, "0.0.0.0");
}