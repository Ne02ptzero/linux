#include <unistd.h>
#include <linux/syscalls.h>

int close(int fd)
{
    if (fd >= 0 && fd <= 2)
    {
        printk(KERN_ERR "Trying to call close() with %d file descriptor\n", fd);
        return -EINVAL;
    }

    /**
     * Little trick used to mock stdin, stdout and stderr for UKL
     * Don't make any sense in kernel space, since a fd could very well
     * be 0.
     */
    return ksys_close(fd - 3);
}
