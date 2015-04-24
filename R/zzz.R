.onLoad <- function(libname, pkgname) {
    library.dynam("showtext", pkgname, libname)
    .add.default.font()
    
    ## C structure stored in package environment. C function can access these
    ## data by looking up variable ".outline_funs" in the package environment.
    .pkg.env$.outline_funs = .Call("showtextNewOutlineFuns", PACKAGE = "showtext")
    
    ## Similar to above. The data can be set by R and accessed by C easily.
    .pkg.env$.nseg = 10L
    .pkg.env$.dpi = 96L
    
    ## This is used to convert physical unit (e.g. inch, point) to device unit.
    ## For vector graphics, device unit is usually expressed in points
    ## (1 point = 1/72 inches), so this ratio would be 1.0
    ## For bitmap graphics, device unit is usually in pixels,
    ## and hence this ratio equals dpi/72.0
    ## This number is used in calculating metric information of characters.
    .pkg.env$.dev_units_per_point = 1.0
    
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

