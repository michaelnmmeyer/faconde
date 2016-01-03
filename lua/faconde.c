#include <lua.h>
#include <lauxlib.h>
#include "../src/api.h"
#include "../src/mem.h"
#include "../src/utf8.h"
#include "../src/macro.h"

#ifdef NDEBUG
   #define SEQ_BUF_SIZE (2 * 256)
#else
   #define SEQ_BUF_SIZE 1
#endif

static char32_t *fetch_sequences(lua_State *lua,
                                 char32_t buf[static SEQ_BUF_SIZE],
                                 int32_t *len1p, int32_t *len2p)
{
   size_t len1;
   const void *seq1 = luaL_checklstring(lua, 1, &len1);
   luaL_argcheck(lua, 1, len1 <= FC_MAX_SEQ_LEN, "sequence too long");

   size_t len2;
   const void *seq2 = luaL_checklstring(lua, 2, &len2);
   luaL_argcheck(lua, 2, len2 <= FC_MAX_SEQ_LEN, "sequence too long");

   char32_t *bufp = buf;
   if (len1 + len2 + 2 > SEQ_BUF_SIZE)
      bufp = fc_malloc((len1 + len2 + 2) * sizeof *bufp);

   *len1p = fc_utf8_decode(bufp, seq1, len1);
   *len2p = fc_utf8_decode(&bufp[*len1p + 1], seq2, len2);
   return bufp;
}

static int fc_lua_glob(lua_State *lua)
{
   int32_t len1, len2;
   char32_t buf[SEQ_BUF_SIZE];
   char32_t *bufp = fetch_sequences(lua, buf, &len1, &len2);

   lua_pushboolean(lua, fc_glob(bufp, &bufp[len1 + 1]));

   if (bufp != buf)
      fc_free(bufp);
   return 1;
}

static int fc_lua_lcsubstr_extract(lua_State *lua)
{
   int32_t len1, len2;
   char32_t buf[SEQ_BUF_SIZE];
   char32_t *bufp = fetch_sequences(lua, buf, &len1, &len2);

   const char32_t *substr;
   int32_t len = fc_lcsubstr_extract(bufp, len1, &bufp[len1 + 1], len2, &substr);

   /* "substr" points into the first sequence, so we can reuse space allocated
    * for the second sequence.
    */
   len = fc_utf8_encode((void *)&bufp[len1 + 1], substr, len);
   lua_pushlstring(lua, (void *)&bufp[len1 + 1], len);

   if (bufp != buf)
      fc_free(bufp);
   return 1;
}

#define _(T, LT)                                                               \
static int fc_dist_common_##T(lua_State *lua,                                  \
            T (*func)(const char32_t *, int32_t, const char32_t *, int32_t))   \
{                                                                              \
   int32_t len1, len2;                                                         \
   char32_t buf[SEQ_BUF_SIZE];                                                 \
   char32_t *bufp = fetch_sequences(lua, buf, &len1, &len2);                   \
                                                                               \
   lua_push##LT(lua, func(bufp, len1, &bufp[len1 + 1], len2));                 \
   if (bufp != buf)                                                            \
      fc_free(bufp);                                                           \
   return 1;                                                                   \
}
_(int, integer)
_(double, number)
#undef _

