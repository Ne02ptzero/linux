#ifndef SYS_UIO_H
#define SYS_UIO_H

#include <uapi/linux/uio.h>

ssize_t writev(unsigned long fd, const struct iovec *vec, unsigned long vlen);
ssize_t readv(unsigned long fd, const struct iovec *vec, unsigned long vlen);

#endif /* SYS_UIO_H */
