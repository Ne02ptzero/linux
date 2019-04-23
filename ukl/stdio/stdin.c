#include "internals.h"

static unsigned char buf[BUFSIZ + UNGET];
FILE __stdin_FILE = {
	.buf = buf + UNGET,
	.buf_size = sizeof buf - UNGET,
	.fd = 0,
	.flags = F_PERM | F_NORD,
	.lbf = '\n',
	.write = __stdio_write,
	.seek = __stdio_seek,
	.close = __stdio_close,
	.lock = -1,
};
