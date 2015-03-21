.onLoad <- function(libname, pkgname) {
    library.dynam("showtext", pkgname, libname)
    .add.default.font()
    
    ## C structure stored in package environment. C function can access these
    ## data by looking up variable ".outline_funs" in the package environment.
    .pkg.env$.outline_funs = .Call("showtextNewOutlineFuns", PACKAGE = "showtext")
    
    ## Similar to above. The data can be set by R and accessed by C easily.
    .pkg.env$.nseg = 10L
    .pkg.env$.dpi = c(96L, 96L)
    
    ## The identifier of the device on which showtext.begin() is called.
    ## Initialized to be a null pointer.
    .pkg.env$.device_id = .Call("showtextNullPointer", PACKAGE = "showtext")
    
    ## Device data to be saved
    .pkg.env$.dd_saved = .Call("showtextNewDevDesc", PACKAGE = "showtext")
    
    ## Whether to use raster image to draw text, or to use polygons
    ## Using raster image is suitable for bitmap devices without anti-alias
    ## feature, such as png(), jpeg(), tiff(), bmp(), and on-screen devices
    ## Drawing text with polygons is appropriate for vector graphics like pdf()
    .pkg.env$.use_raster = FALSE
}

.onUnload <- function(libpath) {
    .Call("showtextFreeOutlineFuns", .pkg.env$.outline_funs, PACKAGE = "showtext")
    .Call("showtextFreeDevDesc", .pkg.env$.dd_saved, PACKAGE = "showtext")
    library.dynam.unload("showtext", libpath)
}

