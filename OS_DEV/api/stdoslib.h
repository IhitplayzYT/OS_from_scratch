/* stdoslib.h */
#pragma once
#include <cstdint>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifndef MAX_OBJECTS
#define MAX_OBJECTS 2000
#endif
/* Typedefinations */
typedef unsigned char i8;
typedef unsigned short i16;
typedef unsigned int i32;
typedef unsigned long i64;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long s64;
typedef float f32;
typedef double f64;
typedef unsigned char byte;
typedef unsigned char boolean;
/* Typedefinations */
/* MACROS */
#define true 1
#define True 1
#define false 0
#define False 0
#define unmut const
#define public __attribute__((visibility("default")))
#define internal __attribute__((visibility("hidden")))
#define constructor __attribute__((constructor))
#define private static
#define packed __attribute__((packed))
#define alloc(x) malloc((int)x)
#define dealloc(x) free(x)
#define fill(a, n, x) _fill((i8 *)a, n, (i8)x)
#define strcopy(a, b) _copy((a), (b))
#define copy(a, b, n) _copyn((i8 *)(a), (i8 *)(b), (n), 1)
#define strncopy(a, b, n) _copyn((a), (b), (n), 0)
#define memcopy(a, b, n) _copyn((i8 *)(a), (i8 *)(b), (n), 1)
#define kprintf(f, args...) printf(f "\n", args)
#define zero(a, n) _fill((i8 *)a, n, 0)
#define memcomp(x, y, z) _memcomp((i8 *)x, (i8 *)y, (i16)z)
#define strcomp(a, b) _strcomp((i8 *)(a), (i8 *)(b))
static signed short _strcomp(i8 *, i8 *);

#define compare(x, y) _compare((x), (y)) && ((x) == (y))
#define _compare(x, y)                                                         \
  _Generic((x),                                                                \
      signed char: _Generic((y),                                               \
          signed char: 1,                                                      \
          default: 0) unsigned char: _Generic((y),                             \
          unsigned char: 1,                                                    \
          default: 0) unsigned short: _Generic((y),                            \
          unsigned short: 1,                                                   \
          default: 0) signed short: _Generic((y),                              \
          signed short: 1,                                                     \
          default: 0) unsigned int: _Generic((y),                              \
          unsigned int: 1,                                                     \
          default: 0) signed int: _Generic((y),                                \
          signed int: 1,                                                       \
          default: 0) unsigned long: _Generic((y),                             \
          unsigned long: 1,                                                    \
          default: 0) signed long: _Generic((y),                               \
          signed long: 1,                                                      \
          default: 0) float: _Generic((y),                                     \
          float: 1,                                                            \
          default: 0) double: _Generic((y), double: 1, default: 0))

struct s_iter {
  i32 i, l, type;
  void **data;
  void *(*next)(struct s_iter *);
};
typedef struct s_iter Iterator;

struct s_vector {
  i32 l, cap;
  i32 type;
  void **data;
  void (*append)(struct s_vector *, void *);
  void (*pop)(struct s_vector *);
  Iterator *(*iterator)(struct s_vector *);
};

struct s_tuple {
  i16 sz, cap;
  void (*add)(struct s_tuple *, void *);
  void **data;
};

typedef struct s_tuple Tuple;
typedef struct s_vector Vector;

struct s_time {
  i16 seconds;
  i16 minutes;
  i16 hours;
  i16 days;
  i16 date_of_month;
  i8 weeks;
  i8 weekday;
  i8 month;
  i16 years;
} packed;
typedef struct s_time Time;

#define freq(x, y) _Generic((y), i8 *: s_freq, i8: c_freq)(x, y)

