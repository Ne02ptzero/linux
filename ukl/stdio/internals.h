#ifndef STDIO_INTERNALS_H
#define STDIO_INTERNALS_H

#include <stdio.h>

#define UNGET 8

#define F_PERM 1
#define F_NORD 4
#define F_NOWR 8
#define F_EOF 16
#define F_ERR 32
#define F_SVB 64
#define F_APP 128

struct _IO_FILE {
	unsigned flags;
	unsigned char *rpos, *rend;
	int (*close)(FILE *);
	unsigned char *wend, *wpos;
	unsigned char *mustbezero_1;
	unsigned char *wbase;
	size_t (*read)(FILE *, unsigned char *, size_t);
	size_t (*write)(FILE *, const unsigned char *, size_t);
	off_t (*seek)(FILE *, off_t, int);
	unsigned char *buf;
	size_t buf_size;
	FILE *prev, *next;
	int fd;
	int pipe_pid;
	long lockcount;
	int mode;
	volatile int lock;
	int lbf;
	void *cookie;
	off_t off;
	char *getln_buf;
	void *mustbezero_2;
	unsigned char *shend;
	off_t shlim, shcnt;
	FILE *prev_locked, *next_locked;
	//struct __locale_struct *locale;
};

int __fmodeflags(const char *mode);
FILE *__fdopen(int fd, const char *mode);
size_t __stdio_write(FILE *f, const unsigned char *buf, size_t len);
size_t __stdio_read(FILE *f, unsigned char *buf, size_t len);
int __stdio_close(FILE *f);
off_t __stdio_seek(FILE *f, off_t off, int whence);
size_t __fwritex(const unsigned char *s, size_t l, FILE *f);
int __towrite(FILE *f);
int __toread(FILE *f);

#endif /* STDIO_INTERNALS_H */
