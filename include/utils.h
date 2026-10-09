#ifndef UTILS_H
#define UTILS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include "file_entry.h"
#include "options.h"

/*
 * Formats file mode into a standard 10-character string (e.g. "drwxr-xr-x").
 * Buffer must be at least 11 bytes long.
 */
void format_mode(mode_t mode, char *buf);

/*
 * Formats size in human-readable notation (e.g. "1.2K", "500B", "14M").
 */
void format_human_size(off_t size, char *buf, size_t buflen);

/*
 * Calculates the number of blocks according to BLOCKSIZE / -k / -h options.
 */
int64_t calculate_blocks(int64_t blocks_512, const Options *opts);

/*
 * Formats block count into string buffer.
 */
void format_blocks(int64_t blocks_512, const Options *opts, char *buf, size_t buflen);

/*
 * Formats timestamp for -l display according to NetBSD standard:
 * "Abbreviated month, day-of-month, hour and minute" (e.g. "Oct 11 08:30").
 */
void format_time(time_t t, char *buf, size_t buflen);

/*
 * Returns the indicator character for -F flag ('/', '*', '@', '=', '|', or '\0').
 */
char get_classify_indicator(const FileEntry *entry);

/*
 * Prints a filename, converting non-printable characters to '?' if -q is active.
 */
void print_filename(const char *name, const Options *opts);

/*
 * Resolves UID to user name, returning numeric string if not found or if numeric_ids is set.
 */
void get_owner_string(uid_t uid, const Options *opts, char *buf, size_t buflen);

/*
 * Resolves GID to group name, returning numeric string if not found or if numeric_ids is set.
 */
void get_group_string(gid_t gid, const Options *opts, char *buf, size_t buflen);

#endif /* UTILS_H */