#define _(name)                                                                \
static int fc_lua_##name(lua_State *lua)                                       \
{                                                                              \
   return fc_dist_common_int(lua, fc_##name);                                  \
}
_(levenshtein)
_(damerau)
_(lcsubstr)
_(lcsubseq)
#undef _

#define MAX_LEV_DIST (lua_Integer)(FC_ARRAY_SIZE(fc_lev_bounded) - 1)

static int fc_lua_lev_bounded(lua_State *lua)
{
   lua_Integer max = luaL_optinteger(lua, 3, MAX_LEV_DIST);
   luaL_argcheck(lua, 3, max >= 0 && max <= MAX_LEV_DIST, "out of bound");
   return fc_dist_common_int(lua, fc_lev_bounded[max]);
}

#define _(name)                                                                \
static int fc_lua_##name(lua_State *lua)                                       \
{                                                                              \
   return fc_dist_common_double(lua, fc_##name);                               \
}
_(jaro)
_(nlcsubseq)
#undef _

static int norm_method(lua_State *lua, int index)
{
   static const char *const norm_methods[3] = {
      [FC_NORM_LSEQ] = "lseq",
      [FC_NORM_LALIGN] = "lalign",
   };
   return luaL_checkoption(lua, index, "lseq", norm_methods);
}

#define _(name)                                                                \
static double fc_lua_##name##_lseq(const char32_t *seq1, int len1, const char32_t *seq2, int len2) \
{                                                                              \
   return fc_##name(FC_NORM_LSEQ, seq1, len1, seq2, len2);                     \
}                                                                              \
static double fc_lua_##name##_lalign(const char32_t *seq1, int len1, const char32_t *seq2, int len2) \
{                                                                              \
   return fc_##name(FC_NORM_LALIGN, seq1, len1, seq2, len2);                   \
}                                                                              \
static int fc_lua_##name(lua_State *lua)                                       \
{                                                                              \
   switch (norm_method(lua, 3)) {                                              \
   case FC_NORM_LSEQ:                                                          \
      return fc_dist_common_double(lua, fc_lua_##name##_lseq);                 \
   case FC_NORM_LALIGN:                                                        \
      return fc_dist_common_double(lua, fc_lua_##name##_lalign);               \
   default:                                                                    \
      fc_fatal("unreachable");                                                 \
   }                                                                           \
}
_(nlevenshtein)
_(ndamerau)
#undef _

#define FC_MEMO_MT "volubile.memo"

struct fc_lua_memo {
   struct fc_memo memo;
   char32_t *seq1;
   char32_t seq2[];
};

/* memo(metric, max_seq_len[, max_dist]) */
static int fc_lua_memo_init(lua_State *lua)
{
   static const char *const metric_names[FC_METRIC_NR + 1] = {
      [FC_LEVENSHTEIN] = "levenshtein",
      [FC_DAMERAU] = "damerau",
      [FC_LCSUBSTR] = "lcsubstr",
      [FC_LCSUBSEQ] = "lcsubseq",
   };
   enum fc_metric metric = luaL_checkoption(lua, 1, NULL, metric_names);

   lua_Integer max_len = luaL_checkinteger(lua, 2);
   luaL_argcheck(lua, 2, max_len >= 0 && max_len <= FC_MAX_SEQ_LEN, "out of range");

   lua_Integer max_dist = luaL_optinteger(lua, 3, FC_MAX_SEQ_LEN);
   luaL_argcheck(lua, 3, max_dist >= 0, "out of range");
   if (max_dist > FC_MAX_SEQ_LEN)
      max_dist = FC_MAX_SEQ_LEN;

   /* We don't know yet the length of the longest reference sequence, so we
    * must choose the longest possible one.
    */
   const size_t size = offsetof(struct fc_lua_memo, seq2) + sizeof(char32_t[2][max_len + 1]);
   struct fc_lua_memo *m = lua_newuserdata(lua, size);

   fc_memo_init(&m->memo, metric, max_len, max_dist);
   m->seq1 = &m->seq2[max_len + 1];

   luaL_getmetatable(lua, FC_MEMO_MT);
   lua_setmetatable(lua, -2);
   return 1;
}

static int32_t fetch_memo_sequence(lua_State *lua, char32_t *seq, size_t max_len)
{
   size_t len;
   const void *str = luaL_checklstring(lua, 2, &len);
   luaL_argcheck(lua, 2, len <= max_len, "sequence too long");

   /* The decoded string could be smaller than FC_MAX_SEQ_LEN even if its
    * encoded version is larger, but we don't bother about this.
    */
   return fc_utf8_decode(seq, str, len);
}

/* We make this a separate method because we need to check that changing the
 * reference sequence doesn't break anything in C.
 */
static int fc_lua_memo_set_ref(lua_State *lua)
{
   struct fc_lua_memo *m = luaL_checkudata(lua, 1, FC_MEMO_MT);
   int32_t len = fetch_memo_sequence(lua, m->seq1, m->memo.mdim - 1);
   fc_memo_set_ref(&m->memo, m->seq1, len);
   return 0;
}

static int fc_lua_memo_compute(lua_State *lua)
{
   struct fc_lua_memo *m = luaL_checkudata(lua, 1, FC_MEMO_MT);
   if (!m->memo.seq1)
      return luaL_error(lua, "reference sequence not set");
   int32_t len = fetch_memo_sequence(lua, m->seq2, m->memo.mdim - 1);
   lua_pushinteger(lua, fc_memo_compute(&m->memo, m->seq2, len));
   return 1;
}

static int fc_lua_memo_fini(lua_State *lua)
{
   struct fc_lua_memo *m = luaL_checkudata(lua, 1, FC_MEMO_MT);
   fc_memo_fini(&m->memo);
   return 0;
}

int luaopen_faconde(lua_State *lua)
{
   const luaL_Reg memo_methods[] = {
      {"set_ref", fc_lua_memo_set_ref},
      {"compute", fc_lua_memo_compute},
      {"__gc", fc_lua_memo_fini},
      {NULL, NULL},
   };
   luaL_newmetatable(lua, FC_MEMO_MT);
   lua_pushvalue(lua, -1);
   lua_setfield(lua, -2, "__index");
   luaL_setfuncs(lua, memo_methods, 0);

   const luaL_Reg lib[] = {
      {"memo", fc_lua_memo_init},
   #define _(name) {#name, fc_lua_##name},
      _(glob)
      _(levenshtein)
      _(lev_bounded)
      _(damerau)
      _(lcsubstr)
      _(lcsubseq)
      _(jaro)
      _(nlcsubseq)
      _(nlevenshtein)
      _(ndamerau)
      _(lcsubstr_extract)
   #undef _
      {NULL, NULL},
   };
   luaL_newlib(lua, lib);

   lua_pushinteger(lua, FC_MAX_SEQ_LEN);
   lua_setfield(lua, -2, "MAX_SEQ_LEN");

   return 1;
}
