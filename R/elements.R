## Internal helpers shared by nextPerm and allPerm.

## The C++ code can write plain integer, double, logical and character vectors
## straight into its output. Anything else (factors, classed vectors such as
## Dates, complex) gets the codes 1..k back, and mapCodes maps them here.
isPlainElements = function(elements){
  is.null(attributes(elements)) &&
    typeof(elements) %in% c("integer", "double", "logical", "character")
}

## x is a vector or matrix of codes. Matrices of factors are returned as
## character matrices, as matrix() would.
mapCodes = function(x, elements){
  r = elements[x]
  
  if(is.matrix(x)){
    if(is.factor(r))
      r = as.character(r)
    dim(r) = dim(x)
  }
  
  return(r)
}
