#include <memory>

#include "InfoRouter.h"
#include "FuncThread.h"

CInfoRouter::CInfoRouter() {
}

CInfoRouter::~CInfoRouter() {
}

void CInfoRouter::Init(int thread_num) {
	for (int i = 0; i < thread_num; i++ ) {
		std::shared_ptr<CFuncThread> func_thread(new CFuncThread(this));
		_func_thread_vec.push_back(func_thread);
	}
}

void CInfoRouter::Destroy() {
	for (int i = 0; i < _func_thread_vec.size(); i++) {
		_func_thread_vec[i]->Stop();
		CRunnable::Sleep(100);
	}
}

void CInfoRouter::PushTask(FuncCallInfo* info) {
	_func_thread_vec[_curent_index]->Push(info);
	_curent_index++;
	if (_curent_index > _func_thread_vec.size() - 1) {
		_curent_index = _curent_index % (int)_func_thread_vec.size();
	}
}

void CInfoRouter::PushRet(FuncCallInfo* info) {
	_out_task_list.Push(info);
}
