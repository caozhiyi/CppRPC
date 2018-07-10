#ifndef HEADER_CRPCSERVER
#define HEADER_CRPCSERVER
#include <memory>

#include "NetObject.h"
#include "CommonStruct.h"

class CInfoRouter;
class CParsePackage;
class CRPCServer {
public:
	CRPCServer();
	~CRPCServer();
	//create func thread and add to router
	void Init(int thread);
	//Destroy func thread
	void Destroy();
	//start work
	void Start(short port, std::string ip);

	bool RegisterFunc(std::string name, std::string func_str, const CommonFunc& func);
	bool RemoveFunc(std::string name);

private:
	void _DoRead(CMemSharePtr<CSocket>& sock, int error);
	void _DoWrite(CMemSharePtr<CSocket>& sock, int error);
	void _DoAccept(CMemSharePtr<CSocket>& sock, int error);
	void _PackageAndSend(CMemSharePtr<CSocket>& sock, FuncCallInfo* info, int code);

private:
	CNetObject			_net;
	std::shared_ptr<CInfoRouter>		_info_router;
	std::shared_ptr<CParsePackage>		_parse_package;

	std::atomic_bool					_need_mutex;
	std::mutex							_mutex;
	std::map<std::string, std::string>	_func_map;
};

#endif