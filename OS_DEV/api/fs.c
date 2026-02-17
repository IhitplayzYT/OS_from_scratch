/*  fs.h  */
#include "disk.h"
#include "fs4disk.h"
#include "os.h"
#include "stdoslib.h"
#include "fs.h"
public Filesystem *FileDescriptors[MAX_FS];

internal Bitmap *mkbitmap(Filesystem *fs, i8 scan) {
  FSblock block;
  i16 idx = 0;
  if (!fs) {
    seterr(BAD_ARG);
    return (Bitmap *)0;
  }
  i16 x = ceil_div(fs->dd->blocks, 8);
  Bitmap *map = (Bitmap *)alloc(x);
  if (!map) {
    seterr(MEM_ERR);
    return (Bitmap *)0;
  }
  if (!scan) {
    zero(map, x);
    return map;
  }

  for (int i = 2; i <= fs->metadata.inodeblocks + 1; i++) {
    if (!dread(fs->dd, (i8 *)&block, i)) {
      dealloc(map);
      seterr(DISK_IO_ERR);
      return (Bitmap *)0;
    }

    for (int j = 0; j < Inodesperblock; j++, idx++) {
      if (block.inodes[j].validtype & 0x01) {
        setbit((i8 *)map, idx);
      } else {
        unsetbit((i8 *)map, idx);
      }
    }
  }

  return map;
}

internal void print_inodes(Filesystem *fs) {
  if (!fs) {
    seterr(BAD_ARG);
    return;
  }
  i16 m = fs->metadata.inodeblocks * Inodesperblock;
  for (int i = 0; i < m; i++) {
    Inode *inode = fetchinode(fs, i);
    if (inode && inode->validtype & 0x01) {
      printf("----Inode---- %d:\nPath: %s\nSize: %d\nInode Type: %s\n", (i + 1),
             concat((i8 * )"/", filestr(&inode->name)), inode->size,
             (inode->validtype == FileType)  ? "File"
             : (inode->validtype == DirType) ? "Directory"
                                             : "????");
      printf("--------\n");
    }
  }
  return;
}
//  yyyyyyyyyy
internal Inode *fetchinode(Filesystem *fs, ptr idx) {
  FSblock bl;
  i16 n, size;
  i8 res;
  Inode *ret;
  ptr x, y;
  if (!fs){
    seterr(BAD_ARG);
    return NULL;
  }
  ret = (Inode *)0;
  for (n=0, x=2; x<(fs->metadata.inodeblocks+2); x++) {
  zero(&bl, BLOCK_SIZE);
  res = dread(fs->dd,&bl.data, x);
  if (!res)
      {seterr(DISK_IO_ERR);return NULL;}
  for (y=0; y< Inodesperblock; y++, n++) {
    if (n == idx) {
        size = sizeof(struct s_inode);
        ret = (Inode *)alloc(size);
        if (!ret)
          {seterr(MEM_ERR);return NULL;}                
      zero(ret, size);
      copy(ret, &bl.inodes[y], size);
      x = -1;
      if (!ret){
      return ret; 
      }
      }
    }

  }

  if (!ret)
      {seterr(INODE_ERR);return NULL;}
  return ret;
}

internal void fsshow(Filesystem *fs) {
  if (!fs) {
    seterr(BAD_ARG);
    return;
  }
  printf("-----FileSystem-----\nDrive- \\%c\nPath: %s\nBlocks= %d\nMagicString "
         "(0x%.04x) (0x%.04x)\nNo of Blocks: [%d]\nNo of Inodes-> %d\nNo of "
         "Inode Blocks->[%d]\n--------------------\n",
         (fs->drive == 1)   ? 'C'
         : (fs->drive == 2) ? 'D'
                            : '?',
         numppend((i8 *)Basepath, fs->drive), fs->dd->blocks, fs->metadata.magic[0],
         fs->metadata.magic[1], fs->metadata.blocks, fs->metadata.inodes,
         fs->metadata.inodeblocks);
}

internal void print_bitmap(Filesystem *fs) {
  if (!fs) {
    seterr(BAD_ARG);
    return;
  }
  print_bytes((void *)fs->bitmap, ceil_div(fs->dd->blocks, 8));
}

internal i16 allocbitmap(Filesystem *fs, Bitmap *bmp) {
  int i;
  if (!bmp || !fs) {
    seterr(BAD_ARG);
    return 0;
  }
  for (i = 0; i < fs->dd->blocks; i++)
    if (!getbit((i8 *)bmp, i)) {
      setbit((i8 *)bmp, i);
      return i + 1;
    }
  return 1;
}

