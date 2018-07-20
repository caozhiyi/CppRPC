#ifndef HEADER_LOOPBUFFER
#define HEADER_LOOPBUFFER

#include <memory>

#include "NetObject.h"
#include "CommonStruct.h"
#include "ParsePackage.h"

typedef std::function<void(int code, std::vector<CAny>& ret)> Call_back;

class CInfoRouter;
class CParsePackage;
class CRPCClient {
public:
	CRPCClient();
	~CRPCClient();
	//start work
	void Start(short port, std::string ip);
	//set call back when rpc server response called;
	void SetCallBack(const std::string& func_name, Call_back& func);
	template<typename...Args>
	bool CallFunc(const std::string& func_name, Args&&...args);
public:
	void _DoRead(CMemSharePtr<CSocket>& sock, int error);
	void _DoWrite(CMemSharePtr<CSocket>& sock, int error);
	void _DoConnect(CMemSharePtr<CSocket>& sock, int error);
	void _DoDisConnect(CMemSharePtr<CSocket>& sock, int error);

private:
	bool				_connected;
	CNetObject			_net;
	std::shared_ptr<CInfoRouter>		_info_router;
	std::shared_ptr<CParsePackage>		_parse_package;

	CMemSharePtr<CSocket>				_socket;
	std::map<std::string, Call_back>	_func_call_map;
	std::map<std::string, std::string>  _func_map;
};

template<typename...Args>
bool CRPCClient::CallFunc(const std::string& func_name, Args&&...args) {
	if (!_func_map.count(func_name)) {
		return false;
	}
	if (!_func_call_map.count(func_name)) {
		return false;
	}
	if (!_connected) {
		return false;
	}


	std::vector<CAny> vec;
	_parse_package->ParseParam(vec, std::forward<Args>(args)...);

	char buf[8192] = { 0 };
	int len = 8192;
	if (!_parse_package->PackageFuncCall(buf, len, func_name, _func_map, vec)) {
		return false;
	}
	_socket->SyncWrite(buf, len);
}

#endif