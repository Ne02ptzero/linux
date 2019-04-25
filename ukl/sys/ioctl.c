#include <sys/ioctl.h>
#include <linux/syscalls.h>

int ioctl(int fd, int req, ...)
{
    void *arg;
    va_list ap;

    va_start(ap, req);
    arg = va_arg(ap, void *);
    va_end(ap);

    return ksys_ioctl(fd, req, (unsigned long)arg);
}
