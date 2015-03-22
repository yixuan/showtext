### What's this package all about?

`showtext` makes it easy to use various types of fonts (TrueType, OpenType,
Type 1, web fonts, etc.) in R graphs. It tries to do the following two things:

- Let R know about these fonts
- Use these fonts to draw text

The motivation to develop this package is that using non-standard
fonts in R graphs (especially for PDF device) is not straightforward,
for example, to create PDF graphs with Chinese characters.
This is because most of the standard fonts used by `pdf()` do not contain
Chinese character glyphs, and users could hardly use system fonts in R.

The [extrafont](https://github.com/wch/extrafont) package developed by
[Winston Chang](https://github.com/wch) is one nice solution to this problem,
which mainly focuses on using TrueType fonts(`.ttf`) in PDF graphics device.
Now `showtext` is able to support more font formats and more graphics devices,
and avoids using external software such as Ghostscript.

### How `showtext` works

Let me explain a little bit about how `pdf()` works.

To my best knowledge, the default PDF device in R does not "draw" the text,
but actually "describes" the text in the PDF file.
That is to say, instead of drawing lines and curves of the actual glyph,
it only embeds information about the text, for example what characters
it has, which font it uses, etc.

However, the text with declared font may be displayed differently in
different OS, which means that the appearance of graph created by `pdf()` is
system dependent. If you unfortunately do not have the declared font
in your system, you may not be able to see the text correctly at all. 

In comparison, `showtext` package tries to solve this problem by
converting text into polygonal outlines (for vector graphics)
or raster images (for bitmap and on-screen graphics), thus having the
same appearance under all platforms. More importantly, `showtext` can
use system font files, so you can show your text in any font that is supported.
This solves the Chinese character problem I mentioned in the beginning
because I can load my favorite Chinese font to R and use that to draw
text. Also, people who view this graph do not need to install the font
that creates the graph. It provides convenience to both graph makers
and graph viewers.

### The Usage

The `showtext` package is designed for "lazy" users.
To create a graph using a specified font, you only need to do:

- (\*) Load the font
- Open the graphics device
- (\*) Claim that you want to use `showtext` to draw the text
- Plot
- Close the device

Only the steps marked with (\*) are newly added. Below is an example:

```r
library(showtext)
## add fonts, available on Windows
font.add("heiti", "simhei.ttf")
font.add("constan", "constan.ttf", italic = "constani.ttf")

library(ggplot2)
p = ggplot(NULL, aes(x = 1, y = 1)) + ylim(0.8, 1.2) +
    theme(axis.title = element_blank(), axis.ticks = element_blank(),
          axis.text = element_blank()) +
    annotate("text", 1, 1.1, family = "heiti", size = 15,
             label = "\u4F60\u597D\uFF0C\u4E16\u754C") +
    annotate("text", 1, 0.9, label = 'Chinese for "Hello, world!"',
             family = "constan", fontface = "italic", size = 12)

pdf("showtext-example.pdf", 7, 4)
showtext.begin()
print(p)
showtext.end()
dev.off()

png("showtext-example-2.png", 700, 400)
showtext.begin()
print(p)
showtext.end()
dev.off()
```

<div align="center">
  <img src="http://i.imgur.com/Z3r9sg2.png" alt="example1" />
</div>

An even more convenient way to use `showtext` is to call the `showtext.auto()`
function, which will automatically use `showtext` for any newly opened devices.
The previous example can be simplified as follows:

```r
library(showtext)
## add fonts, available on Windows
font.add("heiti", "simhei.ttf")
font.add("constan", "constan.ttf", italic = "constani.ttf")

library(ggplot2)
p = ggplot(NULL, aes(x = 1, y = 1)) + ylim(0.8, 1.2) +
    theme(axis.title = element_blank(), axis.ticks = element_blank(),
          axis.text = element_blank()) +
    annotate("text", 1, 1.1, family = "heiti", size = 15,
             label = "\u4F60\u597D\uFF0C\u4E16\u754C") +
    annotate("text", 1, 0.9, label = 'Chinese for "Hello, world!"',
             family = "constan", fontface = "italic", size = 12)

showtext.auto()  ## automatically use showtext for new devices

print(p)  ## on-screen device

pdf("showtext-example-3.pdf", 7, 4)  ## PDF device
print(p)
dev.off()

ggsave("showtext-example-4.png", width = 7, height = 4, dpi = 96)  ## PNG device
```

This example should work fine on Windows. For other OS, you may not have
the `simhei.ttf` font file, but there is no difficulty in using something
else. You can see the next section to learn details about how to load
fonts with `showtext`.

### Loading font

Loading font is actually done by package `sysfonts`.

The easy way to load font into `showtext` is by calling `font.add(family, regular)`,
where `family` is the name that you give to that font (so that later you can
call `par(family = ...)` to use this font in plotting), and `regular` is the
path to the font file. Usually the font file will be located in some "standard"
directories in the system (for example on Windows it is typically C:/Windows/Fonts).
You can use `font.paths()` to check the current search path or add a new one,
and use `font.files()` to list available font files in the search path.

Also, there are many free fonts available and accessible on the web, for instance
the Google Fonts project ([https://www.google.com/fonts](https://www.google.com/fonts)).
`sysfonts` provides an interface to automatically download and register those fonts
through the function `font.add.google()`, as the example below shows.

```r
library(showtext)
font.add.google("Lobster", "lobster")

showtext.auto()

plot(1, pch = 16, cex = 3)
text(1, 1.1, "A fancy dot", family = "lobster", col = "steelblue", cex = 3)
```

<div align="center">
  <img src="http://i.imgur.com/pO87LFy.png" alt="example2" />
</div>

At present `font.add()` supports TrueType fonts(\*.ttf/\*.ttc) and
OpenType fonts(\*.otf), and adding new
font type is trivial as long as FreeType supports it.

Note that `showtext` includes an open source CJK font
[WenQuanYi Micro Hei](http://wenq.org/wqy2/index.cgi?MicroHei%28en%29).
If you just want to show CJK text in your graph, you do not need to add any
extra font at all.

### The internals of `showtext`

Every graphics device in R implements some functions to draw specific graphical
elements, e.g., `path()` and `polygon()` to draw polygons, `raster()` to display
bitmap images, `text()` or `textUTF8()` to show text, etc. What `showtext` does
is to override their own text rendering functions and replace them by hooks
provided in `showtext` that will further call the device's `path()` or `raster()`
functions to draw the character glyphs.

This action is done only when you call `showtext.begin()` and won't modify the
graphics device if you call `showtext.end()` to restore the original device functions back.

