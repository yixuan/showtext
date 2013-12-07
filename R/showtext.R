#' Render text for R graphics devices
#' 
#' Calling this function will use \pkg{showtext} to render text
#' for the current graphics device. The main advantage of
#' \pkg{showtext} is that user can use any ttf/ttc font file for
#' the text rendering, and all text characters will be converted
#' into lines and curves, thus producing consistent output in all
#' platforms. This function would be very useful when the device
#' couldn't render some Unicode characters nicely (for example,
#' pdf device dealing with Chinese characters).
#' 
#' 
#' @param nseg parameter to control the smoothness of characters
#'             that are outlined by \pkg{showtext}. It is the number
#'             of segments of lines to approximate each piece of curve
#'             in the character. The larger \code{nseg} is, the
#'             better visual perception would be, but also with larger
#'             file size for vector graphics. Usually a value
#'             between 5~20 would be enough.
#' 
#' @details This package uses FreeType to load font files and render
#'          text characters. The font loading part is done by function
#'          \code{\link[R2SWF]{font.add}()} in the \pkg{R2SWF}
#'          package. Users could read the help page of
#'          \code{\link[R2SWF]{font.paths}()},
#'          \code{\link[R2SWF]{font.files}()} and
#'          \code{\link[R2SWF]{font.add}()} to learn how to load
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
#'          polygons and lines) in the current device to render
#'          the text. Therefore, the text characters will be finally
#'          converted into lines and polygons.
#'          
#'          Notice that this function is only effective to the current
#'          \bold{ACTIVE} device. So to use this function, the device
#'          you want to work with must have already been opened (through
#'          functions like \code{\link[grDevices]{png}()},
#'          \code{\link[grDevices]{pdf}()}, etc.).
#'          
#'          To switch back, users can call \code{\link{showtext.end}()}
#'          to restore the original device functions. See examples
#'          below for the actual usage of these functions.
#'
#' @export
#' 
#' @author Yixuan Qiu <\url{http://yixuan.cos.name/}>
#' 
#' @examples
#' ###  Enable pdf() to draw Chinese characters  ###
#' old = setwd(tempdir());
#' 
#' ## First, open the device
#' pdf();
#' ## For now we are using the original device functions to draw
#' ## axis labels
#' plot(1, type = "n");
#' ## Turn showtext on
#' showtext.begin();
#' text(1, 1.2, intToUtf8(c(21315, 31179, 19975, 36733)), cex = 5);
#' ## Use another font
#' if("simkai.ttf" %in% font.files()) font.add("kaishu", "simkai.ttf");
#' text(1, 0.8, intToUtf8(c(19968, 32479, 27743, 28246)),
#'      cex = 5, family = "kaishu");
#' ## Turn showtext off
#' showtext.end();
#' ## Turn off the device
#' dev.off();
#' 
#' setwd(old);
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
showtext.end = function()
{
    if(dev.cur() == 1) stop("no active graphics device");
    
    .Call("showtextEnd", PACKAGE = "showtext");
    invisible(NULL);
}
