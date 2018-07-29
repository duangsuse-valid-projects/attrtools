// A simple library for setting/unsetting immutable flag
// Author duangsuse
// License CC-0

#include "libe2.h"
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#if INT_MAX == LONG_MAX
#define E2_IS_LONG
#endif

static void close_noerror(int fd);

/* Get or set file flags on an ext2 file system */
int fgetsetflags(const char *path, unsigned long *get_flags, unsigned long set_flags) {
    struct stat st; // file stat
    int fd, r; // file descriptor, result

#ifdef E2_IS_LONG
    int f;
#endif

    // must be regular file or dir
    if (stat(path, &st) == 0 && !S_ISREG(st.st_mode) && !S_ISDIR(st.st_mode)) {
        errno = EOPNOTSUPP;
        return -1;
    }

    fd = open(path, O_RDONLY | O_NONBLOCK); /* no need to read or write */

    // return error if open fails
    if (fd == -1)
        return -1;

    // not getting file flags
    if (!get_flags) {
#ifdef E2_IS_LONG
        f = (int) set_flags;
        r = ioctl(fd, EXT2_IOC_SETFLAGS, &f);
#else
        r = ioctl(fd, EXT2_IOC_SETFLAGS, (void*) &set_flags);
#endif
    } else {
#ifdef E2_IS_LONG
        r = ioctl(fd, EXT2_IOC_GETFLAGS, &f);
        *get_flags = f;
#else
        r = ioctl(fd, EXT2_IOC_GETFLAGS, (void*) get_flags);
#endif
    }

    close_noerror(fd);
    return r;
}

// close file descriptor, ignore errors
static void close_noerror(int fd) {
    int saved_errno = errno;
    close(fd);
    errno = saved_errno;
}
