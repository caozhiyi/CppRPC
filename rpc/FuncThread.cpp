#include "FuncThread.h"
#include "InfoRouter.h"

CFuncThread::CFuncThread(CInfoRouter* router) : _func_router(router) {
}

CFuncThread::~CFuncThread() {
}

void CFuncThread::Run() {
	while (!_stop) {
		auto t = _Pop();
		if (t) {
			if (_func_manager.CallFunc(t->_func_name, t->_func_param_ret)){
				_func_router->PushTask(t);
			}

		} else {
			continue;
		}
	}
}

void CFuncThread::Stop() {
	_stop = true;
	Push(nullptr);
}

