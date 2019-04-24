#ifndef STDLIB_H
#define STDLIB_H

#include <linux/types.h>
#include <linux/nls.h>

#define MB_CUR_MAX 4

size_t wcrtomb(char *s, wchar_t wc, mbstate_t *st);
int wctomb(char *s, wchar_t wc);

#endif /* STDLIB_H */
