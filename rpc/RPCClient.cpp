#include "RPCClient.h"
#include "InfoRouter.h"
#include "FuncThread.h"
#include "ParsePackage.h"
#include "Log.h"

CRPCClient::CRPCClient() {
}


CRPCClient::~CRPCClient() {
}

//start work
void CRPCClient::Start(short port, std::string ip) {
	CLog::Instance().SetLogLevel(LOG_WARN_LEVEL);
	CLog::Instance().SetLogName("CppNet.txt");
	CLog::Instance().Start();

	_net.Init(1);

	_net.SetConnectionCallback(std::bind(&CRPCClient::_DoConnect, this, std::placeholders::_1, std::placeholders::_2));
	_net.SetWriteCallback(std::bind(&CRPCClient::_DoWrite, this, std::placeholders::_1, std::placeholders::_2));
	_net.SetReadCallback(std::bind(&CRPCClient::_DoRead, this, std::placeholders::_1, std::placeholders::_2));
	_net.SetDisconnectionCallback(std::bind(&CRPCClient::_DoDisConnect, this, std::placeholders::_1, std::placeholders::_2));

	auto sock = _net.Connection(port, ip);
	//net.MainLoop();
	//net.Dealloc();
	_net.Join();
	CLog::Instance().Stop();
	CLog::Instance().Join();
}

void CRPCClient::SetCallBack(Call_back& func) {
	_call_back = func;
}

void CRPCClient::_DoRead(CMemSharePtr<CSocket>& sock, int error) {
	if (error != EVENT_ERROR_NO) {
		return;
	}

	char recv_buf[8192] = { 0 };
	int get_len = 8192;
	int need_len = 0;
	for (;;) {
		sock->_read_event->_buffer->ReadUntil(recv_buf, get_len, "\r\n\r\n", strlen("\r\n\r\n"), need_len);
		if (get_len == 0) {
			break;
		}
		std::vector<CAny> vec;
		int type = 0;
		std::string name;
		int code = NO_ERROR;
		if (!_parse_package->ParseType(recv_buf, get_len, type)) {
			if (_call_back) {
				_call_back(name, PARAM_TYPE_ERROR, vec);
			}
			break;
		}
		if (type & FUNCTION_RET) {
			if (!_parse_package->ParseFuncRet(recv_buf, get_len, code, name, _func_map, vec)) {
				if (_call_back) {
					_call_back(name, PARSE_FUNC_ERROR, vec);
				}
				break;
			}
			_call_back(name, code, vec);

		} else if (type & FUNCTION_INFO) {
			if (!_parse_package->ParseFuncList(recv_buf, get_len, _func_map)) {
				if (_call_back) {
					_call_back(name, PARSE_FUNC_ERROR, vec);
				}
				break;

			} else {
				if (_call_back) {
					_call_back(name, PARAM_TYPE_ERROR, vec);
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
	sock->SyncRead();
}

void CRPCClient::_DoDisConnect(CMemSharePtr<CSocket>& sock, int error) {
	LOG_ERROR("disconnect with server!");
	_socket = _net.Connection(_socket->GetPort(), _socket->GetAddress());
}