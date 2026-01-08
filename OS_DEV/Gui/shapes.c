/* shapes.c */
#include "shapes.h"
i8 videoflag;
RGB ** Palette_Table;

void videomode(i8 mode){
    if (mode > 0x9f) return;
    videoflag = 1;
    xvideomode(mode);
    return;

}

i16 getmax_x(){return MAX_X-1;}

i16 getmax_y(){return MAX_Y-1;}

i8 draw_point(Point * p){
if (!p || !videoflag) return 0;
i16 xm = getmax_x();
i16 ym = getmax_y();
if (p->x > xm || p->y > ym) return 0;
return xdrawpixel(p->x,p->y,p->color);
}

RGB ** set_all_palettes(){
i16 reg,n;
RGB *col;
i8 r,g,b,h;
static RGB* Palette_table[256];
RGB ** clrptr;
for (n = 0,reg = 0; n < 256;n++){
    if (reg > 15) reg = 0;
    if (!reg) r = g = b = 0;
    else if (reg == 15) r = g = b = 0xff;
    else {
    h = reg+1;
    r = g = b = 0;
    r = g = b = (h << 4);
}
col = init_rgb(r,g,b);
if (!col) return (RGB **)0;
set_palette(n,col);
Palette_table[n] = col;
reg++;
}
clrptr = Palette_table;
return clrptr; 
}

void set_palette(i16 reg,RGB* ptr){
if (!ptr) return;
x_set_palette(reg,ptr->red,ptr->green,ptr->blue);
}

i8 draw_line(Line * l){
if (!l || !videoflag || l->start->x > l->end->x || l->start->y > l->end->y ) return 0;
Point * p;
i16 startx,starty,maxx,maxy,maxx_,maxy_,x,y;
maxx_ = getmax_x();
maxy_ = getmax_y();
if (l->start->x == l->end->x){
    starty = l->start->y;
    startx = l->start->x;
    maxy = l->end->y + l->weight;
    maxx = l->start->x + l->weight;

if (maxy > maxy_) maxy = maxy_;
if (maxx > maxx_) maxx = maxx_;

}
else if (l->start->y == l->end->y){
    startx = l->start->x;
    starty = l->start->y;
    maxy = l->start->y + l->weight;
    maxx = l->end->x + l->weight;

if (maxy > maxy_) maxy = maxy_;
if (maxx > maxx_) maxx = maxx_;
    
}else return 0;
save();
for (x = startx;x <maxx;x++){
for (y = starty;y < maxy;y++){
    p = init_point(x,y,l->color);
    if (p) draw_point(p);
    load();
}

}

return 1;
}


//TODO: Add the functionality to to solid fill of the rectangle
i16 draw_rectangle(Rectangle * re){
if (!re || !videoflag ) return 0;
i16 maxx,maxy;
maxx = getmax_x();
maxy = getmax_y();

if (re->p1->x >= re->p2->x  || re->p1->y >= re->p2->y || re->p1->x > maxx || re->p1->y > maxy) return 0;
Line *t,*b,*r,*l;
Point *tl,*tr,*bl,*br;
tl = re->p1;
br = re->p2;
tr = init_point(br->x,tl->y,re->fgcolor);
bl = init_point(tl->x,br->y,re->fgcolor);
if (!tr || !bl ) return 0;
t = init_line(tl,tr,re->fgcolor,re->weight);
l = init_line(tl,bl,re->fgcolor,re->weight);
r = init_line(tr,br,re->fgcolor,re->weight);
b = init_line(bl,br,re->fgcolor,re->weight);
if (!t || !l || !r || !b) return 0;
draw_line(l);
draw_line(r);
draw_line(b);
draw_line(t);




return 1;
}



Point * init_point(i16 x,i16 y,i8 color){
Point * p = (Point *)alloc(sizeof(Point));
if (!p) return (Point *)0;
p->x = x;
p->y = y;
p->color =  (color)?color:DEFAULT_COLOR;
return p;
}

Line * init_line(Point*p1,Point*p2,i8 color,i16 weight){
if (!p1 || !p2 || !weight) return (Line *)0;
Line * l = (Line *)alloc(sizeof(Line));
if (!l) return (Line *)0;
l->start = p1;
l->end = p2;
l->color = (color)?color:DEFAULT_COLOR;
l->weight = weight;
return l;
}


Rectangle * init_rectangle(Point *p1,Point *p2,i8 fg,i8 bg,i8 fill,i16 weight){
if (!p1 || !p2 || !weight) return (Rectangle*) 0;
Rectangle * rect = (Rectangle*)alloc(sizeof(Rectangle));
if (!rect) return (Rectangle *)0;
rect->p1 = p1;
rect->p2 = p2;
rect->weight = weight;
rect->fgcolor = (fg)?fg:DEFAULT_COLOR;
rect->bgcolor = (bg)?bg:DEFAULT_COLOR;
rect->solid = (fill)?fill:DEFAULT_FILL;
return rect;
}

RGB * init_rgb(i8 r,i8 g,i8 b){
    RGB * rgb = (RGB *)alloc(sizeof(RGB));
    if (!rgb) return (RGB*)0;
    rgb->red = r;
    rgb->blue = b;
    rgb->green = g;
return rgb;
}