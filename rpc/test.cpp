#include "Any.h"

#include <vector>
#include <iostream>
#include <string>
#include <functional>

#include "ParsePackage.h"

std::map<std::string, std::pair<std::string, CommonFunc>> func_map;

int main() {
	CParsePackage p;
	func_map["Add"] = std::make_pair("viibvidlcsvi(viibvidlcsvi)", nullptr);
	func_map["Test"] = std::make_pair("ii(vivi)", nullptr);

	char buf[1024] = { 0 };
	int len = sizeof(buf);
	bool res = p.PackageFuncList(buf, len, func_map);
	std::map<std::string, std::string> ret;
	p.ParseFuncList(buf + 2, len, ret);
	int a = 0;
	a++;
}
