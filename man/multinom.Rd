\name{multinom}
\alias{multinom}
\title{Calculate multinomial coefficients
}
\description{
This function calculates the number of permutations of a multiset, this being the multinomial coefficient. If a set \eqn{X} contains \eqn{k} unique elements \eqn{x_1, x_2, \ldots, x_k} with associate counts (or multiplicities) of \eqn{n_1, n_2, \ldots, n_k}, then this function returns
\deqn{\frac{n!}{n_1!n_2!\ldots n_k!}}{n!/(n_1!n_2!\ldots n_k!)}
where \eqn{n = \sum_{i=1}{k}n_i}{n = n_1 + n_2 + \cdots + n_k}.
}
\usage{
multinom(x, counts = FALSE, useDouble = FALSE)
}
\arguments{
  \item{x}{
  Either a multiset (with one or more potentially non-unique elements), or if \code{counts} is \code{TRUE} a set of counts of the unique elements of \eqn{X}. If \code{counts} is \code{FALSE} and \code{x} is not numeric, then x will be coerced into an integer vector internally. If \code{counts} is \code{TRUE} then \code{x} must be a vector of integers that are greater than, or equal to zero.
}
  \item{counts}{
  if \code{counts} is TRUE, then this means x is the set of counts \eqn{n_1, n_2, \ldots, n_k} rather than the set itself
}
  \item{useDouble}{
  if \code{useDouble} is TRUE then the computation will be done using double precision floating point arithmetic. This option was added because the internal code cannot handle integer overflow. The double precision code will may a result that is closer to the truth for large values, but this is not guaranteed. Ideally something like the GMP library should be used, but this is not a priority at this point in time.
  }
}
\details{
multinom depends on C++ code written by Dave Barber which can be found at \url{http://tamivox.org/dave/multinomial/code.html}. The code may require the STL algorithm library to be included in order to compile it.
}
\value{
A single integer representing the multinomial coefficient for the given multiset, or given set of multiplicities.
}
\references{
\url{http://tamivox.org/dave/multinomial/code.html}
}
\author{
James M. Curran, Dave Barber
}
\examples{
## An example with a multiset X = (a,a,a,b,b,c)
## There are 3 a s, 2 b s and 1 c, so the answer should be
## (3+2+1)!/(3!2!1!) = 6!/3!2!1! = 60
x = rep(letters[1:3],3:1)
multinom(x)

## in this example x is a vector of counts
## the answer should be the same as above as x = c(3,2,1)
x = rep(letters[1:3],3:1)
x = as.vector(table(x)) #coerce x into a vector of counts
multinom(x, counts = TRUE)


## An example of integer overflow. x is a vector of counts
## c(12,11,8,8,6,5). The true answer from Maple is
## 11,324,718,121,789,252,764,532,876,767,840,000
## The error in the integer based answer is obvious.
## The error using floating point is not, but from Maple is
## 0.705057123232160000e+10
## Thanks to Lev Dashevskiy for calling my attention to this.
\dontrun{x = c(12,11,8,8,6,5)
multinom(x, counts = TRUE, useDouble = FALSE)
multinom(x, counts = TRUE, useDouble = TRUE)
}
}
\keyword{ combinations }
\keyword{ permutations }
\keyword{ multinomial }