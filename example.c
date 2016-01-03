#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "faconde.h"

#define MAX_WORD_LEN 50

static const char32_t *const lexicon[] = {
   U"expecting",
   U"expediter",
   U"expeditor",
   U"expel",
   NULL,
};

static int32_t ustrlen(const char32_t *s)
{
   int32_t len = 0;

   while (s[len])
      len++;
   return len;
}

int main(void)
{
   // Initialization. We choose the longest common substring algorithm.
   struct fc_memo m;
   fc_memo_init(&m, FC_LCSUBSTR, MAX_WORD_LEN, 0);

   // Set the word to find.
   const char32_t *to_find = U"expeditor";
   fc_memo_set_ref(&m, to_find, ustrlen(to_find));

   // Iterate over all words, in lexicographical order, and compute the length
   // of the longest common substring between our chosen word and the current
   // word at each step.
   for (size_t i = 0; lexicon[i]; i++) {
      int32_t substr_len = fc_memo_compute(&m, lexicon[i], ustrlen(lexicon[i]));
      printf("lexicon[%zu]: %"PRId32"\n", i, substr_len);
   }

   // Cleanup.
   fc_memo_fini(&m);
}
