#ifndef HEADER_CFUNCTHREAD
#define HEADER_CFUNCTHREAD

#include "RunnableAloneTaskList.h"
#include "FuncManager.h"
#include "CommonStruct.h"

class CFuncManager;
class CInfoRouter;
class CFuncThread : public CRunnableAloneTaskList<FuncCallInfo*>
{
public:
	CFuncThread(CInfoRouter* router);
	~CFuncThread();

	//main loop
	virtual void Run();

	virtual void Stop();

private:
	CInfoRouter*	_func_router;
	CFuncManager	_func_manager;
};

#endif