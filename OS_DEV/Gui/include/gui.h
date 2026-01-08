/*  gui.h  */
#pragma once

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
#define packed __attribute__((packed))
#define print(x) print_x((i8*)x)
#define copy(a,b,n) copy_((i8*)a,(i8*)b,n)

/* MACROS */

/* Definations */
#define IP "127.0.0.1"
#define PORT 80
#define ERR_STR "Error %d : %s\n"
/* Definations */

extern void *heap1;
extern i16 heapsize;

/* Function Signatures */
void print_x(i8*);
void putchar(i8);
i8 getchar();
i8 * tostr(i16);
void save();
void load();
void *alloc(i16);
void dealloc_all();
void videomode(i8);
void copy_(i8*,i8*,i16);
i16 open(i8*,i16);
i8 read(i16);
void close(i16);
void init_heap();
void readn(i16,void*,i16);
/* Function Signatures */


