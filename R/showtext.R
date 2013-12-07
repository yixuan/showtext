showtext.begin = function(nseg = 10L)
{
    if(dev.cur() == 1) stop("no active graphics device");
    if(nseg <= 0) stop("nseg must be positive");
    if(nseg > 100) stop("nseg too large");
    
    .pkg.env$.nseg = as.integer(nseg);
    .Call("showtextBegin", PACKAGE = "showtext");
    invisible(NULL);
}

showtext.end = function()
{
    if(dev.cur() == 1) stop("no active graphics device");
    
    .Call("showtextEnd", PACKAGE = "showtext");
    invisible(NULL);
}
