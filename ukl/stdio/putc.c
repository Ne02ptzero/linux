#include <stdio.h>
#include "internals.h"

int putc_unlocked(int c, FILE *f)
{
    return putc(c, f);
}

int putchar(int c)
{
    return putc(c, stdout);
}

int putc(int c, FILE *f)
{
	return (((unsigned char)(c) != (f)->lbf && (f)->wpos != (f)->wend)) ?
		       *(f)->wpos++ = (unsigned char)(c) :
		       __overflow((f), (unsigned char)(c));
}
