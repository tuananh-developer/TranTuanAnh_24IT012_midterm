#ifndef SORT_H
#define SORT_H

#include <stddef.h>
#include "file_entry.h"
#include "options.h"

/*
 * Sorts an array of FileEntry pointers in-place according to active options:
 * - Default: lexicographical order.
 * - -t: time order (most recently modified/accessed/status changed first).
 * - -S: size order (largest first).
 * - -r: reverses the sort order.
 * - -f: leaves unsorted (no-op).
 */
void sort_file_entries(FileEntry **entries, size_t count, const Options *opts);

#endif /* SORT_H */
