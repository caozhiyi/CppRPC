#include <iostream>
#include "RPCClient.h"
#include "Any.h"
#include "Runnable.h"
using namespace std;

void Add1CallBack(const std::string& name, int code, std::vector<CAny>& ret) {
	if (code == NO_ERROR) {
		cout << name << "  " << code << "  " << any_cast<int>(ret[0]) << endl;
	}
}

Call_back func = Add1CallBack;

int main() {
	CRPCClient client;
	client.SetCallBack(func);
	client.Start(8951, "192.168.2.105");
	for (;;) {
		CRunnable::Sleep(1000);
		client.CallFunc("Add1", 100, 200);
	}
}