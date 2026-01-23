/* disk.h */
#pragma once
/*Includes*/
#include "stdoslib.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
/*Includes*/

#ifndef DISK
#define DISK


/* Typedefinations */
typedef unsigned char i8;
typedef unsigned short i16;
typedef unsigned int i32;
typedef unsigned long i64;
/* Typedefinations */


/* MACROS */
#define public __attribute__((visibility("default")))
#define internal __attribute__((visibility("hidden")))
#define packed __attribute__((packed))
#define private static
#define C_drive 0x01
#define D_drive 0x02
#define BLOCK_SIZE (512)
#define MAX_DD 2
#define Basepath "/home/IhitplayzYT/C_FOLDER/OS_DEV/Drive/Disk."
#define disk_io(f,d,a,b) (\
(d) && (lseek((d)->fd,(int)(BLOCK_SIZE*((b)-1)),SEEK_SET) != -1) && ((f)((d)->fd,(i8 *)(a),(i16)BLOCK_SIZE) == BLOCK_SIZE)\
)  // A underlying base fxn that is bases of dread and dwrite 
#define dread(diskdescriptor,buff,b) disk_io(read,(diskdescriptor),(buff),(b)) // Reads from diskdescriptor 512 bytes into buff at an offset of b such 512 blocks
#define dwrite(d,a,b) disk_io(write,(d),(a),(b))  // Writes to diskdescriptor 512 bytes from buff at an offset of b such 512 blocks
/* MACROS */

/* Structs */
// Disk struct that holds the file descriptor for the disk,no of 512 blocks,and the drive no
struct s_disk{
i32 fd;
i16 blocks;
i8 drive_no:2;
} packed;

typedef struct s_disk Disk;
typedef i8 Block[512];
typedef i8 Bootsector[500];

typedef struct s_fs Filesystem;
/* Structs */
extern public Disk* DiskDescriptor[MAX_DD];

/* Function Signatures */
internal Disk * attach(i8 drive_no); // Attach a disk for drive_no 
internal void detach(Disk *); // Detach a disk fro drive_no
public void disk_info(Disk *);  // Prints the disk info 
internal i8 * numppend(i8 *,i8); /* Appends a number to end of a string */
internal Filesystem* fsmount(i8); // Fsmount used to init the filesystem on the disk[Used in disk.c to initialise the DiskDescriptor list]
/* Function Signatures */
#endif