#define DEFINE_NUM_SORT(TYPE, NAME)                                            \
  static inline void NAME(TYPE *arr, i16 n, i8 asc) {                          \
    if (n < 2)                                                                 \
      return;                                                                  \
    i16 mid = n / 2;                                                           \
    TYPE *left = (TYPE *)alloc(mid * sizeof(TYPE));                            \
    TYPE *right = (TYPE *)alloc((n - mid) * sizeof(TYPE));                     \
    for (i16 i = 0; i < mid; i++)                                              \
      left[i] = arr[i];                                                        \
    for (i16 i = mid; i < n; i++)                                              \
      right[i - mid] = arr[i];                                                 \
    NAME(left, mid, asc);                                                      \
    NAME(right, n - mid, asc);                                                 \
    i16 i = 0, j = 0, k = 0;                                                   \
    while (i < mid && j < n - mid) {                                           \
      if (!asc ? (left[i] <= right[j]) : (left[i] >= right[j]))                \
        arr[k++] = left[i++];                                                  \
      else                                                                     \
        arr[k++] = right[j++];                                                 \
    }                                                                          \
    while (i < mid)                                                            \
      arr[k++] = left[i++];                                                    \
    while (j < n - mid)                                                        \
      arr[k++] = right[j++];                                                   \
    dealloc(left);                                                             \
    dealloc(right);                                                            \
  }

#define DEFINE_STRING_SORT(TYPE, NAME)                                         \
  static inline void NAME(TYPE **arr, i16 n, i8 asc) {                         \
    if (n < 2)                                                                 \
      return;                                                                  \
    i16 mid = n / 2;                                                           \
    TYPE **left = (TYPE **)alloc(mid * sizeof(TYPE *));                        \
    TYPE **right = (TYPE **)alloc((n - mid) * sizeof(TYPE *));                 \
    for (i16 i = 0; i < mid; i++)                                              \
      left[i] = arr[i];                                                        \
    for (i16 i = mid; i < n; i++)                                              \
      right[i - mid] = arr[i];                                                 \
    NAME(left, mid, asc);                                                      \
    NAME(right, n - mid, asc);                                                 \
    i16 i = 0, j = 0, k = 0;                                                   \
    while (i < mid && j < n - mid) {                                           \
      if (!asc ? (strcomp(left[i], right[j]) <= 0)                             \
               : (strcomp(left[i], right[j]) >= 0))                            \
        arr[k++] = left[i++];                                                  \
      else                                                                     \
        arr[k++] = right[j++];                                                 \
    }                                                                          \
    while (i < mid)                                                            \
      arr[k++] = left[i++];                                                    \
    while (j < n - mid)                                                        \
      arr[k++] = right[j++];                                                   \
    dealloc(left);                                                             \
    dealloc(right);                                                            \
  }

#define DEFINE_MIN_STR(TYPE, name)                                             \
  static inline TYPE *name(TYPE *X, ...) {                                     \
    va_list args;                                                              \
    va_start(args, X);                                                         \
    TYPE *next;                                                                \
    TYPE *min = X;                                                             \
    while ((next = va_arg(args, TYPE *))) {                                    \
      if (strcomp(next, min) == -1)                                            \
        min = next;                                                            \
    }                                                                          \
    va_end(args);                                                              \
    return min;                                                                \
  }

#define new(NAME, ...) NAME##_init((void *)0, __VA_ARGS__, NULL)

#define DEFINE_MAX_STR(TYPE, name)                                             \
  static inline TYPE *name(TYPE *X, ...) {                                     \
    va_list args;                                                              \
    va_start(args, X);                                                         \
    TYPE *next;                                                                \
    TYPE *max = X;                                                             \
    while ((next = va_arg(args, TYPE *))) {                                    \
      if (strcomp(next, max) == 1)                                             \
        max = next;                                                            \
    }                                                                          \
    va_end(args);                                                              \
    return max;                                                                \
  }

#define DEFINE_MIN(TYPE, name)                                                 \
  static inline TYPE name(TYPE X, ...) {                                       \
    va_list args;                                                              \
    va_start(args, X);                                                         \
    TYPE next;                                                                 \
    TYPE min = X;                                                              \
    while ((next = va_arg(args, TYPE))) {                                      \
      if (min > next)                                                          \
        min = next;                                                            \
    }                                                                          \
    va_end(args);                                                              \
    return min;                                                                \
  }

#define DEFINE_MAX(TYPE, name)                                                 \
  static inline TYPE name(TYPE X, ...) {                                       \
    va_list args;                                                              \
    va_start(args, X);                                                         \
    TYPE next;                                                                 \
    TYPE max = X;                                                              \
    while ((next = va_arg(args, TYPE))) {                                      \
      if (max < next)                                                          \
        max = next;                                                            \
    }                                                                          \
    va_end(args);                                                              \
    return max;                                                                \
  }

