#ifndef STDIO_H
#define STDIO_H

#include <linux/kernel.h>

#define BUFSIZ 1024
#define EOF (-1)

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

typedef struct _IO_FILE FILE;

extern FILE __stdin_FILE;
extern FILE __stdout_FILE;
extern FILE __stderr_FILE;

#define stdin (&__stdin_FILE)
#define stdout (&__stdout_FILE)
#define stderr (&__stderr_FILE)

FILE *fopen(const char *, const char *);
size_t fwrite(const void *, size_t, size_t, FILE *);
size_t fread(void *, size_t, size_t, FILE *);
int fputs(const char *, FILE *);
int puts(const char *);
int fclose(FILE *);
int fflush(FILE *);
int fseek(FILE *, long, int);
int putc(int c, FILE *f);
int putc_unlocked(int c, FILE *f);
int putchar(int c);
int vfprintf(FILE *f, const char *fmt, va_list ap);
int printf(const char *fmt, ...);
int fprintf(FILE *f, const char *fmt, ...);
int vprintf(const char *fmt, va_list ap);
int vsnprintf(char *s, size_t n, const char *fmt, va_list ap);
int sprintf(char *s, const char *fmt, ...);
int snprintf(char *s, size_t n, const char *fmt, ...);

#endif /* STDIO_H */
