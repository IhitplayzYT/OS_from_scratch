/* omnistd.h */
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
#define memcopy(a,b,n) copyn(a,b,n)
#define copyn(a,b,n) _copy((i8*)a,(i8*)b,(i16)n,0)
#define strcopy(a,b,n) _copy((i8*)a,(i8*)b,(i16)n,1)
#define zero(a,n) _fill((i8*)a,0,(i16)n,0)
#define fill(a,c,n) _fill((i8*)a,(i8)c,(i16)n,0)
#define szero(a,n) _fill((i8*)a,0,(i16)n,1)
#define sfill(a,c,n) _fill((i8*)a,(i8)c,(i16)n,1)
#define BUFF_SZ 512
#define snprintf(a,n,fmt,...) _snprintf((i8*)a,(i16)n,(i8*)fmt,__VA_ARGS__)
#define min(a,b) ((a)>(b))?(b):(a)
#define max(a,b) ((a)>(b))?(a):(b)
#define printf(frmt,args...) do { \
i8 buff[BUFF_SZ]; \
snprintf(buff,BUFF_SZ-1,frmt,args);\
print(buff);\
} while (0);
#define len(x) _len((i8*)(x))

#define overflow(x) if (bytes > (min(BUFF_SZ,size)-(x))){\
contin = 0;\
break;\
}

/* MACROS */

/* Definations */
#define IP "127.0.0.1"
#define PORT 80
#define ERR_STR "Error %d : %s\n"
/* Definations */

/* Function Signatures */
void _copy(i8*,i8*,i16,i8);
void _fill(i8*,i8,i16,i8);
i8* _snprintf(i8*,i16,i8*,...);
i16 _len(i8*);
i8* to_hex(i16);
i8 hexchar(i8);
/* Function Signatures */


