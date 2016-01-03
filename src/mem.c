#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "mem.h"

noreturn void fc_fatal(const char *msg, ...)
{
   va_list ap;

   fputs("faconde: ", stderr);
   va_start(ap, msg);
   vfprintf(stderr, msg, ap);
   va_end(ap);
   putc('\n', stderr);
   fflush(stderr);

   abort();
}

void *fc_malloc(size_t size)
{
   assert(size);
   void *mem = malloc(size);
   if (!mem)
      fc_fatal("out of memory");
   return mem;
}
