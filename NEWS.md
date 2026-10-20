# multicool 1.0.1.005

## New features

* `allPerm()` has a new argument `nThreads` to generate large results (at
  least 100,000 permutations) with several threads. It is about 1.4 to 1.9
  times faster with 2 to 4 threads, and close to the speed at which the
  result can be written to memory. Character and factor multisets always use
  one thread. The default `nThreads = 1` works as before.
* `nextPerm()` has a new argument `n` to return the next `n` permutations as
  the rows of a matrix (fewer at the end, then `FALSE`). This makes iterating
  through permutations much faster, while only holding `n` of them in
  memory. For example, `nextPerm(m, 1000)` costs about 0.016 microseconds per
  permutation, the same as `allPerm()`. The default `n = 1` works as before.

## Breaking changes

* `multinom()`, `Stirling2()`/`S2()` and `Bell()`/`B()` now return exact big
  integers of class `bigz` from the 'gmp' package, instead of doubles that
  silently overflowed or lost precision. Use `as.numeric()` to convert them.
  `multinom(useDouble = TRUE)` still returns a double.
* `initMC()` now gives an error, rather than a warning, for input with fewer
  than two elements, and an error for input containing missing values.
* `initMC()` keeps the type of its input, so logical multisets are permuted as
  logicals rather than as the strings `"TRUE"` and `"FALSE"`.

## Bug fixes

* `initMC()` with non-whole numbers (e.g. `c(1.5, 1.5, 2.5)`) permuted the
  counts of each value rather than the values themselves.
* `Stirling2()` and `Bell()` overflowed for moderate arguments, and much
  sooner on Windows (e.g. `Stirling2(20, 5)` and `Bell(16)` were wrong).
* `multinom()` overflowed silently for large coefficients.
* `genComp()` crashed R when `n` was zero or negative, and returned `NULL`
  when `len` was greater than `n`.
* Calling `allPerm()` no longer resets the position of `nextPerm()`.
* Fixed a memory leak in the permutation object.

## Improvements

* `genComp()` is much faster for larger `n`. The time taken previously grew
  quadratically with the number of partitions (about 100 seconds for
  `n = 50`).
* `allPerm()` is 10 to 30 times faster and uses much less memory. It writes
  the elements straight into the result matrix in C++. In the permutation
  benchmarks from <https://stackoverflow.com/questions/22569176>, it now
  takes 1.4 to 1.9 times as long as single-threaded 'RcppAlgos' (before,
  15 to 54 times as long), and 1.0 to 1.5 times as long as 'arrangements'.
* `initMC()` keeps integer input as integer, so `allPerm()` returns an
  integer matrix for integer input.
* `nextPerm()` is about 9 times faster per call (about 0.9 rather than 8.4
  microseconds), because the permutation object is now reached through an
  external pointer rather than an Rcpp Module.
* `nextPerm()` checks that its argument is an `mc` object.
* Using an `mc` object after it has been saved and reloaded gives a clear
  error asking you to call `initMC()` again.
* Native routines are now registered, and documentation and `NAMESPACE` are
  generated with roxygen2.

## Acknowledgements

* Earlier versions of this package used C++ code by Dave Barber
  (<http://tamivox.org/dave/multinomial/code.html>) to compute multinomial
  coefficients. It has been replaced by exact computation with the 'gmp'
  package, but we thank Dave for the code that the package relied on for
  many years.
