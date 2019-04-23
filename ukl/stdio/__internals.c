#include <fcntl.h>
#include <unistd.h>
#include <linux/vmalloc.h>
#include "internals.h"
#include <sys/uio.h>

int __fmodeflags(const char *mode)
{
	int flags;

	if (strchr(mode, '+'))
		flags = O_RDWR;
	else if (*mode == 'r')
		flags = O_RDONLY;
	else
		flags = O_WRONLY;

	if (strchr(mode, 'x'))
		flags |= O_EXCL;
	if (strchr(mode, 'e'))
		flags |= O_CLOEXEC;
	if (*mode != 'r')
		flags |= O_CREAT;
	if (*mode == 'w')
		flags |= O_TRUNC;
	if (*mode == 'a')
		flags |= O_APPEND;
	return flags;
}

FILE *__fdopen(int fd, const char *mode)
{
	FILE *f;

	if (!strchr("rwa", *mode)) {
		// errno = EINVAL;
		return NULL;
	}

	f = vmalloc(sizeof(*f) + UNGET + BUFSIZ);
	if (f == NULL)
		return NULL;

	memset(f, 0, sizeof(*f));

	/* Impose mode restrictions */
	if (!strchr(mode, '+'))
		f->flags = (*mode == 'r') ? F_NOWR : F_NORD;

	/* Apply close-on-exec flag */
	if (strchr(mode, 'e'))
		fcntl(fd, F_SETFD, FD_CLOEXEC);

	/* Set append mode on fd if opened for append */
	if (*mode == 'a') {
		int flags = fcntl(fd, F_GETFL, 0);

		if (!(flags & O_APPEND))
			fcntl(fd, F_SETFL, flags | O_APPEND);
		f->flags |= F_APP;
	}

	f->fd = fd;
	f->buf = (unsigned char *)f + sizeof(*f) + UNGET;
	f->buf_size = BUFSIZ;

	/* Activate line buffered mode for terminals */
	f->lbf = EOF;

	f->read = __stdio_read;
	f->write = __stdio_write;
	f->seek = __stdio_seek;
	f->close = __stdio_close;

	return f;
}

size_t __stdio_write(FILE *f, const unsigned char *buf, size_t len)
{
	struct iovec iovs[2] = { { .iov_base = f->wbase,
				   .iov_len = f->wpos - f->wbase },
				 { .iov_base = (void *)buf, .iov_len = len } };
	struct iovec *iov = iovs;
	size_t rem = iov[0].iov_len + iov[1].iov_len;
	int iovcnt = 2;
	ssize_t cnt;

	while (true) {
		cnt = writev(f->fd, iov, iovcnt);
		if (cnt == rem) {
			f->wend = f->buf + f->buf_size;
			f->wpos = f->wbase = f->buf;
			return len;
		}
		if (cnt < 0) {
			f->wpos = f->wbase = f->wend = 0;
			f->flags |= F_ERR;
			return iovcnt == 2 ? 0 : len - iov[0].iov_len;
		}
		rem -= cnt;
		if (cnt > iov[0].iov_len) {
			cnt -= iov[0].iov_len;
			iov++;
			iovcnt--;
		}
		iov[0].iov_base = (char *)iov[0].iov_base + cnt;
		iov[0].iov_len -= cnt;
	}
}

size_t __stdio_read(FILE *f, unsigned char *buf, size_t len)
{
	struct iovec iov[2] = {
		{ .iov_base = buf, .iov_len = len - !!f->buf_size },
		{ .iov_base = f->buf, .iov_len = f->buf_size }
	};
	ssize_t cnt;

	cnt = iov[0].iov_len ? readv(f->fd, iov, 2) :
			       read(f->fd, iov[1].iov_base, iov[1].iov_len);
	if (cnt <= 0) {
		f->flags |= cnt ? F_ERR : F_EOF;
		return 0;
	}
	if (cnt <= iov[0].iov_len)
		return cnt;

	cnt -= iov[0].iov_len;
	f->rpos = f->buf;
	f->rend = f->buf + cnt;
	if (f->buf_size)
		buf[len - 1] = *f->rpos++;
	return len;
}

int __stdio_close(FILE *f)
{
	return close(f->fd);
}

off_t __stdio_seek(FILE *f, off_t off, int whence)
{
	return lseek(f->fd, off, whence);
}

int __towrite(FILE *f)
{
	f->mode |= f->mode - 1;
	if (f->flags & F_NOWR) {
		f->flags |= F_ERR;
		return EOF;
	}
	/* Clear read buffer (easier than summoning nasal demons) */
	f->rpos = f->rend = 0;

	/* Activate write through the buffer. */
	f->wpos = f->wbase = f->buf;
	f->wend = f->buf + f->buf_size;

	return 0;
}

int __toread(FILE *f)
{
	f->mode |= f->mode - 1;

	if (f->wpos != f->wbase)
		f->write(f, 0, 0);

	f->wpos = f->wbase = f->wend = 0;

	if (f->flags & F_NORD) {
		f->flags |= F_ERR;
		return EOF;
	}

	f->rpos = f->rend = f->buf + f->buf_size;
	return (f->flags & F_EOF) ? EOF : 0;
}
