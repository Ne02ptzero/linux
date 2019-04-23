#ifndef UNISTD_H
#define UNISTD_H

#include <linux/kernel.h>

ssize_t write(int, const void *, size_t);
ssize_t read(int, void *, size_t);
int close(int);
off_t lseek(unsigned int, off_t, unsigned int);

#endif /* UNISTD_H */