internal void freebitmap(Filesystem *fs, Bitmap *bmp, i16 x) {
  if (!bmp || !fs) {
    seterr(BAD_ARG);
    return;
  }
  unsetbit((i8 *)bmp, x - 1);
  return;
}

internal Filesystem *fsformat(Disk *dd, Bootsector *mbr, i8 force) {
  if (!dd) {
    seterr(NOT_MOUNT_ERR);
    return (Filesystem *)0;
  }
  if (openfiles(dd)) {
    if (!force) {
      seterr(BUSY_ERR);
      return (Filesystem *)0;
    } else
      closeallfiles(dd);
  }
  Suprblk super;
  Inode inode;
  FSblock fsb;
  Filesystem *fs = (Filesystem *)alloc(sizeof(Filesystem));
  if (!fs) {
    seterr(MEM_ERR);
    return (Filesystem *)0;
  }
  i16 blocks,inodeblocks,size;
  Inode idx;
  blocks = dd->blocks;
  inodeblocks = (blocks / 10);
  if (blocks % 10)
      inodeblocks++;
  
  idx.validtype = DirType;
  idx.size = 0;
  zero(&idx.name, 11);
  idx.indirect = 0;
  size = (sizeof(ptr)*DirectPtrsperInode);
  zero(&idx.direct, size);
  super.magic[0] = magic_str1;
  super.magic[1] = magic_str2;
  super.inodes = 1;
  super.inodeblocks = inodeblocks;
  super.blocks = blocks;
  super._ = 0;


  fs->drive = dd->drive_no;
  fs->dd = dd;
  inode.validtype = DirType;
  inode.size = 0;
  zero((i8 *)&inode.name, 11);
  inode.indirect = 0;
  zero((i8 *)inode.direct, sizeof(ptr) * DirectPtrsperInode);

  super.inodes = 1;
  super.blocks = dd->blocks;
  super.inodeblocks = ceil_div(super.blocks, 10);
  super._ = 0;
  i8 ok = 1;
  FSblock idxblock;
    if (mbr)
        copy( &super.boot,  mbr, 500);
    else
        zero( &super.boot, 500);

    ok = dwrite(dd, &super, 1);
    if (!ok)
        {seterr(DISK_IO_ERR);return NULL;}

    zero( &idxblock, BLOCK_SIZE);
    copy( &idxblock,  &idx, sizeof(idx));
    ok = dwrite(dd, &idxblock, 2);
    if (!ok)
        {seterr(DISK_IO_ERR);return NULL;}


    zero( &fsb, BLOCK_SIZE);
    for (int n=0; n<inodeblocks; n++) {
        ok = dwrite(dd, &fsb, (n+3));
        if (!ok)
        {seterr(DISK_IO_ERR);return NULL;}

    }

    size = sizeof(Filesystem);
    fs = (Filesystem *)alloc(size);
    if (!fs)
      {seterr(MEM_ERR);return NULL;}

    zero( fs, size);

    fs->drive = dd->drive_no;
    fs->dd = dd;
    copy( &fs->metadata,  &super, BLOCK_SIZE);
    Bitmap * bm = mkbitmap(fs, false);
    size =
          1 // superblock
        + fs->metadata.inodeblocks; // # of inode blocks;
    for (int n=0; n<=size; n++)
        setbit(bm, n);
    fs->bitmap = bm; ptr root = create_inode(fs, parse_name((i8 *)"/"), DirType);
  return fs;
}

internal i8 *filestr(Filename *n) {
  if (!n || !*n->name) {
    seterr(BAD_FILE_NAME);
    return (i8 *)0;
  };
  if (!*n->ext) {
    return n->name;
  }
  static i8 buff[13];
  strcopy(buff, n->name);
  buff[len(n->name)] = '.';
  strcopy(buff + len(n->name) + 1, n->ext);
  return buff;
}

