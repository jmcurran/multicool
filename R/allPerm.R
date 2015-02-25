allPerm = function(mcObj){
    if(class(mcObj) != "mc")
      stop("mcObject must be of class mc")
        
    r = allPermC(mcObj$id);
    x = unlist(r)
    
    if(mcObj$mode == "integer"){
      return(matrix(x, ncol = mcObj$length, byrow = TRUE))
    }else{
      return(matrix(mcObj$elements[x], ncol = mcObj$length, byrow = TRUE))
    }
}
