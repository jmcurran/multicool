\name{nextPerm}
\alias{nextPerm}
\title{Return the next permutation of the multiset}
\description{
  This function returns the next permuation of the multiset if there is
  one. \code{initMC} called before \code{nextPerm} can be called.
}
\usage{
nextPerm(mcObj)
}
\arguments{
  \item{mcObj}{an S3 object of class \code{mc} which must be created
  with \code{initPerm}}
}
\value{
  either a vector with the next permutation of the multiset or \code{FALSE}
  when all permutations have been returned
}
\seealso{
  nextPerm
}
\examples{
x = c(1,1,2,2)
m1 = initMC(x)

for(i in 1:6){
  cat(paste(paste(nextPerm(m1),collapse=","),"\n"))
}

## an example with letters
x = letters[1:4]
m2 = initMC(x)
nextPerm(m2)
nextPerm(m2)
## and so on
}
\author{James M. Curran}
