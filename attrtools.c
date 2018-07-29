#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <errno.h>

#include "libe2.h"

// Chattr/Lsattr util by duangsuse
// Utility to (bulk) Lsattr/Chattr file(s)
// Created at Jul, 2018
// Language C99
// License: CC-0 (Public Domain)

#define ATTR_QUERY 0
#define ATTR_PIN 1
#define ATTR_UNPIN 2

#ifndef DEBUG
#define DEBUG 0
#endif

#define ATTR_I EXT2_IMMUTABLE_FL
#define ATTR_A EXT2_APPEND_FL

// F**king not found in my Fedora
int lstat(const char *pathname, struct stat *statbuf);

void print_usage(char *program) {
    printf("Attrtools Usage:\n  %s +i <path> ; Set Immutable\n  %s -i <path> ; Unset Immutable\n  %s <path> ; Is Immutable?\n", program, program, program);
}

// Main util function
// Usage:
// atttrtools [+/-] i filepath
// attrtools filepath
int main(int argc, char **argv) {
    char type = ATTR_QUERY;
    // attrtools +i (or -i) [path] argc = 3
    // attrtools [path] argc = 2
    char *program = argv[0];
    char *path = NULL;

    // detect mode
    if (argc == 3) {
        char *first_arg = argv[1];
        if (strlen(first_arg) != 2) {
#if __WORDSIZE == 64
            fprintf(stderr, "Bad mode length: %lu\n", strlen(first_arg));
#else
            fprintf(stderr, "Bad mode length: %u\n", strlen(first_arg));
#endif
            return 1;
        } else {
            // is unpin file
            if (first_arg[0] == '+')
                type = ATTR_PIN;
            else if (first_arg[0] == '-')
                type = ATTR_UNPIN;
            else {
                fprintf(stderr, "Bad mode: %s\n", first_arg);
                print_usage(program);
                return 1;
            }
            path = argv[2];
        }
    } else if (argc == 2) {
        type = ATTR_QUERY;
        path = argv[1];
    } else {
        fprintf(stderr, "Bad argument length: %i\n", argc);
        print_usage(program);
        return 1;
    }

    if (DEBUG) {
        printf("Type is %i, path is %s\n", type, path);
    }

    // logic
    struct stat st;
    if (lstat(path, &st) != 0) {
        fprintf(stderr, "Cannot stat: %s: %s\n", path, strerror(errno));
        return 1;
    }

    // print detailed info
    printf("File: %s\n", path);

    if (S_ISDIR(st.st_mode)) {
        printf("Is a Dir\n");
    } else if (S_ISLNK(st.st_mode)) {
        printf("Is a Link\n");
    }

    if (!S_ISREG(st.st_mode)) {
        printf("Warn: Is NOT a regular file\n");
    }

    // do chattr/lsattr
    fsflags_t ff;
    fsflags_t new_ff;

    // get file attributes
    if (fgetflags(path, &ff) != 0) {
        fprintf(stderr, "Failed to fgetflags: %s\n", strerror(errno));
        return 2;
    }

    switch (type) {
    case ATTR_QUERY:
        fprintf(stderr, "%lu\n", ff);
        if (ff & ATTR_I)
            fprintf(stderr, "+i\n");
        if (ff & ATTR_A)
            fprintf(stderr, "+a\n");
        break;
    case ATTR_PIN:
        // chattr +i
        new_ff = ff | ATTR_I;
        if (fsetflags(path, new_ff) != 0) {
            fprintf(stderr, "Failed to chattr +i: %s\n", strerror(errno));
        }
        break;
    case ATTR_UNPIN:
        // chattr -i
        new_ff = ff & ~ATTR_I;
        if (fsetflags(path, new_ff) != 0) {
            fprintf(stderr, "Failed to chattr -i: %s\n", strerror(errno));
        }
        break;
    }

    return 0;
}

