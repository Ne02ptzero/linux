#include <stdio.h>
#include "internals.h"

#include <linux/string.h>

int fputs(const char *s, FILE *f)
{
	size_t l = strlen(s);
	return (fwrite(s, 1, l, f) == l) - 1;
}
