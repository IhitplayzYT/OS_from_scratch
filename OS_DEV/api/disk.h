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
)
#define dread(d,a,b) disk_io(read,(d),(a),(b))
#define dwrite(d,a,b) disk_io(write,(d),(a),(b))



/* MACROS */

/* Structs */
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
#ifndef X_H
#define X_H
extern public Disk* DiskDescriptor[MAX_DD];
#endif

/* Function Signatures */
internal Disk * attach(i8);
internal void detach(Disk *);
public void disk_info(Disk *);
internal i8 * numppend(i8 *,i8); /* Appends a number to end of a string */
internal Filesystem* fsmount(i8);

/* Function Signatures */

#endif