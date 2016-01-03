#include "api.h"

/* Could be refactored to remove recursion altogether. */
bool fc_glob(const char32_t *pat, const char32_t *str)
{
   for (;;) {
      switch (*pat) {
      case U'\0':
         return !*str;
         break;
      case U'?':
         if (!*str++)
            return false;
         pat++;
         break;
      case U'*':
         /* Could skip following trailing stars, don't think it's that useful.
          */
         if (!*++pat)
            return true;
         do {
            if (fc_glob(pat, str))
               return true;
         } while (*str++);
         return false;
         break;
      case U'[': {
         bool invert = false;
         pat++;
         if (*pat == U'^') {
            invert = true;
            pat++;
         }
         while ((*pat == *str) == invert) {
            if (!*pat || *pat == U']')
               return false;
            pat++;
         }
         do {
            if (!*pat++)
               return false;
         } while (*pat != U']');
         pat++;
         str++;
         break;
      }
      default:
         if (*str++ != *pat++)
            return false;
         break;
      }
   }
   return false;
}
