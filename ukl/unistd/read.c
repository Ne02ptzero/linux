#include <unistd.h>
#include <linux/syscalls.h>

#include <ukl/ukl_internals.h>

ssize_t read(int fd, void *buf, size_t count)
{
	/* We don't support pipe, stdin, stdout or stderr reading */
	if (fd >= 0 && fd <= 2) {
		UKL_BAD_FD("read", fd);
		return -EINVAL;
	}

	return ksys_read(fd, buf, count);
}
