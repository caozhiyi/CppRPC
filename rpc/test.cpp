#include "Any.h"

#include <vector>
#include <iostream>
#include <string>
#include <functional>

std::vector<int> Add(int a, int b) {
	std::vector<int> res;
	res.push_back(a);
	res.push_back(b);
	return res;
}

std::function<std::vector<int>(int, int)> func = Add;

int main() {
	CAny t1 = (120);
	CAny t2 = (std::string("it is a test"));
	CAny t3 = func;

	std::vector<CAny> vec;
	vec.push_back(t1);
	vec.push_back(t2);
	vec.push_back(t3);
	for (int i = 0; i < vec.size(); i++) {
		if (typeid(int) == vec[i].Type()) {
			std::cout << vec[i].Type().name() << any_cast<int>(vec[i]) << std::endl;
		} else if (typeid(std::string) == vec[i].Type()) {
			std::cout << vec[i].Type().name()  << any_cast<std::string>(vec[i]) << std::endl;
		} else if (typeid(std::function<std::vector<int>(int, int)>) == vec[i].Type()) {
			std::cout << vec[i].Type().name() << any_cast<std::function<std::vector<int>(int, int)>>(vec[i])(5,7)[0] << std::endl;
		}
	}

	int i = 0; 
	i++;
}