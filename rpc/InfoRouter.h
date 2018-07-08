#ifndef HEADER_CINFOROUTER
#define HEADER_CINFOROUTER

#include <string>
#include <vector>
#include <mutex>
#include <atomic>

#include "Any.h"
#include "TaskQueue.h"

struct FuncCallInfo;
class CFuncThread;

class CInfoRouter
{
public:
	CInfoRouter();
	~CInfoRouter();
	//create call function thread;
	void Init(int thread_num);
	//stop all call function thread
	void Destroy();
	//push call info
	void PushTask(FuncCallInfo* info);
	//push call function return value
	void PushRet(FuncCallInfo* info);

private:
	CTaskQueue<FuncCallInfo*>	_out_task_list;
	
	std::atomic_int								_curent_index;
	std::vector<std::shared_ptr<CFuncThread>>	_func_thread_vec;
};

#endif