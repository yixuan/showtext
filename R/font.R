# environment to store several important variables
.pkg.env = new.env();

# use font.add() to add default font
.add.default.font = function()
{
    if(!"sysfonts" %in% loadedNamespaces())
        stop("package sysfonts is not loaded");
    
    if("wqy-microhei" %in% font.families()) return(invisible(NULL));
    
    lib.loc = if("showtext" %in% loadedNamespaces())
                  dirname(getNamespaceInfo("showtext", "path"))
              else NULL;

    fontfile = system.file("fonts", "wqy-microhei.ttc.xz",
                           package = "showtext", lib.loc = lib.loc);
    con = xzfile(fontfile, "rb");
    reader = readBin(con, "raw", 5177400);
    close(con);
    outfile = file.path(tempdir(), "wqy-microhei.ttc");
    writeBin(reader, outfile);
    font.add("wqy-microhei", outfile);

    invisible(NULL);
}

