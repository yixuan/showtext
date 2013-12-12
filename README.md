### What's this package all about?
`showtext` is an R package to draw text strings in R graphs.

-- Wait, R already has `text()` function to do that...

Yes, but drawing text is a very complicated task, and it always depends on
the specific **Graphics Device**.
(Graphics device is the engine to create images.
For example, R provides PDF device, called by function `pdf()`,
to create graphs in PDF format)
Sometimes the graphics device doesn't support text drawing nicely,
**especially** in using fonts.

From my own experience, I find it always troublesome to create PDF
graphs with Chinese characters. This is because most of the standard
fonts used by `pdf()` don't contain Chinese character glyphs, and
even worse users could hardly use the fonts that are already installed
in their operating system. (It seems possible to do that, but quite
difficult and indirect for most users, I believe.)

`showtext` tries to do the following two things:

- Let R know about these system fonts
- Use these fonts to draw text

### Why `pdf()` doesn't work and how `showtext` works
Let me explain a little bit about how `pdf()` works.

To my best knowledge (may be wrong, so please point it out if I make
mistakes), the default PDF device of R doesn't "draw" the text,
but actually "describes" the text in the PDF file.
That is to say, instead of drawing lines and curves of the actual glyph,
it only embeds information about the text, for example what characters
it has, which font it uses, etc.

However, the text with declared font may be displayed differently in
different OS. The two images below are the screenshots of the same PDF
file created by R but viewed under Windows and Linux respectively.

This means that the appearance of graph created by `pdf()` is
system dependent. If you unfortunately don't have the declared font
in your system, you may not be able to see the text correctly at all. 

In comparison, `showtext` package tries to solve this problem by
converting text into lines and curves, thus having the same appearance
under all platforms. More importantly, `showtext` can use system font
files, so you can show your text in any font you want.
This solves the Chinese character problem I mentioned in the beginning
because I can load my favorite Chinese font to R and use that to draw
text. Also, people who view this graph don't need to install the font
that creates the graph. It provides convenience to both graph makers
and graph viewers.

### The Usage
Before I show the usage of `showtext` I shall reference a nice package
written by
[Winston Chang](https://github.com/wch),
the [extrafont](https://github.com/wch/extrafont) package.
It is essentially trying to the same thing: make R aware of the system
fonts, and then embed them in the PDF file.

This package is almost perfect except that it requires some kind of
configuration, either per R session or per PDF file. Also, it requires
Ghostscript to be installed. For lazy users (like me, for example),
it would be nicer if things are done automatically as much as possible.

The `showtext` package is definitely designed for lazy users.
To create a graph using a specified font, you only need to do:

- (\*) Load the font
- Open the graphics device
- (\*) Claim that you want to use `showtext` to draw the text
- Plot
- Close the device

Only the steps marked with (\*) are newly added. Below is an example:

```r
library(showtext)
font.add("fang", "simfang.ttf")
pdf("showtext-ex1.pdf")
showtext.begin()
plot(1, type = "n")
text(1, 1, intToUtf8(c(82, 35821, 35328)), cex = 10, family = "fang")
showtext.end()
dev.off()
```

The use of `intToUtf8()` is for convenience if you can't view or input
Chinese characters. You can instead use
```r
text(1, 1, "R语言", cex = 10, family = "fang")
```

This example should work fine on Windows. For other OS, you may not have
the `simfang.ttf` font file, but there is no difficulty in using something
else. You can see the next section to learn details about how to load
a font with `showtext`.

### Loading font
TODO

### The internals of `showtext`
TODO
