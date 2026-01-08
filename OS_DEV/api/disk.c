/* Disk.c */

#include "disk.h"
#include "fs.h"

internal i8 attached;
public Disk *DiskDescriptor[MAX_DD];

public void dinit() {
  attached = 0;
  int n;
  for (n = 1; n <= MAX_DD; n++)
    DiskDescriptor[n - 1] = attach(n);

  if (*DiskDescriptor) {
    fsformat(DiskDescriptor[0], NULL, 0);
  }

  for (n = 1; n <= MAX_DD; n++)
    detach(DiskDescriptor[n - 1]);
}

internal i8 *numppend(i8 *a, i8 n) {
  if (!n || n > 9)
    return 0;
  static char buff[256];
  zero(buff, 255);
  strcopy(buff, a);
  int l = len(a);
  buff[l] = 0x30 + n;
  buff[l + 1] = '\0';
  return buff;
}

public void disk_info(Disk *dd) {
  if (!dd || !dd->drive_no)
    return;
  printf("------(Disk %d)------\nAttached ? %s\nPath : %s\nFD -> %d\nBlock "
         "Size(Bytes) = %d\n--------------------\n",
         dd->drive_no, attached & dd->drive_no ? "True" : "False",
         numppend(Basepath, dd->drive_no), dd->fd, dd->blocks);
}

internal Disk *attach(i8 d_no) {
  if (d_no == 1 || d_no == 2)
    ;
  else
    return (Disk *)0;
  if (attached & d_no)
    return (Disk *)0;
  Disk *dd = (Disk *)alloc(sizeof(Disk));
  if (!dd) {
    dealloc(dd);
    return (Disk *)0;
  }
  i8 *path = numppend(Basepath, d_no);
  int fd = open(path, O_RDWR);
  if (fd < 3) {
    close(dd->fd);
    dealloc(dd);
    return (Disk *)0;
  }
  dd->fd = fd;
  dd->drive_no = d_no;
  struct stat buf;
  int t = fstat(dd->fd, &buf);
  if (t) {
    close(dd->fd);
    dealloc(dd);
    return (Disk *)0;
  }
  dd->blocks = (buf.st_blocks - 1);
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
