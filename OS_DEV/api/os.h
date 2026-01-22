/* os.h */
#pragma once
#include "fs.h"
#include "stdoslib.h"
/* Defines */
#define public __attribute__((visibility("default")))
#define internal __attribute__((visibility("hidden")))
#define private static
#define ERR_BUF_SIZE 10
/* Defines */


/* Typedefs */
typedef unsigned char i8;
typedef unsigned short i16;
typedef unsigned int i32;
typedef unsigned long i64;
/* Typedefs */

/* Error Number Implem */
#ifndef ERR_GLOBAL
#define ERR_GLOBAL
extern public i8 err_number;
extern public i8 INIT_FLAG;
#endif
#define reterr(x) do {  \
    err_number = (x); \
    return (x);       \
    } while (0);      
#define throw() return 0
#define seterr(x)do {err_number = (x);} while(0)
/* Error Number Implem */


/* Error Number defs */
enum{NO_ERR,BAD_FD,IO_ERR,CLOSED_FD,NO_INIT_ERR,BUFF_OVRFLW,NOT_MOUNT_ERR,DISK_IO_ERR,BUSY_ERR,MEM_ERR,BAD_ARG,BAD_FILE_NAME,INODE_ERR,LIMIT_ERR,NET_ERR,BAD_DIR,TYPE_ERR,PATH_ERR,FILE_NOT_FOUND} public;
/* Error Number defs */

/*Function Signatures */
public i8 write_byte(i8, i8); /* Writes a singular byte to a fd */
public i8 read_byte(i8); /* Reads a singular byte from a fd */
public void init(); /* Initialises some functionalities */
public void dinit(); /* Initialises the disk/drive*/
public i8 get_errno();  /* Returns the Err_Number */
public void print_err();  /* Prints Err_Number with Description */
public void print_err_buff();  /* Print our last ERR_BUFF_SIZE errors that occured */

/* Function Signatures */