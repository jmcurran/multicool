\name{Bell}
\alias{Bell}
\alias{B}
\title{
Compute the Bell numbers
}
\description{
This function computes the Bell numbers, which is the summ of Stirling numbers of the second kind, \eqn{S(n, k)}{S(n, k)}, over \eqn{k = 1,\ldots, n}{k}, i.e. 
\deqn{B_n = \sum_{k=1}^{n}S(n, k),n \ge 1}
}
\usage{
Bell(n)
B(n)
}
\arguments{
  \item{n}{A vector of one or more non-zero positive integers}
}
\value{
An vector of Bell numbers
}
\seealso{Stirling2}
\references{
\url{http://en.wikipedia.org/wiki/Stirling_numbers_of_the_second_kind#Recurrence_relation}
}
\author{James Curran}
\examples{
## returns B(6)
Bell(6)

## returns B(1), B(2), ..., B(6)
B(1:6)
}
\keyword{partitions}
