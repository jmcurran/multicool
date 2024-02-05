#' @useDynLib multicool
#' @import Rcpp
#' @import methods

# .onLoad <- function(libname, pkgname) {
#   loadRcppModules()
# }

loadModule("Multicool", TRUE)
