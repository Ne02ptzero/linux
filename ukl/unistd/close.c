#include <unistd.h>
#include <linux/syscalls.h>

int close(int fd)
{
    return ksys_close(fd);
}
