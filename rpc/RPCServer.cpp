#include "RPCServer.h"
#include "InfoRouter.h"
#include "FuncThread.h"
#include "ParsePackage.h"
#include "Log.h"

CRPCServer::CRPCServer() : _info_router(new CInfoRouter), _parse_package(new CParsePackage), _need_mutex(false){

}

CRPCServer::~CRPCServer() {

}

void CRPCServer::Init(int thread) {
	for (int i = 0; i < thread; i++) {
		auto thread = std::shared_ptr<CFuncThread>(new CFuncThread(_info_router));
		_info_router->AddThread(thread);
	}
}

void CRPCServer::Destroy() {
	_info_router->StopAllThread();
}

void CRPCServer::Start(short port, std::string ip) {
	CLog::Instance().SetLogLevel(LOG_DEBUG_LEVEL);
	CLog::Instance().SetLogName("CppNet.txt");
	CLog::Instance().Start();

	_net.Init(4);

	_net.SetAcceptCallback(std::bind(&CRPCServer::_DoAccept, this, std::placeholders::_1, std::placeholders::_2));
	_net.SetWriteCallback(std::bind(&CRPCServer::_DoWrite, this, std::placeholders::_1, std::placeholders::_2));
	_net.SetReadCallback(std::bind(&CRPCServer::_DoRead, this, std::placeholders::_1, std::placeholders::_2));

	_net.ListenAndAccept(port, ip);

	for (;;) {
		auto info = _info_router->GetRet();
		if (info) {
			auto socket_ptr = info->_socket.Lock();
			if (socket_ptr) {
				std::function<void()> func = std::bind(&CRPCServer::_PackageAndSend, this, socket_ptr, info, NO_ERROR);
				socket_ptr->PostTask(func);
			}
		}
	}

	_net.Join();
	CLog::Instance().Stop();
	CLog::Instance().Join();
}

bool CRPCServer::RegisterFunc(std::string name, std::string func_str, const CommonFunc& func) {
	_need_mutex = true;
	std::unique_lock<std::mutex> lock(_mutex);
	if (_func_map.count(name)) {
		_need_mutex = false;
		return false;
	}
	_func_map[name] = func_str;
	_info_router->RegisterFunc(name, func);
	_need_mutex = false;
	return true;
}

bool CRPCServer::RemoveFunc(std::string name) {
	_need_mutex = true;
	std::unique_lock<std::mutex> lock(_mutex);
	if (_func_map.count(name)) {
		_func_map.erase(name);
		_info_router->RemoveFunc(name);
		_need_mutex = false;
		return true;
	}
	_need_mutex = false;
	return false;
}

void CRPCServer::_DoRead(CMemSharePtr<CSocket>& sock, int error) {
	if (!(error & EVENT_ERROR_NO)) {
		return;
	}
	int get_len = 0;
	int need_len = 0;
	for (;;) {
		char* recv_buf = sock->_pool->PoolLargeMalloc<char>(1024, get_len);
		int read_len = sock->_read_event->_buffer->ReadUntil(recv_buf, get_len, "\r\n\r\n", strlen("\r\n\r\n"), need_len);
		//recv buf is too small
		if (read_len == 0 && need_len > 0) {
			sock->_pool->PoolLargeFree(recv_buf, get_len);
			recv_buf = sock->_pool->PoolLargeMalloc<char>(need_len, get_len);
			sock->_read_event->_buffer->ReadUntil(recv_buf, get_len, "\r\n\r\n", strlen("\r\n\r\n"), need_len);
		}
		//get a comlete message
		if (read_len > 0) {
			auto info = sock->_pool->PoolNew<FuncCallInfo>();
			if (_need_mutex) {
				std::unique_lock<std::mutex> lock(_mutex);
				if (_parse_package->ParseFuncCall(recv_buf + 2, read_len - 2, info->_func_name, _func_map, info->_func_param_ret)) {
					info->_socket = sock;
					_info_router->PushTask(info);
				} else {
					LOG_ERROR("parse function call request failed!");
				}

			} else {
				if (_parse_package->ParseFuncCall(recv_buf + 2, read_len - 2, info->_func_name, _func_map, info->_func_param_ret)) {
					info->_socket = sock;
					_info_router->PushTask(info);
				} else {
					LOG_ERROR("parse function call request failed!");
				}
			}
			sock->_pool->PoolLargeFree(recv_buf, get_len);

		} else {
			sock->_pool->PoolLargeFree(recv_buf, get_len);
			break;
		}
	}
	sock->SyncRead();
}

void CRPCServer::_DoWrite(CMemSharePtr<CSocket>& sock, int error) {
	if (!(error & EVENT_ERROR_NO)) {
		LOG_ERROR("send response to client failed!");
	}
}

void CRPCServer::_DoAccept(CMemSharePtr<CSocket>& sock, int error) {
	char buf[8192] = { 0 };
	int len = 8192;
	if (_need_mutex) {
		std::unique_lock<std::mutex> lock(_mutex);
		if (!_parse_package->PackageFuncList(buf, len, _func_map)) {
			LOG_ERROR("package functnion info failed!");
			abort();
		}
	} else {
		if (!_parse_package->PackageFuncList(buf, len, _func_map)) {
			LOG_ERROR("package functnion info failed!");
			abort();
		}
	}
	sock->SyncWrite(buf, len);
	sock->SyncRead();
	CRunnable::Sleep(10000);
	sock->SyncRead();
}

void CRPCServer::_PackageAndSend(CMemSharePtr<CSocket>& sock, FuncCallInfo* info, int code) {
	if (!info) {
		LOG_ERROR("function info is null!");
		return;
	}
	bool send = true;
	int get_len = 0;
	int need_len = 0;
	char* send_buf = sock->_pool->PoolLargeMalloc<char>(1024, get_len);
	need_len = get_len;
	if (!_parse_package->PackageFuncRet(send_buf, need_len, code, info->_func_name, _func_map, info->_func_param_ret)) {
		sock->_pool->PoolLargeFree<char>(send_buf, get_len);
		if (need_len == get_len) {
			send_buf = sock->_pool->PoolLargeMalloc<char>(8192, get_len);
			if (_need_mutex) {
				std::unique_lock<std::mutex> lock(_mutex);
				if (!_parse_package->PackageFuncRet(send_buf, need_len, code, info->_func_name, _func_map, info->_func_param_ret)) {
					sock->_pool->PoolLargeFree(send_buf, get_len);
					LOG_ERROR("package function response failed!");
					send = false;
				}

			} else {
				if (!_parse_package->PackageFuncRet(send_buf, need_len, code, info->_func_name, _func_map, info->_func_param_ret)) {
					sock->_pool->PoolLargeFree(send_buf, get_len);
					LOG_ERROR("package function response failed!");
					send = false;
				}
			}
			
		} else {
			LOG_ERROR("package function response failed!");
			send = false;
		}
	}
	if (send) {
		sock->SyncWrite(send_buf, need_len);
	}
	sock->_pool->PoolLargeFree<char>(send_buf, get_len);
}