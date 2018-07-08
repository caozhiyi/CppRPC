#ifdef __linux__
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include "LinuxFunc.h"

int SetSocketNoblocking(unsigned int sock) {
	int old_option = fcntl(sock, F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(sock, F_SETFL, new_option);
	return old_option;
}

int SetReusePort(unsigned int sock) {
	int opt = 1;
	int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEPORT,
		&opt, static_cast<socklen_t>(sizeof(opt)));
	return ret;
}

void SetCoreFileUnlimit() {
	struct rlimit rlim;
	struct rlimit rlim_new;
	if (getrlimit(RLIMIT_CORE, &rlim) == 0) {
		rlim_new.rlim_cur = rlim_new.rlim_max = RLIM_INFINITY;
		if (setrlimit(RLIMIT_CORE, &rlim_new) != 0) {
			rlim_new.rlim_cur = rlim_new.rlim_max = rlim.rlim_max;
			setrlimit(RLIMIT_CORE, &rlim_new);
		}
	}
}
#endif