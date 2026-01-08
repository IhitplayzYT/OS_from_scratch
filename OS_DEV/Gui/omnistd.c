/* omnistd.c */
#include "omnistd.h"
#include "gui.h"

void _copy(i8* a,i8* b,i16 size,i8 flag){
i8 *p,*q;
i16 n;
for (p=a,q=b,n = size; n && (!flag || *q);n--,p++,q++) *p = *q;
if (flag){*p = 0;}

}

void _fill(i8*a,i8 c,i16 n,i8 flag){
i8*p;
if (flag) for (p=a;n && *a;n--,p++) *p = c;
else for (p=a;n;n--,p++) *p = c;
}


i8* _snprintf(i8*a,i16 size,i8* frmt,...){
if (!a || !frmt) return 0;
i16 bytes = 0,l;
i8* dptr,*sptr,contin=1,*p,**pp; 
sptr = frmt;
dptr = (i8*)a;
pp = &frmt;
void * mem;

while (*sptr && contin){
switch (*sptr){
case '%':
{   sptr++;
    switch(*sptr){
        case 0:
        break;

        case 'c':
         overflow(2);
        p = *++pp;
        mem = (void *)p;
        *dptr++ =(i8)mem;
        bytes ++;
        sptr++;
        break;

        case 's':
        p = *++pp;
        l = len(p);
        if (l) {
        overflow((l+1));
        strcopy(dptr,p,l);
        dptr += l;
        bytes +=l;}
        sptr++;
        break;

        case 'x':
            overflow(5);
            p = *++pp;
            i16 val;
            memcopy(&val,p,2);
            p = to_hex(val);
            strcopy(dptr,p,4);
            dptr+=4;
            bytes += 4;
            sptr++;
            break;
        case '%':
            overflow(2);
            *dptr++ = '%';
            bytes ++;
            sptr++;
            break;

    }
    break;
}

case '\n':
{   overflow(2);
    *dptr ++ = '\r';
    *dptr ++ = '\n';
    sptr++;
    bytes +=2;
    break; 
}

default:
{   overflow(2);
    *dptr ++ = *sptr ++;
    bytes ++;;
    break;
}
}

}




*dptr = 0;
bytes ++;
return a;
}



i16 _len(i8* s){
i16 c;
i8* p = s;
for (c=0;*p;p++,c++);
return c;
}

i8 hexchar(i8 c){
 if (c < 10) return (c +'0');
 else return ((c-10)+'A');
}

i8* to_hex(i16 x){
    i8 a_,b_,c_,d_;
    i8 a,b,c,d;
    d_ = (x & 0x000f);
    c_ = ((x & 0x00f0) >> 4);
    b_ = ((x & 0x0f00) >> 8);
    a_ = ((x & 0xf000) >> 12);
    a = hexchar(a_);
    b = hexchar(b_);
    c = hexchar(c_);
    d = hexchar(d_);
    static  i8 buff[5];
    buff[0] = a;
    buff[1] = b;
    buff[2] = c;
    buff[3] = d;
    buff[4] = 0;
    return (i8*)buff;
}
