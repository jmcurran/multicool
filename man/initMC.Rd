\name{initMC}
\alias{initMC}
\title{Initialise the permutation object}
\description{
  This function initialises the permutation object. It must be called
  before \code{nextPerm} can be called
}
\usage{
initMC(x)
}
\arguments{
  \item{x}{a vector of integers, reals, logicals or characters}
}
\value{
  a object of class \code{mc} which is a list containing elements
  \itemize{
    \item{mode}{ - the mode of the original data in \code{x}, "integer",
      "double", or \code{mode(x)}}

    \item{set}{ - either the multiset being permuted if \code{mode} is
      "integer" or a set of integers corresponding to the elements of
      the multiset}
    \item{elements}{ - if \code{mode} is not "integer" then this contains
      the elements being permuted otherwise \code{NULL}}
    \item{length}{ - the length of the multiset}
    \item{id}{ - the id of the multiset. This argument is used when the
      user is permuting more than one multiset}
    }
}
\seealso{
  nextPerm
}
\examples{
x = c(1,1,2,2)
m1 = initMC(x)
m1

## a non-integer example

x = rep(letters[1:4],c(2,1,2,2))
m2 = initMC(x)
m2
}
\author{James M. Curran}