// Fix SOME ERROR [DONOT ADD c: IN THIS START FROM ROOT]
internal Filename *parse_name(i8 *str) {
    if (!str || !*str) {
        seterr(BAD_ARG);
        return (Filename *)0;
    }
    Filename *fname = (Filename *)alloc(sizeof(Filename));
    if (!fname) {
        seterr(MEM_ERR);
        return (Filename *)0;
    }
    zero(fname, sizeof(Filename));
    if (strcomp(str, ".") == 0 || strcomp(str, "..") == 0) {
        i8 i = 0;
        while (str[i] && i < 8) {
            fname->name[i] = str[i];
            i++;
        }
        fname->name[i] = 0;
        return fname;
    }
    if (str[0] == '.') {
        i8 i = 0;
        while (str[i] && i < 8) {
            fname->name[i] = str[i];
            i++;
        }
        fname->name[i] = 0;
        return fname;
    }
    i8 i = 0, n = 0, e = 0;
    i8 seen_dot = 0;
    while (str[i]) {
        if (str[i] == '.' && !seen_dot) {
            seen_dot = 1;
            i++;
            continue;
        }
        if (!seen_dot) {
            if (n < 8)
                fname->name[n++] = str[i];
        } else {
            if (e < 3)
                fname->ext[e++] = str[i];
        }
        i++;
    }
    fname->name[n] = 0;
    fname->ext[e] = 0;
    return fname;
}


internal Filesystem *fsmount(i8 drive) {
  if (drive > MAX_FS) {
    seterr(LIMIT_ERR);
    return (Filesystem *)0;
  }
  Disk *dd = DiskDescriptor[drive - 1];
  if (!dd) {
    seterr(MEM_ERR);
    return (Filesystem *)0;
  }
  Filesystem *fs = (Filesystem *)alloc(sizeof(Filesystem));
  if (!fs) {
    seterr(MEM_ERR);
    return (Filesystem *)0;
  }
  fs->drive = drive;
  fs->dd = dd;
  fs->bitmap = mkbitmap(fs, 1);
  if (!fs->bitmap) {
    dealloc(fs);
    return (Filesystem *)0;
  }

  if (!dread(dd, (i8 *)&fs->metadata, 1)) {
    seterr(DISK_IO_ERR);
    dealloc(fs);
    return (Filesystem *)0;
  }
  kprintf("Mounted disk 0x%02x on drive %c:", fs->drive,
          (drive == 1)   ? 'C'
          : (drive == 2) ? 'D'
                         : '?');
  FileDescriptors[drive - 1] = fs;
  
//  Dir *dir = open_dir((i8 *)"c:/helog");
ptr idx = mkdirectory((i8 *)"c:/helog");
printf("idx = %d\n",idx);


  return fs;
}


internal void fsunmount(Filesystem *fs) {
  if (!fs) {
    seterr(BAD_ARG);
    return;
  }
  FileDescriptors[fs->drive - 1] = (Filesystem *)0;
  kprintf("Unmounted disk 0x%x on drive %c:", fs->drive,
          (fs->drive == 1)   ? 'C'
          : (fs->drive == 2) ? 'D'
                             : '?');
  dealloc(fs);
}

internal i8 validfname(Filename *name, Type t) {
  if (!name || !t || (t == DirType && *name->ext)) {
    seterr(BAD_ARG);
    return 0;
  }
  for (int i = 0; name->name[i] != '\0'; i++) {
    if (!validchar(name->name[i])) {
      seterr(BAD_FILE_NAME);
      return 0;
    }
  }

  if (t == FileType) {
    for (int i = 0; name->ext[i] != '\0'; i++) {
      if (!validchar(name->ext[i])) {
        seterr(BAD_FILE_NAME);
        return 0;
      }
    }
  }
  return 1;
}

// TODO:  MAKE SURE BITMAP ALSO GETS UPDATED HERE
// Err in ctreate_inode and alloc_inode ?

//   yyyyyy
internal ptr create_inode(Filesystem *fs, Filename *name, Type t) {
  if (!fs) {
    seterr(BAD_ARG);
    return 0;
  };
  if (!validfname(name, t))
    reterr(BAD_FILE_NAME);
  ptr idx = inode_alloc(fs);
  if (!idx) {
    seterr(INODE_ERR);
    return 0;
  }
  Inode *ino = (Inode *)alloc(sizeof(Inode));
  if (!ino) {
    seterr(MEM_ERR);
    return 0;
  }
  if (!ino) {
    seterr(INODE_ERR);
    return 0;
  }
  zero(ino, sizeof(Inode));
  ino->validtype = t;
  ino->size = 0;
  memcopy(&(ino->name), name, sizeof(Filename));
  if (!save_inode(fs, ino, idx)) {
    seterr(INODE_ERR);
    dealloc(ino);
    return 0;
  }

  return idx;
}

