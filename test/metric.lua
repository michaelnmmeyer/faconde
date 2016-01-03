local faconde = require("faconde")

local tests = {}

function tests.levenshtein()
   local cases = {
		"", "", "0",
		"", "a", "1",
		"a", "", "1",
		"a", "a", "0",
		"", "ab", "2",
		"ab", "", "2",
		"ab", "a", "1",
		"ab", "b", "1",
		"a", "ab", "1",
		"b", "ab", "1",
		"ab", "ab", "0",
		"ab", "aab", "1",
		"ab", "b", "1",
		"ab", "ac", "1",
		"abc", "adb", "2",
		"aé", "é", "1",
	}
	for i = 1, #cases, 3 do
	   local ret = tonumber(cases[i + 2])
	   assert(faconde.levenshtein(cases[i], cases[i + 1]) == ret)
	   assert(faconde.levenshtein(cases[i + 1], cases[i]) == ret)
	end
end

function tests.nlevenshtein()
   local cases = {
		"s", "", "", "0.0",
		"s", "a", "", "1.0",
		"s", "", "a", "1.0",
		"s","ab", "ab", "0.0",
		"s", "ab", "aab", "0.33333333333333333333333333333333333333333333333333333333333",
		"s", "ab", "b", "0.5",
		"s", "ab", "ac", "0.5",
		"s", "abc", "adb", "0.6666666666666666666666666666666666666666666666666666666666",
		"c", "abc", "adb", "0.5",
	}
	local modes = {s = "lseq", c = "lalign"}

	for i = 1, #cases, 4 do
	   local mode = assert(modes[cases[i]])
	   local ret = tonumber(cases[i + 3])
	   assert(faconde.nlevenshtein(cases[i + 1], cases[i + 2], mode) == ret)
	   assert(faconde.nlevenshtein(cases[i + 2], cases[i + 1], mode) == ret)
	end
end

function tests.ndamerau()
   local cases = {
		"s", "", "", "0.0",
		"s", "a", "", "1.0",
		"s", "", "a", "1.0",
		"s", "ab", "ab", "0.0",
		"s", "ab", "aab", "0.33333333333333333333333333333333333333333333333333333333333",
		"s", "ab", "b", "0.5",
		"s", "ab", "ac", "0.5",
		"s", "abc", "adb", "0.6666666666666666666666666666666666666666666666666666666666",
		"c", "abc", "adb", "0.5",
		"s", "aé", "é", "0.5",
		"s", "aé", "éa", "0.5",
		"s", "abé", "aéb", "0.333333333333333333333333333333333333333333333333333333333333",
		"s", "abéc", "aébc", "0.25",
	}
	local modes = {s = "lseq", c = "lalign"}

	for i = 1, #cases, 4 do
	   local mode = assert(modes[cases[i]])
	   local ret = tonumber(cases[i + 3])
	   assert(faconde.ndamerau(cases[i + 1], cases[i + 2], mode) == ret)
	   assert(faconde.ndamerau(cases[i + 2], cases[i + 1], mode) == ret)
	end
end

function tests.damerau()
   local cases = {
   		"", "", "0",
		"", "a", "1",
		"a", "", "1",
		"a", "a", "0",
		"", "ab", "2",
		"ab", "", "2",
		"ab", "a", "1",
		"ab", "b", "1",
		"a", "ab", "1",
		"b", "ab", "1",
		"ab", "ab", "0",
		"ab", "aab", "1",
		"ab", "b", "1",
		"ab", "ac", "1",
		"abc", "adb", "2",
		"aé", "é", "1",
		"ab", "ba", "1",
		"dbc", "acb", "2",
		"déc", "acé", "2",
		"décb", "acé8", "3",
	}
	for i = 1, #cases, 3 do
	   local ret = tonumber(cases[i + 2])
	   assert(faconde.damerau(cases[i], cases[i + 1]) == ret)
	   assert(faconde.damerau(cases[i + 1], cases[i]) == ret)
	end
end

function tests.lcsubstr()
   local cases = {
   		"", "", "0",
		"a", "", "0",
		"a", "a", "1",
		"ab", "ac", "1",
		"ba", "ca", "1",
		"ab", "bd", "1",
		"abcd", "adbc", "2",
		"foo", "foo", "3",
		"foobar", "foo", "3",
		"barfoo", "foo", "3",
		"barfoobar", "foo", "3",
		"abab", "baba", "3",
		"repletes", "Aaron", "1",
	}
	for i = 1, #cases, 3 do
	   local ret = tonumber(cases[i + 2])
	   assert(faconde.lcsubstr(cases[i], cases[i + 1]) == ret)
	   assert(faconde.lcsubstr(cases[i + 1], cases[i]) == ret)
	end
