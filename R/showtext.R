#' Setting Options for 'showtext' package
#' 
#' This function sets parameters that will affect the apprearance of the
#' graphs created with \pkg{showtext}.
#' 
#' @param \dots Options to be set, expressed in \code{name = value} pairs.
#'              It can also be a list containing these pairs (for example,
#'              the list returned by a previous call of \code{showtext.opts()}).
#'              Currently accepted parameters are \code{nseg} and \code{dpi}.
#'              See the \strong{Options Used} section.
#' 
#' @section Options Used:
#' \describe{
#'     \item{\code{nseg}}{Parameter to control the smoothness of the outlines
#'                        of glyphs, typically used in vector graphics devices
#'                        such as \code{pdf()} and \code{svg()}.
#'                        It is the number of line segments to approximate
#'                        a piece of curve in the glyph. The larger \code{nseg} is,
#'                        the smoother text outlines would be, but also with
#'                        larger file size for vector graphics. Usually a value
#'                        between 5~20 would be enough.}
#'     \item{\code{dpi}}{An integer that gives the resolution
#'                       of the device. This parameter is only used in bitmap
#'                       and on-screen graphics devices such as \code{png()} and
#'                       \code{x11()}, to determine the pixel size of text from
#'                       point size. For example, if \code{dpi} is set to 96,
#'                       then a character with 12 point size will have a pixel
#'                       size of \code{12 * 96 / 72 = 16}.}
#' }
#' @export
#' 
#' @author Yixuan Qiu <\url{http://statr.me/}>
#' 
#' @examples \dontrun{
#' ## Set dpi to 200
#' op = showtext.opts(dpi = 200)
#' 
#' png("dpi_200.png", 800, 800, res = 200)
#' par(family = "sans")
#' showtext.begin()
#' 
#' set.seed(123)
#' plot(density(rnorm(100)))
#' 
#' showtext.end()
#' dev.off()
#' 
#' ## Restore old options
#' showtext.opts(op)
#' 
#' png("dpi_96.png", 800, 800, res = 96)
#' par(family = "sans")
#' showtext.begin()
#' 
#' set.seed(123)
#' plot(density(rnorm(100)))
#' 
#' showtext.end()
#' dev.off()
#' }
showtext.opts = function(...)
{
    old_opts = list(nseg = .pkg.env$.nseg, dpi = .pkg.env$.dpi)
    
    opts = list(...)
    ## If ... is a list containing the parameter components
    ## (usually returned by a previous showtext.opts() call)
    if(length(opts) == 1 && is.list(opts[[1]]))
        opts = opts[[1]]
    if("nseg" %in% names(opts))  nseg = opts$nseg  else  nseg = old_opts$nseg
    if("dpi" %in% names(opts))  dpi = opts$dpi  else  dpi = old_opts$dpi
    
    nseg = as.integer(nseg)[1]
    dpi = as.integer(dpi)[1]
    
    if(nseg <= 0)  stop("nseg must be positive")
    if(nseg > 100) stop("nseg too large")
    if(dpi <= 0)   stop("dpi must be positive")
    
    .pkg.env$.nseg = nseg
    .pkg.env$.dpi = dpi
    
    invisible(old_opts)
}


