#ifndef UNISTD_H
#define UNISTD_H

#include <linux/kernel.h>

ssize_t write(int, const void *, size_t);
ssize_t read(int, void *, size_t);
int close(int);

#endif /* UNISTD_H */
