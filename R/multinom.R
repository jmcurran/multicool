#' Calculate multinomial coefficients
#' 
#' This function calculates the number of permutations of a multiset, this
#' being the multinomial coefficient. If a set \eqn{X} contains \eqn{k} unique
#' elements \eqn{x_1, x_2, \ldots, x_k} with associate counts (or
#' multiplicities) of \eqn{n_1, n_2, \ldots, n_k}, then this function returns
#' \deqn{\frac{n!}{n_1!n_2!\ldots n_k!}}{n!/(n_1!n_2!\ldots n_k!)} where \eqn{n
#' = \sum_{i=1}^{k}n_i}{n = n_1 + n_2 + \cdots + n_k}.
#' 
#' @param x Either a multiset (with one or more potentially non-unique
#' elements), or if \code{counts} is \code{TRUE} a set of counts of the unique
#' elements of \eqn{X}. If \code{counts} is \code{FALSE} and \code{x} is not
#' numeric, then x will be coerced into an integer vector internally. If
#' \code{counts} is \code{TRUE} then \code{x} must be a vector of integers that
#' are greater than, or equal to zero.
#' @param counts if \code{counts} is TRUE, then this means x is the set of
#' counts \eqn{n_1, n_2, \ldots, n_k} rather than the set itself
#' @param useDouble kept for backwards compatibility, and no longer has any
#' effect. The coefficient is always computed exactly, and returned as a
#' double unless \code{exact} is \code{TRUE}.
#' @param exact if \code{TRUE} the exact result is returned as a big integer
#' of class \code{bigz} from the \pkg{gmp} package.
#' @return The multinomial coefficient for the given multiset, or given set of
#' multiplicities. By default this is a double: the exact value rounded to
#' double precision, so it is exact up to \eqn{2^{53}}{2^53} and correct to
#' about 15 significant digits beyond that. If \code{exact} is \code{TRUE}
#' it is an exact \code{bigz}.
#' @author James M. Curran
#' @keywords combinations permutations multinomial
#' @examples
#' 
#' ## An example with a multiset X = (a,a,a,b,b,c)
#' ## There are 3 a s, 2 b s and 1 c, so the answer should be
#' ## (3+2+1)!/(3!2!1!) = 6!/3!2!1! = 60
#' x = rep(letters[1:3],3:1)
#' multinom(x)
#' 
#' ## in this example x is a vector of counts
#' ## the answer should be the same as above as x = c(3,2,1)
#' x = rep(letters[1:3],3:1)
#' x = as.vector(table(x)) #coerce x into a vector of counts
#' multinom(x, counts = TRUE)
#' 
#' 
#' ## An example of a large coefficient. x is a vector of counts
#' ## c(12,11,8,8,6,5). The true answer from Maple is
#' ## 11,324,718,121,789,252,764,532,876,767,840,000
#' ## exact = TRUE gives this. The default double precision result only
#' ## keeps the first 15 or so significant digits.
#' ## Thanks to Lev Dashevskiy for calling my attention to this.
#' x = c(12,11,8,8,6,5)
#' multinom(x, counts = TRUE)
#' multinom(x, counts = TRUE, exact = TRUE)
#'
#' @export multinom
multinom = function(x, counts = FALSE, useDouble = FALSE, exact = FALSE){
  
  u = NULL
  
  if(!counts){
    u = as.vector(table(x))
  }else{
    ## make sure x is a vector of counts
    is.wholenumber = function(x, tol = .Machine$double.eps^0.5){abs(x - round(x)) < tol}
    
    if(any(!is.wholenumber(x)) || any(x < 0))
      stop("if counts == TRUE then all elements of x must be integer and >= 0")
    
    u = round(x)
  }
  
  ## n! / (n_1! n_2! ... n_k!) is always a whole number, so integer
  ## division is exact
  r = gmp::factorialZ(sum(u)) %/% prod(gmp::factorialZ(u))
  
  if(exact)
    return(r)

  return(as.numeric(r))
}
