#ifndef FCNTL_H
#define FCNTL_H

#include <stdarg.h>
#include <linux/syscalls.h>

int open(const char *filename, int flags, ...);
int fcntl(int fd, int cmd, ...);

#endif /* FCNTL_H */
