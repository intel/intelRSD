#include "shim.h"
#include <sys/socket.h>

// wedge between system and actual system call.   (Used by test harness to intercept calls)

ssize_t sys_send (int __fd, const void *__buf, size_t __n, int __flags)
{
    return send(__fd,__buf,__n,__flags);
}

