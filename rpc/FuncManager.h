#ifndef HEADER_CFUNCMANAGER
#define HEADER_CFUNCMANAGER
/***************************************
define a function to string:
type info:
i	int
c	char
s	string
d	double
l	long
b	bool
vi	vector<int>
vc	vector<char>
...
string function format:
return_type_list(param_type_list)
for example:
bbi(ii)	means params are two int that
return are two bool and one int
****************************************/

#include <functional>
#include <vector>
#include <map>
#include <mutex>

#include "Any.h"
#include "CommonStruct.h"

class CFuncManager
{
public:
	CFuncManager();
	~CFuncManager();

	//register function to map
	bool RegisterFunc(std::string name, std::string str_func, const CommonFunc& func);
	bool RemoveFunc(std::string name);

	//find function by name
	CommonFunc FindFunc(const std::string& name);
	//find string function by name
	std::string FindFuncStr(const std::string& name);
	//call function by name. Thread unsafety. param_ret use in/out
	bool CallFunc(const std::string& name, std::vector<CAny>& param_ret);
	//get string function vector
	std::vector<std::string> GetStrFuncVec() const;

private:
	std::map<std::string, std::pair<std::string, CommonFunc>> _func_map;
};

#endif