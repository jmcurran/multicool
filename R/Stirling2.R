#' Compute Stirling numbers of the second kind
#' 
#' This function computes Stirling numbers of the second kind, \eqn{S(n,
#' k)}{S(n, k)}, which count the number of ways of partitioning n distinct
#' objects in to k non-empty sets.
#' 
#' The implementation on this function is a simple recurrence relation which
#' defines \deqn{S(n, k) = kS(n - 1, k), + S(n - 1, k - 1)} for \eqn{k > 0}
#' with the inital conditions \eqn{S(0, 0) = 1} and \eqn{S(n, 0) = S(0, n) =
#' 0}. If \code{n} and \code{n} have different lengths then \code{expand.grid}
#' is used to construct a vector of (n, k) pairs
#' 
#' @aliases Stirling2 S2
#' @param n A vector of one or more positive integers
#' @param k A vector of one or more positive integers
#' @return An vector of Stirling numbers of the second kind
#' @author James Curran
#' @references
#' \url{https://en.wikipedia.org/wiki/Stirling_numbers_of_the_second_kind#Recurrence_relation}
#' @keywords partitions
#' @examples
#' 
#' ## returns S(6, 3)
#' Stirling2(6, 3)
#' 
#' ## returns S(6,1), S(6,2), ..., S(6,6)
#' S2(6, 1:6)
#' 
#' ## returns S(6,1), S(5, 2), S(4, 3)
#' S2(6:4, 1:3)
#' 
#' @export Stirling2
Stirling2 = function(n, k){
  if(any(n < 0) || any(k < 0))
    stop("n and k must be positive integers, n >= 0, k >= 0")
  
  nN = length(n)
  nK = length(k)
  
  if(nN > 1 || nK > 1){
    if(nN != nK){
      grid = expand.grid(n = n, k = k)
      n = grid$n
      k = grid$k
      
      nN = nK = nrow(grid)
    }
  }
  
  result = rep(0, nN)
  for(r in 1:nN){
    result[r] = Stirling2C(n[r], k[r])
  }
  
  return(result)
}

S2 = function(n, k){Stirling2(n, k)}



#' Compute the Bell numbers
#' 
#' This function computes the Bell numbers, which is the summ of Stirling
#' numbers of the second kind, \eqn{S(n, k)}{S(n, k)}, over \eqn{k = 1,\ldots,
#' n}{k}, i.e.  \deqn{B_n = \sum_{k=1}^{n}S(n, k),n \ge 1}
#' 
#' 
#' @aliases Bell B
#' @param n A vector of one or more non-zero positive integers
#' @return An vector of Bell numbers
#' @author James Curran
#' @seealso Stirling2
#' @references
#' \url{https://en.wikipedia.org/wiki/Stirling_numbers_of_the_second_kind#Recurrence_relation}
#' @keywords partitions
#' @examples
#' 
#' ## returns B(6)
#' Bell(6)
#' 
#' ## returns B(1), B(2), ..., B(6)
#' B(1:6)
#' 
#' @export Bell
Bell = function(n){
  if(any(n <=0))
    stop("n must be greater than or equal to 1")
  
  nN = length(n)
  result = rep(0, nN)
  
  for(r in 1:nN)
    result[r] = sum(Stirling2(n[r], 1:n[r]))
  
  return(result)
}

B = function(n){Bell(n)}
