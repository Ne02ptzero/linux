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

	/*
	 * Little trick used to mock stdin, stdout and stderr for UKL
	 * Don't make any sense in kernel space, since a fd could very well
	 * be 0.
	 */
	fd -= 3;

	return ksys_read(fd, buf, count);
}
