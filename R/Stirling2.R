## Returns a list, named by n, of the rows S(n, 0), ..., S(n, n) as exact
## bigz values for each of the requested values of n. All the rows are built
## in one pass of the recurrence S(r, j) = j S(r - 1, j) + S(r - 1, j - 1),
## so the cost is O(max(n)^2) big integer operations.
stirling2Rows = function(n){
  wanted = sort(unique(n))
  rows = vector("list", length(wanted))
  names(rows) = wanted
  
  row = gmp::as.bigz(1) ## S(0, 0)
  if(0 %in% wanted)
    rows[["0"]] = row
  
  if(max(wanted) >= 1){
    for(r in 1:max(wanted)){
      padded = c(row, gmp::as.bigz(0))
      row = c(gmp::as.bigz(0), (1:r) * padded[2:(r + 1)] + padded[1:r])
      
      if(r %in% wanted)
        rows[[as.character(r)]] = row
    }
  }
  
  return(rows)
}

isWholeNumber = function(x){
  is.numeric(x) && !anyNA(x) && all(abs(x - round(x)) < .Machine$double.eps^0.5)
}

#' Compute Stirling numbers of the second kind
#' 
#' This function computes Stirling numbers of the second kind, \eqn{S(n,
#' k)}{S(n, k)}, which count the number of ways of partitioning n distinct
#' objects in to k non-empty sets.
#' 
#' The implementation on this function is a simple recurrence relation which
#' defines \deqn{S(n, k) = kS(n - 1, k), + S(n - 1, k - 1)} for \eqn{k > 0}
#' with the inital conditions \eqn{S(0, 0) = 1} and \eqn{S(n, 0) = S(0, n) =
#' 0}. If \code{n} and \code{k} have different lengths then \code{expand.grid}
#' is used to construct a vector of (n, k) pairs
#'
#' @param n A vector of one or more non-negative integers
#' @param k A vector of one or more non-negative integers
#' @return A vector of Stirling numbers of the second kind, as exact big
#' integers of class \code{bigz} from the \pkg{gmp} package. Use
#' \code{as.numeric} to convert them to double precision if required.
#' @author James Curran
#' @seealso \code{\link[gmp]{as.bigz}}
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
#' ## the values are exact, even when they are very large
#' S2(100, 50)
#' 
#' @export Stirling2
Stirling2 = function(n, k){
  if(!isWholeNumber(n) || !isWholeNumber(k) || any(n < 0) || any(k < 0))
    stop("n and k must be non-negative integers, n >= 0, k >= 0")
  
  n = round(n)
  k = round(k)
  
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
  
  rows = stirling2Rows(n)
  result = gmp::as.bigz(rep(0, nN))
  
  for(r in 1:nN){
    if(k[r] <= n[r])
      result[r] = rows[[as.character(n[r])]][k[r] + 1]
  }
  
  return(result)
}

#' @rdname Stirling2
#' @export
S2 = function(n, k){Stirling2(n, k)}



#' Compute the Bell numbers
#' 
#' This function computes the Bell numbers, which is the summ of Stirling
#' numbers of the second kind, \eqn{S(n, k)}{S(n, k)}, over \eqn{k = 1,\ldots,
#' n}{k}, i.e.  \deqn{B_n = \sum_{k=1}^{n}S(n, k),n \ge 1}
#' 
#' 
#' @param n A vector of one or more non-zero positive integers
#' @return A vector of Bell numbers, as exact big integers of class
#' \code{bigz} from the \pkg{gmp} package. Use \code{as.numeric} to convert
#' them to double precision if required.
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
  if(!isWholeNumber(n) || any(n < 1))
    stop("n must be an integer greater than or equal to 1")
  
  n = round(n)
  rows = stirling2Rows(n)
  
  nN = length(n)
  result = gmp::as.bigz(rep(0, nN))
  
  for(r in 1:nN)
    result[r] = sum(rows[[as.character(n[r])]])
  
  return(result)
}

#' @rdname Bell
#' @export
B = function(n){Bell(n)}
