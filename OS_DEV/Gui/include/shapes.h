/* shapes.h */
#pragma once
#include "gui.h"
#include "assembly.h"

struct s_point{
i16 x;
i16 y;
i8 color:4;
} packed;

typedef struct s_point Point;

struct s_line{
Point * start;
Point * end;
i8 color:4;
i16 weight;
}packed;
typedef struct s_line Line;

struct s_rectangle{
Point * p1,* p2;
i8 fgcolor:4;
i8 bgcolor:4;
i8 solid:1;
i16 weight;
} packed;

typedef struct s_rectangle Rectangle;

struct s_rgb_color{
    i8 red;
    i8 blue;
    i8 green;
    i8 _;
}packed;

typedef struct s_rgb_color RGB;
typedef RGB Color_table[16];



#define MAX_X 640
#define MAX_Y 480
#define x40_25_T 0x00
#define x640_480_16 0x12
#define DEFAULT_COLOR 0xf
#define DEFAULT_FILL 0x00

#ifndef OMG
#define OMG
extern i8 videoflag;
#endif

void videomode(i8);
i8 draw_point(Point*);
i8 draw_line(Line *);
i16 getmax_x();
i16 getmax_y();
i16 draw_rectangle(Rectangle *);
void set_palette(i16,RGB*);
RGB** set_all_palettes();

// -----------Constructors----------------//
Point * init_point(i16,i16,i8);
Line * init_line(Point*,Point*,i8,i16);
Rectangle * init_rectangle(Point *,Point *,i8,i8,i8,i16);
RGB * init_rgb(i8,i8,i8);