end

function tests.lcsubseq()
   local cases = {
		"", "", "0",
		"a", "", "0",
		"a", "a", "1",
		"abc", "abc", "3",
		"abc", "adc", "2",
		"abc", "dbf", "1",
		"abc", "def", "0",
		"acab", "ab", "2",
		"acab", "aab", "3",
	}
	for i = 1, #cases, 3 do
	   local ret = tonumber(cases[i + 2])
	   assert(faconde.lcsubseq(cases[i], cases[i + 1]) == ret)
	   assert(faconde.lcsubseq(cases[i + 1], cases[i]) == ret)
	end
end

function tests.nlcsubseq()
   local cases = {
		"", "foo", "1.0",
		"foo", "", "1.0",
		"foo", "foo", "0.0",
		"foo", "frob", "0.4285714285714286",
	}
	for i = 1, #cases, 3 do
	   local ret = tonumber(cases[i + 2])
	   assert(faconde.nlcsubseq(cases[i], cases[i + 1]) == ret)
	   assert(faconde.nlcsubseq(cases[i + 1], cases[i]) == ret)
	end
end

function tests.lcsubstr_extract()
   local cases = {
      "", "", "",
      "", "a", "",
      "abc", "def", "",
      "ab", "a", "a",
      "ab", "b", "b",
      "abc", "abd", "ab",
      "abc", "bcd", "bc",
      "f", "f", "f",
      "f", "o", "",
      "foo", "oo", "oo",
      "foo", "f", "f",
      "foobar", "fzobar", "obar",
   }
   for i = 1, #cases, 3 do
      assert(faconde.lcsubstr_extract(cases[i], cases[i + 1]) == cases[i + 2], cases[i] .. ".." .. cases[i + 1])
      assert(faconde.lcsubstr_extract(cases[i + 1], cases[i]) == cases[i + 2])
   end
end

function tests.lev_bounded()
   local INF = 3333333
   local cases = {
   [0] = {
	   "", "", 0,
	   "a", "a", 0,
	   "a", "b", INF,
	   "b", "a", INF,
	 },
	 [1] = {
      "", "", 0,
      "", "a", 1,
      "a", "a", 0,
      "a", "b", 1,
      "ab", "ba", INF,
      "ab", "cd", INF,
      "abcd", "abcd", 0,
      "abcd", "abce", 1,
      "abcd", "acbd", INF,
      "abcd", "dcba", INF,
	 },
	 [2] = {
      "", "", 0,
      "", "a", 1,
      "", "ab", 2,
      "a", "a", 0,
      "a", "b", 1,
      "a", "ab", 1,
      "a", "abc", 2,
      "ab", "ba", 2,
      "ab", "cd", 2,
      "abc", "ade", 2,
      "abc", "abcde", 2,
      "abcde", "abc", 2,
      "abcd", "abcd", 0,
      "abcd", "abce", 1,
      "a", "abcd", INF,
      "abcd", "dcba", INF,
   },
	}
	for i, cases_i in ipairs(cases) do
	   for j = 1, #cases_i, 3 do
	      local ret = faconde.lev_bounded(cases_i[j], cases_i[j + 1])
	      if cases_i[j + 2] == INF then
	         assert(ret > i)
         else
            assert(ret == cases_i[j + 2])
         end
	   end
	end
end

