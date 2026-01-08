/*  fs.h  */
#pragma once
/*Includes*/
#include "api.h"
#include "disk.h"
#include "stdoslib.h"
/*Includes*/

/* Typedefinations */
typedef unsigned char i8;
typedef unsigned short i16;
typedef unsigned int i32;
typedef unsigned long i64;
/* Typedefinations */

/* MACROS */
#define public __attribute__((visibility("default")))
#define internal __attribute__((visibility("hidden")))
#define private static
/* MACROS */

/* Definations */
#define IP "127.0.0.1"
#define PORT 80
#define ERR_STR "Error %d : %s\n"
#define magic_str1 (0xfade)
#define magic_str2 (0xaa55)
#define Inodesperblock (16)
#define DirectPtrsperInode (8)
#define PtrsperBlock (256)
#define MAX_FS MAX_DD
#define MAX_FILE_NAME 10
#define MAX_PATH_LEN 256
#define VALID_VOCAB                                                            \
  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.-_"
#define MAX_FILE_PER_DIR (DirectPtrsperInode + PtrsperBlock)
/* Definations */
typedef i16 ptr;
typedef i8 Bootsector[500];
internal packed enum e_type {
  InvalidType = 0x00,
  FileType = 0x01,
  DirType = 0x03
};
typedef enum e_type Type;

typedef i8 Bitmap;

/* Superblock 512B */
internal struct s_sb {
  Bootsector boot; // 500 bytes bootsector code         = 500 bytes
  i16 _;           // Reserved 2 bytes 0x00             = 2 bytes
  i16 blocks;      // No of Blocks                      = 2 bytes
  i16 inodes;      // No of Inodes                      = 2 bytes
  i16 inodeblocks; // No of Inode Blocks                = 2 bytes
  i16 magic[2];    // Magic string hex {0xfade,0xaa55}  = 4 bytes
} packed;          /*512 Bytes*/

typedef struct s_sb Suprblk;

/* FileSystem */
internal struct s_fs {
  i8 drive;         // Drive no  (Option TODO: DD can provide driveno)
  Disk *dd;         // Disk descriptor onto which fs mounted
  i8 *bitmap;       // Bitmap to check active and available inodes
  Suprblk metadata; // Metadata of superblock
} packed;

typedef struct s_fs Filesystem;

internal struct fname { // Support for extentions of max 3 chars
  i8 name[8];
  i8 ext[3];
} packed;
typedef struct fname Filename;

/* Inode 32B */
internal struct s_inode {
  i8 validtype;                   // Inode Type (Valid/File/Dir)  = 1 byte
  i16 size;                       // Size of file pointed by inode = 2 bytes
  Filename name;                  // 11 byte filename
  ptr indirect;                   // 2 byte indirect ptr
  ptr direct[DirectPtrsperInode]; //  2 * 8 = 16 bytes
} packed;
typedef struct s_inode Inode;

/* FSBLOCK(Block pointed to by indirect ptr) */
internal union u_block {
  Suprblk super;
  i8 data[BLOCK_SIZE];
  ptr ptrs[PtrsperBlock];
  Inode inodes[Inodesperblock];
} packed;
typedef union u_block FSblock;

internal struct s_file_info {
  i16 size;
  ptr idx;
} packed;
typedef struct s_file_info File_stat;

internal struct s_path {
  Filesystem *fs;
  Filename *target;
  struct s_Tok_ret *inter;
} packed;

typedef struct s_path Path;

struct s_file_entry {
  ptr idx;
  Filename name;
  i16 size;
  Type filetype;
} packed;
typedef struct s_file_entry File_entry;

struct s_ls {
  i16 count;
  File_entry *arr[];
} packed;
typedef struct s_ls Ls;

struct s_dir {
  Filesystem *fs;
  ptr idx;
  Filename name;
  Ls *entrys;
} packed;
typedef struct s_dir Dir;

struct s_file_meta {
  Time create, modify;
  i8 perm : 3;
  i16 size;
  Path *path;
  Ls *subdirs;
} packed;
typedef struct s_file_meta File_Metadata;

#ifndef omsdhwr
#define omsdhwr
extern public Filesystem *FileDescriptors[MAX_FS];
#endif

/* Function Signatures */
int main(int, char **);
internal Filesystem *fsformat(Disk *, Bootsector *,
                              i8); // To format and mount fs onto a disk
internal Bitmap *mkbitmap(Filesystem *, i8);           // To generate bitmap
internal i16 allocbitmap(Filesystem *, Bitmap *);      // Alloc bitmap
internal void freebitmap(Filesystem *, Bitmap *, i16); // Free bitmap
internal i16 openfiles(Disk *);                        // YET to IMPLEM TODO:
internal void closeallfiles(Disk *);                   // YET to IMPLEM TODO:
internal void fsshow(Filesystem *);                    // Details of Filesystem
internal Inode *fetchinode(Filesystem *, ptr);         // Retrieve the ith inode
internal i8 *filestr(Filename *); // Convert s_Filename to filename string
internal void print_inodes(Filesystem *); // Print all available active inodes
internal void print_bitmap(Filesystem *); // Prints entire bitmap
internal Filesystem *fsmount(i8);         // Mount the Fs
internal void fsunmount(Filesystem *);    // Unmount the fs;
internal ptr create_inode(Filesystem *, Filename *, Type); //  Creates an inode
#define destroy_inode(f, p)                                                    \
  inode_dealloc((Filesystem *)fs, (ptr)p);    // Destroys a inode
internal ptr inode_alloc(Filesystem *);       // Helper fxn to create_inode
internal i8 inode_dealloc(Filesystem *, ptr); // Helper fxn to destroy_inode
internal ptr
save_inode(Filesystem *, Inode *,
           ptr); // Write thew inode to disk, hekper to create_inode
internal i8 validfname(Filename *,
                       Type); // Checks validity of Filename struct of Type type
internal File_stat *fsstat(Filesystem *, ptr); // File_stat struct of a file
internal void fstatshow(File_stat *);          // Displays fstat struct
internal Filename *
parse_name(i8 *); // Pareses the filename string to a s_Filename struct
internal i8 *filetostr(Filename *);
internal i8 *eval_path(i8 *);
internal ptr read_dir(Filesystem *, ptr, Filename *);
internal Filename *toggle_fname_case(Filename *, i8);
internal Path *init_path(i8 *, Filesystem *);
internal void filename_show(Filename *);
internal void show(void *, i8 *);
internal void show_path(Path *);
internal i8 validchar(i8);
internal void show_ls(Ls *);
internal Dir *open_dir(i8 *);
internal Ls *listfiles(Filesystem *, Inode *);
internal void show_inode(Inode *);
internal void wipe_fs(Filesystem *);
internal void wipe_disk(Disk *);
internal ptr search_subdir(Filesystem *, Inode *, i8 *);
internal ptr get_ptr(i8 *, Filesystem *);
/* Function Signatures */
