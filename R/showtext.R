#' Render text for R graphics devices
#' 
#' Calling this function will use \pkg{showtext} to render text
#' for the current graphics device. The main advantage of
#' \pkg{showtext} is that user can use any supported font file for
#' the text rendering, and all text glyphs will be converted
#' into lines and curves, thus producing device independent output in all
#' platforms. This function would be very useful if you want to 
#' use non-standard fonts in the graphics device (for example,
#' pdf device dealing with Chinese characters). The usage of this
#' function is pretty simple: just open your graphics device, and
#' "claim" that you want to use \pkg{showtext} by calling this function.
#' See the "Examples" section for details.
#' 
#' 
#' @param nseg parameter to control the smoothness of the outlines
#'             of glyphs. It is the number
#'             of segments of lines to approximate each piece of curve
#'             in the glyph. The larger \code{nseg} is, the
#'             better visual perception would be, but also with larger
#'             file size for vector graphics. Usually a value
#'             between 5~20 would be enough.
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
#'          low-level drawing functions (e.g., functions to draw
#'          polygons and lines) in the current device to draw
#'          the glyph. As a result, glyphs of the text will be finally
#'          converted into lines and polygons, which means that the system
#'          where the graph is viewed doesn't need to install the fonts
#'          that create the graph.
#'          
#'          Notice that this function is only effective to the current
#'          \bold{ACTIVE} device. So to use this function, the device
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
#' @author Yixuan Qiu <\url{http://yixuan.cos.name/}>
#' 
#' @seealso \code{\link{showtext.end}()}
#' 
#' @examples \dontrun{
#' old = setwd(tempdir());
#' 
#' ###  Enable pdf() to draw Chinese characters nicely  ###
#' ###  Requires the simkai.ttf font file, usually      ###
#' ###  installed in Windows                            ###
#' 
#' ## First, open the device
#' pdf("showtext-ex1.pdf");
#' 
#' ## For now we are using the original device functions to draw axis labels
#' plot(1, type = "n");
#' 
#' ## Then turn showtext on and draw some characters
#' showtext.begin();
#' text(1, 1.2, intToUtf8(c(21315, 31179, 19975, 36733)), cex = 5);
#' 
#' ## Use another font
#' if("simkai.ttf" %in% font.files()) font.add("kaishu", "simkai.ttf");
#' text(1, 0.8, intToUtf8(c(19968, 32479, 27743, 28246)),
#'      cex = 5, family = "kaishu");
#'      
#' ## Turn showtext off
#' showtext.end();
#' 
#' ## Also turn off the device
#' dev.off();
#' 
#' 
#' ###  Download font file from web  ###
#' 
#' download.file("http://fontpro.com/download-family.php?file=36289",
#'               "newrocker.ttf", mode="wb");
#' download.file("http://fontpro.com/download-family.php?file=35128",
#'               "cutetumblr.ttf", mode ="wb");
#' 
#' font.add("newrocker", "newrocker.ttf");
#' font.add("cutetumblr", "cutetumblr.ttf");
#' 
#' pdf("showtext-ex2.pdf", 8, 5);
#' plot(1, type = "n");
#' showtext.begin();
#' text(1, 1.2, "Let me tell you a story", cex = 4, family = "newrocker");
#' text(1, 0.8, "Long long ago...", cex = 4, family = "cutetumblr");
#' showtext.end();
#' dev.off();
#' 
#' setwd(old);
#' 
#' }
showtext.begin = function(nseg = 10L)
{
    if(dev.cur() == 1) stop("no active graphics device");
    if(nseg <= 0) stop("nseg must be positive");
    if(nseg > 100) stop("nseg too large");
    
    .pkg.env$.nseg = as.integer(nseg);
    .Call("showtextBegin", PACKAGE = "showtext");
    invisible(NULL);
}

#' Turn off "showtext" text rendering
#' 
#' This function will turn off the \pkg{showtext} functionality
#' of rendering text. When you call this function, the current
#' active device should be the same with the one when you call
#' \code{\link{showtext.begin}()}, or an error will be issued.
#' See the example in \code{\link{showtext.begin}()}.
#'
#' @export
#' 
#' @author Yixuan Qiu <\url{http://yixuan.cos.name/}>
#' 
#' @seealso \code{\link{showtext.begin}()}
showtext.end = function()
{
    if(dev.cur() == 1) stop("no active graphics device");
    
    .Call("showtextEnd", PACKAGE = "showtext");
    invisible(NULL);
}
