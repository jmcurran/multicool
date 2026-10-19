# multicool 1.0.1.002

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
* `allPerm()` builds its result matrix directly, using less memory.
* `nextPerm()` checks that its argument is an `mc` object.
* Native routines are now registered, and documentation and `NAMESPACE` are
  generated with roxygen2.

## Acknowledgements

* Earlier versions of this package used C++ code by Dave Barber
  (<http://tamivox.org/dave/multinomial/code.html>) to compute multinomial
  coefficients. It has been replaced by exact computation with the 'gmp'
  package, but we thank Dave for the code that the package relied on for
  many years.
