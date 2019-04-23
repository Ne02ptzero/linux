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
int fclose(FILE *);
int fflush(FILE *);
int fseek(FILE *, long, int);

#endif /* STDIO_H */
