#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/sysmacros.h>
#include "display.h"
#include "utils.h"

static void calculate_column_widths(FileEntry **entries, size_t count, const Options *opts,
                                    ColumnWidths *widths, int64_t *total_blocks_512) {
    memset(widths, 0, sizeof(ColumnWidths));
    *total_blocks_512 = 0;

    for (size_t i = 0; i < count; i++) {
        FileEntry *e = entries[i];
        if (!e || e->stat_error != 0) continue;

        *total_blocks_512 += e->st.st_blocks;

        /* Inode width */
        if (opts->inode) {
            char buf[32];
            int len = snprintf(buf, sizeof(buf), "%llu", (unsigned long long)e->st.st_ino);
            if (len > widths->inode_width) widths->inode_width = len;
        }

        /* Block count width */
        if (opts->show_blocks) {
            char buf[32];
            format_blocks(e->st.st_blocks, opts, buf, sizeof(buf));
            int len = (int)strlen(buf);
            if (len > widths->blocks_width) widths->blocks_width = len;
        }

        /* Long format fields */
        if (opts->long_format) {
            /* Links width */
            char buf[32];
            int len = snprintf(buf, sizeof(buf), "%lu", (unsigned long)e->st.st_nlink);
            if (len > widths->nlink_width) widths->nlink_width = len;

            /* Owner width */
            char owner_buf[64];
            get_owner_string(e->st.st_uid, opts, owner_buf, sizeof(owner_buf));
            len = (int)strlen(owner_buf);
            if (len > widths->owner_width) widths->owner_width = len;

            /* Group width */
            char group_buf[64];
            get_group_string(e->st.st_gid, opts, group_buf, sizeof(group_buf));
            len = (int)strlen(group_buf);
            if (len > widths->group_width) widths->group_width = len;

            /* Size or major/minor numbers */
            if (S_ISCHR(e->st.st_mode) || S_ISBLK(e->st.st_mode)) {
                widths->has_device_files = 1;
                len = snprintf(buf, sizeof(buf), "%u", major(e->st.st_rdev));
                if (len > widths->major_width) widths->major_width = len;
                len = snprintf(buf, sizeof(buf), "%u", minor(e->st.st_rdev));
                if (len > widths->minor_width) widths->minor_width = len;
            } else {
                if (opts->human_readable) {
                    format_human_size(e->st.st_size, buf, sizeof(buf));
                    len = (int)strlen(buf);
                } else {
                    len = snprintf(buf, sizeof(buf), "%lld", (long long)e->st.st_size);
                }
                if (len > widths->size_width) widths->size_width = len;
            }
        }
    }

    if (widths->has_device_files) {
        int dev_len = widths->major_width + widths->minor_width + 2; /* "maj, min" */
        if (dev_len > widths->size_width) {
            widths->size_width = dev_len;
        }
    }
}

void display_entries(FileEntry **entries, size_t count, const Options *opts, int is_directory_listing) {
    if (!entries) return;

    ColumnWidths widths;
    int64_t total_blocks_512 = 0;
    calculate_column_widths(entries, count, opts, &widths, &total_blocks_512);

    /* Print total blocks line if applicable */
    if (is_directory_listing && (opts->long_format || opts->show_blocks)) {
        char total_buf[64];
        format_blocks(total_blocks_512, opts, total_buf, sizeof(total_buf));
        printf("total %s\n", total_buf);
    } else if (!is_directory_listing && opts->show_blocks && isatty(STDOUT_FILENO)) {
        char total_buf[64];
        format_blocks(total_blocks_512, opts, total_buf, sizeof(total_buf));
        printf("total %s\n", total_buf);
    }

    for (size_t i = 0; i < count; i++) {
        FileEntry *e = entries[i];
        if (!e) continue;

        if (e->stat_error != 0) {
            /* If stat failed, print name and proceed */
            print_filename(e->name, opts);
            putchar('\n');
            continue;
        }

        /* Inode (-i) */
        if (opts->inode) {
            printf("%*llu ", widths.inode_width, (unsigned long long)e->st.st_ino);
        }

        /* Blocks (-s) */
        if (opts->show_blocks) {
            char block_buf[32];
            format_blocks(e->st.st_blocks, opts, block_buf, sizeof(block_buf));
            printf("%*s ", widths.blocks_width, block_buf);
        }

        /* Long listing format (-l, -n) */
        if (opts->long_format) {
            char mode_buf[16];
            format_mode(e->st.st_mode, mode_buf);
            printf("%s ", mode_buf);

            printf("%*lu ", widths.nlink_width, (unsigned long)e->st.st_nlink);

            char owner_buf[64];
            get_owner_string(e->st.st_uid, opts, owner_buf, sizeof(owner_buf));
            printf("%-*s ", widths.owner_width, owner_buf);

            char group_buf[64];
            get_group_string(e->st.st_gid, opts, group_buf, sizeof(group_buf));
            printf("%-*s ", widths.group_width, group_buf);

            if (S_ISCHR(e->st.st_mode) || S_ISBLK(e->st.st_mode)) {
                printf("%*u, %*u ", widths.major_width, major(e->st.st_rdev),
                                    widths.minor_width, minor(e->st.st_rdev));
            } else {
                char size_buf[32];
                if (opts->human_readable) {
                    format_human_size(e->st.st_size, size_buf, sizeof(size_buf));
                } else {
                    snprintf(size_buf, sizeof(size_buf), "%lld", (long long)e->st.st_size);
                }
                printf("%*s ", widths.size_width, size_buf);
            }

            char time_buf[32];
            time_t file_time = file_entry_get_time(e, opts);
            format_time(file_time, time_buf, sizeof(time_buf));
            printf("%s ", time_buf);
        }

        /* File name */
        print_filename(e->name, opts);

        /* Classified indicator (-F) */
        if (opts->classify) {
            char ind = get_classify_indicator(e);
            if (ind != '\0') {
                putchar(ind);
            }
        }

        /* Symbolic link destination */
        if (opts->long_format && e->symlink_target) {
            printf(" -> %s", e->symlink_target);
        }

        putchar('\n');
    }
}
