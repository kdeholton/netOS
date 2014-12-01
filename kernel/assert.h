/* See COPYRIGHT for copyright information. */

#ifndef JOS_INC_ASSERT_H
#define JOS_INC_ASSERT_H

#include "debug.h"

void _panic(const char*, int, const char*, ...) __attribute__((noreturn));


#define assert(x)   \
  do { if (!(x)) Debug::panic("assertion failed: %s", #x); } while (0)

// static_assert(x) will generate a compile-time error if 'x' is false.
#define static_assert(x)  switch (x) case 0: case (x):

#endif /* !JOS_INC_ASSERT_H */
