#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "options.h"

void options_init(Options *opts) {
    if (!opts) return;

    memset(opts, 0, sizeof(Options));

    /* Super-user (UID == 0) has -A set by default */
    if (getuid() == 0) {
        opts->almost_all = 1;
    }

    /*
     * Non-printable character handling defaults:
     * -q is default when output is to a terminal.
     * -w is default when output is not to a terminal.
     */
    if (isatty(STDOUT_FILENO)) {
        opts->non_printable_q = 1;
        opts->raw_chars = 0;
    } else {
        opts->non_printable_q = 0;
        opts->raw_chars = 1;
    }

    /* Default block size unit is 512 bytes */
    opts->blocksize_unit = 512;

    /* Check BLOCKSIZE environment variable */
    const char *bsize_env = getenv("BLOCKSIZE");
    if (bsize_env && *bsize_env) {
        char *endptr = NULL;
        long val = strtol(bsize_env, &endptr, 10);
        if (val > 0) {
            if (*endptr == 'k' || *endptr == 'K') {
                val *= 1024;
            } else if (*endptr == 'm' || *endptr == 'M') {
                val *= 1024 * 1024;
            } else if (*endptr == 'g' || *endptr == 'G') {
                val *= 1024 * 1024 * 1024;
            }
            if (val >= 512) {
                opts->blocksize_unit = (size_t)val;
            }
        }
    }
}

int options_parse(Options *opts, int argc, char *argv[]) {
    if (!opts) return -1;

    int ch;
    /* Reset getopt internal index */
    optind = 1;

    while ((ch = getopt(argc, argv, "AacdFfhiklnqRrSstuw")) != -1) {
        switch (ch) {
            case 'A':
                opts->almost_all = 1;
                break;
            case 'a':
                opts->all = 1;
                break;
            case 'c':
                opts->time_ctime = 1;
                opts->time_atime = 0;
                break;
            case 'd':
                opts->dir_as_file = 1;
                opts->recursive = 0;
                break;
            case 'F':
                opts->classify = 1;
                break;
            case 'f':
                opts->unsorted = 1;
                opts->all = 1;
                break;
            case 'h':
                opts->human_readable = 1;
                opts->kilobytes = 0;
                break;
            case 'i':
                opts->inode = 1;
                break;
            case 'k':
                opts->kilobytes = 1;
                opts->human_readable = 0;
                break;
            case 'l':
                opts->long_format = 1;
                opts->numeric_ids = 0;
                break;
            case 'n':
                opts->long_format = 1;
                opts->numeric_ids = 1;
                break;
            case 'q':
                opts->non_printable_q = 1;
                opts->raw_chars = 0;
                break;
            case 'R':
                opts->recursive = 1;
                opts->dir_as_file = 0;
                break;
            case 'r':
                opts->reverse_sort = 1;
                break;
            case 'S':
                opts->sort_by_size = 1;
                opts->sort_by_time = 0;
                break;
            case 's':
                opts->show_blocks = 1;
                break;
            case 't':
                opts->sort_by_time = 1;
                opts->sort_by_size = 0;
                break;
            case 'u':
                opts->time_atime = 1;
                opts->time_ctime = 0;
                break;
            case 'w':
                opts->raw_chars = 1;
                opts->non_printable_q = 0;
                break;
            default:
                fprintf(stderr, "usage: ls [-AacdFfhiklnqRrSstuw] [file ...]\n");
                return -1;
        }
    }

    /* Adjust blocksize_unit if -k is active and not overridden by -h */
    if (opts->kilobytes && !opts->human_readable) {
        opts->blocksize_unit = 1024;
    }

    return optind;
}
