#ifndef HEADER_CFUNCTHREAD
#define HEADER_CFUNCTHREAD

#include "RunnableAloneTaskList.h"
#include "FuncManager.h"

struct FuncCallInfo {
	std::string			_func_name;
	std::vector<CAny>	_func_param_ret;
};

class CFuncManager;
class CInfoRouter;
class CFuncThread : public CRunnableAloneTaskList<FuncCallInfo*>
{
public:
	CFuncThread(CInfoRouter* router);
	~CFuncThread();

	//main loog
	virtual void Run();

	virtual void Stop();

private:
	CInfoRouter*	_func_router;
	CFuncManager	_func_manager;
};

#endif