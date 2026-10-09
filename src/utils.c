#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/sysmacros.h>
#include "utils.h"

void format_mode(mode_t mode, char *buf) {
    if (!buf) return;

    /* 1. File type character */
    if (S_ISDIR(mode)) {
        buf[0] = 'd';
    } else if (S_ISLNK(mode)) {
        buf[0] = 'l';
    } else if (S_ISCHR(mode)) {
        buf[0] = 'c';
    } else if (S_ISBLK(mode)) {
        buf[0] = 'b';
    } else if (S_ISSOCK(mode)) {
        buf[0] = 's';
    } else if (S_ISFIFO(mode)) {
        buf[0] = 'p';
#ifdef S_ISWHT
    } else if (S_ISWHT(mode)) {
        buf[0] = 'w';
#endif
    } else {
        buf[0] = '-';
    }

    /* 2. Owner permissions */
    buf[1] = (mode & S_IRUSR) ? 'r' : '-';
    buf[2] = (mode & S_IWUSR) ? 'w' : '-';
    if (mode & S_ISUID) {
        buf[3] = (mode & S_IXUSR) ? 's' : 'S';
    } else {
        buf[3] = (mode & S_IXUSR) ? 'x' : '-';
    }

    /* 3. Group permissions */
    buf[4] = (mode & S_IRGRP) ? 'r' : '-';
    buf[5] = (mode & S_IWGRP) ? 'w' : '-';
    if (mode & S_ISGID) {
        buf[6] = (mode & S_IXGRP) ? 's' : 'S';
    } else {
        buf[6] = (mode & S_IXGRP) ? 'x' : '-';
    }

    /* 4. Other permissions */
    buf[7] = (mode & S_IROTH) ? 'r' : '-';
    buf[8] = (mode & S_IWOTH) ? 'w' : '-';
    if (mode & S_ISVTX) {
        buf[9] = (mode & S_IXOTH) ? 't' : 'T';
    } else {
        buf[9] = (mode & S_IXOTH) ? 'x' : '-';
    }

    buf[10] = '\0';
}

void format_human_size(off_t size, char *buf, size_t buflen) {
    if (!buf || buflen == 0) return;

    static const char prefixes[] = "BKMGTPE";
    double dsize = (double)size;
    int p_index = 0;

    while (dsize >= 1024.0 && p_index < 6) {
        dsize /= 1024.0;
        p_index++;
    }

    if (p_index == 0) {
        snprintf(buf, buflen, "%lldB", (long long)size);
    } else if (dsize < 10.0) {
        snprintf(buf, buflen, "%.1f%c", dsize, prefixes[p_index]);
    } else {
        snprintf(buf, buflen, "%.0f%c", dsize, prefixes[p_index]);
    }
}

int64_t calculate_blocks(int64_t blocks_512, const Options *opts) {
    if (!opts) return blocks_512;

    size_t unit = opts->blocksize_unit;
    if (unit == 0) unit = 512;

    int64_t bytes = blocks_512 * 512;
    /* Round up partial units to the next integer value */
    int64_t count = (bytes + (int64_t)unit - 1) / (int64_t)unit;
    return count;
}

void format_blocks(int64_t blocks_512, const Options *opts, char *buf, size_t buflen) {
    if (!buf || buflen == 0) return;

    if (opts && opts->human_readable) {
        format_human_size(blocks_512 * 512, buf, buflen);
    } else {
        int64_t count = calculate_blocks(blocks_512, opts);
        snprintf(buf, buflen, "%lld", (long long)count);
    }
}

void format_time(time_t t, char *buf, size_t buflen) {
    if (!buf || buflen == 0) return;

    struct tm tm_buf;
    if (localtime_r(&t, &tm_buf) == NULL) {
        snprintf(buf, buflen, "Unknown");
        return;
    }

    /* Format according to NetBSD spec: month, day, HH:MM */
    strftime(buf, buflen, "%b %e %H:%M", &tm_buf);
}

char get_classify_indicator(const FileEntry *entry) {
    if (!entry || entry->stat_error != 0) return '\0';

    mode_t mode = entry->st.st_mode;

    if (S_ISDIR(mode)) return '/';
    if (S_ISLNK(mode)) return '@';
    if (S_ISSOCK(mode)) return '=';
    if (S_ISFIFO(mode)) return '|';
#ifdef S_ISWHT
    if (S_ISWHT(mode)) return '%';
#endif
    if (S_ISREG(mode) && (mode & (S_IXUSR | S_IXGRP | S_IXOTH))) {
        return '*';
    }

    return '\0';
}

void print_filename(const char *name, const Options *opts) {
    if (!name) return;

    if (opts && opts->non_printable_q) {
        for (const char *p = name; *p; p++) {
            if (isprint((unsigned char)*p)) {
                putchar(*p);
            } else {
                putchar('?');
            }
        }
    } else {
        fputs(name, stdout);
    }
}

void get_owner_string(uid_t uid, const Options *opts, char *buf, size_t buflen) {
    if (!buf || buflen == 0) return;

    if (opts && opts->numeric_ids) {
        snprintf(buf, buflen, "%u", (unsigned int)uid);
        return;
    }

    struct passwd *pw = getpwuid(uid);
    if (pw && pw->pw_name) {
        snprintf(buf, buflen, "%s", pw->pw_name);
    } else {
        snprintf(buf, buflen, "%u", (unsigned int)uid);
    }
}

void get_group_string(gid_t gid, const Options *opts, char *buf, size_t buflen) {
    if (!buf || buflen == 0) return;

    if (opts && opts->numeric_ids) {
        snprintf(buf, buflen, "%u", (unsigned int)gid);
        return;
    }

    struct group *gr = getgrgid(gid);
    if (gr && gr->gr_name) {
        snprintf(buf, buflen, "%s", gr->gr_name);
    } else {
        snprintf(buf, buflen, "%u", (unsigned int)gid);
    }
}
