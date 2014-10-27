initMC = function(x){
    if(length(x) > 1){
        r = NULL
        id = floor(100*runif(1))+1

        if(is.numeric(x)){
            ## Test for whole number, with tolerance for representation
            ## From post by Tony Plate<tplate_at_acm.org>
            tolerance = .Machine$double.eps^0.5
            if(isTRUE(all(abs(x - round(x))<  tolerance))){ ## integer args
                r = .C("MC", set = as.integer(x), nx = as.integer(length(x)),
                                                  id = as.integer(id))
                mcObj = list(mode = "integer", set = r$set,
                             elements = NULL, length = r$nx,
                             id = r$id)
                class(mcObj) = "mc"
                return(mcObj)
            }else{ ## doubles
                tbl = table(x)
                elements = names(tbl)
                set = rep(1:length(tbl), tbl)

                r = .C("MC", set = as.integer(set), nx = as.integer(length(set)),
                                                  id = as.integer(id))

                mcObj = list(mode = "double", set = r$set, elements = elements,
                             length = r$nx, id = r$id)
                class(mcObj) = "mc"
                return(mcObj)
            }
        }else{ ## logicals and characters and who know's whatelse
             tbl = table(x)
             elements = names(tbl)
             set = rep(1:length(tbl), tbl)

             r = .C("MC", set = as.integer(set), nx = as.integer(length(set)),
                                                  id = as.integer(id))

             mcObj = list(mode = mode(x), set = r$set, elements = elements,
                          length = r$nx, id = r$id)
             class(mcObj) = "mc"
             return(mcObj)
         }
    }else{
        stop("The permutations of a vector of length 1 are not very interesting")
    }
}
