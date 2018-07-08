#include "FuncManager.h"

CFuncManager::CFuncManager() {

}


CFuncManager::~CFuncManager() {
	_func_map.clear();
}

bool CFuncManager::RegisterFunc(std::string name, std::string str_func, const CommonFunc& func) {
	if (_func_map.count(name)) {
		return false;
	}
	_func_map[name] = std::make_pair(str_func, func);
	return true;
}

bool CFuncManager::RemoveFunc(std::string name) {
	auto iter = _func_map.find(name);
	if (iter != _func_map.end()) {
		_func_map.erase(iter);
		return true;
	}
	return false;
}

CommonFunc CFuncManager::FindFunc(const std::string& name) {
	auto iter = _func_map.find(name);
	if (iter != _func_map.end()) {
		return iter->second.second;
	}
	return nullptr;
}

std::string CFuncManager::FindFuncStr(const std::string& name) {
	auto iter = _func_map.find(name);
	if (iter != _func_map.end()) {
		return iter->second.first;
	}
	return nullptr;
}

bool CFuncManager::CallFunc(const std::string& name, std::vector<CAny>& param_ret) {
	auto iter = _func_map.find(name);
	if (iter == _func_map.end()) {
		return false;
	}

	//client responsible for parameter verification
	//so can direct call here
	param_ret = iter->second.second(param_ret);
	return true;
}

std::vector<std::string> CFuncManager::GetStrFuncVec() const {
	std::vector<std::string> res;
	for (auto iter = _func_map.begin(); iter != _func_map.end(); ++iter) {
		res.push_back(iter->second.first);
	}
	return res;
}