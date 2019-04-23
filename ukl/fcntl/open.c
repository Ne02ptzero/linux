#include <fcntl.h>
#include <linux/syscalls.h>

#include <stdarg.h>
#include <ukl/ukl_internals.h>

static int __fd_stdin = -1;
static int __fd_stdout = -1;
static int __fd_stderr = -1;

int open(const char *filename, int flags, ...)
{
	umode_t mode = 0;
	int ret = -1;

	/*
	 * On first call, allocate the first three file descriptor in order
	 * to be compliant with userspace behavior
	 */
	if (__fd_stdin == -1)
		__fd_stdin = do_sys_open(AT_FDCWD, "/dev/console", O_RDONLY, 0);
	if (__fd_stdout == -1)
		__fd_stdout = do_sys_open(AT_FDCWD, "/dev/console", O_RDONLY, 0);
	if (__fd_stderr == -1)
		__fd_stderr = do_sys_open(AT_FDCWD, "/dev/console", O_RDONLY, 0);

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
		fcntl(ret, F_SETFD, FD_CLOEXEC);

	return ret;
}
