# environment to store several important variables
.pkg.env = new.env()

# use font.add() to add default font
.add.default.font = function()
{
    fontfile = system.file("fonts", "wqy-microhei.ttc.zip", package = "showtextdb")
    outdir = tempdir()
    utils::unzip(fontfile, exdir = outdir)
    outfile = file.path(outdir, "wqy-microhei.ttc")
    sysfonts::font.add("wqy-microhei", outfile)
}

