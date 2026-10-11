# Midterm Project – Implement ls(1) (NetBSD 10.1 Specification)

- **Student Name:** Trần Tuấn Anh
- **Student ID:** 24IT012
- **Course:** System Programming
- **Project Repository:** [https://github.com/tuananh-developer/TranTuanAnh_24IT012_midterm](https://github.com/tuananh-developer/TranTuanAnh_24IT012_midterm)

---

## 1. Project Overview
This project is an implementation of the UNIX `ls(1)` command from scratch in C, adhering to the **NetBSD 10.1 `ls(1)`** specification. The implementation interacts directly with Linux/UNIX system calls and POSIX APIs to inspect directory entries, read file metadata, and format output according to standard command-line flags.

---

## 2. Implemented Features & Options
The utility supports all 20 options specified in the NetBSD manual page:

| Flag | Description |
|:---:|---|
| `-A` | List all entries except `.` and `..`. Automatically enabled for super-user (root). |
| `-a` | Include directory entries whose names begin with a dot (`.`). |
| `-c` | Use file status change time (`ctime`) instead of last modification time for sorting (`-t`) or printing (`-l`). |
| `-d` | Treat directories as plain files (do not list contents) and do not follow symbolic links in argument list. |
| `-F` | Append file type indicators (`/` for dir, `*` for executable, `@` for symlink, `=` for socket, `\|` for FIFO). |
| `-f` | Disable sorting (read order) and automatically enable `-a`. |
| `-h` | Display file and block sizes in human-readable format (B, K, M, G, T). Overrides `-k`. |
| `-i` | Print file serial number (inode number). |
| `-k` | Report block sizes in kilobytes (1024 bytes). Overridden by `-h`. |
| `-l` | List files in long format (mode, links, owner, group, size/device, date/time, name). |
| `-n` | Long format displaying numeric user and group IDs instead of names. |
| `-q` | Replace non-printable characters in filenames with `?` (default when stdout is a terminal). |
| `-R` | Recursively list all subdirectories encountered. |
| `-r` | Reverse the sort order. |
| `-S` | Sort files by size, largest first. |
| `-s` | Display file system block count used by each file. |
| `-t` | Sort files by modification time (most recent first). |
| `-u` | Use last access time (`atime`) instead of last modification time for sorting (`-t`) or printing (`-l`). |
| `-w` | Force raw printing of non-printable characters (default when stdout is not a terminal). |

### Flag Overrides & Precedence:
- `-w` and `-q` override each other (last flag takes effect).
- `-l` and `-n` override each other (last flag takes effect).
- `-c` and `-u` override each other (last flag takes effect).
- `-R` and `-d` override each other (last flag takes effect).
- The rightmost of `-k` and `-h` overrides the previous flag.
- `-f` overrides `-t`, `-S`, `-r`, and automatically turns on `-a`.

---

## 3. Environment Variables
- `BLOCKSIZE`: Sets block size unit for `-s` and `total` if `-h` and `-k` are not specified (defaults to 512 bytes).
- `TZ`: Configures the timezone used for formatting timestamps.

---

## 4. Project Structure
```
.
├── Makefile                # Build automation
├── README.md               # Project documentation & report
├── .gitignore              # Ignores binaries and *.o object files
├── include/
│   ├── options.h           # Command-line options parsing & state
│   ├── file_entry.h        # File entry metadata structures & helpers
│   ├── sort.h              # Sorting routines (lexicographical, time, size, reverse)
│   ├── display.h           # Output rendering (long format, indicators, columns)
│   ├── traverse.h          # Directory traversal and recursion (-R, -d)
│   └── utils.h             # Utility functions (formatting, permissions, sizes)
└── src/
    ├── main.c              # Application entry point
    ├── options.c           # Parsing & validation of CLI flags
    ├── file_entry.c        # Metadata retrieval via stat/lstat
    ├── sort.c              # Sorting logic implementation
    ├── display.c           # Display formatting & rendering
    ├── traverse.c          # Traversal & recursion implementation
    └── utils.c             # Human-readable sizes, permissions string, etc.
```

---

## 5. Compilation & Usage

### Building the Project
To compile the project with strict compiler flags (`-Wall -Wextra -Werror -pedantic`):
```bash
make
```

To clean up compiled binaries and object files:
```bash
make clean
```

### Running
```bash
./my_ls [options] [file ...]
```

Examples:
```bash
# Basic listing
./my_ls

# Long format with human readable sizes and classified indicators
./my_ls -lhF

# Recursive listing of subdirectories
./my_ls -lR /path/to/dir

# Multiple targets
./my_ls -la file1.txt /var/log /tmp
```

---

## 6. Author
- **Trần Tuấn Anh** - Student ID: `24IT012`
