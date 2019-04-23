#include <unistd.h>
#include <linux/syscalls.h>

off_t lseek(unsigned int fd, off_t offset, unsigned int whence)
{
    return ksys_lseek(fd, offset, whence);
}
