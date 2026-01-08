/* api.c */

#define ERR_GLOBAL
#include "api.h"

public i8 err_number = 0;
public i8 INIT_FLAG = 0;
public char err[][25] = {"No Errors","Bad/Invalid FD","I/O Error","Closed FD","Init not called","Buffer Overflow","Disk not mounted","Disk IO Error","Unclosed/Busy File","Memory Error","Invalid Argument(s)","Invalid File/Dir Name","Invalid Inode","Limit Reached","Network Error","Invalid Directory","Mismatched Types","Path Not Found","Unable to Open Dir/File"};


private i8 fds[MAX_FD];
/* 
fd[0] -> stderr
fd[1] -> stdin
fd[2] -> stdout
fd[...] -> file stream FDs
*/

/* Return : 1 -> Open FD, 0 -> Closed FD, NO_INIT_ERR */
private i8 isopen(i8 fd)
{struct stat _;
   if (!INIT_FLAG) reterr(NO_INIT_ERR);
    if (fd < 3) return 0;
    i8 psxfd = getposixfd(fd);
    if (!psxfd)return 0;
    if  (fstat(psxfd,&_) == -1) return 0;
    return 1;
}

/* Return : 0 -> Sucessful write to fd , else Err_number*/
public i8 write_byte(i8 fd, i8 chr)
{ if (!INIT_FLAG) reterr(NO_INIT_ERR);
    if (fd < 0) reterr(BAD_FD);
    if (fd > 1  && !isopen(fd)) reterr(CLOSED_FD);
    i8 buff[2] = {0};
    buff[0] = chr;
    i8 psxfd = getposixfd(fd);
    if (!psxfd) reterr(CLOSED_FD);
    psxfd = (psxfd == 1) ? 0 : (psxfd == 2) ? 1: (psxfd);
if (!write(psxfd,buff,1)) reterr(IO_ERR);
return 0;
}


/* Return : 0 -> Sucessful read from fd , else Err_number*/
public i8 read_byte(i8 fd) {
if (!INIT_FLAG) reterr(NO_INIT_ERR);
if (fd < 0) reterr(BAD_FD);
if (fd > 2 && !isopen(fd)) reterr(CLOSED_FD);
char buff[2] = {0};
    i8 psxfd = getposixfd(fd);
    if (!psxfd) reterr(CLOSED_FD);
    psxfd = (psxfd == 1) ? 0 : (psxfd == 2) ? 1: (psxfd);
if (!read(psxfd,buff,1)) reterr(IO_ERR);
return (i8)*buff;
}


private void init_fds(){
fds[0] = 1;
fds[1] = 2;
for (int i = 2;i < MAX_FD ;i ++)fds[i] = 0;
}

public void print_err(){
printf(ERR_STR,err_number,err[err_number]);
}

public void init(){
init_fds();
err_number = 0;
INIT_FLAG = 1;
dinit();
}

public i8 get_errno(){return err_number;}

