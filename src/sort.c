#include <stdlib.h>
#include <string.h>
#include "sort.h"

static const Options *s_opts = NULL;

static int get_entry_timespec(const FileEntry *entry, time_t *sec, long *nsec) {
    if (!entry || entry->stat_error != 0) {
        *sec = 0;
        *nsec = 0;
        return -1;
    }

#if defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200809L
    if (s_opts && s_opts->time_atime) {
        *sec = entry->st.st_atim.tv_sec;
        *nsec = entry->st.st_atim.tv_nsec;
    } else if (s_opts && s_opts->time_ctime) {
        *sec = entry->st.st_ctim.tv_sec;
        *nsec = entry->st.st_ctim.tv_nsec;
    } else {
        *sec = entry->st.st_mtim.tv_sec;
        *nsec = entry->st.st_mtim.tv_nsec;
    }
#else
    if (s_opts && s_opts->time_atime) {
        *sec = entry->st.st_atime;
        *nsec = 0;
    } else if (s_opts && s_opts->time_ctime) {
        *sec = entry->st.st_ctime;
        *nsec = 0;
    } else {
        *sec = entry->st.st_mtime;
        *nsec = 0;
    }
#endif
    return 0;
}

static int compare_entries(const void *p1, const void *p2) {
    const FileEntry *a = *(const FileEntry **)p1;
    const FileEntry *b = *(const FileEntry **)p2;

    int cmp = 0;

    /* Sort by size (-S) */
    if (s_opts && s_opts->sort_by_size) {
        if (a->st.st_size > b->st.st_size) {
            cmp = -1; /* Largest first */
        } else if (a->st.st_size < b->st.st_size) {
            cmp = 1;
        } else {
            cmp = strcmp(a->name, b->name);
        }
    }
    /* Sort by time (-t) */
    else if (s_opts && s_opts->sort_by_time) {
        time_t sec_a = 0, sec_b = 0;
        long nsec_a = 0, nsec_b = 0;
        get_entry_timespec(a, &sec_a, &nsec_a);
        get_entry_timespec(b, &sec_b, &nsec_b);

        if (sec_a > sec_b) {
            cmp = -1; /* Most recent first */
        } else if (sec_a < sec_b) {
            cmp = 1;
        } else if (nsec_a > nsec_b) {
            cmp = -1;
        } else if (nsec_a < nsec_b) {
            cmp = 1;
        } else {
            cmp = strcmp(a->name, b->name);
        }
    }
    /* Default: lexicographical order */
    else {
        cmp = strcmp(a->name, b->name);
    }

    /* Reverse sort (-r) */
    if (s_opts && s_opts->reverse_sort) {
        cmp = -cmp;
    }

    return cmp;
}

void sort_file_entries(FileEntry **entries, size_t count, const Options *opts) {
    if (!entries || count <= 1 || !opts) return;

    /* If unsorted (-f), do nothing */
    if (opts->unsorted) return;

    s_opts = opts;
    qsort(entries, count, sizeof(FileEntry *), compare_entries);
    s_opts = NULL;
}