internal ptr inode_alloc(Filesystem *fs) {
  if (!fs) {
    seterr(BAD_ARG);
    return 0;
  }
  ptr n, idx = 0;
  Inode *p = (Inode *)alloc(sizeof(Inode));
  if (!p) {
    seterr(MEM_ERR);
    return 0;
  }
  zero(p, sizeof(Inode *));
  i16 t = Inodesperblock * fs->metadata.inodeblocks;
  for (n = 0; n < t; n++) {
    p = fetchinode(fs, n);
    if (!p) {
      seterr(INODE_ERR);
      return 0;
    }
    if (!(p->validtype & 0x01)) {
      idx = n;
      p->validtype = 0x01;
      if (!save_inode(fs, p, idx)) {
        idx = 0;
        seterr(INODE_ERR);
        break;
      }
      fs->metadata.inodes++;
      dwrite(fs->dd, &fs->metadata, 1);
      break;
    }
  }
  if (!p)
    dealloc(p);
  return idx;
}

internal ptr save_inode(Filesystem *fs, Inode *inode, ptr idx) {
  if (!fs || !inode) {
    seterr(BAD_ARG);
    return 0;
  }
  ptr bno = ((ptr)idx / 16) + 2;
  ptr mod = idx % 16;
  i8 buff[512];
  if (!dread(fs->dd, buff, bno)) {
    seterr(DISK_IO_ERR);
    return 0;
  }
  memcopy(&buff[sizeof(Inode) * mod], inode, sizeof(Inode));
  if (!dwrite(fs->dd, buff, bno)) {
    seterr(DISK_IO_ERR);
    return 0;
  }
  return bno;
}

internal i8 inode_dealloc(Filesystem *fs, ptr p) {
  if (!fs) {
    seterr(BAD_ARG);
    return 0;
  }
  Inode *inode = fetchinode(fs, p);
  if (!inode) {
    seterr(INODE_ERR);
    return 0;
  }
  inode->validtype = InvalidType;
  ptr b = save_inode(fs, inode, p);
  if (!b) {
    dealloc(inode);
    seterr(INODE_ERR);
    return 0;
  }
  return 1;
}

internal File_stat *fsstat(Filesystem *fs, ptr p) {
  if (!fs) {
    seterr(BAD_ARG);
    return (File_stat *)0;
  }
  Inode *ino = fetchinode(fs, p);
  if (!ino) {
    seterr(MEM_ERR);
    return 0;
  }
  File_stat *fstat = (File_stat *)alloc(sizeof(File_stat));
  if (!fstat) {
    seterr(MEM_ERR);
    dealloc(ino);
    return 0;
  }
  fstat->idx = p;
  fstat->size = ino->size;
  dealloc(ino);
  return fstat;
}

// TODO: TO MAKE IT COMPATIBLE WITH PATH
internal void fstatshow(File_stat *fst) {
  if (!fst) {
    seterr(BAD_ARG);
    return;
  }
  printf("-----FILE-----\nSize: %d\nInode num: %d\n-----------\n", fst->size,
         fst->idx);
}

internal i8 *eval_path(i8 *path) {
    if (!path || !*path) {
        seterr(BAD_ARG);
        return (i8 *)"/";
    }

    i32 l = freq(path, (i8)'/');
    i8 **stack = (i8 **)alloc(sizeof(i8 *) * (l + 1));
    if (!stack) {
        seterr(MEM_ERR);
        return (i8 *)"/";
    }
    i16 top = 0;
    struct s_Tok_ret *ret = tokenise(path, '/');
    if (!ret) {
        dealloc(stack);
        seterr(MEM_ERR);
        return (i8 *)"/";
    }
    for (i32 i = 0; i < ret->n; i++) {
        i8 *tok = ret->ret[i];
        if (!*tok || strcomp(tok, ".") == 0)
            continue;
        if (strcomp(tok, "..") == 0) {
            if (top > 0) top--;
            continue;
        }
        if (strcomp(tok, "c:") == 0 || strcomp(tok, "d:") == 0 ||
            strcomp(tok, "C:") == 0 || strcomp(tok, "D:") == 0)
            continue;

        stack[top] = (i8 *)alloc(MAX_FILE_NAME);
        if (!stack[top]) {
            seterr(MEM_ERR);
            goto cleanup;
        }
        strcopy(stack[top], tok);
        top++;
    }

    i8 *simplified_path = (i8 *)alloc(MAX_PATH_LEN);
    if (!simplified_path) {
        seterr(MEM_ERR);
        goto cleanup;
    }
    i32 k = 0;
    simplified_path[k++] = '/';
    for (i16 i = 0; i < top; i++) {
        i32 len = strcopy(simplified_path + k, stack[i]);
        k += len;
        if (i != top - 1)
            simplified_path[k++] = '/';
    }
    simplified_path[k] = '\0';

cleanup:
    for (i16 i = 0; i < top; i++)
        dealloc(stack[i]);
    dealloc(stack);
    dealloc(ret);
    return simplified_path;
}


