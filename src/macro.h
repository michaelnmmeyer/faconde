#ifndef FC_MACRO_H
#define FC_MACRO_H

#define FC_ARRAY_SIZE(a) (sizeof(a) / sizeof (a)[0])

#define FC_MIN(a, b) ((a) < (b) ? (a) : (b))
#define FC_MIN3(a, b, c) FC_MIN(a, FC_MIN(b, c))

#define FC_MAX(a, b) ((a) > (b) ? (a) : (b))
#define FC_MAX3(a, b, c) FC_MAX(a, FC_MAX(b, c))

#define FC_SWAP(T, a, b) do {                                                  \
   T tmp = a;                                                                  \
   a = b;                                                                      \
   b = tmp;                                                                    \
} while (0)

/* a, b, c = b, c, a */
#define FC_SWAP3(T, a, b, c) do {                                              \
   T tmp = a;                                                                  \
   a = b;                                                                      \
   b = c;                                                                      \
   c = tmp;                                                                    \
} while (0)

#endif
