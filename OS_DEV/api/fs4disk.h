#ifndef FS4DISK
#define FS4DISK
#define internal __attribute__((visibility("hidden")))
typedef struct s_fs Filesystem;
typedef struct s_disk Disk;
typedef unsigned char i8;
typedef i8 Bootsector[500];

internal Filesystem *fsformat(Disk *, Bootsector *,i8);
#endif

