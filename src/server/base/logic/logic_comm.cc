#include "logic_comm.h"
#include <sys/socket.h>
namespace base_logic{

int LogicComm::SendFull(int socket, const char *buffer, size_t nbytes){
	ssize_t amt = 0;
	ssize_t total = 0;
	const char *buf = buffer;

	do {
		amt = nbytes;
		amt = send (socket, buf, amt, 0);
		buf = buf + amt;
		nbytes -= amt;
		total += amt;
	} while (amt != -1 && nbytes > 0);

	return (int)(amt == -1 ? amt : total);
}

char *LogicComm::SafeStrncpy (char *dest, size_t dest_size, const char *src, size_t src_size)
{
	size_t n = dest_size < src_size ? dest_size : src_size;
	memset(dest,'\0',dest_size);
	memcpy (dest, src, n);
	return dest;
}

}
