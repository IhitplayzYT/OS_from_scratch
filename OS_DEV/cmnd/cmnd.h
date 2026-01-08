/* cmnd.h */
#pragma once
/*Includes*/
#include <stdio.h>
#include "os.h"
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
/* MACROS */

/* Definations */
#define IP "127.0.0.1"
#define PORT 80
#define ERR_STR "Error %d : %s\n"
/* Definations */

/* Function Signatures */
int main(int, char **);
/* Function Signatures */