#define DEFINE_MIN_ARR(TYPE, name)                                             \
  static inline TYPE name(TYPE *X, ...) {                                      \
    TYPE min = *X;                                                             \
    va_list args;                                                              \
    va_start(args, X);                                                         \
    i32 l = va_arg(args, i32);                                                 \
    for (int i = 0; i < l; i++)                                                \
      if (*(X + i) < min)                                                      \
        min = *(X + i);                                                        \
    va_end(args);                                                              \
    return min;                                                                \
  }

#define DEFINE_MAX_ARR(TYPE, name)                                             \
  static inline TYPE name(TYPE *X, ...) {                                      \
    TYPE max = *X;                                                             \
    va_list args;                                                              \
    va_start(args, X);                                                         \
    i32 l = va_arg(args, i32);                                                 \
    for (int i = 0; i < l; i++)                                                \
      if (*(X + i) > max)                                                      \
        max = *(X + i);                                                        \
    va_end(args);                                                              \
    return max;                                                                \
  }

#define DEFINE_MIN_ARR_S(TYPE, name)                                           \
  static inline TYPE *name(TYPE **X, ...) {                                    \
    TYPE *min = *X;                                                            \
    va_list args;                                                              \
    va_start(args, X);                                                         \
    i32 l = va_arg(args, i32);                                                 \
    for (int i = 0; i < l; i++)                                                \
      if (strcomp(min, *(X + i)) == 1)                                         \
        min = *(X + i);                                                        \
    va_end(args);                                                              \
    return min;                                                                \
  }

#define DEFINE_MAX_ARR_S(TYPE, name)                                           \
  static inline TYPE *name(TYPE **X, ...) {                                    \
    TYPE *max = *X;                                                            \
    va_list args;                                                              \
    va_start(args, X);                                                         \
    i32 l = va_arg(args, i32);                                                 \
    for (int i = 0; i < l; i++)                                                \
      if (strcomp(max, *(X + i)) == -1)                                        \
        max = *(X + i);                                                        \
    va_end(args);                                                              \
    return max;                                                                \
  }

typedef __builtin_va_list va_list;
#define va_start(ap, last) __builtin_va_start(ap, last)
#define va_arg(ap, type) __builtin_va_arg(ap, type)
#define va_end(ap) __builtin_va_end(ap)

#define trash(arg, ...) ((void)0)

#ifndef IMP_DEF
#define IMP_DEF

DEFINE_NUM_SORT(char, sort_i8)
DEFINE_NUM_SORT(short, sort_i16)
DEFINE_NUM_SORT(int, sort_i32)
DEFINE_NUM_SORT(long, sort_i64)
DEFINE_NUM_SORT(unsigned char, sort_u8)
DEFINE_NUM_SORT(unsigned short, sort_u16)
DEFINE_NUM_SORT(unsigned int, sort_u32)
DEFINE_NUM_SORT(unsigned long, sort_u64)
DEFINE_NUM_SORT(float, sort_f)
DEFINE_NUM_SORT(double, sort_d)

DEFINE_MIN_ARR(i16, min_i16a);
DEFINE_MIN_ARR(i32, min_i32a);
DEFINE_MIN_ARR(i64, min_i64a);
DEFINE_MIN_ARR(s16, min_s16a);
DEFINE_MIN_ARR(s32, min_s32a);
DEFINE_MIN_ARR(s64, min_s64a);
DEFINE_MIN_ARR(f32, min_fa);
DEFINE_MIN_ARR(f64, min_da);
DEFINE_MIN_ARR_S(i8, min_i8sa);
DEFINE_MIN_ARR_S(s8, min_s8sa);
DEFINE_MIN_ARR_S(char, min_csa);

DEFINE_MAX_ARR(i16, max_i16a);
DEFINE_MAX_ARR(i32, max_i32a);
DEFINE_MAX_ARR(i64, max_i64a);
DEFINE_MAX_ARR(s16, max_s16a);
DEFINE_MAX_ARR(s32, max_s32a);
DEFINE_MAX_ARR(s64, max_s64a);
DEFINE_MAX_ARR(f32, max_fa);
DEFINE_MAX_ARR(f64, max_da);
DEFINE_MAX_ARR_S(i8, max_i8sa);
DEFINE_MAX_ARR_S(s8, max_s8sa);
DEFINE_MAX_ARR_S(char, max_csa);

