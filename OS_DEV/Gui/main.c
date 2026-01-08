/*   main.c   */
asm(".code16gcc\n"
    ".globl _start\n"
    "_start:\n"
    "call main\n"
    "call exit\n");
#include "gui.h"
#include "assembly.h"
#include "shapes.h"
#include "bmp.h"
#include "omnistd.h"

void * heapptr,* heapsaved;
//extern RGB ** Palette_Table;

void main(){
videoflag = 0;
//Palette_Table = (RGB**)0;
init_heap();
videomode(x640_480_16);
//Palette_Table = set_all_palettes();
//if (!Palette_Table) return;
Bitmap * bmp = parse_bmp((i8*)"output",50,50);
bmp_show(bmp);
draw_bmp(bmp);
dealloc_all();
return;
}

void readn(i16 fd, void* buf, i16 n) {
    i8* p = buf;
    for (i16 i=0; i<n; i++) {
        i8 b = read(fd);
        if (b == -1) {return;}
        p[i] = b;
        
    }
}


void init_heap(){
dealloc_all();
heapptr=heap1;
}

void putchar(i8 c){
xputchar(c);
return;
}

void print_x(i8* str){
if (!str) return;
i8* p;
    for (p=str;*p;p++) putchar(*p);
return;
}

void * alloc(i16 size){
if (!size) return (void *)0;
i16 n_size = heapptr - heap1 + size;
if (n_size > heapsize) return (void *)0;
void * p = heapptr;
heapptr = (void *)((i8 *)heapptr + size);
return p;
}

void dealloc_all(){
    heapptr = heap1;
    heapsaved = heapptr;
    return;
}

i8 getchar(void){
i8 al,ah,ax,ret;
ax = xgetchar();
al = (ax & 0x0f);
ah = ((ax & 0xf0)  >> 4);
ret = (al) ? al : ah;
return ret;
}

i8* tostr(i16 x){
i8 *buff = (i8*)alloc(20);
buff[19] = 0;
i8 i = 18;
if (x == 0) {buff[i] = '0';
return buff+i;}
else{
while (x > 0){
buff[i] = '0' + x % 10;
x /= 10;
i--;
}}

return buff+i+1;
}


void save(){
    heapsaved = heapptr;
}

void load(){
    heapptr = heapsaved;
}

void copy_(i8*a,i8*b,i16 n){
i8*p,*q;
for (p=a,q=b;n;n--,p++,q++)*p = *q;
}


i16 open(i8* path,i16 offset){
i16 fd = x_open(path); 
if (!fd) return 0;
i8 re = x_move(fd,offset);
if (!re) {close(fd);return 0;}
return fd;
}

i8 read(i16 fd){
i16 n;
i8 ah,al;
n = x_read(fd);
al = (n & 0xff);
ah = ((n >> 8) & 0xff);
if (ah) {print("error");return 0;}
else 
    return al;
}


void close(i16 fd){
x_close(fd);
}