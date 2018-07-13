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
	client.Start(8951, "172.16.81.250");
	for (;;) {
		client.CallFunc("Add1", 100, 200);
		CRunnable::Sleep(2000);
	}
}