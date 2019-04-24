# whiterose
###### Linux UniKernel

### Clone
```
git clone https://github.com/Ne02ptzero/whiterose.git
```

### Configuration
One must toggle the UKL mode with the `UKL_LINUX` configuration token. You can
activate it with `make defconfig` or by hand with `make menuconfig`

### Compilation
Compilation of the UKL program is pretty simple:
```
export ukl-obj-m := main.o

all:
	make -C ../whiterose/ UKL=$(PWD) bzImage
```
This is a basic Makefile that adds the `main.c` file in the kernel. Here,
`whiterose` is my kernel tree.

### Running
Once the kernel is compiled, one can launch it via usual tools. Here's my
command line:
```
qemu-system-x86_64 -enable-kvm -m 1G -s -kernel ../whiterose/arch/x86/boot/bzImage -append "console=ttyS0 root=/dev/sda ukl quiet" -nographic -hda qemu-image.img
```

The important bit is passing `ukl` to the kernel command line, otherwise the UKL
will not be runned.

### Example program

You can find some working examples here: https://github.com/jzck/unikernel-demo

```
#include <unistd.h>
#include <fcntl.h>

#ifndef USERSPACE
# define main ukl_main
#endif

int main(void) {
    char        buf[256] = { 0 };
    int         fd = open("/etc/passwd", O_RDONLY);
    ssize_t     ret;

    if (fd == -1)
        return 1;

    while ((ret = read(fd, buf, sizeof(buf))))
    {
        buf[ret] = 0;
        write(1, buf, ret);
    }

    close(fd);

    return 0;
}
```

One can either compile this program in the UKL or in userspace with:
```
gcc main.c -DUSERSPACE -o out
```
