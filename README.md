## showtext<img src="https://statr.me/images/sticker-showtext.png" alt="showtext" height="150px" align="right" />

### What's this package all about?

**showtext** makes it easy to use various types of fonts (TrueType, OpenType,
Type 1, web fonts, etc.) in R plots. The motivation to develop this package
is that using non-standard fonts in R plots (especially for PDF device)
is not straightforward, for example, when creating PDF with Chinese characters.
This is because most of the standard fonts used by `pdf()` do not contain
Chinese character glyphs, and users could hardly use system fonts in R.

The [extrafont](https://github.com/wch/extrafont) package developed by
[Winston Chang](https://github.com/wch) is one nice solution to this problem,
which mainly focuses on using TrueType fonts (`.ttf`) in PDF graphics device.
Now **showtext** is able to support more font formats and more graphics devices,
and avoids using external software such as Ghostscript.

### A quick example

```r
library(showtext)
## Loading Google fonts (https://fonts.google.com/)
font_add_google("Gochi Hand", "gochi")
font_add_google("Schoolbell", "bell")
font_add_google("Covered By Your Grace", "grace")
font_add_google("Rock Salt", "rock")

## Automatically use showtext to render text for future devices
showtext_auto()

## Tell showtext the resolution of the device,
## only needed for bitmap graphics. Default is 96
showtext_opts(dpi = 96)

set.seed(123)
x = rnorm(10)
y = 1 + x + rnorm(10, sd = 0.2)
y[1] = 5
mod = lm(y ~ x)

op = par(cex.lab = 2, cex.axis = 1.5, cex.main = 2)
plot(x, y, pch = 16, col = "steelblue",
     xlab = "X variable", ylab = "Y variable", family = "gochi")
grid()
title("Draw Plots Before You Fit A Regression", family = "bell")
text(-0.5, 4.5, "This is the outlier", cex = 2, col = "steelblue",
     family = "grace")
abline(coef(mod))
abline(1, 1, col = "red")
par(family = "rock")
text(1, 1, expression(paste("True model: ", y == x + 1)),
     cex = 1.5, col = "red", srt = 20)
text(0, 2, expression(paste("OLS: ", hat(y) == 0.79 * x + 1.49)),
     cex = 1.5, srt = 15)
legend("topright", legend = c("Truth", "OLS"), col = c("red", "black"), lty = 1)

par(op)
```

<div align="center">
  <img src="https://i.imgur.com/7dmcchI.png" alt="quick_example" />
</div>

In this example we first load some fonts that are available online
through [Google Fonts](https://fonts.google.com/), and then tell R
to render text using **showtext** by calling the `showtext_auto()`
function. All the remaining part is exactly the same as the usual plotting
commands.

This example should work on most graphics devices, including `pdf()`,
`png()`, `postscript()`, and on-screen devices such as `windows()` on
Windows and `x11()` on Linux.

### How **showtext** works

Let me first explain a little bit how `pdf()` works.

To my best knowledge, the default PDF device in R does not "draw" the text,
but actually "describes" the text in the PDF file.
That is to say, instead of drawing lines and curves of the actual glyph,
it only embeds information about the text, for example what characters
it has, which font it uses, etc.

However, the text with declared font may be displayed differently in
different OS, which means that the appearance of graph created by `pdf()` is
system dependent. If you unfortunately do not have the declared font
in your system, you may not be able to see the text correctly at all. 

In comparison, **showtext** package tries to solve this problem by
converting text into color-filled polygonal outlines (for vector graphics)
or raster images (for bitmap and on-screen graphics), thus having the
same appearance under all platforms. People who view this graph do not
need to install the font that creates the graph. It provides convenience to
both graph makers and graph viewers.

More importantly, **showtext** can use system font files, so you can
show text in the graph with your favourite font face, as long as it
is supported by FreeType. See section **Loading fonts** below.

### The usage

To create a graph using a specified font, you simply do the following:

- (\*) Load the font to be used by **showtext**
- Open the graphics device
- (\*) Claim that you want to use **showtext** to draw the text
- Plot
- Close the device

Only the steps marked with (\*) are new. If you want to use
**showtext** globally, you can call the function `showtext_auto()`
once, and then all the devices after that will automatically use
**showtext** to render text, as the example in the beginning shows.

If you want to have finer control on which part of the code should use
**showtext**, functions `showtext_begin()` and `showtext_end()` will help.
Only plotting functions enclosed by this pair of calls will use **showtext**,
and others not. For example, to change the title font only, we can do:

```r
library(showtext)
font_add_google("Schoolbell", "bell")

## By default the automatic call of showtext is disabled
## You can manually turn it off using the line below
## showtext_auto(enable = FALSE)

## To use showtext_begin() and showtext_end() you need to
## explicitly open a graphics device
png("demo.png", 700, 600, res = 96)
set.seed(123)
x = rnorm(10)
y = 1 + x + rnorm(10, sd = 0.2)
y[1] = 5
mod = lm(y ~ x)

op = par(cex.lab = 1.5, cex.axis = 1.5, cex.main = 2)
plot(x, y, pch = 16, col = "steelblue",
     xlab = "X variable", ylab = "Y variable")
grid()

## Use showtext only for this part
showtext_begin()
title("Draw Plots Before You Fit A Regression", family = "bell")
showtext_end()

text(-0.5, 4.5, "This is the outlier", cex = 2, col = "steelblue")
abline(coef(mod))
abline(1, 1, col = "red")
text(1, 1, expression(paste("True model: ", y == x + 1)),
     cex = 1.5, col = "red", srt = 20)
text(0, 2, expression(paste("OLS: ", hat(y) == 0.79 * x + 1.49)),
     cex = 1.5, srt = 15)
legend("topright", legend = c("Truth", "OLS"), col = c("red", "black"), lty = 1)

par(op)
dev.off()
```

<div align="center">
  <img src="https://i.imgur.com/uZAJafE.png" alt="demo-2" />
</div>

### Loading fonts

Loading font is actually done by package **sysfonts**.

The easy way to load font into **showtext** is by calling `font_add(family, regular)`,
where `family` is the name that you assign to that font (so that later you can
call `par(family = ...)` to use this font in plotting), and `regular` is the
path to the font file. That is to say, only knowing the "font name" is not
enough, since they are usually system dependent. On the contrary, font file
is the entity that actually provides the character glyphs.

Usually the font files are located in some "standard" directories in the system 
(for example on Windows it is typically `C:\Windows\Fonts`).
You can use `font_paths()` to check the current search path or add a new one,
and use `font_files()` to list available font files in the search path.
`font_files()` also lists some other useful information, for example the family
name that you usually use to specify a font, and the font face for different
variants. Below is an example to show the results on my machine:

```r
head(font_files())

##                     path                           file               family
## 1 ***/.local/share/fonts            Flavors-Regular.ttf              Flavors
## 2 ***/.local/share/fonts FrederickatheGreat-Regular.ttf Fredericka the Great
## 3 ***/.local/share/fonts           GandhiSerif-Bold.otf         Gandhi Serif
## 4 ***/.local/share/fonts     GandhiSerif-BoldItalic.otf         Gandhi Serif
## 5 ***/.local/share/fonts         GandhiSerif-Italic.otf         Gandhi Serif
## 6 ***/.local/share/fonts        GandhiSerif-Regular.otf         Gandhi Serif
##
##          face       version                    ps_name
## 1     Regular Version 1.001            Flavors-Regular
## 2     Regular Version 1.001 FrederickatheGreat-Regular
## 3        Bold Version 1.001           GandhiSerif-Bold
## 4 Bold Italic Version 1.001     GandhiSerif-BoldItalic
## 5      Italic Version 1.001         GandhiSerif-Italic
## 6     Regular Version 1.001        GandhiSerif-Regular
```

And the code below demonstrates how to load and use system fonts on Windows:

```r
library(showtext)
## Add fonts that are available on Windows
font_add("heiti", "simhei.ttf")
font_add("constan", "constan.ttf", italic = "constani.ttf")

library(ggplot2)
p = ggplot(NULL, aes(x = 1, y = 1)) + ylim(0.8, 1.2) +
    theme(axis.title = element_blank(), axis.ticks = element_blank(),
          axis.text = element_blank()) +
    annotate("text", 1, 1.1, family = "heiti", size = 15,
             label = "\u4F60\u597D\uFF0C\u4E16\u754C") +
    annotate("text", 1, 0.9, label = 'Chinese for "Hello, world!"',
             family = "constan", fontface = "italic", size = 12)

## Automatically use showtext for new devices
showtext_auto()

## On-screen device
x11()
print(p)
dev.off()

## PDF device
pdf("showtext-example-3.pdf", 7, 4)
print(p)
dev.off()

## PNG device
ggsave("showtext-example-4.png", width = 7, height = 4, dpi = 96)

## Turn off if no longer needed
showtext_auto(FALSE)
```

<div align="center">
  <img src="https://i.imgur.com/Z3r9sg2.png" alt="example1" />
</div>

For other OS, you may not have the `simhei.ttf` font file, but there is no
difficulty in using something else. At present `font_add()` supports TrueType
fonts(\*.ttf/\*.ttc) and OpenType fonts(\*.otf), and adding new
font type is trivial as long as FreeType supports it.

Also, there are many free fonts available and accessible on the web, for instance
the Google Fonts project ([https://fonts.google.com/](https://fonts.google.com/)).
**sysfonts** provides an interface to automatically download and register those fonts
through the function `font_add_google()`, as the example below shows.

```r
library(showtext)
font_add_google("Lobster", "lobster")

showtext_auto()

plot(1, pch = 16, cex = 3)
text(1, 1.1, "A fancy dot", family = "lobster", col = "steelblue", cex = 3)
```

<div align="center">
  <img src="https://i.imgur.com/pO87LFy.png" alt="example2" />
</div>

### CJK fonts

**showtext** includes an open source CJK (Chinese, Japanese, and Korean) font
[WenQuanYi Micro Hei](http://wenq.org/wqy2/index.cgi?MicroHei%28en%29).
If you just want to show CJK text in your graph, simply specify the `wqy-microhei`
family name in plotting functions.

Another option is to install the
[Source Han Sans](https://github.com/adobe-fonts/source-han-sans)/[Serif](https://github.com/adobe-fonts/source-han-serif)
fonts locally using the following code:

```r
library(showtext)
font_install(source_han_serif())
font_families()
## [1] "sans"                "serif"               "mono"                "wqy-microhei"       
## [5] "source-han-serif-cn"
```

See `?font_install` and `?source_han` for more details.

<div align="center">
  <img src="https://i.imgur.com/oOejBrI.png" alt="source han" />
</div>

### The internals of **showtext**

Every graphics device in R implements some functions to draw specific graphical
elements, e.g., `path()` and `polygon()` to draw polygons, `raster()` to display
bitmap images, `text()` or `textUTF8()` to show text, etc. What `showtext` does
is to override their own text rendering functions and replace them by hooks
provided in **showtext** that will further call the device's `path()` or `raster()`
functions to draw the character glyphs.

This action is done only when you call `showtext_begin()` and won't modify the
graphics device if you call `showtext_end()` to restore the original device functions back.

### Compatibility with RStudio

Starting from version 0.9, **showtext** can work well with the RStudio graphics
device (RStudioGD). Simply call `showtext_auto()` in the RStudio session and then
the plots will be displayed correctly.