function tests.jaro()
   -- Data is extracted from Winkler's paper:
   --    http://www.census.gov/srd/papers/pdf/rrs2006-02.pdf
   -- Something is wrong with some results, commented them out.
   -- The similarity coefficient is in the first column
   local cases = {
      "SHACKLEFORD SHACKELFORD 0.970 0.982 0.925 0.818",
      "DUNNINGHAM CUNNIGHAM 0.896 0.896 0.917 0.889",
      "NICHLESON NICHULSON 0.926 0.956 0.906 0.889",
      "JONES JOHNSON 0.790 0.832 0.000 0.667",
      "MASSEY MASSIE 0.889 0.933 0.845 0.667",
      "ABROMS ABRAMS 0.889 0.922 0.906 0.833",
      -- "HARDIN MARTINEZ 0.000 0.000 0.000 0.143",
      -- "ITMAN SMITH 0.000 0.000 0.000 0.000",
      "JERALDINE GERALDINE 0.926 0.926 0.972 0.889",
      "MARHTA MARTHA 0.944 0.961 0.845 0.667",
      "MICHELLE MICHAEL 0.869 0.921 0.845 0.625",
      "JULIES JULIUS 0.889 0.933 0.906 0.833",
      "TANYA TONYA 0.867 0.880 0.883 0.800",
      "DWAYNE DUANE 0.822 0.840 0.000 0.500",
      "SEAN SUSAN 0.783 0.805 0.800 0.400",
      "JON JOHN 0.917 0.933 0.847 0.750",
      -- "JON JAN 0.000 0.000 0.000 0.667",
   }
   for _, case in ipairs(cases) do
      local seq1, seq2, expect = case:match("([%a]+)%s+([%a]+)%s+(%d+%.%d+)")
      expect = 1 - tonumber(expect)
      local ret = faconde.jaro(seq1, seq2)
      assert(math.abs(expect - ret) < 0.001)
   end
end

local function load_words()
   local words = {}
   local longest_word = 0
   for word in io.lines("/usr/share/dict/words") do
      table.insert(words, word)
      if longest_word < #word then
         longest_word = #word
      end
   end
   return words, longest_word
end

-- Ensure we get the same results with the lev_bounded group of functions
-- and with the standard algorithm.
function tests.lev_bounded_dict()
   local words = load_words()
   local ref_word = words[math.random(#words)]
   for _, word in ipairs(words) do
      local dist0 = faconde.lev_bounded(ref_word, word, 0)
      local dist1 = faconde.lev_bounded(ref_word, word, 1)
      local dist2 = faconde.lev_bounded(ref_word, word, 2)

      local dist = faconde.levenshtein(ref_word, word)
      if dist == 0 then
         assert(dist0 == 0 and dist1 == 0 and dist2 == 0)
      elseif dist == 1 then
         assert(dist0 > 0 and dist1 == 1 and dist2 == 1)
      elseif dist == 2 then
         assert(dist0 > 0 and dist1 > 1 and dist2 == 2)
      else
         assert(dist0 > 0 and dist1 > 1 and dist2 > 2)
      end
   end
end

local metrics = {"levenshtein", "damerau", "lcsubstr", "lcsubseq"}

function tests.memo()
   local words, max_len = load_words()
   for _, name in ipairs(metrics) do
      local max_dist = math.random(max_len + 3)
      local memo = faconde.memo(name, max_len, transpos, max_dist)
      local ref_word
      for i, word in ipairs(words) do
         -- Should work fine even after changing the current sequence.
         if i % math.random(777) == 1 then
            ref_word = words[math.random(#words)]
            memo:set_ref(ref_word)
         end
      ::compute::
		   local dist = memo:compute(word)
		   -- Levenshtein or Damerau.
         if name == "levenshtein" or name == "damerau" then
            local dist2 = faconde[name](ref_word, word)
            assert(dist2 <= max_dist and dist == dist2 or dist >= dist2)
		   -- Lcsubstr, lcsubseq.
		   else
		      assert(dist == faconde[name](ref_word, word))
		   end
		   -- Should work fine when a candidate sequence is compared with the
		   -- reference sequence more than one time consecutively.
		   if math.random(33) == 3 then
		     goto compute
		   end
      end
   end
end

-- Checks that no overflow happens when allocating a matrix large enough to cope
-- with two sequences of length FC_MAX_SEQ_LEN. This must be run under Valgrind
-- to be useful at all.
function tests.limits()
   local max_len = faconde.MAX_SEQ_LEN
   local dummy = string.rep("$", max_len)
   -- Standard.
   for _, func in ipairs{"levenshtein", "nlevenshtein", "damerau", "ndamerau",
                         "lcsubstr", "lcsubstr_extract",
                         "lcsubseq", "nlcsubseq", "jaro"} do
      faconde[func](dummy, dummy)
   end
   -- Memoized.
   for _, name in ipairs(metrics) do
      local memo = faconde.memo(name, max_len)
      memo:set_ref(dummy)
      memo:compute(dummy)
   end
end

local seed = arg[1] and tonumber(arg[1]) or os.time()
print(arg[0], seed)

for name, fn in pairs(tests) do
   print(name)
   fn()
end
