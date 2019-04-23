#include "internals.h"
#include <linux/string.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

size_t fread(void *destv, size_t size, size_t nmemb, FILE *f)
{
	unsigned char *dest = destv;
	size_t len = size * nmemb, l = len, k;
	if (!size)
		nmemb = 0;

	f->mode |= f->mode - 1;

	if (f->rpos != f->rend) {
		/* First exhaust the buffer. */
		k = MIN(f->rend - f->rpos, l);
		memcpy(dest, f->rpos, k);
		f->rpos += k;
		dest += k;
		l -= k;
	}

	/* Read the remainder directly */
	for (; l; l -= k, dest += k) {
		k = __toread(f) ? 0 : f->read(f, dest, l);
		if (!k) {
			return (len - l) / size;
		}
	}

	return nmemb;
}
