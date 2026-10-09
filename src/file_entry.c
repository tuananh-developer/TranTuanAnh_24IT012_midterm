#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "file_entry.h"

FileEntry *file_entry_create(const char *dirpath, const char *name, int follow_symlink) {
    if (!name) return NULL;

    FileEntry *entry = calloc(1, sizeof(FileEntry));
    if (!entry) return NULL;

    entry->name = strdup(name);
    if (!entry->name) {
        free(entry);
        return NULL;
    }

    /* Construct full path */
    if (!dirpath || dirpath[0] == '\0') {
        entry->path = strdup(name);
    } else {
        size_t dlen = strlen(dirpath);
        size_t nlen = strlen(name);
        int need_slash = (dirpath[dlen - 1] != '/');
        size_t path_len = dlen + nlen + (need_slash ? 1 : 0) + 1;

        entry->path = malloc(path_len);
        if (entry->path) {
            if (need_slash) {
                snprintf(entry->path, path_len, "%s/%s", dirpath, name);
            } else {
                snprintf(entry->path, path_len, "%s%s", dirpath, name);
            }
        }
    }

    if (!entry->path) {
        free(entry->name);
        free(entry);
        return NULL;
    }

    /* Retrieve stat information */
    int stat_res = -1;
    if (follow_symlink) {
        stat_res = stat(entry->path, &entry->st);
        if (stat_res == -1 && errno == ENOENT) {
            /* If broken symlink, fallback to lstat */
            stat_res = lstat(entry->path, &entry->st);
        }
    } else {
        stat_res = lstat(entry->path, &entry->st);
    }

    if (stat_res == -1) {
        entry->stat_error = errno;
    } else {
        entry->stat_error = 0;
        if (S_ISDIR(entry->st.st_mode)) {
            entry->is_dir = 1;
        }

        /* If entry is a symbolic link, read the destination target */
        if (S_ISLNK(entry->st.st_mode)) {
            char target_buf[1024];
            ssize_t len = readlink(entry->path, target_buf, sizeof(target_buf) - 1);
            if (len != -1) {
                target_buf[len] = '\0';
                entry->symlink_target = strdup(target_buf);
            }
        }
    }

    return entry;
}

void file_entry_free(FileEntry *entry) {
    if (!entry) return;
    free(entry->name);
    free(entry->path);
    free(entry->symlink_target);
    free(entry);
}

void file_entry_array_free(FileEntry **entries, size_t count) {
    if (!entries) return;
    for (size_t i = 0; i < count; i++) {
        file_entry_free(entries[i]);
    }
    free(entries);
}

time_t file_entry_get_time(const FileEntry *entry, const Options *opts) {
    if (!entry || !opts) return 0;
    if (opts->time_atime) {
        return entry->st.st_atime;
    }
    if (opts->time_ctime) {
        return entry->st.st_ctime;
    }
    return entry->st.st_mtime;
}