internal ptr read_dir(Filesystem *fs, ptr p, Filename *name) {
  if (!fs || !p || !name)
    return 0;
  if (!validfname(name, DirType)) {
    seterr(BAD_FILE_NAME);
    return 0;
  }
  Inode *inode = fetchinode(fs, p);
  if (!inode) {
    seterr(INODE_ERR);
    return 0;
  }
  //  ERR MAYBE HERE
  if (inode->validtype != DirType) {
    dealloc(inode);
    seterr(TYPE_ERR);
    return 0;
  }
  ptr idx;
  name = toggle_fname_case(name, 0);
  for (ptr i = 0; i < DirectPtrsperInode; i++) {
    idx = inode->direct[i];
    if (!idx)
      continue;
    Inode *pointed = fetchinode(fs, idx);
    if (!pointed)
      continue;
    if (memcomp(name, (i8 *)&pointed->name, 11) == 0) {
      dealloc(inode);
      dealloc(pointed);
      return idx;
    }
    dealloc(pointed);
  }

  if (!inode->indirect) {
    dealloc(inode);
    seterr(INODE_ERR);
    return 0;
  }
  ptr bno = inode->indirect;
  FSblock block;
  if (!dread(fs->dd, &block, bno)) {
    dealloc(inode);
    seterr(DISK_IO_ERR);
    return 0;
  }
  for (ptr i = 0; i < Inodesperblock; i++) {
    idx = block.ptrs[i];
    if (!idx)
      continue;
    Inode *pointed = fetchinode(fs, idx);
    if (!pointed)
      continue;
    if (memcomp(name, (i8 *)&pointed->name, 11) == 0) {
      dealloc(inode);
      dealloc(pointed);
      return idx;
    }
    dealloc(pointed);
  }
  dealloc(inode);
  return 0;
}

internal Filename *toggle_fname_case(Filename *fn, i8 flag) {
  if (!fn) {
    seterr(BAD_ARG);
    return (Filename *)0;
  }
  Filename *name = (Filename *)alloc(sizeof(Filename));
  if (!name) {
    seterr(MEM_ERR);
    return (Filename *)0;
  }
  if (flag == 0) {
    memcopy(name->name, tolwrn(fn->name, 8), 8);
    memcopy(name->ext, tolwrn(fn->ext, 3), 3);
  } else {
    memcopy(name->name, toupprn(fn->name, 8), 8);
    memcopy(name->ext, tolwrn(fn->ext, 3), 3);
  }
  return name;
}

internal void show(void *arg, i8 *s) {
  if (!arg) {
    seterr(BAD_ARG);
    return;
  }
  if (strcomp(s, "inodes") == 0)
    print_inodes((Filesystem *)arg);
  else if (strcomp(s, "bitmap") == 0)
    print_bitmap((Filesystem *)arg);
  else if (strcomp(s, "fstat") == 0)
    fstatshow((File_stat *)arg);
  else if (strcomp(s, "filesystem") == 0)
    fsshow((Filesystem *)arg);
  else if (strcomp(s, "filename") == 0)
    filename_show((Filename *)arg);
  else if (strcomp(s, "path") == 0)
    show_path((Path *)arg);
  else if (strcomp(s, "ls") == 0)
    show_ls((Ls *)arg);
  else if (strcomp(s, "inode") == 0)
    show_inode((Inode *)arg);
  else {
    seterr(BAD_ARG);
    return;
  }
}

internal void filename_show(Filename *fn) {
  if (!fn) {
    seterr(BAD_ARG);
    return;
  }
  if (*fn->ext)
    printf("Filename: %s.%.3s\n", fn->name, fn->ext);
  else
    printf("Dirname: %s\n", fn->name);
}

internal i8 validchar(i8 c) {
  i8 *p = (i8 *)VALID_VOCAB;
  for (; *p; p++) {
    if (*p == c)
      return 1;
  }
  return 0;
}

///  ///  massive seg fault here   TODO:

