local glob = require("faconde").glob

-- Test cases are taken from sqlite3 (quota-glob.test) and from the kernel
-- (https://github.com/torvalds/linux/blob/master/lib/glob.c), with a few
-- modifications and additions.

local tests = {
   ------------------------------
   -- sqlite
   ------------------------------
   "abcdefg", "abcdefg", true,
   "abcdefG", "abcdefg", false,
   "abcdef", "abcdefg", false,
   "abcdefgh", "abcdefg", false,
   "abcdef?", "abcdefg", true,
   "abcdef?", "abcdef", false,
   "abcdef?", "abcdefgh", false,
   "abcdefg", "abcdef?", false,
   "abcdef?", "abcdef?", true,
   "*abcdef", "abcdef", true,
   "abcdef*", "abcdef", true,
   "ab*cdef", "abcdef", true,
   "ab*def", "abcdef", true,
   "a*f", "abcdef", true,
   "ab*", "abcdef", true,
   "*f", "abcdef", true,
   "*", "abcdef", true,
   "a*b", "abcdef", false,
   "a*c*f", "abcdef", true,
   "f[opqr]o", "foo", true,
   "f[opqr", "foo", false,			-- Invalid, shouldn't match.
   "f[pqr", "fo", false,			-- Invalid, shouldn't match.
   "f[pqro", "fo", false,			-- Invalid, shouldn't match.
   "f[^pqro", "fo", false,			-- Invalid, shouldn't match.
   "f[^pqr", "fr", false,			-- Invalid, shouldn't match.
   "f[opqr]o", "fso", false,
   "f[opqrs]o", "fso", true,
   "f[a]]", "f]", true,				-- Right bracket in a group.
   "f[]]", "f]", true,				-- Right bracket in a group.
   "f[[]", "f[", true,				-- Left bracket in a group.
   "f[[a]", "f[", true,				-- Left bracket in a group.
   "f[^]]", "fa", true,				-- Right bracket in a group.
   "f[^]]", "f]", false,
   "f[^opqr]o", "fso", true,
   "brichot", "bricq", false,
   "foo]", "foo]", true,			-- Right bracket interpreted as literal if not
                                 -- preceded by a left bracket.
   ------------------------------
   -- kernel
   ------------------------------
	-- /* Some basic tests */
	true, "a", "a",
	false, "a", "b",
	false, "a", "aa",
	false, "a", "",
	true, "", "",
	false, "", "a",
	-- /* Simple character class tests */
	true, "[a]", "a",
	false, "[a]", "b",
	false, "[^a]", "a",
	true, "[^a]", "b",
	true, "[ab]", "a",
	true, "[ab]", "b",
	false, "[ab]", "c",
	true, "[^ab]", "c",
	-- /* Simple wild cards */
	true, "?", "a",
	false, "?", "aa",
	false, "??", "a",
	true, "?x?", "axb",
	false, "?x?", "abx",
	false, "?x?", "xab",
	-- /* Asterisk wild cards (backtracking) */
	false, "*??", "a",
	true, "*??", "ab",
	true, "*??", "abc",
	true, "*??", "abcd",
	false, "??*", "a",
	true, "??*", "ab",
	true, "??*", "abc",
	true, "??*", "abcd",
	false, "?*?", "a",
	true, "?*?", "ab",
	true, "?*?", "abc",
	true, "?*?", "abcd",
	true, "*b", "b",
	true, "*b", "ab",
	false, "*b", "ba",
	true, "*b", "bb",
	true, "*b", "abb",
	true, "*b", "bab",
	true, "*bc", "abbc",
	true, "*bc", "bc",
	true, "*bc", "bbc",
	true, "*bc", "bcbc",
	-- /* Multiple asterisks (complex backtracking) */
	true, "*ac*", "abacadaeafag",
	true, "*ac*ae*ag*", "abacadaeafag",
	true, "*a*b*[bc]*[ef]*g*", "abacadaeafag",
	false, "*a*b*[ef]*[cd]*g*", "abacadaeafag",
	true, "*abcd*", "abcabcabcabcdefg",
	true, "*ab*cd*", "abcabcabcabcdefg",
	true, "*abcd*abcdef*", "abcabcdabcdeabcdefg",
	false, "*abcd*", "abcabcabcabcefg",
	false, "*ab*cd*", "abcabcabcabcefg",
}

for i = 1, #tests, 3 do
   local pattern, str, ret = tests[i], tests[i + 1], tests[i + 2]
   if type(pattern) == "boolean" then
      ret, pattern, str = pattern, str, ret
   end
   if glob(pattern, str) ~= ret then
      error(string.format("%s %s -> %s", pattern, str, glob(pattern, str)))
   end
end

-- Invalid start byte
assert(not glob("[fg]", "\xfff"))
assert(glob("[\xfffg]", "f"))
