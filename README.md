# OS From Scratch

A hobby operating system development project featuring a custom filesystem, 16-bit GUI, and command-line interface implementation.

## Overview

This project is an educational OS development initiative that implements core operating system components from scratch, including:

- **Custom Filesystem**: A complete filesystem implementation with inode-based structure, bitmap allocation, and directory management
- **16-bit GUI**: Real-mode graphics interface with BMP rendering and shape drawing capabilities
- **Disk Management**: Virtual disk abstraction layer supporting multiple drives
- **Standard Library**: Custom standard library with memory management, string operations, and data structures
- **Command Interface**: CLI for interacting with the filesystem and system components

## Project Structure

```
OS_from_scratch/
├── OS_DEV/
│   ├── api/           # Core API and filesystem implementation
│   │   ├── api.c/h    # Main API functions and error handling
│   │   ├── disk.c/h   # Disk I/O and drive management
│   │   ├── fs.c/h     # Filesystem implementation (inodes, directories, paths)
│   │   ├── stdoslib.c/h # Custom standard library
│   │   └── util.c     # Utility functions
│   ├── Gui/           # 16-bit real-mode GUI
│   │   ├── main.c     # GUI entry point
│   │   ├── shapes.c   # Shape drawing functions
│   │   ├── bmp.c      # BMP image parsing and rendering
│   │   ├── omnistd.c  # GUI standard library
│   │   └── xgfx.asm   # Assembly graphics routines
│   ├── cmnd/          # Command-line interface
│   │   ├── cmnd.c/h   # CLI implementation
│   ├── Drive/         # Virtual disk images
│   │   ├── Disk.1     # First virtual drive (32MB)
│   │   └── Disk.2     # Second virtual drive (32MB)
│   └── util/          # Compiled utility binary
└── README.md
```

## Dependencies

### Build Dependencies

- **GCC**: GNU Compiler Collection (with C2x/C23 support)
- **NASM**: Netwide Assembler (for GUI assembly code)
- **LD**: GNU Linker
- **Make**: Build automation tool

### System Dependencies

- Linux operating system
- Standard C library headers
- POSIX system calls (unistd.h, sys/stat.h, fcntl.h)

## Building

### Build All Components

```bash
cd OS_DEV/api
make

cd ../cmnd
make

cd ../Gui
make
```

### Individual Component Builds

**API Library (Shared Object & Utility):**
```bash
cd OS_DEV/api
make clean
make
```
This builds:
- `api.so` - Shared library containing filesystem and disk APIs
- `../util` - Utility binary

**Command-Line Interface:**
```bash
cd OS_DEV/cmnd
make clean
make
```
This builds:
- `cmnd` - Command-line interface executable

**GUI (16-bit Binary):**
```bash
cd OS_DEV/Gui
make clean
make
```
This builds:
- `gui.com` - 16-bit real-mode GUI binary

### Clean Build Artifacts

```bash
# API
cd OS_DEV/api && make clean

# Command
cd OS_DEV/cmnd && make clean

# GUI
cd OS_DEV/Gui && make clean
```

## Usage

### Setting Up Virtual Disks

The project uses virtual disk images located in `OS_DEV/Drive/`. Ensure these files exist:

```bash
# Create 32MB disk images if needed
dd if=/dev/zero of=OS_DEV/Drive/Disk.1 bs=512 count=65536
dd if=/dev/zero of=OS_DEV/Drive/Disk.2 bs=512 count=65536
```

**Note**: The disk path is hardcoded in `disk.h`. Update the `Basepath` macro if your disks are located elsewhere:

```c
#define Basepath "/path/to/your/Drive/Disk."
```

### Running the Command-Line Interface

```bash
cd OS_DEV/cmnd
./cmnd
```

The CLI initializes the filesystem and mounts the virtual disks.

### Running the GUI

The GUI is a 16-bit real-mode binary designed to run in a DOS emulator or similar environment:

```bash
cd OS_DEV/Gui
# Use with DOSBox, QEMU, or similar 16-bit environment
dosbox gui.com
```

### Using the API Library

The `api.so` shared library can be linked against your own programs:

```c
#include "api.h"
#include "os.h"

int main() {
    init();  // Initialize the OS API
    // Use filesystem functions
    return 0;
}
```

Compile with:
```bash
gcc -o myprogram myprogram.c -L./OS_DEV/api -l:api.so -I./OS_DEV/api
```

## Filesystem Architecture

### Design

The filesystem uses an inode-based design with the following characteristics:

- **Block Size**: 512 bytes
- **Inode Size**: 32 bytes
- **Inodes per Block**: 16
- **Direct Pointers per Inode**: 8
- **Indirect Pointer Support**: Single-level indirection
- **Filename Format**: 8.3 (8 characters name, 3 character extension)

### Key Structures

**Superblock**: Contains filesystem metadata including:
- Boot sector (500 bytes)
- Total blocks
- Total inodes
- Inode block count
- Magic string (0xfade, 0xaa55)

