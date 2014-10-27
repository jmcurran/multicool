nextPerm = function(mcObj){
    bCont = TRUE
    r = .C("next", set = as.integer(mcObj$set),
                   b = as.integer(bCont), id = as.integer(mcObj$id))

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
