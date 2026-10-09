#ifndef FILE_ENTRY_H
#define FILE_ENTRY_H

#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include "options.h"

typedef struct {
    char *name;             /* File name (e.g., "stdio.h") */
    char *path;             /* Full path (e.g., "/usr/include/stdio.h") */
    struct stat st;         /* File status information from lstat/stat */
    char *symlink_target;   /* Path of linked-to file if symbolic link */
    int stat_error;         /* 0 if successful, errno if failed */
    int is_dir;             /* 1 if directory, 0 otherwise */
} FileEntry;

/*
 * Creates a new FileEntry for a file given its directory and name.
 * If dirpath is NULL or empty, name is treated as the full path.
 * follow_symlink: if non-zero, uses stat(); if 0, uses lstat().
 */
FileEntry *file_entry_create(const char *dirpath, const char *name, int follow_symlink);

/*
 * Frees memory allocated for a FileEntry.
 */
void file_entry_free(FileEntry *entry);

/*
 * Frees an array of FileEntry pointers.
 */
void file_entry_array_free(FileEntry **entries, size_t count);

/*
 * Retrieves the effective timestamp of a file based on options (-c, -u, or mtime).
 */
time_t file_entry_get_time(const FileEntry *entry, const Options *opts);

#endif /* FILE_ENTRY_H */
