#include "RPCClient.h"
#include "InfoRouter.h"
#include "FuncThread.h"
#include "ParsePackage.h"
#include "Log.h"

CRPCClient::CRPCClient() : _connected(false){
}


CRPCClient::~CRPCClient() {
}

//start work
void CRPCClient::Start(short port, std::string ip) {
	CLog::Instance().SetLogLevel(LOG_DEBUG_LEVEL);
	CLog::Instance().SetLogName("CppNet.txt");
	CLog::Instance().Start();

	_net.Init(1);

	_net.SetConnectionCallback(std::bind(&CRPCClient::_DoConnect, this, std::placeholders::_1, std::placeholders::_2));
	_net.SetWriteCallback(std::bind(&CRPCClient::_DoWrite, this, std::placeholders::_1, std::placeholders::_2));
	_net.SetReadCallback(std::bind(&CRPCClient::_DoRead, this, std::placeholders::_1, std::placeholders::_2));
	_net.SetDisconnectionCallback(std::bind(&CRPCClient::_DoDisConnect, this, std::placeholders::_1, std::placeholders::_2));

	//_socket = _net.Connection(port, ip, "\r\n\r\n", strlen("\r\n\r\n"));
	_socket = _net.Connection(port, ip);
	//net.MainLoop();
	//net.Dealloc();
	//_net.Join();
	//CLog::Instance().Stop();
	//CLog::Instance().Join();
}

void CRPCClient::SetCallBack(const std::string& func_name, Call_back& func) {
	_func_call_map[func_name] = func;
}

void CRPCClient::_DoRead(CMemSharePtr<CSocket>& sock, int error) {
	if (error != EVENT_ERROR_NO) {
		return;
	}
	char recv_buf[8192] = { 0 };
	int get_len = 8192;
	int need_len = 0;
	int recv_len = 0;
	for (;;) {
		get_len = sock->_read_event->_buffer->ReadUntil(recv_buf, 8192, "\r\n\r\n", strlen("\r\n\r\n"), need_len);
		if (get_len == 0) {
			break;
		}
		std::vector<CAny> vec;
		int type = 0;
		std::string name;
		int code = NO_ERROR;
		if (!_parse_package->ParseType(recv_buf, get_len, type)) {
			if (_func_call_map.count(name)) {
				_func_call_map[name](PARAM_TYPE_ERROR, vec);
			}
			break;
		}
		if (type & FUNCTION_RET) {
			if (!_parse_package->ParseFuncRet(recv_buf + 2, get_len - 2, code, name, _func_map, vec)) {
				if (_func_call_map.count(name)) {
					_func_call_map[name](PARSE_FUNC_ERROR, vec);
				}
				break;
			}
			if (_func_call_map.count(name)) {
				_func_call_map[name](code, vec);
			}

		} else if (type & FUNCTION_INFO) {
			if (!_parse_package->ParseFuncList(recv_buf + 2, get_len - 2, _func_map)) {
				if (_func_call_map.count(name)) {
					_func_call_map[name](PARSE_FUNC_ERROR, vec);
				}
				break;

			} else {
				if (_func_call_map.count(name)) {
					_func_call_map[name](PARAM_TYPE_ERROR, vec);
				}
				break;
			}
		}
	}
	sock->SyncRead();
}

void CRPCClient::_DoWrite(CMemSharePtr<CSocket>& sock, int error) {
	if (!(error & EVENT_ERROR_NO)) {
		LOG_ERROR("send response to client failed!");
	}
}

void CRPCClient::_DoConnect(CMemSharePtr<CSocket>& sock, int error) {
	_connected = true;
//#ifdef __linux__
//	CRunnable::Sleep(1000);//connect may delay in linux
//#endif // !__linux__
	sock->SyncWrite("\r\n\r\n", strlen("\r\n\r\n"));
	sock->SyncRead();
}

void CRPCClient::_DoDisConnect(CMemSharePtr<CSocket>& sock, int error) {
	LOG_ERROR("disconnect with server!");
	_socket = _net.Connection(_socket->GetPort(), _socket->GetAddress());
}