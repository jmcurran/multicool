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

All C++ is reached through Rcpp attributes (`// [[Rcpp::export(rng = false)]]`). There is no Rcpp Module any more.

1. **Permutations (`src/multicool.cpp`)**: the `Multicool` class holds the cool-lex state as a linked list. The nodes live in a `std::vector` that is never resized, and the list uses raw pointers into it, so copying is deleted. Keep it that way. `advance()` is Williams' step, pointer moves only. `step()` returns the initial permutation on its first call and calls `advance()` after that. The list is read with `head()`.
   - `initMC()` (`R/initMC.r`) maps any input vector (numeric, character, logical, factor, ...) to integer codes `1..k` via `elements = sort(unique(x))` and `match()`. It returns an S3 list of class `mc` (`mode`, `set`, `elements`, `length`, `plain`, `mc`). `mc` is an `XPtr<Multicool>` from `mcCreate()`; it is NULL after save/reload, and `getMC()` gives a clear error for that. `plain` records whether `elements` is a plain integer/double/logical/character vector (`isPlainElements()` in `R/elements.R`).
   - `fillRows<RTYPE>()` writes up to `n` permutations from the list straight into an uninitialised matrix of the elements' type, shrinking it at the end. `fillDispatch()` picks the type; non-plain elements get the codes, which R maps with `mapCodes()`.
   - `nextPerm(mcObj, n)` calls `mcNextPerm(mcObj, n)`, which takes the whole `mc` list and checks its arguments in C++, because R-side checks cost more than the permutation. Keep the R wrapper minimal: a single call is about 0.9 µs. With `n = 1` it returns a vector, and it returns `NULL` (so `FALSE` in R) when nothing is left.
   - `allPerm()` calls `mcAllPerm()` with the exact row count (the multinomial coefficient, computed in R). It iterates over a fresh `Multicool`, so it doesn't disturb `nextPerm`.
   - With `nThreads > 1` and at least 100,000 rows, `fillParallel<RTYPE>()` makes a serial pointer-only pass recording a `Checkpoint` (list order + position of `i`) at the start of each block, then `std::thread`s rebuild a `Multicool` from each checkpoint and fill their rows through raw pointers. Worker threads must make no R API calls, so STRSXP output is always serial, and every per-block object is built before any thread starts. `src/Makevars` adds `-pthread`. The gain is capped by memory: just allocating and touching the result costs about as much as the 4-thread fill.
   - Benchmark changes against RcppAlgos and arrangements, using the permutation cases in https://stackoverflow.com/questions/22569176.
2. **Partitions (`src/compositions.cpp`)**: `generateCompositions` is a port of Jerome Kelleher's Python `accel_asc`. The original is kept, unchanged, in `inst/python/accel_asc.py` with a header recording where it came from, because his site is gone. Cite that file and the arXiv DOI rather than adding web links: dead URLs cause CRAN NOTEs. `generateCompositions` is wrapped by `genComp`. In C++, collect results in `std::vector` and `wrap()` them once at the end: `Rcpp::List::push_back` copies the whole list on every call.

`Stirling2`/`S2`, `Bell`/`B` and `multinom` are pure R and use `gmp`.

## Conventions and gotchas

- `NAMESPACE`, `man/*.Rd`, `R/RcppExports.R` and `src/RcppExports.cpp` are generated. Edit the roxygen comments or the C++ source, then regenerate them; don't hand-edit these files. Package-level directives (`@useDynLib multicool, .registration = TRUE`, `@import Rcpp methods`) live in `R/multicool-package.R`. Native routines are registered, so R code should call the generated wrappers in `R/RcppExports.R`, not `.Call` with a string name.
- Large counts are computed exactly in R with `gmp` big integers (`bigz`): `Stirling2`/`Bell` via the shared helper `stirling2Rows()`, and `multinom` via `factorialZ`. They return doubles by default (`as.numeric()` of the exact result) and `bigz` only with `exact = TRUE`. `multinom(useDouble =)` is kept but has no effect. Keep the default return type numeric: nine CRAN packages import multicool, and alphabetr, SIDES and splitSelect break if these return `bigz` (assigned into numeric matrices, mixed with doubles). Before a CRAN release, check the reverse dependencies with `tools::package_dependencies("multicool", reverse = TRUE, db = available.packages())`. Internal callers that need the exact count, such as `allPerm`, pass `exact = TRUE`. Dave Barber's C++ multinomial code (`multinomial.h`) was removed in 1.0.1.002. It was very slow for large sums, and it is acknowledged in `NEWS.md`. Don't move these counts into C++ integer types: R can't receive values above 2^53 without losing precision, and `long` is only 32 bits on Windows.
- The package is published on CRAN, so changes should pass `R CMD check --as-cran` with no NOTEs. Recent commits were about clearing CRAN NOTEs. Any new top-level file that isn't part of the package (like this one) must be added to `.Rbuildignore`.
- The repo lives in Dropbox, which sometimes creates `* (... conflicted copy ...).*` files. Delete them. Their names contain spaces, which break `make`. `.Rbuildignore` keeps them out of `R CMD build`, but in-place builds (`devtools::document()`, `load_all()`) fail while they exist.
- Compiled objects (`src/*.o`, `*.so`, `*.dll`) are gitignored build artifacts.
