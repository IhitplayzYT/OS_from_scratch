/* api.h */
#pragma once
/*Includes*/
#include "stdoslib.h"
#include "os.h"
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
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
#define SWAP(X, Y) (X) ^= (Y) ^= (X) ^= (Y)
#define getposixfd(x) fds[(x)]
/* MACROS */

/* Definations */
#define IP "127.0.0.1"
#define PORT 80
#define ERR_STR "[%s:%d] Error %d : %s\n"
#define MAX_FD 256
/* Definations */

/* Function Signatures */
private i8 isopen(i8);
private void init_fds();
/* Function Signatures */
