#include "disk.h"
#include "fs4disk.h"
#include "fs.h"
#include "os.h"
#include "stdoslib.h"
#include <stdio.h>
#include <string.h>

typedef unsigned char i8;
typedef unsigned short i16;

void usage_format(i8 *str) {
  fprintf(stderr, "Usage: %s format [-s] <drive>\n", str);
  exit(-1);
}

void usage(i8 *str) {
  fprintf(stderr, "Usage: %s <command> [arguments]\n", str);
  exit(-1);
}

void cmd_format(i8 *s1, i8 *s2) {
  i8 drive, *drivestr, bootable;
  if (!s1)
    usage_format((i8 *)"diskutil");
  else if (!s2) {
    bootable = 0;
    drivestr = s1;
  } else {
    if (s1[0] == '-' && s1[1] == 's') {
      bootable = 1;
      drivestr = s2;
    } else
      usage_format((i8 *)"diskutil");
  }
  drive = (strcomp(drivestr,"c") == 0 || strcomp(drivestr,"C") == 0)   ? 2
          : ((strcomp(drivestr,"d")==0) || strcomp(drivestr,"D") == 0) ? 1
                                                       : 3;
                                              
  if (drive == 3)
    usage_format((i8 *)"diskutil");
  if (bootable) {
    fprintf(stderr, "Boot not supported\n");
    exit(-1);
  }
  i8 force;
  printf("This will format the Disk %c erase all data.Proceed?[y/N]\n",
         drivestr[0]);
  scanf("%c", &force);
  if (force != 'y' && force != 'Y')
    return;
  i8 iforce = (force == 'y' || force == 'Y') ? 1 : 0;

  printf("Formatting Disk %c...\n", drivestr[0]);
  dinit();
  Disk *d = attach(drive);
  if (!d) {
    printf("Bad Disk\n");
    exit(-1);
  }
  Filesystem *fs = fsformat(d, 0, iforce);
  ptr root = inode_alloc(fs);
  if (!fs) {
    printf("Formatting Error\n");
    exit(-1);
  }
  printf("Disk Formatted!\n");
  show(fs, (i8 *)"filesystem");
  ptr idx1 = create_inode(fs, parse_name((i8 *)"hello.bat"), FileType);
  ptr idx2 = create_inode(fs, parse_name((i8 *)"helog"), DirType);

//   NO ERRORS BEFORE THIS POINTS


  //  Fix this shows way too many inodes and many were empty
   show(fetchinode(fs,1),(i8 *)"inode");
   Ls * ls = listfiles(fs,fetchinode(fs,0));
   printf("%d\n",ls->count);
  inode_dealloc(fs, idx1);
  inode_dealloc(fs, idx2);
  detach(d);
  free(fs);
  return;
}

int main(int argc, char *argv[]) {
  char *s1, *s2;
  if (argc < 2)
    usage_format((i8 *)argv[0]);
  else if (argc == 2)
    s1 = s2 = 0;
  else if (argc == 3) {
    s1 = argv[2];
    s2 = 0;
  } else {
    s1 = argv[2];
    s2 = argv[3];
  }
  i8 *cmd = (i8 *)argv[1];
  if (!strcmp((const char *)cmd, "format"))
    cmd_format((i8 * )s1, (i8  *)s2);
  else
    usage((i8 *)argv[1]);

  return 0;
}
