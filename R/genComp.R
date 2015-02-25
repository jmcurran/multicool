genComp = function(n, len = TRUE){
  if(len & is.logical(len)){
    return(generateCompositions(n))
  }else if(is.numeric(len) & len <= n) {
    constraintFn = function(l){
      if(length(l) <= len)
        return(TRUE)
      else
        return(FALSE)
    }
    l = generateCompositions(n)
    return(l[sapply(l, constraintFn)])
  }
}