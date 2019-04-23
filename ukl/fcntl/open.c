#include <fcntl.h>
#include <linux/syscalls.h>

#include <stdarg.h>
#include <ukl/ukl_internals.h>

int open(const char *filename, int flags, ...)
{
	umode_t mode = 0;
	int ret = -1;

	/* Parse the flags */
	if ((flags & O_CREAT) || (flags & O_TMPFILE) == O_TMPFILE) {
		va_list ap;

		va_start(ap, flags);
		mode = va_arg(ap, umode_t);
		va_end(ap);
	}

	/* Make the call */
	ret = do_sys_open(AT_FDCWD, filename, flags, mode);

	if (ret >= 0 && (flags & O_CLOEXEC))
		fcntl(ret + 3, F_SETFD, FD_CLOEXEC);

	/*
	 * Little trick used to mock stdin, stdout and stderr for UKL
	 * Don't make any sense in kernel space, since a fd could very well
	 * be 0, so we start at 3.
	 */
	return ret + 3;
}
