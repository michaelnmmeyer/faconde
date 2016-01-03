# faconde

Efficient fuzzy matching

## Purpose

This is a selection of approximate string matching algorithms. The following are
available:

* Levenshtein distance
* Damerau-Levenshtein distance
* Longest common substring
* Longest common subsequence
* Jaro-Winkler distance

We use an original method based on memoization to speed up the algorithms.


## Building

The library is available in source form, as an amalgamation. Compile `faconde.c`
together with your source code, and use the interface described in `faconde.h`.
A C11 compiler is required for compilation, which means either GCC or CLang on
Unix. Then, typically:

    $ cc -std=c11 -c faconde.c -o faconde.o

## Details

### Standard algorithms

Standard algorithms are implemented to use as less space as possible (a few rows
at worst instead of a full matrix). The following are available:

    levenshtein    Levenshtein distance
    damerau        Damerau-Levenshtein distance
    lcsubseq       Longest common subsequence
    lcsubstr       Longest common substring
    jaro           Jaro-Winkler distance

Normalized versions of `levenshtein`, `damerau`, and `lcsubseq`, are
available. These functions return a float between 0 and 1, where 0 stands for
equality.

### Levenshtein optimizations

Two additional functions `lev_bounded1()` and `lev_bounded2()` are available for
computing the Levenshtein distance between two strings upto a maximum predefined
value (1 or 2). They are much faster than the standard algorithms, and don't
allocate memory at all.

### Memoized algorithms

A common use case of approximate string matching algorithms is searching a
lexicon for words resembling some chosen input word. In this scenario, a single
word is compared against several words in turn. Provided the lexicon is sorted,
a significant speedup can be obtained through the use of memoization.

The main idea is described in [Hanov, Fast and Easy Levenshtein distance using a
Trie](http://stevehanov.ca/blog/index.php?id=114). Our implementation doesn't
require any specific data structures. It also works for the longest common
substring and longest common subsequence algorithms. See the file `example.c` in
this directory for a practical usage example.

Here is a table of the obtained speedup, relative to the brute-force approach,
for each matching algorithm. We use the Unix dictionary, from which we extract
300 query words at random for searching. Notice that the Levenshtein distance
and Damerau-Levenshtein distance algorithms are made faster when a maximum edit
distance is defined:

               levenshtein 1.30
                   damerau 1.57
                  lcsubstr 1.74
                  lcsubseq 1.52
    levenshtein_max_dist=1 4.85
    levenshtein_max_dist=2 3.72
        damerau_max_dist=1 6.41
        damerau_max_dist=2 4.80

Run `perf.sh` in the `test` directory to reproduce.
