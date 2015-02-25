nextPerm = function(mcObj){
    r = nextPermC(mcObj$id)

    if(r$b){
        if(mcObj$mode == "integer"){
            return(r$set)
        }else{
            return(mcObj$elements[r$set])
        }
    }else{
        return(FALSE)
    }
}
