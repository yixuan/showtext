.onLoad <- function(libname, pkgname) {
    library.dynam("showtext", pkgname, libname)
    .add.default.font()
    .pkg.env$.outline.funs = .Call("showtextLoadOutlineFuns",
                                   PACKAGE = "showtext")
    .pkg.env$.nseg = 10L
    ## The identifier of the device on which showtext.begin() is called.
    ## Initialized to be a null pointer.
    .pkg.env$.device_id = .Call("showtextNullPointer", PACKAGE = "showtext")
    .pkg.env$.dd_saved = .Call("showtextNewDevDesc", PACKAGE = "showtext")
}

.onUnload <- function(libpath) {
    .Call("showtextCleanOutlineFuns", .pkg.env$.outline.funs,
          PACKAGE = "showtext")
    .Call("showtextFreeDevDesc", .pkg.env$.dd_saved, PACKAGE = "showtext")
    library.dynam.unload("showtext", libpath)
}