internal Path *init_path(i8 *path, Filesystem *fs) {
  if (!path || !*path) {
    seterr(BAD_ARG);
    return (Path *)0;
  }
  path = eval_path(path);
  Path *ans = (Path *)alloc(sizeof(Path));
  if (!ans) {
    seterr(MEM_ERR);
    return (Path *)0;
  }
  struct s_Tok_ret *ret = tokenise(path, '/');
  if (!ret) {
    return (Path *)0;
  }
  i8 drive = DiskDescriptor[0]->drive_no;
  if (fs)
    ans->fs = fs;
  else
    ans->fs = fsmount(2);
  ans->target = parse_name(ret->ret[ret->n - 1]);
  if (!ans->target) {
    seterr(BAD_FILE_NAME);
    return (Path *)0;
  }
  ret->ret[--ret->n] = NULL;
  ans->inter = ret;
  return ans;
}

internal void show_path(Path *p) {
  if (!p || !p->target)
    return;

  if (!*p->target->ext)
    printf("----Path----\nDisk: %s\nTarget Dir: %s\n",
           (p->fs->drive == 1)   ? "C"
           : (p->fs->drive == 2) ? "D"
                                 : "None",
           p->target->name);
  else
    printf("----Path----\nDisk: %s\nTarget File: %s.%.3s\n",
           (p->fs->drive == 1)   ? "C"
           : (p->fs->drive == 2) ? "D"
                                 : "None",
           p->target->name, p->target->ext);
  printf("Dirs: [");
  for (int i = 0; i < p->inter->n; i++) {
    printf("%s", p->inter->ret[i]);
    if (i != p->inter->n - 1)
      printf(", ");
  }
  printf("]\n-----------\n");
}

internal void show_ls(Ls *ls) {
  if (!ls)
    return;
  for (i16 i = 0; i < ls->count; i++) {
    if (!*ls->arr[i]->name.ext)
      printf("%d. %s | %s | %d bytes ", i + 1,
             (ls->arr[i]->filetype == DirType)    ? "Dir"
             : (ls->arr[i]->filetype == FileType) ? "File"
                                                  : "None",
             ls->arr[i]->name.name, ls->arr[i]->size);
    else
      printf("%d. %s | %s.%s | %d bytes ", i + 1,
             (ls->arr[i]->filetype == DirType)    ? "Dir"
             : (ls->arr[i]->filetype == FileType) ? "File"
                                                  : "None",
             ls->arr[i]->name.name, ls->arr[i]->name.ext, ls->arr[i]->size);
  }
}

internal Dir *open_dir(i8 *str) {
  if (!str) {
    seterr(BAD_ARG);
    return (Dir *)0;
  }
  Dir *dir = (Dir *)alloc(sizeof(Dir));
  if (!dir) {
    seterr(MEM_ERR);
    return (Dir *)0;
  }
  Path *path = init_path(str, (Filesystem *)0);
  if (!path) {
    seterr(PATH_ERR);
    return (Dir *)0;
  }
//  {xxxxxxx}
  Inode *ino = fetchinode(path->fs, 0);
//  {xxxxxxx}
  if (!ino) {
    dealloc(path);
    seterr(INODE_ERR);
    return (Dir *)0;
  }
  
  ptr x;
  ptr dirptr = 1;
  for (i16 i = 0; i < path->inter->n; i++) {
    Filename *name = parse_name(path->inter->ret[i]);
    printf("%s\n", filestr(name));
    if (!name) {
      seterr(BAD_FILE_NAME);
      return (Dir *)0;
    }
    // Check here  (xx)
    x = read_dir(path->fs, dirptr, name);
    //
    if (!x) {
      dealloc(path);
      dealloc(ino);
      seterr(PATH_ERR);
      return (Dir *)0;
    }
    dirptr = x;
  }
  x = read_dir(path->fs, dirptr, path->target);
  if (!x) {
    dealloc(path);
    dealloc(ino);
    seterr(PATH_ERR);
    return (Dir *)0;
  }
  dirptr = x;
  ino = fetchinode(path->fs, dirptr);
  if (!ino) {
    dealloc(path);
    seterr(FILE_NOT_FOUND);
    return (Dir *)0;
  }
  if (ino->validtype != DirType) {
    dealloc(ino);
    dealloc(path);
    seterr(FILE_NOT_FOUND);
    return (Dir *)0;
  }
  dir->fs = path->fs;
  memcopy(&dir->name, path->target, sizeof(Filename));
  dir->idx = x;
  Ls *filelist = listfiles(dir->fs, ino);
  if (!filelist) {
    seterr(PATH_ERR);
    dealloc(ino);
    dealloc(path);
    return (Dir *)0;
  }
  dir->entrys = filelist;
  dealloc(ino);
  return dir;
}

