#ifndef OPTIONS_H
#define OPTIONS_H

#include <stddef.h>

typedef struct {
    int all;                /* -a: include entries beginning with '.' */
    int almost_all;         /* -A: list all except '.' and '..' */
    int time_ctime;         /* -c: status change time */
    int dir_as_file;        /* -d: treat directories as plain files */
    int classify;           /* -F: append type indicators */
    int unsorted;           /* -f: output not sorted, enables -a */
    int human_readable;     /* -h: human readable sizes */
    int inode;              /* -i: print inode number */
    int kilobytes;          /* -k: sizes in kilobytes (1024 bytes) */
    int long_format;        /* -l: long listing format */
    int numeric_ids;        /* -n: numeric owner and group IDs */
    int non_printable_q;    /* -q: replace non-printable chars with '?' */
    int recursive;          /* -R: recursively list subdirectories */
    int reverse_sort;       /* -r: reverse sort order */
    int sort_by_size;       /* -S: sort by size (largest first) */
    int show_blocks;        /* -s: display file system block count */
    int sort_by_time;       /* -t: sort by time */
    int time_atime;         /* -u: access time */
    int raw_chars;          /* -w: print raw non-printable chars */

    size_t blocksize_unit;  /* Block size unit (default 512 or BLOCKSIZE env) */
} Options;

/*
 * Initializes default options based on environment and terminal status.
 */
void options_init(Options *opts);

/*
 * Parses command-line arguments and applies NetBSD override rules.
 * Returns the index in argv of the first operand, or -1 on error.
 */
int options_parse(Options *opts, int argc, char *argv[]);

#endif /* OPTIONS_H */
