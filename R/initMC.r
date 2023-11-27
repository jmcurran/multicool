#' Initialise the permutation object
#' 
#' This function initialises the permutation object. It must be called before
#' \code{nextPerm} can be called
#' 
#' 
#' @param x a vector of integers, reals, logicals or characters
#' @return a object of class \code{mc} which is a list containing elements
#' \itemize{
#' \item{mode}{ - the mode of the original data in \code{x}, "integer",
#' "double", or \code{mode(x)}}
#' \item{set}{ - either the multiset being permuted if \code{mode} is "integer" or
#' a set of integers corresponding to the elements of the multiset}
#' \item{elements}{ - if \code{mode} is not "integer" then this contains the
#' elements being permuted otherwise \code{NULL} }
#' \item{length} { - the length of the multiset }
#' \item{mc}{ - a pointer to the internal C++ Multicool object. Users
#' should not use this unless they really know what they are doing}
#' }
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
#' @export initMC
initMC = function(x){
  if(length(x) > 1){
    r = NULL

    if(is.numeric(x)){
      ## Test for whole number, with tolerance for representation
      ## From post by Tony Plate<tplate_at_acm.org>
      tolerance = .Machine$double.eps^0.5
      if(isTRUE(all(abs(x - round(x))<  tolerance))){ ## integer args
        tbl = table(x)
        elements = as.numeric(names(tbl))
        set = rep(1:length(tbl), tbl)
        
        r = new(Multicool, set)
        mcObj = list(mode = "integer", set = r$set(), elements = elements, 
                     length = r$length(), mc = r)
        
        class(mcObj) = "mc"
        return(mcObj)
      }else{ ## doubles
        tbl = table(x)
        elements = as.numeric(tbl)
        set = rep(1:length(tbl), tbl)
        
        r = new(Multicool, set)
        mcObj = list(mode = "double", set = r$set(), elements = elements,
                     length = r$length(), mc = r)
        class(mcObj) = "mc"
        return(mcObj)
      }
    }else{ ## logicals and characters and who know's whatelse
      tbl = table(x)
      elements = names(tbl)
      set = rep(1:length(tbl), tbl)
      
      r = new(Multicool, set)
      mcObj = list(mode = mode(x), set = r$set(), elements = elements,
                   length = r$length(), mc = r)
      class(mcObj) = "mc"
      return(mcObj)
    }
  }else{
    warning("The permutations of a vector of length 1 are not very interesting")
  }
}
