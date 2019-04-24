#include <stdio.h>
#include "internals.h"

#include <linux/string.h>

int fputs(const char *s, FILE *f)
{
	size_t l = strlen(s);

	return (fwrite(s, 1, l, f) == l) - 1;
}

int puts(const char *s)
{
	int r;
	r = -(fputs(s, stdout) < 0 || putc_unlocked('\n', stdout) < 0);
	return r;
}
