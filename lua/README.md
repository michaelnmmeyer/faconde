# faconde-lua

Lua binding of faconde

## Building

Check the value of the variable `LUA_VERSION` in the makefile in this directory.
Then invoke the usual:

    $ make && sudo make install

You can also pass in the correct version number on the command-line:

    $ make LUA_VERSION=5.3 && sudo make install LUA_VERSION=5.3

## Details

All functions expect valid UTF-8 strings as input. Other than that, the API
closely follows the C one. See the library header for full explanations.

Main functions:

    faconde.levenshtein(str1, str2)
    faconde.damerau(str1, str2)
    faconde.lcsubstr(str1, str2)
    faconde.lcsubseq(str1, str2)

Normalized metrics:

    faconde.jaro(str1, str2)
    faconde.nlcsubseq(str1, str2)
    faconde.nlevenshtein(str1, str2[, normalization_method])
    faconde.ndamerau(str1, str2[, normalization_method])
       `normalization_method` must be one of "lseq" and "lalign". Default is
       "lseq".

Memoized algorithms:

    faconde.memo(metric, max_str_len[, max_dist])
       `metric` must be one of "levenshtein", "damerau", "lcsubstr", or
       "lcsubseq". Returns a memoization handle.
    memo:set_ref(str)
    memo:compute(str)

Other functions:

    faconde.lev_bounded(str1, str2[, max_dist])
       `max_dist` must be an integer between 0 and 2 inclusive. Default is 2.
    faconde.lcsubstr_extract(str1, str2)
    faconde.glob(pattern, str)