internal Ls *listfiles(Filesystem *fs, Inode *dir) {
  if (!fs || !dir) {
    seterr(BAD_ARG);
    return (Ls *)0;
  }
  i16 c = 0;
  File_entry arr[MAX_FILE_PER_DIR];
  Inode *inode;
  FSblock block;
  ptr iptr;
  for (int i = 0; i < DirectPtrsperInode; i++) {
    iptr = dir->direct[i];
    inode = fetchinode(fs, iptr);
    if (!inode || !inode->validtype)
      continue;
    c++;
    arr[c - 1].idx = iptr;
    memcopy(&arr[c - 1].name, &inode->name, sizeof(Filename));
    arr[c - 1].size = inode->size;
    arr[c - 1].filetype = inode->validtype;
    free(inode);
  }

  if (dir->indirect) {
    zero(&block, BLOCK_SIZE);
    iptr = dir->indirect;
    if (!dread(fs->dd, &block, iptr)) {
      seterr(DISK_IO_ERR);
      return (Ls *)0;
    }
    for (int i = 0; i < PtrsperBlock; i++) {
      iptr = block.ptrs[i];
      if (!iptr)
        continue;
      inode = fetchinode(fs, iptr);
      if (!inode)
        continue;
      c++;
      arr[c - 1].idx = iptr;
      memcopy(&arr[c - 1].name, &inode->name, sizeof(Filename));
      arr[c - 1].size = inode->size;
      arr[c - 1].filetype = inode->validtype;
      free(inode);
    }
  }
  Ls *ret = (Ls *)alloc(sizeof(Ls));
  for (int i = 0; i < c; i++)
    ret->arr[i] = (File_entry *)alloc(sizeof(File_entry *));
  ret->count = c;
  copy(ret->arr, arr, sizeof(File_entry) * (c - 1));
  return ret;
}

internal void show_inode(Inode *ino) {
  printf("----Inode----\nName: %s\nType: %s\nSize: %d\n\tPointers:\n",
         filestr(&ino->name),
         (ino->validtype == DirType)    ? "Directory"
         : (ino->validtype == FileType) ? "File"
                                        : "None",
         ino->size);
  if (!ino->indirect)
    printf("Indir: %hu\n", ino->indirect);
  printf("Direct: [");
  for (int i = 0; i < DirectPtrsperInode; i++) {
    printf(" %hu", ino->direct[i]);
  }
  printf(" ]\n");
}

internal i8 *filetostr(Filename *fn) {
  if (!fn)
    return (i8 *)0;
  i8 *str = (i8 *)alloc(sizeof(i8) * 11);
  if (!str)
    return (i8 *)0;
  memcopy(str, fn->name, 8);
  if (!*fn->ext)
    return str;
  else {
    i16 l = len(str);
    str[l - 1] = '.';
    memcopy(str + l, fn->ext, 3);
  }
  return str;
}

internal void wipe_fs(Filesystem *fs) {
  i8 buff[512] = {0};
  for (int i = 0; i < fs->dd->blocks + 2; i++) {
    if (!dwrite(fs->dd, buff, 0)) {
      seterr(DISK_IO_ERR);
      return;
    }
  }
}

internal void wipe_disk(Disk *dd) {
  i8 buff[512] = {0};
  for (int i = 0; i < dd->blocks + 2; i++) {
    if (!dwrite(dd, buff, 0)) {
      seterr(DISK_IO_ERR);
      return;
    }
  }
}

// TODO: Working on this and its helper fxn
internal ptr get_ptr(i8 *s, Filesystem *fs) {
  if (!s || !*s) {
    seterr(BAD_ARG);
    return 0;
  }
  if (!fs)
    fs = FileDescriptors[0];
  Inode *ino = fetchinode(fs, 0);
  for (i16 i = 0; i < DirectPtrsperInode; i++) {
    Inode *t = fetchinode(fs, ino->direct[i]);
    if (!t || t->validtype == InvalidType)
      continue;
    if (strcomp(filestr(&t->name), s) == 0) {
      dealloc(t);
      dealloc(ino);
      return ino->direct[i];
    }
    ptr x = search_subdir(fs, t, s);
    dealloc(t);
  }
  Inode *t = fetchinode(fs, ino->indirect);
  if (!t) {
    seterr(INODE_ERR);
    return 0;
  }
  if (strcomp(filestr(&t->name), s) == 0) {
    dealloc(t);
    ptr y = ino->indirect;
    dealloc(ino);
    return y;
  }
  ptr x = search_subdir(fs, t, s);
  dealloc(t);
  dealloc(ino);
  return x;
}

