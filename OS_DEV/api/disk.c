/* Disk.c */

#include "disk.h"
#include "fs4disk.h"

internal i8 attached;
public Disk *DiskDescriptor[MAX_DD];

public void dinit() {
  i8 n;

  attached = 0;
 for (n=1; n<=MAX_DD; n++)
      *(DiskDescriptor+(n-1)) = attach(n);
  
  if (*DiskDescriptor)
      fsmount(1);

 for (n=1; n<=MAX_DD; n++)
      detach(DiskDescriptor[(n-1)]);

  return;
}

internal i8 *numppend(i8 *a, i8 n) {
  if (!n || n > 9)
    return 0;
  static char buff[256];
  zero(buff, 255);
  strcopy((i8 * )buff, (i8 *)a);
  int l = len(a);
  buff[l] = 0x30 + n;
  buff[l + 1] = '\0';
  return (i8 *)buff;
}

public void Disk_info(Disk *dd) {
  if (!dd || !dd->drive_no)
    return;
  printf("------(Disk %d)------\nAttached ? %s\nPath : %s\nFD -> %d\nBlock "
         "Size(Bytes) = %d\n--------------------\n",
         dd->drive_no, attached & dd->drive_no ? "True" : "False",
         numppend((i8 *)Basepath, dd->drive_no), dd->fd, dd->blocks);
}

internal Disk *attach(i8 d_no) {
    Disk *dd;
    i16 size;
    i8 *file;
    signed int tmp;
    struct stat sbuf;

    if ((d_no==1) || (d_no==2));
    else return (Disk *)0;

    if (attached & d_no)
        return (Disk *)0;

    size = sizeof(Disk);
    dd = (Disk *)malloc(size);
    if (!dd)
        return (Disk *)0;
    
    zero(dd, size);
    file = numppend((i8 *)Basepath, d_no);
    tmp = open((char *)file, O_RDWR);
    if (tmp < 3) {
        free(dd);
        return (Disk *)0;
    }
    dd->fd = tmp;

    tmp = fstat(dd->fd, &sbuf);
    if (tmp || !sbuf.st_blocks)
    {
        close(dd->fd);
        free(dd);

        return (Disk *)0;
    }
    dd->blocks = (sbuf.st_blocks-1);
    dd->drive_no = d_no;
    attached |= d_no;
    return dd;
}

internal void detach(Disk *dd) {
  if (!dd)
    return;
  attached = ~(dd->drive_no) & attached;
  close(dd->fd);
  dealloc(dd);
  dd = NULL;
}
