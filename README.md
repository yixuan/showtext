### What's this package all about?
`showtext` is an R package to draw text in R graphs using system fonts.
It tries to do the following two things:

- Let R know about these system fonts
- Use these fonts to draw text

The motivation to develop this package is that using non-standard
fonts in PDF device is not straightforward, for example, to create PDF
graphs with Chinese characters. This is because most of the standard
fonts used by `pdf()` do not contain Chinese character glyphs, and
users could hardly use the fonts that are already installed
in their operating systems.

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
converting text into lines and curves, thus having the same appearance
under all platforms. More importantly, `showtext` can use system font
files, so you can show your text in any font you want.
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
library(ggplot2)
bg = ggplot(NULL, aes(x = 1, y = 1)) + ylim(0.8, 1.2) +
    theme(axis.title = element_blank(), axis.ticks = element_blank(),
          axis.text = element_blank())
txt1 = annotate("text", 1, 1.1, family = "heiti", size = 15,
                label = intToUtf8(c(20320, 22909, 65292, 19990, 30028, 65281)))
txt2 = annotate("text", 1, 0.9, label = 'Chinese for "Hello, world!"',
                family = "constan", fontface = "italic", size = 12)

library(showtext)
## add fonts, available on Windows
font.add("heiti", "simhei.ttf")
font.add("constan", "constan.ttf", italic = "constani.ttf")
pdf("showtext-example.pdf", 7, 4)
showtext.begin()
print(bg + txt1 + txt2)
showtext.end()
dev.off()
```

<div align="center">
  <img src="http://i.imgur.com/Z3r9sg2.png" alt="example1" />
</div>

The use of `intToUtf8()` is for convenience if you can't view or input
Chinese characters. You can instead use
```r
label = "你好，世界！"
```

This example should work fine on Windows. For other OS, you may not have
the `simhei.ttf` font file, but there is no difficulty in using something
else. You can see the next section to learn details about how to load
a font with `showtext`.

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

library(Cairo)
CairoPNG("showtext-example-2.png", 504, 504)
showtext.begin()
plot(1, pch = 16, cex = 3)
text(1, 1.1, "A fancy dot", family = "lobster", col = "steelblue", cex = 3)
showtext.end()
dev.off()
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

### Known issues
The image created by bitmap graphics devices (`png()`, `jpeg()`, ...)
looks ugly because they don't support anti-alias feature well. To produce
high-quality output, try to use the
[Cairo](http://cran.r-project.org/web/packages/Cairo/index.html) package.

### The internals of `showtext`
Every graphics device in R implements some functions to draw specific graphical
elements, e.g., `line()` to draw lines, `path()` and `polygon()` to draw polygons,
`text()` or `textUTF8()` to show text, etc. What `showtext` does is to override
their own text rendering functions and replace them by hooks provided in `showtext`
that will further call the device's `path()`, `polygon()` or `line()` to draw the
character glyphs.

This action is done only when you call `showtext.begin()` and won't modify the
graphics device if you call `showtext.end()` to restore the original device functions back.

