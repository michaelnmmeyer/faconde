#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "../src/api.h"
#include "../src/mem.h"
#include "../src/utf8.h"

struct token {
   size_t len;
   unsigned char str[];
};

struct token *token_new(const char *str, size_t len)
{
   struct token *t = fc_malloc(offsetof(struct token, str) + len);
   t->len = len;
   memcpy(t->str, str, len);
   return t;
}

#define FC_GROW(data, size, alloc) do {                                        \
   size_t need = size + 1;                                                     \
   if (need > alloc) {                                                         \
      if (alloc) {                                                             \
         alloc += alloc >> 1;                                                  \
         if (alloc < need)                                                     \
            alloc = need;                                                      \
         data = realloc(data, alloc * sizeof *data);                           \
      } else {                                                                 \
         alloc = need < 16 ? 16 : need;                                        \
         data = malloc(alloc * sizeof *data);                                  \
      }                                                                        \
   }                                                                           \
} while (0)

#define MAX_LINE 2048

struct token **read_tokens(FILE *fp, size_t *nrp)
{
   struct token **ts = NULL;
   size_t nr = 0, alloc = 0;

   char line[MAX_LINE];
   while (fgets(line, sizeof line, fp)) {
      size_t len = strlen(line);
      if (len > 1) {
         FC_GROW(ts, nr, alloc);
         ts[nr++] = token_new(line, len - 1);
      }
   }

   FC_GROW(ts, nr, alloc);
   ts[nr] = NULL;

   *nrp = nr;
   return ts;
}


struct perf_case {
   const char *name;
   enum fc_metric metric;

   int (*vanilla)(const char32_t *, int, const char32_t *, int);
   int (*memoized)(struct fc_memo *, const char32_t *, int);
   int max_dist;

   double vanilla_time;
   double memoized_time;
};

static struct perf_case perf_cases[] = {
   {"levenshtein",            FC_LEVENSHTEIN, fc_levenshtein, fc_memo_levenshtein, INT32_MAX},
   {"damerau",                FC_DAMERAU,     fc_damerau,     fc_memo_damerau,     INT32_MAX},
   {"lcsubstr",               FC_LCSUBSTR,    fc_lcsubstr,    fc_memo_lcsubstr,    INT32_MAX},
   {"lcsubseq",               FC_LCSUBSEQ,    fc_lcsubseq,    fc_memo_lcsubseq,    INT32_MAX},
   {"levenshtein_max_dist=1", FC_LEVENSHTEIN, fc_levenshtein, fc_memo_levenshtein, 1        },
   {"levenshtein_max_dist=2", FC_LEVENSHTEIN, fc_levenshtein, fc_memo_levenshtein, 2        },
   {"damerau_max_dist=1",     FC_DAMERAU,     fc_damerau,     fc_memo_damerau,     1        },
   {"damerau_max_dist=2",     FC_DAMERAU,     fc_damerau,     fc_memo_damerau,     2        },
   {NULL,                     0,              0,              0,                   0        },
};

#define ROUNDS_NR 1
#define TOKENS_NR 300

static void run_vanilla(int (*f)(const char32_t *, int, const char32_t *, int),
                        struct token **ts, const char32_t *seq1, int len1)
{
   for (size_t n = 0; n < ROUNDS_NR; n++) {
      for (size_t i = 0; ts[i]; i++) {
         char32_t seq2[MAX_LINE];
         int len2 = fc_utf8_decode(seq2, ts[i]->str, ts[i]->len);
         f(seq1, len1, seq2, len2);
      }
   }
}

static void run_memoized(int (*f)(struct fc_memo *, const char32_t *, int),
                         struct token **ts, struct fc_memo *m)
{
   for (size_t n = 0; n < ROUNDS_NR; n++) {
      for (size_t i = 0; ts[i]; i++) {
         char32_t seq2[MAX_LINE];
         int len2 = fc_utf8_decode(seq2, ts[i]->str, ts[i]->len);
         f(m, seq2, len2);
      }
   }
}

static void run_perf_case(struct perf_case *pc, struct token **ts,
                          const char32_t *seq1, int len1)
{
   struct fc_memo m;
   fc_memo_init(&m, pc->metric, MAX_LINE, pc->max_dist);
   fc_memo_set_ref(&m, seq1, len1);

   clock_t s, e;

   s = clock();
   run_vanilla(pc->vanilla, ts, seq1, len1);
   e = clock();
   pc->vanilla_time += e - s;

   s = clock();
   run_memoized(pc->memoized, ts, &m);
   e = clock();
   pc->memoized_time += e - s;

   fc_memo_fini(&m);
}

int main(void)
{
   size_t nr;
   struct token **ts = read_tokens(stdin, &nr);

   srand(time(NULL));

   for (size_t n = 0; n < TOKENS_NR; n++) {
      size_t t = rand() % nr;
      char32_t seq1[MAX_LINE];
      int len1 = fc_utf8_decode(seq1, ts[t]->str, ts[t]->len);
      for (struct perf_case *pc = perf_cases; pc->name; pc++)
         run_perf_case(pc, ts, seq1, len1);
   }

   for (struct perf_case *pc = perf_cases; pc->name; pc++)
      printf("%22s %.02f\n", pc->name, pc->vanilla_time / pc->memoized_time);
}
