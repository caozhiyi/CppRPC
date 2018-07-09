#ifndef HEADER_COMMONSTRUCT
#define HEADER_COMMONSTRUCT

#include <vector>
#include <string>
#include <thread>

#include "PoolSharedPtr.h"

enum MessageType {
	FUNCTION_CALL	= 0x01,	//client call functnion request 
	FUNCTION_RET	= 0x02, //server return functnion response
	FUNCTION_INFO	= 0x04  //server notice functnion info
};

enum ERROR_CODE {
	NO_ERROR			= 0,
	PARAM_TYPE_ERROR	= 1,
	PARAM_NUM_ERROR		= 2,
};

class CAny;
class CSocket;
struct FuncCallInfo {
	std::string				_func_name;
	std::vector<CAny>		_func_param_ret;

	std::thread::id			_thread_id;
	CMemWeakPtr<CSocket>	_socket;
};


typedef std::function<std::vector<CAny>(std::vector<CAny>)> CommonFunc;
#endif
