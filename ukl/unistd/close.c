#include <unistd.h>
#include <linux/syscalls.h>

#include <ukl/ukl_internals.h>

int close(int fd)
{
	if (fd >= 0 && fd <= 2) {
		UKL_BAD_FD("close", fd);
		return -EINVAL;
	}

	/*
	 * Little trick used to mock stdin, stdout and stderr for UKL
	 * Don't make any sense in kernel space, since a fd could very well
	 * be 0.
	 */
	return ksys_close(fd - 3);
}
