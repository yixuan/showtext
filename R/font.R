# environment to store several important variables
.pkg.env = new.env();

# use font.add() to add default font
.add.default.font = function()
{
    lib.loc = if("showtext" %in% loadedNamespaces())
                  dirname(getNamespaceInfo("showtext", "path"))
              else NULL;

    wqy.r = system.file("fonts", "wqy-microhei.ttc",
                        package = "showtext", lib.loc = lib.loc);
    
    if("R2SWF" %in% loadedNamespaces())
    {
        font.add("wqy", wqy.r);
    } else {
        stop("package R2SWF is not loaded");
    }

    invisible(NULL);
}

