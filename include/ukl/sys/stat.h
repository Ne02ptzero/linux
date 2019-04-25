#ifndef STAT_H
#define STAT_H

#include <linux/stat.h>

int stat(const char *, struct stat *);
int fstat(int, struct stat *);
int lstat(const char *, struct stat *);
int newfstatat(int, const char *, struct stat *, int);

#endif /* STAT_H */
