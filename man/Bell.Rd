% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/Stirling2.R
\name{Bell}
\alias{Bell}
\alias{B}
\title{Compute the Bell numbers}
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
\description{
This function computes the Bell numbers, which is the summ of Stirling
numbers of the second kind, \eqn{S(n, k)}{S(n, k)}, over \eqn{k = 1,\ldots,
n}{k}, i.e.  \deqn{B_n = \sum_{k=1}^{n}S(n, k),n \ge 1}
}
\section{Functions}{
\itemize{
\item \code{B()}: Compute the Bell numbers

}}
\examples{

## returns B(6)
Bell(6)

## returns B(1), B(2), ..., B(6)
B(1:6)

}
\references{
\url{https://en.wikipedia.org/wiki/Stirling_numbers_of_the_second_kind#Recurrence_relation}
}
\seealso{
Stirling2
}
\author{
James Curran
}
\keyword{partitions}
