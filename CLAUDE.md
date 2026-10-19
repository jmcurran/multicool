# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

`multicool` is a CRAN R package (Rcpp-based) for generating permutations of multisets in cool-lex order (Aaron Williams' loopless algorithm), plus integer partitions (Jerome Kelleher's algorithm), exact multinomial coefficients, and Stirling numbers of the second kind / Bell numbers. There is no test suite; the roxygen `@examples` are the only executable checks, run by `R CMD check`. User-facing changes are recorded in `NEWS.md`.

## Commands

Run from the repository root (in R, or via `Rscript -e`):

```r
Rcpp::compileAttributes()   # regenerate R/RcppExports.R and src/RcppExports.cpp after changing any // [[Rcpp::export]] function
devtools::document()        # regenerate man/*.Rd and NAMESPACE from roxygen comments
devtools::load_all()        # compile and load for interactive testing
devtools::check()           # full R CMD check (what CRAN runs)
devtools::run_examples()    # run only the roxygen examples
```

Or from a shell: `R CMD build .` then `R CMD check --as-cran multicool_<version>.tar.gz`.

## Architecture

There are two separate ways R reaches C++:

1. **Rcpp Module (`Multicool` class)**: `src/multicool.cpp` defines a C++ class that holds the permutation state as a linked list, exposed via `RCPP_MODULE(Multicool)`. `R/zzz.R` loads it with `loadModule("Multicool", TRUE)`, and `DESCRIPTION` declares `RcppModules: Multicool`. R code builds instances with `new(Multicool, set)`, where `set` must be sorted in non-decreasing order. The list nodes live in a `std::vector` that is never resized, and the list uses raw pointers into it, so copying is deleted. Keep it that way.
   - `initMC()` (`R/initMC.r`) maps any input vector (numeric, character, logical, factor) to integer codes `1..k` via `elements = sort(unique(x))` and `match()`, builds the C++ object, and returns an S3 list of class `mc` (`mode`, `set`, `elements`, `length`, `mc` = the C++ object).
   - `nextPerm()` and `allPerm()` call module methods and map integer codes back through `mcObj$elements`. `nextPerm` changes state on the C++ object, so successive calls on the same `mc` object move through the permutations. The C++ `nextPerm` must call `hasNext()` *before* reading the state; otherwise its order differs from `allPerm` (see commit 2ac306c). `allPerm` iterates over a fresh copy built from the initial state, so it doesn't disturb `nextPerm`. It is given the row count (the multinomial coefficient, computed in R) and the elements. For plain integer, double, logical and character elements, the templated `fillPerms<RTYPE>()` walks the list straight into an uninitialised matrix of that type. Otherwise (classed vectors, complex) R passes `NULL`, gets the codes back, and maps them itself. `advance()` moves the list on without copying state; `hasNext()` is `advance()` plus `setState()`. Benchmark changes against RcppAlgos and arrangements, using the permutation cases in https://stackoverflow.com/questions/22569176.
2. **Rcpp attributes**: `generateCompositions` (in `src/compositions.cpp`) is a plain `// [[Rcpp::export]]` function. It is internal (not exported in `NAMESPACE`) and is wrapped by `genComp`. In C++, collect results in `std::vector` and `wrap()` them once at the end: `Rcpp::List::push_back` copies the whole list on every call.

`Stirling2`/`S2`, `Bell`/`B` and `multinom` are pure R and use `gmp`.

## Conventions and gotchas

- `NAMESPACE`, `man/*.Rd`, `R/RcppExports.R` and `src/RcppExports.cpp` are generated. Edit the roxygen comments or the C++ source, then regenerate them; don't hand-edit these files. Package-level directives (`@useDynLib multicool, .registration = TRUE`, `@import Rcpp methods`) live in `R/multicool-package.R`. Native routines are registered, so R code should call the generated wrappers in `R/RcppExports.R`, not `.Call` with a string name.
- Large counts are computed exactly in R with `gmp` big integers (`bigz`): `Stirling2`/`Bell` via the shared helper `stirling2Rows()`, and `multinom` via `factorialZ`. `multinom(useDouble = TRUE)` just converts the exact result with `as.numeric()`. Dave Barber's C++ multinomial code (`multinomial.h`) was removed in 1.0.1.002. It was very slow for large sums, and it is acknowledged in `NEWS.md`. Don't move these counts into C++ integer types: R can't receive values above 2^53 without losing precision, and `long` is only 32 bits on Windows.
- The package is published on CRAN, so changes should pass `R CMD check --as-cran` with no NOTEs. Recent commits were about clearing CRAN NOTEs. Any new top-level file that isn't part of the package (like this one) must be added to `.Rbuildignore`.
- The repo lives in Dropbox, which sometimes creates `* (... conflicted copy ...).*` files. Delete them. Their names contain spaces, which break `make`. `.Rbuildignore` keeps them out of `R CMD build`, but in-place builds (`devtools::document()`, `load_all()`) fail while they exist.
- Compiled objects (`src/*.o`, `*.so`, `*.dll`) are gitignored build artifacts.
