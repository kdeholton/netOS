#include "stdint.h"

void *
memmove(void *dst, const void *src, size_t n)
{
  const char *s;
  char *d;

  s = (const char*)src;
  d = (char*)dst;
  if (s < d && s + n > d) {
    s += n;
    d += n;
    if ((int64_t)s%4 == 0 && (int64_t)d%4 == 0 && n%4 == 0)
      asm volatile("std; rep movsl\n"
        :: "D" (d-4), "S" (s-4), "c" (n/4) : "cc", "memory");
    else
      asm volatile("std; rep movsb\n"
        :: "D" (d-1), "S" (s-1), "c" (n) : "cc", "memory");
    // Some versions of GCC rely on DF being clear
    asm volatile("cld" ::: "cc");
  } else {
    if ((int64_t)s%4 == 0 && (int64_t)d%4 == 0 && n%4 == 0)
      asm volatile("cld; rep movsl\n"
        :: "D" (d), "S" (s), "c" (n/4) : "cc", "memory");
    else
      asm volatile("cld; rep movsb\n"
        :: "D" (d), "S" (s), "c" (n) : "cc", "memory");
  }
  return dst;
}

