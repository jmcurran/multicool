## Submission

This is an update of multicool (1.0.1 -> 1.0.2). It fixes several bugs, makes
the permutation functions much faster, and computes multinomial coefficients,
Stirling numbers and Bell numbers exactly with 'gmp' (now in Imports). See
NEWS.md for details.

## Test environments

* Local: Windows 11, R 4.6.1
* win-builder: R-devel
* macOS builder (mac.r-project.org): R-release
* R-hub (GitHub Actions): Ubuntu, R-devel

## R CMD check results

0 errors | 0 warnings | 1 note

* Possibly misspelled words in DESCRIPTION: O'Sullivan

  This is a surname in a reference (Kelleher and O'Sullivan, 2009).

## Reverse dependencies

We checked all 9 reverse dependencies (alphabetr, BMRMM, Claddis, DNAtools,
ks, Pv3Rs, relSim, SIDES, splitSelect) with R CMD check against this version,
and against the current CRAN version (1.0.1) as a baseline. All 9 pass with
Status: OK against both. No problems were found.

The default return types of multinom(), Stirling2() and Bell() are unchanged
(doubles), so existing code that uses them is unaffected; exact big-integer
results are only returned with the new argument exact = TRUE.
