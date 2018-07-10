#ifndef HEADER_LOOPBUFFER
#define HEADER_LOOPBUFFER

#include <memory>

#include "NetObject.h"
#include "CommonStruct.h"

typedef std::function<void(const std::string& name, int code, std::vector<CAny>& ret)> Call_back;

class CInfoRouter;
class CParsePackage;
class CRPCClient {
public:
	CRPCClient();
	~CRPCClient();
	//start work
	void Start(short port, std::string ip);
	//set call back when rpc server response called;
	void SetCallBack(Call_back& func);
	template<typename...Args>
	bool CallFunc(const std::string& func_name, Args&&...args);
public:
	void _DoRead(CMemSharePtr<CSocket>& sock, int error);
	void _DoWrite(CMemSharePtr<CSocket>& sock, int error);
	void _DoConnect(CMemSharePtr<CSocket>& sock, int error);
	void _DoDisConnect(CMemSharePtr<CSocket>& sock, int error);

	template <typename T, typename ...Args>
	void _ParseParam(std::vector<CAny>& vec, T&& first, Args&&... args);
	template <class T>
	void _ParseParam(std::vector<CAny>& vec, T&& end);

private:
	Call_back			_call_back;
	CNetObject			_net;
	std::shared_ptr<CInfoRouter>		_info_router;
	std::shared_ptr<CParsePackage>		_parse_package;

	CMemSharePtr<CSocket>				_socket;
	std::map<std::string, std::string>	_func_map;
};

template<typename...Args>
bool CRPCClient::CallFunc(const std::string& func_name, Args&&...args) {
	if (!_func_map.count(func_name)) {
		return false;
	}

	std::vector<CAny> vec;
	_ParseParam(vec, std::forward<Args>(args)...);

	char buf[8192] = { 0 };
	int len = 8192;
	if (!_parse_package->PackageFuncCall(buf, len, func_name, _func_map, vec)) {
		return false;
	}
	_socket->SyncWrite(buf, len);
}

template <typename T, typename ...Args>
void CRPCClient::_ParseParam(std::vector<CAny>& vec, T&& first, Args&&... args) {
	vec.push_back(CAny(std::forward<T>(first)));
	_ParseParam(vec, std::forward<Args>(args)...);
}

template <class T>
void CRPCClient::_ParseParam(std::vector<CAny>& vec, T&& end) {
	vec.push_back(CAny(std::forward<T>(end)));
}

#endif