.onLoad <- function(libname, pkgname) {
    library.dynam("showtext", pkgname, libname)
    showtextdb::load_showtext_fonts()
    
    ## C structure stored in package environment. C function can access these
    ## data by looking up variable ".outline_funs" in the package environment.
    .pkg.env$.outline_funs = .Call("showtext_new_outline_funs", PACKAGE = "showtext")
    
    ## Similar to above. The data can be set by R and accessed by C easily.
    .pkg.env$.nseg = 10L
    .pkg.env$.dpi = 96L
    
    ## An environment to store device-specific data
    ## Each object in this environment is associated with a device on which
    ## showtext is turned on. Each object is a list containing necessary
    ## information about the device, and the variable name is a unique identifier
    ## of the device. Right now the list contains the following fields:
    ##
    ## ==> use_raster:          Whether to use raster image to draw text, or to use polygons.
    ##                          Using raster image is suitable for bitmap devices without
    ##                          anti-alias feature, such as png(), jpeg(), tiff(), bmp(),
    ##                          and on-screen devices. Drawing text with polygons is
    ##                          appropriate for vector graphics like pdf().
    ##
    ## ==> dev_units_per_point: This is used to convert physical unit (e.g. inch, point)
    ##                          to device unit. For vector graphics, device unit is usually
    ##                          expressed in points (1 point = 1/72 inches), so this ratio
    ##                          would be 1.0. For bitmap graphics, device unit is usually
    ##                          in pixels, and hence this ratio equals dpi/72.0. This number
    ##                          is used in calculating metric information of characters.
    ##
    ## ==> dd_saved:            Device data to be saved before showtext is turned on.
    ##                          This is used to restore the original state of the device
    ##                          after showtext is turned off.
    .pkg.env$.devs = new.env()
}

.onUnload <- function(libpath) {
    ## Free memory of .pkg.env$.outline_funs
    rm(list = ".outline_funs", envir = .pkg.env)
    ## Free memory of objects in .pkg.env$.devs
    rm(list = ls(.pkg.env$.devs), envir = .pkg.env$.devs)
    gc()

    library.dynam.unload("showtext", libpath)
}
