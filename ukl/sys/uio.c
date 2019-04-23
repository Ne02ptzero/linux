#include <sys/uio.h>
#include <linux/fs.h>
#include <unistd.h>

#include <ukl/ukl_internals.h>

extern ssize_t do_writev(unsigned long fd, const struct iovec __user *vec,
			 unsigned long vlen, rwf_t flags);
extern ssize_t do_readv(unsigned long fd, const struct iovec __user *vec,
			unsigned long vlen, rwf_t flags);

ssize_t writev(unsigned long fd, const struct iovec *vec, unsigned long vlen)
{
	if (fd == 0) {
		UKL_BAD_FD("writev", 0);
		return -EINVAL;
	}

	/*
	 * For stdout and stderr, don't bother calling the fs, let's go straight
	 * to our wrappers
	 */
	if (fd == 1 || fd == 2) {
		unsigned long i;
		ssize_t ret = 0;

		for (i = 0; i < vlen; i++)
			ret += write(fd, vec[i].iov_base, vec[i].iov_len);

		return ret;
	}
	return do_writev(fd, vec, vlen, 0);
}

ssize_t readv(unsigned long fd, const struct iovec *vec, unsigned long vlen)
{
	if (fd >= 0 && fd <= 2)
	{
	    UKL_BAD_FD("readv", fd);
	    return -EINVAL;
	}

	return do_readv(fd, vec, vlen, 0);
}
