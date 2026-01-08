/* bmp.h */
#pragma once
#include "gui.h"
#include "shapes.h"


typedef unsigned char i8;
typedef unsigned short i16;
typedef unsigned int i32;
typedef unsigned long i64;

#define public __attribute__((visibility("default")))
#define internal __attribute__((visibility("hidden")))
#define private static
#define packed __attribute__((packed))
#define getcolor(x,y) (y)

struct s_bmp {   // size = 14 bytes
i16 sign;
i16 file_size;
i16 _;
i16 __,___;
i16 offset;
i16 ____;
}packed;

typedef struct s_bmp Bmp_header;

struct s_bmp_info{
i16 _,__;   // Size of info header = 40 
i16 width;
i16 ___;
i16 height;
i16 ____;
i16 _____;  //Planes
i16 ______;  //BPS
i16 _______,________;  // Compression
i16 __________,____________;  // Compressed image size
i16 _____________,______________;  // xpixels
i16 _______________,________________;  //ypixels
i16 _________________,__________________;  // colors used
i16 ___________________,____________________; 
} packed;

typedef struct s_bmp_info Bmp_Info_header;


struct s_bitmap{
i8 *filename;
i16 x,y;
Bmp_header * header;
Bmp_Info_header * info;
Color_table * color;
} packed;

typedef struct s_bitmap Bitmap;

Bitmap * parse_bmp(i8*,i16,i16);
i8 draw_bmp(Bitmap*);
i8* addbmp(i8*);
void bmp_show(Bitmap*);
//i8 getcolor(Color_table*,i8);