#' Initialise the permutation object
#' 
#' This function initialises the permutation object. It must be called before
#' \code{nextPerm} can be called
#' 
#' 
#' @param x a vector of integers, reals, logicals or characters, with at least
#' two elements and no missing values
#' @return a object of class \code{mc} which is a list containing elements
#' \item{mode}{ - the mode of the original data in \code{x}, "integer",
#' "double", or \code{mode(x)}}
#' \item{set}{ - a multiset of integer codes \code{1, ..., k}, one code for
#' each unique element of \code{x}}
#' \item{elements}{ - the unique elements of \code{x}, so that
#' \code{elements[set]} recovers the multiset being permuted}
#' \item{length}{ - the length of the multiset }
#' \item{mc}{ - a pointer to the internal C++ Multicool object. Users
#' should not use this unless they really know what they are doing}
#'
#' @author James M. Curran
#' @seealso nextPerm
#' @examples
#' 
#' x = c(1,1,2,2)
#' m1 = initMC(x)
#' m1
#' 
#' ## a non-integer example
#' 
#' x = rep(letters[1:4],c(2,1,2,2))
#' m2 = initMC(x)
#' m2
#'
#' ## a real-valued example
#'
#' m3 = initMC(c(1.5, 1.5, 2.5))
#' allPerm(m3)
#'
#' @export initMC
initMC = function(x){
  if(length(x) < 2)
    stop("x must have at least two elements")
  
  if(anyNA(x))
    stop("x must not contain missing values")
  
  mode = mode(x)
  
  if(is.numeric(x)){
    ## Test for whole number, with tolerance for representation
    ## From post by Tony Plate<tplate_at_acm.org>
    tolerance = .Machine$double.eps^0.5
    if(all(abs(x - round(x)) < tolerance)){
      x = round(x)
      mode = "integer"
    }else{
      mode = "double"
    }
  }
  
  ## code each unique element as 1, ..., k so that elements[set] recovers x
  ## (sorted). Using unique() rather than names(table(x)) keeps the type of x.
  elements = sort(unique(x))
  set = sort(match(x, elements))
  
  r = new(Multicool, set)
  mcObj = list(mode = mode, set = r$set(), elements = elements, 
               length = r$length(), mc = r)
  class(mcObj) = "mc"
  
  return(mcObj)
}
