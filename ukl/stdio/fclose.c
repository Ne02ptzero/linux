#include <stdio.h>
#include "internals.h"

#include <linux/vmalloc.h>

int fclose(FILE *f)
{
	int r;

	r = fflush(f);
	r |= f->close(f);
	vfree(f->getln_buf);
	vfree(f);
	return r;
}
