#ifndef DISPLAY_H
#define DISPLAY_H

#include <stddef.h>
#include "file_entry.h"
#include "options.h"

typedef struct {
    int inode_width;
    int blocks_width;
    int nlink_width;
    int owner_width;
    int group_width;
    int size_width;
    int major_width;
    int minor_width;
    int has_device_files;
} ColumnWidths;

/*
 * Displays an array of FileEntry structures.
 * is_directory_listing: 1 if listing directory contents (prints total line when -l or -s),
 *                       0 if listing command-line file arguments.
 */
void display_entries(FileEntry **entries, size_t count, const Options *opts, int is_directory_listing);

#endif /* DISPLAY_H */
