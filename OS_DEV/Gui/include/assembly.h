/* assembly.h */
#pragma once
#include "gui.h"
void xputchar(i8);
void xvideomode(i8);
i8 xgetchar();
i8 xdrawpoint_txt(i16,i16);
i8 xdrawpixel(i16,i16,i8);
i16 x_open(i8*);
i8 x_move(i16,i16);
i8 x_close(i16);
i8 x_read(i16);
void x_set_palette(i16,i8,i8,i8);