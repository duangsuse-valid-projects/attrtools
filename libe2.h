// A simple library for setting/unsetting immutable flag
// Author duangsuse
// License CC-0

#pragma once

#include <linux/ext2_fs.h>

/* An ext2 filesystem file attribute processing library */

/* File system flags type */
typedef unsigned long fsflags_t;

/* Get or set a file flags on an ext2 file system
 *
 * @param path File path to operate
 * @param get_flags flags buffer to write to
 * @param set_flags flags buffer to set to
 * @return -1 if failed, or ioctl() result
 */
int fgetsetflags(const char *path, fsflags_t *get_flags, fsflags_t set_flags);

/* Get a file flags on an ext2 file system
 *
 * @param path File path to operate
 * @param flags flags buffer to write to
 * @return -1 if failed, or ioctl() result
 */
#define fgetflags(path, flags) fgetsetflags(path, flags, 0)

/* Set a file flags on an ext2 file system
 *
 * @param path File path to operate
 * @param flags flags buffer to set to
 * @return -1 if failed, or ioctl() result
 */
#define fsetflags(path, flags) fgetsetflags(path, NULL, flags)

#define LIBE2_H_FLAGS

#ifndef LIBE2_H_FLAGS
/* File attributes on an ext2 file system */
const uint32_t e2attr_flags_value[] = {
#ifdef ENABLE_COMPRESSION
    EXT2_COMPRBLK_FL,
    EXT2_DIRTY_FL,
    EXT2_NOCOMPR_FL,
    EXT2_ECOMPR_FL,
#endif
    EXT2_INDEX_FL,
    EXT2_SECRM_FL,
    EXT2_UNRM_FL,
    EXT2_SYNC_FL,
    EXT2_DIRSYNC_FL,
    EXT2_IMMUTABLE_FL,
    EXT2_APPEND_FL,
    EXT2_NODUMP_FL,
    EXT2_NOATIME_FL,
    EXT2_COMPR_FL,
    EXT3_JOURNAL_DATA_FL,
    EXT2_NOTAIL_FL,
    EXT2_TOPDIR_FL
};

/* File attribute names on an ext2 file system */
#ifdef ENABLE_COMPRESSION
const char e2attr_flags_name[] = {
    "BZXE" "I" "suSDiadAcjtT"
};
#else
const char e2attr_flags_name[] = {
    "I" "suSDiadAcjtT"
};
#endif
#endif
