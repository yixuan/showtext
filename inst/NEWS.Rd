\name{NEWS}
\title{News for Package "showtext"}

\section{Changes in showtext version 0.8}{
  \itemize{
    \item Fixed a resolution issue for bitmap graphics devices in the \pkg{Cairo} package
          (\href{https://github.com/yixuan/showtext/issues/33}{#33})
    \item Updated the list of supported bitmap graphics devices
    \item Updated the vignette and the README file to introduce a workaround for the
          RStudioGD issue (\href{https://github.com/yixuan/showtext/issues/7}{#7})
  }
}

\section{Changes in showtext version 0.7-1}{
  \itemize{
    \item Fixed an issue for the MS Gothic font
          (\href{https://github.com/yixuan/showtext/issues/29}{#29})
  }
}

\section{Changes in showtext version 0.7}{
  \itemize{
    \item Now a new rule for the default font family is made: if the user does not
          specify a font family, then the default one will be used to
          render the text. If all characters in the string have Unicodes smaller than 1024,
          then the default font family is "sans"; otherwise "wqy-microhei" is used
    \item Fixed the \code{VignetteBuilder} entry in the \code{DESCRIPTION} file
          according to CRAN's policy
  }
}

\section{Changes in showtext version 0.6}{
  \itemize{
    \item Fixed a device crash when labels are blank
          (\href{https://github.com/yixuan/showtext/issues/20}{#20})
    \item Updated the README and vignette to introduce some new features
  }
}

\section{Changes in showtext version 0.5-1}{
  \itemize{
    \item Fixed errors in building the vignette
    \item Fixed installation problems on Windows
  }
}

\section{Changes in showtext version 0.5}{
  \itemize{
    \item All previous API functions now have aliases replacing the dots
          with underscores in the function names, for example \code{showtext_auto()}
          is equivalent to \code{showtext.auto()}. The "underscore" naming is
          preferred, and the "dot" version will be gradually \strong{deprecated}
    \item Fixed PROTECT errors detected by CRAN
    \item Registered native routines per CRAN's policy
  }
}

\section{Changes in showtext version 0.4-6}{
  \itemize{
    \item Fixed a UBSAN issue 
    \item Added a cleanup script per CRAN's policy
  }
}

\section{Changes in showtext version 0.4-5}{
  \itemize{
    \item Fixed the text() device function for string beginning with "\\n"
          (\href{https://github.com/yixuan/showtext/issues/9}{#9})
    \item Added a package vignette
  }
}

\section{Changes in showtext version 0.4-4}{
  \itemize{
    \item Fixed a memory leak problem
  }
}

\section{Changes in showtext version 0.4-3}{
  \itemize{
    \item Fixed a compilation conflict with FreeType 2.6, thanks to Kurt Hornik
  }
}

\section{Changes in showtext version 0.4-2}{
  \itemize{
    \item When the requested font family is not found, the program will give
          warnings, and then the default font will be used
          (previously this was done silently)
  }
}

\section{Changes in showtext version 0.4-1}{
  \itemize{
    \item Fixed bugs in calculating metric information of characters
  }
}

\section{Changes in showtext version 0.4}{
  \itemize{
    \item New function \code{shwotext.auto()} to automatically call
          \code{showtext.begin()} in any newly opened graphics devices,
          saving a lot of typing
    \item Better visual effect for a number of bitmap and on-screen devices
          including \code{png()}, \code{jpeg()}, \code{tiff()}, \code{bmp()},
          \code{x11()} and \code{windows()}. For these devices we use
          raster image rather than polygons to draw text
    \item Options setting has been moved to a new function \code{showtext.opts()}
          (previously \code{nseg} is in \code{showtext.begin()})
    \item Fixed bug in setting device parameter \code{canHAdj}
  }
}

\section{Changes in showtext version 0.3}{
  \itemize{
    \item Font files have been moved to the \pkg{showtextdb} package
    \item Fixed potential problem that may crash the graphics device
  }
}

\section{Changes in showtext version 0.2-2}{
  \itemize{
    \item The included font has been compressed to reduce package size
  }
}

\section{Changes in showtext version 0.2-1}{
  \itemize{
    \item Updated README
    \item Eliminated some warnings of CRAN check
  }
}

\section{Changes in showtext version 0.2}{
  \itemize{
    \item Now depends on the \pkg{sysfonts} package
  }
}

\section{Changes in showtext version 0.1}{
  \itemize{
    \item Initial release
  }
}