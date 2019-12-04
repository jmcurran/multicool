#' Return the next permutation of the multiset
#' 
#' This function returns the next permuation of the multiset if there is one.
#' \code{initMC} called before \code{nextPerm} can be called.
#' 
#' 
#' @param mcObj an S3 object of class \code{mc} which must be created with
#' \code{initMC}
#' @return either a vector with the next permutation of the multiset or
#' \code{FALSE} when all permutations have been returned
#' @author James M. Curran
#' @seealso nextPerm
#' @examples
#' 
#' x = c(1,1,2,2)
#' m1 = initMC(x)
#' 
#' for(i in 1:6){
#'   cat(paste(paste(nextPerm(m1),collapse=","),"\n"))
#' }
#' 
#' ## an example with letters
#' x = letters[1:4]
#' m2 = initMC(x)
#' nextPerm(m2)
#' nextPerm(m2)
#' ## and so on
#' 
#' @export nextPerm
nextPerm = function(mcObj){
  r = mcObj$mc$nextPerm()
  
  if(r$b){
    return(mcObj$elements[r$set])
  }else{
    return(FALSE)
  }
}
