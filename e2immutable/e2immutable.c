/************************************************************************
  e2immutable is a tiny 'exported' command line 'library' for reading 'Immutable/Append' and changing 'Immutable' attribute
  Copyright (C) 2018 duangsuse

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
************************************************************************/


#include <unistd.h>
#include <stdlib.h>

#include <errno.h>

#include <stdio.h>
#include <fcntl.h>

#include <sys/ioctl.h>
#include <sys/stat.h>

#define ATTR_I 0x00000010 // Ext2fs "Immutable" file attribute
#define ATTR_A 0x00000020 // Ext2fs "Only Append" file attribute

#define EXT2_IOC_GETFLAGS _IOR('f', 1, long)
#define EXT2_IOC_SETFLAGS _IOW('f', 2, long)

#if __WORDSIZE == 64
#define E2IMM_IS_64BIT
#endif

#define addattr(fattr, flag) (fattr) = (fattr) | (flag)
#define delattr(fattr, flag) (fattr) = (fattr) & ~(flag)

typedef unsigned long fsattrs_t;

// close file descriptor, ignore errors
static inline void close_silently(int fd) {
  int saved_errno = errno;
  close(fd); // Operate
  errno = saved_errno;
}

// Open a file descriptor for reading or changing file attribute
// return -1 if open fails
// return -2 if stat fails
// return -3 if file not acceptable
int open_attrsctl_fd(const char *path) {
  struct stat fstate;

  if (stat(path, &fstate) != 0)
    return -2; // stat fails
  if (!S_ISREG(fstate.st_mode) && !S_ISDIR(fstate.st_mode))
    return -3; // not acceptable

  return open(path, O_RDONLY | O_NONBLOCK);
}

// Get file attribute
// return -2 if stat fails
// return -1 if failed
int fgetattr(const char *path, fsattrs_t *buffer) {
  int fd, ret;
#ifdef E2IMM_IS_64BIT
  int flags; // 64bit
#endif

  fd = open_attrsctl_fd(path);
  if (fd == -2)
    return -2;
  if (fd == -1)
    return -1;

  if (fd == -3) {
    errno = EOPNOTSUPP;
    return -1;
  }

#ifdef E2IMM_IS_64BIT
  ret = ioctl(fd, EXT2_IOC_GETFLAGS, &flags);
  *buffer = (unsigned long) flags;
#else
  ret = ioctl(fd, EXT2_IOC_GETFLAGS, (void *) buffer);
#endif

  close_silently(fd);
  return ret;
}

// Set file attribute
// return -2 if stat fails
// return -1 if failed
int fsetattr(const char *path, fsattrs_t *buffer) {
  int fd, ret;
#ifdef E2IMM_IS_64BIT
  int flags; // 64bit
#endif

  fd = open_attrsctl_fd(path);
  if (fd == -2)
    return -2;
  if (fd == -1)
    return -1;

  if (fd == -3) {
    errno = EOPNOTSUPP;
    return -1;
  }

#ifdef E2IMM_IS_64BIT
  flags = (int)*buffer;
  ret = ioctl(fd, EXT2_IOC_SETFLAGS, &flags);
#else
  ret = ioctl(fd, EXT2_IOC_SETFLAGS, (void *) buffer);
#endif

  close_silently(fd);
  return ret;
}

// Author: duangsuse
// Utility for changing file "Immutable" attribute and reading file "Append"
// "Immutable" attribute
// Language: C99 / C++ 11
// Created: Jul, 2018 License: GPLv2

// Usage: e2immutable (+/-/@) <file path>
// @: query file attr
//   return 0: no attribute
//   return 255: stat/cmdline fails
//   return 254: fgetfattr fails
//   return 1: +i
//   return 2: +a
//   return 3: +i +a
// +: add immutable attribute
//   return 0: OK, changed
//   return 1: OK, unchanged
//   return 255: stat/cmdline fails
//   return 254: fsetfattr fails
// -: remove immutable attribute
//   return 0: OK, changed
//   return 1: OK, unchanged
//   return 255: stat/cmdline fails
//   return 254: fsetfattr fails
int main(int argc, char *argv[]) {
  if (argc != 3)
    exit(-1);

  // is argv[1] single character?
  if (argv[1][1] != '\00')
    exit(-1);

  char *path = argv[2];

  int ret = 0;
  fsattrs_t fs;

  switch (fgetattr(path, &fs)) {
    case -1:
      perror("");
      return -2;
    case -2:
      return -1;
    default:
      break;
  }

  switch (argv[1][0]) {
    case '@':
      if (fs & ATTR_A)
        ret += 2;
      if (fs & ATTR_I)
        ret += 1;
      return ret;
    case '+':
      if (fs & ATTR_I)
        return 1;
      addattr(fs, ATTR_I);
      break;
    case '-':
      if (!(fs & ATTR_I))
        return 1;
      delattr(fs, ATTR_I);
      break;
    default:
      return -1;
  }

  switch (fsetattr(path, &fs)) {
    case -1:
      perror("");
      return -2;
    case -2:
      return -1;
    default:
      break;
  }

  return 0;
}
