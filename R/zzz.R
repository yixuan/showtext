.onLoad <- function(libname, pkgname) {
    library.dynam("showtext", pkgname, libname);
    .add.default.font();
    .pkg.env$.outline.funs = .Call("showtextLoadOutlineFuns",
                                   PACKAGE = "showtext");
    .pkg.env$.nseg = 10L;
    .pkg.env$.gdd.save = .Call("showtextReturnNullPointer",
                               PACKAGE = "showtext");
    .pkg.env$.dd.save = .Call("showtextLoadDevDesc",
                              PACKAGE = "showtext");
}

.onUnload <- function(libpath) {
    .Call("showtextCleanOutlineFuns", .pkg.env$.outline.funs,
          PACKAGE = "showtext");
    .Call("showtextCleanDevDesc", .pkg.env$.dd.save,
          PACKAGE = "showtext");
    library.dynam.unload("showtext", libpath);
}

