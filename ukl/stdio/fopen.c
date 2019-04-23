#include "internals.h"

#include <unistd.h>
#include <fcntl.h>

FILE *fopen(const char *filename, const char *mode)
{
	FILE *f = NULL;
	int fd;
	int flags;

	/* Check for valid initial mode character */
	if (!strchr("rwa", *mode)) {
		// errno
		return NULL;
	}

	flags = __fmodeflags(mode);
	fd = open(filename, flags, 0666);
	if (fd < 0)
		return NULL;

	if (flags & O_CLOEXEC)
		fcntl(fd, F_SETFD, FD_CLOEXEC);

        f = __fdopen(fd, mode);
	if (f)
		return f;

	close(fd);
	return NULL;
}
