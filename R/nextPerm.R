nextPerm = function(mcObj){
  r = nextPermC(mcObj$id)
  
  if(r$b){
    return(mcObj$elements[r$set])
    
  }else{
    return(FALSE)
  }
}
