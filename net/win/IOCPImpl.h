#ifndef __linux__
#ifndef HEADER_IOCPIMPL
#define HEADER_IOCPIMPL

#include <winsock2.h>
#include <MSWSock.h>
#pragma comment(lib,"ws2_32.lib")

#include "EventActions.h"
#include "PoolSharedPtr.h"

#define MAX_BUFFER_LEN        8192
class Cevent;
struct EventOverlapped {
	OVERLAPPED	_overlapped;
	WSABUF      _wsa_buf;
	int			_event_flag_set;
	char        _lapped_buffer[MAX_BUFFER_LEN];
	void*		_event;

	EventOverlapped() {
		_event_flag_set = 0;
		memset(&_overlapped, 0, sizeof(_overlapped));
		memset(_lapped_buffer, 0, MAX_BUFFER_LEN);
		_wsa_buf.buf = _lapped_buffer;
		_wsa_buf.len = MAX_BUFFER_LEN;
	}

	~EventOverlapped() {
		int i = 0;
		i++;
	}

	void Clear() {
		_event_flag_set = 0;
		memset(_lapped_buffer, 0, MAX_BUFFER_LEN);
	}
};

class CIOCPImpl : public CEventActions
{
public:
	CIOCPImpl();
	~CIOCPImpl();

	virtual bool Init();
	virtual bool Dealloc();

	virtual bool AddTimerEvent(unsigned int interval, int event_flag, CMemSharePtr<CEventHandler>& event);
	virtual bool AddSendEvent(CMemSharePtr<CEventHandler>& event);
	virtual bool AddRecvEvent(CMemSharePtr<CEventHandler>& event);
	virtual bool AddAcceptEvent(CMemSharePtr<CAcceptEventHandler>& event);
	virtual bool AddConnection(CMemSharePtr<CEventHandler>& event, const std::string& ip, short port);
	virtual bool AddDisconnection(CMemSharePtr<CEventHandler>& event);
	virtual bool DelEvent(CMemSharePtr<CEventHandler>& event);

	virtual void ProcessEvent();

public:
	void SetIocpHandler(HANDLE handler) { _iocp_handler = handler; }
	HANDLE GetIocpHandler() { return _iocp_handler; }

private:
	bool _PostRecv(CMemSharePtr<CEventHandler>& event);
	bool _PostAccept(CMemSharePtr<CAcceptEventHandler>& event);
	bool _PostSend(CMemSharePtr<CEventHandler>& event);
	bool _PostConnection(CMemSharePtr<CEventHandler>& event, const std::string& ip, short port);
	bool _PostDisconnection(CMemSharePtr<CEventHandler>& event);

	void _DoTimeoutEvent(std::vector<TimerEvent>& timer_vec);
	void _DoEvent(EventOverlapped *socket_context, int bytes);
private:
	HANDLE	_iocp_handler;
};

#endif
#endif