DEFINE_MIN(i8, min_i8);
DEFINE_MIN(i16, min_i16);
DEFINE_MIN(i32, min_i32);
DEFINE_MIN(i64, min_i64);
DEFINE_MIN(s8, min_s8);
DEFINE_MIN(s16, min_s16);
DEFINE_MIN(s32, min_s32);
DEFINE_MIN(s64, min_s64);
DEFINE_MIN(f32, min_f);
DEFINE_MIN(f64, min_d);
DEFINE_MIN_STR(i8, min_i8s);
DEFINE_MIN_STR(s8, min_s8s);
DEFINE_MAX(i8, max_i8);
DEFINE_MAX(i16, max_i16);
DEFINE_MAX(i32, max_i32);
DEFINE_MAX(i64, max_i64);
DEFINE_MAX(s8, max_s8);
DEFINE_MAX(s16, max_s16);
DEFINE_MAX(s32, max_s32);
DEFINE_MAX(s64, max_s64);
DEFINE_MAX(f32, max_f);
DEFINE_MAX(f64, max_d);
DEFINE_MAX_STR(i8, max_i8s);
DEFINE_MAX_STR(s8, max_s8s);

#define DEF_PRINT_ARR(TYPE, name, fmt)                                         \
  static inline void name(TYPE arr, ...) {                                     \
    va_list args;                                                              \
    va_start(args, arr);                                                       \
    i32 l = va_arg(args, i32);                                                 \
    for (int i = 0; i < l; i++)                                                \
      printf(fmt, arr[i]);                                                     \
    printf("\n");                                                              \
    va_end(args);                                                              \
  }

#define DEF_PRINT_ARGS(TYPE, name, fmt)                                        \
  static inline void name(TYPE a, ...) {                                       \
    va_list args;                                                              \
    va_start(args, a);                                                         \
    TYPE c = a;                                                                \
    printf(fmt, c);                                                            \
    while (c = va_arg(args, TYPE))                                             \
      printf(fmt, c);                                                          \
    printf("\n");                                                              \
    va_end(args);                                                              \
  }

DEF_PRINT_ARR(i8 *, print_i8a, "%hhu ");
DEF_PRINT_ARR(i16 *, print_i16a, "%hu ");
DEF_PRINT_ARR(i32 *, print_i32a, "%u ");
DEF_PRINT_ARR(i64 *, print_i64a, "%lu ");
DEF_PRINT_ARR(s8 *, print_s8a, "%c ");
DEF_PRINT_ARR(s16 *, print_s16a, "%hd ");
DEF_PRINT_ARR(s32 *, print_s32a, "%d ");
DEF_PRINT_ARR(s64 *, print_s64a, "%ld ");
DEF_PRINT_ARR(f32 *, print_f32a, "%f ");
DEF_PRINT_ARR(f64 *, print_f64a, "%lf ");
DEF_PRINT_ARR(char **, print_charsa, "%s ");
DEF_PRINT_ARR(char *, print_crsa, "%c ");
DEF_PRINT_ARR(i8 **, print_i8sa, "%s ");
DEF_PRINT_ARR(s8 **, print_s8sa, "%s ");

DEF_PRINT_ARGS(i8, print_i8, "%c ");
DEF_PRINT_ARGS(i16, print_i16, "%hu ");
DEF_PRINT_ARGS(i32, print_i32, "%u ");
DEF_PRINT_ARGS(i64, print_i64, "%lu ");
DEF_PRINT_ARGS(s8, print_s8, "%c ");
DEF_PRINT_ARGS(char, print_chars, "%c ");
DEF_PRINT_ARGS(s16, print_s16, "%hd ");
DEF_PRINT_ARGS(s32, print_s32, "%d ");
DEF_PRINT_ARGS(s64, print_s64, "%ld ");
DEF_PRINT_ARGS(f32, print_f32, "%f ");
DEF_PRINT_ARGS(f64, print_f64, "%lf ");

