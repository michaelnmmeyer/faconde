#ifndef FC_UTF8_H
#define FC_UTF8_H

#include <stddef.h>
#include <uchar.h>

static inline char32_t fc_utf8_decode_char(const unsigned char *str, size_t clen)
{
   switch (clen) {
   case 1:
      return str[0];
   case 2:
      return ((str[0] & 0x1F) << 6) | (str[1] & 0x3F);
   case 3:
      return ((str[0] & 0x0f) << 12) | ((str[1] & 0x3f) << 6) | (str[2] & 0x3f);
   default:
      return ((str[0] & 0x07) << 18) | ((str[1] & 0x3f) << 12) |
             ((str[2] & 0x3f) << 6)  | (str[3] & 0x3f);
   }
}

static inline size_t fc_utf8_decode(char32_t *restrict dest,
                                    const unsigned char *restrict str,
                                    size_t len)
{
   static const unsigned char table[256] = {
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
      3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
      4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0,
   };

   size_t ulen = 0;

   for (size_t i = 0; i < len; ) {
      size_t clen = table[str[i]];
      if (clen == 0 || i + clen > len) {
         clen = 1;
         dest[ulen++] = U'�';
      } else {
         dest[ulen++] = fc_utf8_decode_char(&str[i], clen);
      }
      i += clen;
   }

   dest[ulen] = U'\0';
   return ulen;
}

static inline size_t fc_utf8_encode_char(unsigned char *dest, char32_t c)
{
   if (c < 0x80) {
      *dest = c;
      return 1;
   }
   if (c < 0x800) {
      dest[0] = 0xc0 | ((c & 0x07c0) >> 6);
      dest[1] = 0x80 | (c & 0x003f);
      return 2;
   }
   if (c < 0x10000) {
      dest[0] = 0xe0 | ((c & 0xf000) >> 12);
      dest[1] = 0x80 | ((c & 0x0fc0) >>  6);
      dest[2] = 0x80 | (c & 0x003f);
      return 3;
   }
   dest[0] = 0xf0 | ((c & 0x1c0000) >> 18);
   dest[1] = 0x80 | ((c & 0x03f000) >> 12);
   dest[2] = 0x80 | ((c & 0x000fc0) >>  6);
   dest[3] = 0x80 | (c & 0x00003f);
   return 4;
}

static inline size_t fc_utf8_encode(unsigned char *restrict dest,
                                    const char32_t *restrict str, size_t ulen)
{
   size_t len = 0;

   for (size_t i = 0; i < ulen; i++)
      len += fc_utf8_encode_char(&dest[len], str[i]);

   dest[len] = '\0';
   return len;
}

#endif
