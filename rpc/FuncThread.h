#ifndef HEADER_CFUNCTHREAD
#define HEADER_CFUNCTHREAD

#include "RunnableAloneTaskList.h"
#include "CommonStruct.h"

class CFuncManager;
class CInfoRouter;
class CFuncThread : public CRunnableAloneTaskList<FuncCallInfo*>
{
public:
	CFuncThread(std::shared_ptr<CInfoRouter>& router);
	~CFuncThread();

	//main loop
	virtual void Run();

	virtual void Stop();
	//register function to map
	bool RegisterFunc(const std::string& name, const CommonFunc& func);
	bool RemoveFunc(const std::string& name);

	//find function by name
	CommonFunc FindFunc(const std::string& name);
	//call function by name. Thread unsafety. param_ret use in/out
	bool CallFunc(const std::string& name, std::vector<CAny>& param_ret);

private:

	std::map<std::string, CommonFunc>	_func_map;
	std::shared_ptr<CInfoRouter>		_func_router;
};

#endif