#' Rendering Text for R Graphics Devices
#' 
#' Calling this function will use \pkg{showtext} to render text
#' for the current graphics device. The main advantage of
#' \pkg{showtext} is that user can use any supported font file for
#' the text rendering, and text glyphs will be converted
#' into polygons (for vector graphics) or raster images
#' (for bitmap and on-screen graphics),
#' thus producing device independent output on all platforms.
#' This function would be useful if you want to use non-standard fonts
#' in the graphics device. The usage of this function is easy:
#' simply open the graphics device, and "claim" that you want to use
#' \pkg{showtext} by calling this function.
#' See the \strong{Examples} section for details.
#' 
#' @details This package uses FreeType to load font files and render
#'          text characters. The font loading part is done by function
#'          \code{\link[sysfonts]{font.add}()} in the \pkg{sysfonts}
#'          package. Users could read the help page of
#'          \code{\link[sysfonts]{font.paths}()},
#'          \code{\link[sysfonts]{font.files}()} and
#'          \code{\link[sysfonts]{font.add}()} to learn how to load
#'          a font file into R. \pkg{showtext} package
#'          has a built-in font file of the WenQuanYi Micro Hei
#'          family, and it will be loaded automatically, so users can
#'          use this function directly without any extra settings.
#'          
#'          The mechanism of this function is that it can replace the
#'          text rendering functions contained in the current device.
#'          \pkg{showtext} will first use FreeType to analyze the
#'          outline of each character in the text, and then call some
#'          low-level drawing functions in the current device to draw
#'          the glyph. As a result, glyphs of the text will be finally
#'          converted into polygons or raster images, which means that
#'          the system where the graph is viewed does not need to install
#'          the fonts that create the graph.
#'          
#'          Notice that this function is only effective to the current
#'          \strong{ACTIVE} device. So to use this function, the device
#'          you want to work with must have already been opened (through
#'          functions like \code{\link[grDevices]{png}()},
#'          \code{\link[grDevices]{pdf}()}, etc.).
#'          
#'          To switch back, users can call \code{\link{showtext.end}()}
#'          to restore the original device functions. See examples
#'          below for the usage of these functions.
#'
#' @export
#' 
#' @author Yixuan Qiu <\url{http://statr.me/}>
#' 
#' @seealso \code{\link{showtext.opts}()}, \code{\link{showtext.auto}()},
#'          \code{\link{showtext.end}()}
#' 
#' @examples \dontrun{
#' old = setwd(tempdir())
#' 
#' ###  Enable pdf() to draw Chinese characters nicely  ###
#' ###  Requires the simkai.ttf font file, usually      ###
#' ###  installed in Windows                            ###
#' 
#' ## First, open the device
#' pdf("showtext-ex1.pdf")
#' 
#' ## For now we are using the original device functions to draw axis labels
#' plot(1, type = "n")
#' 
#' ## Then turn showtext on and draw some characters
#' showtext.begin()
#' text(1, 1.2, intToUtf8(c(21315, 31179, 19975, 36733)), cex = 5)
#' 
#' ## Use another font
#' if("simkai.ttf" %in% font.files()) font.add("kaishu", "simkai.ttf")
#' text(1, 0.8, intToUtf8(c(19968, 32479, 27743, 28246)),
#'      cex = 5, family = "kaishu")
#'      
#' ## Turn showtext off
#' showtext.end()
#' 
#' ## Also turn off the device
#' dev.off()
#' 
#' 
#' ###  Download font file from web  ###
#' 
#' download.file("http://fontpro.com/download-family.php?file=36289",
#'               "newrocker.ttf", mode="wb")
#' download.file("http://fontpro.com/download-family.php?file=35128",
#'               "cutetumblr.ttf", mode ="wb")
#' 
#' font.add("newrocker", "newrocker.ttf")
#' font.add("cutetumblr", "cutetumblr.ttf")
#' 
#' png("showtext-ex2.png", 800, 500)
#' plot(1, type = "n")
#' showtext.begin()
#' text(1, 1.2, "Let me tell you a story", cex = 4, family = "newrocker")
#' text(1, 0.8, "Long long ago...", cex = 4, family = "cutetumblr")
#' showtext.end()
#' dev.off()
#' 
#' setwd(old)
#' 
#' }
showtext.begin = function()
{
    if(dev.cur() == 1) stop("no active graphics device")
    
    ## For some bitmap and on-screen devices, we use raster image
    ## instead of polygon to draw text
    device_using_raster = c("png", "PNG", "jpeg", "tiff", "bmp",
                            "X11", "X11cairo", "windows")
    if(names(dev.cur()) %in% device_using_raster)
    {
        .pkg.env$.use_raster = TRUE
        .pkg.env$.dev_units_per_point = as.numeric(.pkg.env$.dpi / 72.0)
    } else {
        .pkg.env$.use_raster = FALSE
        .pkg.env$.dev_units_per_point = 1.0
    }

    .Call("showtextBegin", PACKAGE = "showtext")
    invisible(NULL)
}


#' Turning Off 'showtext' Text Rendering
#' 
#' This function will turn off the \pkg{showtext} functionality
#' of rendering text. When you call this function, the current
#' active device should be the same as the one when you call
#' \code{\link{showtext.begin}()}, or an error will be issued.
#' See the example in \code{\link{showtext.begin}()}.
#'
#' @export
#' 
#' @author Yixuan Qiu <\url{http://statr.me/}>
#' 
#' @seealso \code{\link{showtext.begin}()}
showtext.end = function()
{
    if(dev.cur() == 1) stop("no active graphics device")
    
    .Call("showtextEnd", PACKAGE = "showtext")
    invisible(NULL)
}


#' Automatically Using 'showtext' for New Graphics Devices
#' 
#' This function could turn on/off the automatic use of \pkg{showtext}
#' functionality. If turned on, any newly opened graphics devices will use
#' \pkg{showtext} to draw text. This helps to avoid the repeated calls of
#' \code{\link{showtext.begin}()} and \code{\link{showtext.end}()}.
#' 
#' @param enable \code{TRUE} to turn on and \code{FALSE} to turn off
#'
#' @export
#' 
#' @author Yixuan Qiu <\url{http://statr.me/}>
#' 
#' @seealso \code{\link{showtext.begin}()}, \code{\link{showtext.end}()}
#' 
#' @examples \dontrun{
#' pdf("test1.pdf")
#' plot(1, main = "\u6b22\u8fce")  ## may not render properly
#' dev.off()
#' 
#' ## Automatically use showtext for future devices
#' showtext.auto()
#' plot(1, main = "\u6b22\u8fce", family = "wqy-microhei")
#' 
#' pdf("test2.pdf")
#' plot(1, main = "\u6b22\u8fce", family = "wqy-microhei")
#' dev.off()
#' 
#' ## Turn off if needed
#' showtext.auto(FALSE)
#' }
showtext.auto = function(enable = TRUE)
{
    enable = as.logical(enable)
    
    has_hook = length(getHook("before.plot.new")) > 0
    is_showtext_hook = sapply(getHook("before.plot.new"), identical,
                              y = showtext::showtext.begin)
    
    has_hook_grid = length(getHook("grid.newpage")) > 0
    is_showtext_hook_grid = sapply(getHook("grid.newpage"), identical,
                                   y = showtext::showtext.begin)

    already_hooked = has_hook && any(is_showtext_hook)
    already_hooked_grid = has_hook_grid && any(is_showtext_hook_grid)
    
    if(enable)
    {
        if(!already_hooked)
            setHook("before.plot.new", showtext::showtext.begin)
        if(!already_hooked_grid)
            setHook("grid.newpage", showtext::showtext.begin)
    } else {
        if(already_hooked)
        {
            old_hooks = getHook("before.plot.new")
            new_hooks = old_hooks[!is_showtext_hook]
            setHook("before.plot.new", new_hooks, "replace")
        }
        if(already_hooked_grid)
        {
            old_hooks = getHook("grid.newpage")
            new_hooks = old_hooks[!is_showtext_hook_grid]
            setHook("grid.newpage", new_hooks, "replace")
        }
    }
}
