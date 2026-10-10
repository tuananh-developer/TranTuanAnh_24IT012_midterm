#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include "traverse.h"
#include "file_entry.h"
#include "sort.h"
#include "display.h"

static int g_first_output_block = 1;

static int list_directory(const char *dirpath, const Options *opts, int print_header) {
    DIR *dir = opendir(dirpath);
    if (!dir) {
        fprintf(stderr, "ls: %s: %s\n", dirpath, strerror(errno));
        return 1;
    }

    if (print_header) {
        if (!g_first_output_block) {
            putchar('\n');
        }
        printf("%s:\n", dirpath);
    }
    g_first_output_block = 0;

    size_t capacity = 64;
    size_t count = 0;
    FileEntry **entries = malloc(capacity * sizeof(FileEntry *));
    if (!entries) {
        closedir(dir);
        return 1;
    }

    struct dirent *dp;
    while ((dp = readdir(dir)) != NULL) {
        /* Filter entries according to -a and -A */
        if (!opts->all) {
            if (opts->almost_all) {
                if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
                    continue;
                }
            } else {
                if (dp->d_name[0] == '.') {
                    continue;
                }
            }
        }

        /* Within directories, always use lstat (follow_symlink = 0) */
        FileEntry *entry = file_entry_create(dirpath, dp->d_name, 0);
        if (!entry) continue;

        if (count >= capacity) {
            capacity *= 2;
            FileEntry **new_entries = realloc(entries, capacity * sizeof(FileEntry *));
            if (!new_entries) {
                file_entry_free(entry);
                break;
            }
            entries = new_entries;
        }
        entries[count++] = entry;
    }

    closedir(dir);

    /* Sort entries */
    sort_file_entries(entries, count, opts);

    /* Display entries */
    display_entries(entries, count, opts, 1);

    int status = 0;

    /* Handle recursive traversal (-R) */
    if (opts->recursive) {
        for (size_t i = 0; i < count; i++) {
            FileEntry *e = entries[i];
            if (e && e->stat_error == 0 && S_ISDIR(e->st.st_mode)) {
                /* Never recurse into "." or ".." */
                if (strcmp(e->name, ".") == 0 || strcmp(e->name, "..") == 0) {
                    continue;
                }
                int sub_res = list_directory(e->path, opts, 1);
                if (sub_res != 0) {
                    status = sub_res;
                }
            }
        }
    }

    file_entry_array_free(entries, count);
    return status;
}

int traverse_operands(int operand_count, char *operands[], const Options *opts) {
    g_first_output_block = 1;
    int exit_status = 0;

    /* If no operands given, list current directory "." */
    if (operand_count == 0) {
        if (opts->dir_as_file) {
            /* With -d, list "." as a file */
            FileEntry *entry = file_entry_create(NULL, ".", 0);
            if (entry) {
                FileEntry *arr[1] = { entry };
                display_entries(arr, 1, opts, 0);
                file_entry_free(entry);
            }
            return 0;
        }
        return list_directory(".", opts, opts->recursive ? 1 : 0);
    }

    /* Allocate collections for files and directories */
    size_t file_cap = (size_t)operand_count;
    size_t file_count = 0;
    FileEntry **files = malloc(file_cap * sizeof(FileEntry *));

    size_t dir_cap = (size_t)operand_count;
    size_t dir_count = 0;
    FileEntry **dirs = malloc(dir_cap * sizeof(FileEntry *));

    if (!files || !dirs) {
        free(files);
        free(dirs);
        return 1;
    }

    /*
     * NetBSD specification:
     * -d: Directories are listed as plain files, and symbolic links
     *     in the argument list are not indirected through.
     * When -d is NOT specified, symlinks in argument list are followed
     * to determine if target is a directory.
     */
    int follow_cli_symlinks = (!opts->dir_as_file);

    for (int i = 0; i < operand_count; i++) {
        const char *arg = operands[i];
        FileEntry *entry = file_entry_create(NULL, arg, follow_cli_symlinks);
        if (!entry) {
            exit_status = 1;
            continue;
        }

        if (entry->stat_error != 0) {
            fprintf(stderr, "ls: %s: %s\n", arg, strerror(entry->stat_error));
            file_entry_free(entry);
            exit_status = 1;
            continue;
        }

        if (opts->dir_as_file || !entry->is_dir) {
            files[file_count++] = entry;
        } else {
            dirs[dir_count++] = entry;
        }
    }

    /* Sort non-directory operands */
    if (file_count > 0) {
        sort_file_entries(files, file_count, opts);
        display_entries(files, file_count, opts, 0);
        g_first_output_block = 0;
    }

    /* Sort directory operands */
    if (dir_count > 0) {
        sort_file_entries(dirs, dir_count, opts);

        /* Print headers if more than one operand or recursive */
        int need_headers = (operand_count > 1 || opts->recursive);

        for (size_t i = 0; i < dir_count; i++) {
            FileEntry *d = dirs[i];
            int res = list_directory(d->path, opts, need_headers);
            if (res != 0) {
                exit_status = res;
            }
        }
    }

    file_entry_array_free(files, file_count);
    file_entry_array_free(dirs, dir_count);

    return exit_status;
}
