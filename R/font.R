# environment to store several important variables
.pkg.env = new.env();

# use font.add() to add default font
.add.default.font = function()
{
    if(!"sysfonts" %in% loadedNamespaces())
        stop("package sysfonts is not loaded");
    
    if("wqy" %in% font.families()) return(invisible(NULL));
    
    lib.loc = if("showtext" %in% loadedNamespaces())
                  dirname(getNamespaceInfo("showtext", "path"))
              else NULL;

    wqy.r = system.file("fonts", "wqy-microhei.ttc",
                        package = "showtext", lib.loc = lib.loc);
    
    font.add("wqy", wqy.r);

    invisible(NULL);
}