#define printarr(x, ...)                                                       \
  _Generic((x),                                                                \
      i8 *: print_i8a,                                                         \
      i16 *: print_i16a,                                                       \
      i32 *: print_i32a,                                                       \
      i64 *: print_i64a,                                                       \
      s8 *: print_s8a,                                                         \
      s16 *: print_s16a,                                                       \
      s32 *: print_s32a,                                                       \
      s64 *: print_s64a,                                                       \
      f32 *: print_f32a,                                                       \
      f64 *: print_f64a,                                                       \
      char *: print_crsa,                                                      \
      char **: print_charsa,                                                   \
      i8 **: print_i8sa,                                                       \
      s8 **: print_s8sa,                                                       \
      i8: print_i8,                                                            \
      i16: print_i16,                                                          \
      i32: print_i32,                                                          \
      i64: print_i64,                                                          \
      s8: print_s8,                                                            \
      s16: print_s16,                                                          \
      s32: print_s32,                                                          \
      s64: print_s64,                                                          \
      f32: print_f32,                                                          \
      f64: print_f64,                                                          \
      char: print_chars)(x, __VA_ARGS__, NULL)

static signed short _strcomp(i8 *a, i8 *b) {
  if (!a || !*a) return -1;
  if (!b || !*b) return 1;
  i8 *p = a, *q = b;
  while (*p && *q) {
    if (*p > *q)
      return 1;
    if (*p < *q)
      return -1;
    p++;
    q++;
  }
  return (*p == *q) ? 0 : (!*p) ? -1 : 1;
}

DEFINE_STRING_SORT(char, _strsort)

#define DEF_FIXED(TYPE, name)                                                  \
  static inline TYPE name(TYPE arg, i32 len) {                                 \
    TYPE buff = (TYPE)alloc(len);                                              \
    for (int i = 0; i < len; i++)                                              \
      buff[i] = arg[len - 1 - i];                                              \
    return buff;                                                               \
  }

DEF_FIXED(char *, rev_char);
DEF_FIXED(s8 *, rev_s8);
DEF_FIXED(i8 *, rev_i8);

#define strreverse(x, y)                                                       \
  _Generic((x), char *: rev_char, i8 *: rev_i8, s8 *: rev_s8)(x, y)

#define max(x, ...)                                                            \
  _Generic((x),                                                                \
      i8: max_i8,                                                              \
      i16: max_i16,                                                            \
      i32: max_i32,                                                            \
      i64: max_i64,                                                            \
      s8: max_s8,                                                              \
      s16: max_s16,                                                            \
      s32: max_s32,                                                            \
      s64: max_s64,                                                            \
      f32: max_f,                                                              \
      f64: max_d,                                                              \
      i8 *: max_i8s,                                                           \
      s8 *: max_s8s,                                                           \
      i16 *: max_i16a,                                                         \
      i32 *: max_i32a,                                                         \
      i64 *: max_i64a,                                                         \
      s16 *: max_s16a,                                                         \
      s32 *: max_s32a,                                                         \
      s64 *: max_s64a,                                                         \
      f32 *: max_fa,                                                           \
      f64 *: max_da,                                                           \
      i8 * *: max_i8sa,                                                        \
      s8 * *: max_s8sa,                                                        \
      char **: max_csa)(x, __VA_ARGS__, NULL)

#define pre_concat(x, y) x##y

#define min(x, ...)                                                            \
  _Generic((x),                                                                \
      i8: min_i8,                                                              \
      i16: min_i16,                                                            \
      i32: min_i32,                                                            \
      i64: min_i64,                                                            \
      s8: min_s8,                                                              \
      s16: min_s16,                                                            \
      s32: min_s32,                                                            \
      s64: min_s64,                                                            \
      f32: min_f,                                                              \
      f64: min_d,                                                              \
      i8 *: min_i8s,                                                           \
      s8 *: min_s8s,                                                           \
      i16 *: min_i16a,                                                         \
      i32 *: min_i32a,                                                         \
      i64 *: min_i64a,                                                         \
      s16 *: min_s16a,                                                         \
      s32 *: min_s32a,                                                         \
      s64 *: min_s64a,                                                         \
      f32 *: min_fa,                                                           \
      f64 *: min_da,                                                           \
      i8 * *: min_i8sa,                                                        \
      s8 * *: min_s8sa,                                                        \
      char **: min_csa)(x, __VA_ARGS__, NULL)

#define DEFINE_SUM(TYPE)                                                       \
  static TYPE _sum##TYPE(TYPE x, ...) {                                        \
    va_list args;                                                              \
    TYPE res = x, temp;                                                        \
    va_start(args, x);                                                         \
    while ((temp = va_arg(args, TYPE))) {                                      \
      res += temp;                                                             \
    }                                                                          \
    return res;                                                                \
  }