**Inode**: Represents files and directories with:
- Valid type (File/Directory/Invalid)
- Size in bytes
- Filename (8.3 format)
- Indirect pointer
- 8 direct block pointers

**Directory**: Special inodes containing:
- Direct pointers to child inodes
- Optional indirect pointer for additional entries

### Filesystem Operations

The API provides the following operations:

- **Mount/Unmount**: `fsmount()`, `fsunmount()`
- **Directory Operations**: `open_dir()`, `listfiles()`, `mkdirectory()`
- **Path Operations**: `init_path()`, `eval_path()`, `path2inode()`
- **Inode Management**: `create_inode()`, `fetchinode()`, `inode_dealloc()`
- **Bitmap Management**: `mkbitmap()`, `allocbitmap()`, `freebitmap()`

## Examples

### Example 1: Initializing the Filesystem

```c
#include "os.h"

int main() {
    init();  // Initialize OS API (mounts disks automatically)
    printf("Filesystem initialized\n");
    return 0;
}
```

### Example 2: Creating a Directory

```c
#include "fs.h"

// Create directory "c:/helog"
ptr dir_idx = mkdirectory((i8*)"c:/helog");
if (dir_idx) {
    printf("Directory created successfully\n");
} else {
    printf("Failed to create directory\n");
}
```

### Example 3: Listing Directory Contents

```c
#include "fs.h"

Dir *dir = open_dir((i8*)"c:/");
if (dir) {
    show_ls(dir->entrys);
    dealloc(dir);
}
```

### Example 4: Filesystem Information

```c
#include "fs.h"

Filesystem *fs = FileDescriptors[0];  // Get mounted filesystem
if (fs) {
    fsshow(fs);  // Print filesystem metadata
    print_inodes(fs);  // List all active inodes
}
```

## Error Handling

The API uses a global error number system with the following error codes:

| Error Code | Description |
|------------|-------------|
| 0 | No Errors |
| 1 | Bad/Invalid FD |
| 2 | I/O Error |
| 3 | Closed FD |
| 4 | Init not called |
| 5 | Buffer Overflow |
| 6 | Disk not mounted |
| 7 | Disk IO Error |
| 8 | Unclosed/Busy File |
| 9 | Memory Error |
| 10 | Invalid Argument(s) |
| 11 | Invalid File/Dir Name |
| 12 | Invalid Inode |
| 13 | Limit Reached |
| 14 | Network Error |
| 15 | Invalid Directory |
| 16 | Mismatched Types |
| 17 | Path Not Found |
| 18 | Unable to Open Dir/File |

Use `get_errno()` to retrieve the last error and `print_err()` to print error details.

## Technical Details

### Memory Management

The project uses custom memory allocation macros:
- `alloc(size)` - Wrapper around malloc
- `dealloc(ptr)` - Wrapper around free
- `zero(ptr, len)` - Zero out memory

### Type System

Custom integer types are used throughout:
- `i8` - unsigned char (8-bit)
- `i16` - unsigned short (16-bit)
- `i32` - unsigned int (32-bit)
- `i64` - unsigned long (64-bit)

### Visibility Attributes

GCC visibility attributes control symbol export:
- `public` - Default visibility (exported)
- `internal` - Hidden visibility (internal)
- `private` - Static (file-local)

## GUI Component

The GUI component runs in 16-bit real mode and provides:

- **Video Modes**: Support for various resolutions (e.g., 640x480 16-color)
- **BMP Rendering**: Parse and display BMP images
- **Shape Drawing**: Draw basic shapes (lines, rectangles, etc.)
- **Custom Heap**: Simple memory allocator for GUI operations
- **Assembly Graphics**: Low-level graphics routines in xgfx.asm

### Building GUI Requirements

The GUI requires:
- 16-bit compiler flags (`-m16`)
- Freestanding environment (`-ffreestanding`)
- Custom linker script (`gui.ld`)
- NASM for assembly files

## Known Limitations

- Disk path is hardcoded in `disk.h`
- GUI requires 16-bit environment (DOS emulator)
- Filesystem currently supports basic operations only
- No network implementation (placeholder)
- Limited error recovery mechanisms
- No multi-threading support

## Future Work

- [ ] Implement file read/write operations
- [ ] Add network stack support
- [ ] Improve error handling and recovery
- [ ] Add more filesystem features (permissions, timestamps)
- [ ] Expand GUI capabilities
- [ ] Add shell commands to CLI
- [ ] Implement process management
- [ ] Add memory protection

## License

This is a hobby/educational project. Use at your own risk.

## Contributing

This is a personal learning project. Suggestions and improvements are welcome through issues and pull requests.

## Author

IhitplayzYT

## Acknowledgments

- Inspired by various OS development tutorials and resources
- Uses concepts from UNIX filesystem design
- Implements standard algorithms for data structures
