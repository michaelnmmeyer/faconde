#ifndef FC_MEM_H
#define FC_MEM_H

#include <stdlib.h>
#include <stdarg.h>
#include <stdnoreturn.h>

noreturn void fc_fatal(const char *msg, ...);

void *fc_malloc(size_t size)
#ifdef ___GNUC__
   __attribute__((malloc))
#endif
   ;

#define fc_free free

#endif