#define DEFINE_SUB(TYPE)                                                       \
  static TYPE _sub##TYPE(TYPE x, ...) {                                        \
    va_list args;                                                              \
    TYPE res = x, temp;                                                        \
    va_start(args, x);                                                         \
    while ((temp = va_arg(args, TYPE))) {                                      \
      res -= temp;                                                             \
    }                                                                          \
    return res;                                                                \
  }

#define DEFINE_MUL(TYPE)                                                       \
  static TYPE _mul##TYPE(TYPE x, ...) {                                        \
    va_list args;                                                              \
    TYPE res = x, temp;                                                        \
    va_start(args, x);                                                         \
    while ((temp = va_arg(args, TYPE))) {                                      \
      res *= temp;                                                             \
    }                                                                          \
    return res;                                                                \
  }

#define DEFINE_DIV(TYPE)                                                       \
  static TYPE _div##TYPE(TYPE x, ...) {                                        \
    va_list args;                                                              \
    TYPE res = x, temp;                                                        \
    va_start(args, x);                                                         \
    while ((temp = va_arg(args, TYPE))) {                                      \
      if (temp)                                                                \
        continue;                                                              \
      res /= temp;                                                             \
    }                                                                          \
    return res;                                                                \
  }

DEFINE_SUM(double);
DEFINE_SUB(double);
DEFINE_MUL(double);
DEFINE_DIV(double);
DEFINE_SUM(long);
DEFINE_SUB(long);
DEFINE_MUL(long);
DEFINE_DIV(long);

#define sum(x, ...)                                                            \
  _Generic((x),                                                                \
      char: _sumchar,                                                          \
      short: _sumshort,                                                        \
      int: _sumint,                                                            \
      long: _sumlong,                                                          \
      double: _sumdouble,                                                      \
      float: _sumfloat)(x, ..., NULL);

#define sub(x, ...)                                                            \
  _Generic((x),                                                                \
      char: _subchar,                                                          \
      short: _subshort,                                                        \
      int: _subint,                                                            \
      long: _sublong,                                                          \
      double: _subdouble,                                                      \
      float: _subfloat)(x, ..., NULL);

#define mul(x, ...)                                                            \
  _Generic((x),                                                                \
      char: _mulchar,                                                          \
      short: _mulshort,                                                        \
      int: _mulint,                                                            \
      long: _mullong,                                                          \
      double: _muldouble,                                                      \
      float: _mulfloat)(x, ..., NULL);

#define div(x, ...)                                                            \
  _Generic((x),                                                                \
      char: _divchar,                                                          \
      short: _divshort,                                                        \
      int: _divint,                                                            \
      long: _divlong,                                                          \
      double: _divdouble,                                                      \
      float: _divfloat)(x, ..., NULL);

#define sort(x, l, f)                                                          \
  _Generic((x),                                                                \
      i8 *: sort_u8,                                                           \
      char *: sort_i8,                                                         \
      float *: sort_f,                                                         \
      double *: sort_d,                                                        \
      unsigned short *: sort_u16,                                              \
      unsigned int *: sort_u32,                                                \
      unsigned long *: sort_u64,                                               \
      signed short *: sort_i16,                                                \
      signed int *: sort_i32,                                                  \
      signed long *: sort_i64,                                                 \
      char **: _strsort)(x, l, f);

#define stoi(x) _stoi((i8 *)x)
#define stoi8(x) _stoi8((i8 *)x)
#define stoi16(x) _stoi16((i8 *)x)
#define stoi32(x) _stoi32((i8 *)x)
#define stoi64(x) _stoi64((i8 *)x)
#define pow(x, y) _pow((double)x, (int)y)

#define endian(x) _Generic((x), i16: endian16, i32: endian32, i64: endian64)(x)

#define DEF_LEN(TYPE, name)                                                    \
  static inline i32 name(TYPE str) {                                           \
    i32 n = 0;                                                                 \
    for (; str[n]; n++)                                                        \
      ;                                                                        \
    return n;                                                                  \
  }

DEF_LEN(i8 *, len_i8);
DEF_LEN(char *, len_char);
DEF_LEN(s8 *, len_s8);