internal ptr search_subdir(Filesystem *fs, Inode *inode, i8 *s) {
  if (!inode)
    return 0;
  i16 k = 0;
  for (i16 i = 0; i < DirectPtrsperInode; i++) {
    Inode *t = fetchinode(fs, inode->direct[i]);
    if (!t)
      continue;

    if (t->validtype == DirType || t->validtype == FileType) {
      k += 1;
    } else
      continue;

    if (strcomp(filestr(&t->name), s) == 0) {
      dealloc(t);
      dealloc(inode);
      return inode->direct[i];
    }
    return search_subdir(fs, t, s);
    dealloc(t);
  }
  Inode *t = fetchinode(fs, inode->indirect);
  if (!t) {
    return 0;
  }
  if (strcomp(filestr(&t->name), s) == 0) {
    dealloc(t);
    ptr y = inode->indirect;
    dealloc(inode);
    return y;
  }
  return search_subdir(fs, t, s);
}

internal Inode *get_inode(i8 *path, Filesystem *fs) {
  if (!path || !*path)
    return (Inode *)0;
  Inode *ino;
  Path *p = init_path(path, (!fs) ? FileDescriptors[0] : fs);
  if (!path)
    return (Inode *)0;
  Inode *inode = fetchinode(p->fs, 0);
  for (int i = 0; i < p->inter->n; i++) {
  }

  return ino;
}

internal ptr path2inode(Path * p ){
  ptr iptr = 1, tmp;
  i16 size, n;
  Filename name;
  if (*p->inter->ret[0])
      for (n=0; *p->inter->ret[n]; n++) {
          zero(&name, sizeof(Filename));
          size = len(p->inter->ret[n]);
          if (!size)
              break;
          memcopy(&name.name,&p->inter->ret[n], size);
          tmp = read_dir(p->fs, iptr, &name);
          if (!tmp)
              reterr(PATH_ERR);
          iptr = tmp;
  }

  tmp = read_dir(p->fs, iptr, p->target);
  if (!tmp) reterr(PATH_ERR)
  else return tmp;
}

internal ptr mkdirectory(i8 * s){
char buff[255] = {0},target[256] = {0};
strncopy((i8 *)&buff, s,255);
i8 * p = find_chrr(buff, '/');
if (!p) reterr(PATH_ERR);
*p++ = 0;
strncopy((i8 *)&target,p,255);
p = (i8 *)buff;
Path * path2 = init_path(p, NULL)  ;
if (!path2) reterr(PATH_ERR); 
ptr in = path2inode(path2);
if (!in) reterr(PATH_ERR)
Filename name;
zero(&name,sizeof(Filename));
memcopy(&name.name,&target, 8);
ptr idx = create_inode(path2->fs, &name, DirType);
if (!idx) reterr(INODE_ERR);
Inode * inode = fetchinode(path2->fs,idx);
if (!inode) {inode_dealloc(path2->fs,in);reterr(INODE_ERR);}
int i;
for (i = 0 ;i < DirectPtrsperInode;i++){
  if (!inode->direct[i]) break;
}

if (!inode->direct[i]){
inode->direct[i] = idx;
if (!save_inode(path2->fs, inode,idx)){
inode_dealloc(path2->fs, idx);
dealloc(inode);
reterr(INODE_ERR);
}
dealloc(inode);
return idx;
}
ptr block;
if (!inode->indirect){
block = allocbitmap(path2->fs, path2->fs->bitmap);
inode->indirect = block;
if (!save_inode(path2->fs, inode,idx)){
  inode_dealloc(path2->fs, idx);
  dealloc(inode);
  freebitmap(path2->fs, path2->fs->bitmap, block);
  reterr(INODE_ERR);
  }
}
  FSblock bl;
zero(&bl.data,BLOCK_SIZE);
bl.ptrs[0] = idx;
if (!dwrite(path2->fs->dd,&bl.data,block)){
inode->indirect = 0;
inode_dealloc(path2->fs, idx);
dealloc(inode);
freebitmap(path2->fs, path2->fs->bitmap, block);
reterr(INODE_ERR);
}
dealloc(inode);
return idx;

}

// TODO:


// TODO: Implement these two
internal i16 openfiles(Disk *dd) { return 0; }
internal void closeallfiles(Disk *dd) { return; }
