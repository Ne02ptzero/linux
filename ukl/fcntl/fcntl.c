#include <fcntl.h>
#include <unistd.h>
#include <linux/security.h>

#include <ukl/ukl_internals.h>

extern long do_fcntl(int fd, unsigned int cmd, unsigned long arg,
		     struct file *filp);
extern int check_fcntl_cmd(unsigned cmd);

static long sys_fcntl(int fd, unsigned int cmd, unsigned long arg)
{
	struct fd f = fdget_raw(
		fd); /* Get the file associated with the file descriptor */
	long err = -EBADF;

	/* Not a file, nothing to do! */
	if (!f.file)
		return err;

	/* On PATH mode, check that the cmd issued is compliant */
	if (unlikely(f.file->f_mode) & FMODE_PATH) {
		if (!check_fcntl_cmd(cmd)) {
			/* cmd is not compliant, release the file lock and return the err */
			fdput(f);
			return err;
		}
	}

	/* Check the rights on file */
	err = security_file_fcntl(f.file, cmd, arg);

	/* Do the syscall */
	if (!err)
		err = do_fcntl(fd, cmd, arg, f.file);

	/* Release the lock and return the code */
	fdput(f);
	return err;
}

int fcntl(int fd, int cmd, ...)
{
	unsigned long arg;
	va_list ap;

	if (fd >= 0 && fd <= 2) {
		UKL_BAD_FD("fcntl", fd);
		return -EINVAL;
	}

	/* Read optionnals arguments */
	va_start(ap, cmd);
	arg = va_arg(ap, unsigned long);
	va_end(ap);

	if (cmd == F_SETFL)
		arg |= O_LARGEFILE;

	if (cmd == F_SETLKW)
		return sys_fcntl(fd, cmd, arg);

	/* If the fd is a socket, get the process owner's pid */
	if (cmd == F_GETOWN) {
		struct f_owner_ex ex;
		int ret = sys_fcntl(fd, F_GETOWN_EX, &ex);

		if (ret == -EINVAL)
			return sys_fcntl(fd, cmd, arg);

		if (ret)
			return ret;
		return ex.type == F_OWNER_PGRP ? -ex.pid : ex.pid;
	}

	/* Clone the fd, with FD_CLOEXEC flag */
	if (cmd == F_DUPFD_CLOEXEC) {
		int ret = sys_fcntl(fd, F_DUPFD_CLOEXEC, arg); /* Try to dup */

		if (ret != -EINVAL) {
			/* Dup successful, set the flag and return the new fd */
			if (ret >= 0)
				sys_fcntl(ret, F_SETFD, FD_CLOEXEC);
			return ret;
		}

		/* Dup has failed, cleanup and return error */
		ret = sys_fcntl(fd, F_DUPFD_CLOEXEC, 0);
		if (ret != -EINVAL) {
			if (ret >= 0)
				close(ret);
			return -EINVAL;
		}

		/* Last try, make the dup and set in two calls */
		ret = sys_fcntl(fd, F_DUPFD, arg);
		if (ret >= 0)
			sys_fcntl(ret, F_SETFD, FD_CLOEXEC);
		return ret;
	}

	return sys_fcntl(fd, cmd, arg);
}