#define len(x) _Generic((x), i8 *: len_i8, s8 *: len_s8, char *: len_char)(x)

#define print(fmt, ...) vprintf(fmt, __VA_ARGS__)
#endif
/* MACROS */
#define init_filter(TYPE, fxn)                                                 \
  TYPE *pre_concat(TYPE##_##fxn, _filter)(TYPE * arr, i32 n, i32 * ret_len) {  \
    TYPE *res = (TYPE *)alloc(sizeof(TYPE) * n);                               \
    if (!res)                                                                  \
      return (TYPE *)0;                                                        \
    i32 k = 0;                                                                 \
    for (i32 i = 0; i < n; i++)                                                \
      if (fxn(arr[i]))                                                         \
        res[k++] = arr[i];                                                     \
    if (!k)                                                                    \
      return (TYPE *)0;                                                        \
    TYPE *ret = (TYPE *)realloc(res, (k) * sizeof(TYPE));                      \
    if (!ret)                                                                  \
      return (TYPE *)0;                                                        \
    *ret_len = k;                                                              \
    return ret;                                                                \
  };

struct s_Tok_ret {
  i8 **ret;
  i16 n;
};

/* Function Signatures */
public
void _fill(i8 *, i16, i8); /* Fills fixed no of bytes to input hex/char*/
public
i16 _copy(i8 *, i8 *); /* Copy contents from second to first string */
public
i16 _copyn(i8 *, i8 *, i16, i8); /* Copy a 'N' chars from src to dest string*/
public
i8 *concat(i8 *, i8 *);
public
i16 floor_div(i16, i16);
public
i16 ceil_div(i16, i16);
public
void print_bytes(void *, i32);
public
void print_hex(void *, i32);
public
i8 _getbit(i8 *, i16);
public
void _setbit(i8 *, i16);
public
void _unsetbit(i8 *, i16);
public
void _flipbit(i8 *, i16);
public
i8 getbit(i8 *, i16);
public
void setbit(i8 *, i16);
public
void unsetbit(i8 *, i16);
public
void flipbit(i8 *, i16);
public
double precision(double, i8);
public
i32 ipaddr(i8 *);
public
i8 *ipstr(i32);
public
i16 net_port(i16);
public
i16 endian16(i16 x);
public
i32 endian32(i32 x);
public
i64 endian64(i64 x);
public
Vector *Vector_init(void *, ...);
public
Tuple *Tuple_init(void *, ...);
public
void v_append(struct s_vector *, void *);
public
void t_add(Tuple *, void *);
public
void v_print(struct s_vector *);
public
void v_pop(struct s_vector *);
Iterator *iterator(struct s_vector *);
void *next(Iterator *);
public
int _stoi(i8 *);
public
i64 _stoi64(i8 *);
public
i32 _stoi32(i8 *);
public
i16 _stoi16(i8 *);
public
i8 _stoi8(i8 *);
public
double _pow(double, int);
public
void FINALIZE();
public
long cur_time();
public
i64 ticks_elapsed();
public
i64 tick_freq();
public
i64 seconds_elapsed();
public
i8 *fmttime(Time *);
public
Time *curr_time();
public
i8 *strchar(i8 *, i8);
public
s16 strcharidx(i8 *, i8);
public
i8 *strstrs(i8 *, i8 *);
public
s16 strstrsidx(i8 *, i8 *);
public
void print_s_Tok_ret(struct s_Tok_ret *);
public
struct s_Tok_ret *tokenise(i8 *, i8); // Tokenises and return a array of string
public
void FINALISE(); /*  WORK IN PROGRESS  TODO:*/
public
i8 *ascii2hex(i8); // Converts a byte to hex
public
i8 hex2ascii(i8 *); // Converts a hex string into a singular ascii byte
/* Function Signatures */
public
i16 c_freq(i8 *, i8);
public
i16 s_freq(i8 *, i8 *);
public
i8 _memcomp(i8 *, i8 *, i16);
public
i8 *touppr(i8 *);
public
i8 *tolwr(i8 *);
public
i8 *toupprn(i8 *, i16);
public
i8 *tolwrn(i8 *, i16);
public
i8 isalphabetic(i8 *);
public
i8 isnumeric(i8 *);
public
i8 *invert_bits(i8 *, i32);
public
i8 flip_byte(i8);
