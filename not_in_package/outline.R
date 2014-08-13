library(asciir)
outline = char2outline(family = "sans", fontface = 3)
seg = char2seg(family = "sans", fontface = 3)
seg3 = char2seg(family = "sans", fontface = 3, nseg = 3)

pdf("outline-0.pdf")
op = par(mar = c(0, 0, 0, 0))
plot(1, type = "n", xlim = range(seg$x, na.rm = TRUE),
     ylim = range(seg$y, na.rm = TRUE), asp = 1)
polypath(seg$x, seg$y, col = "black")
with(subset(outline, type != "B"),
     points(x, y, pch = 16, col = rgb(70, 130, 180, 150, max = 255), cex = 3))
with(subset(outline, type == "B"),
     points(x, y, pch = 16, col = rgb(255, 0, 0, 150, max = 255), cex = 3))
par(op)
dev.off()

pdf("outline-1.pdf", 6, 6)
op = par(mar = c(0, 0, 0, 0))
plot(1, type = "n", xlim = range(seg$x, na.rm = TRUE),
     ylim = range(seg$y, na.rm = TRUE), asp = 1)
polypath(seg$x, seg$y, col = "black")
par(op)
dev.off()

pdf("outline-2.pdf", 6, 6)
op = par(mar = c(0, 0, 0, 0))
plot(1, type = "n", xlim = range(seg$x, na.rm = TRUE),
     ylim = range(seg$y, na.rm = TRUE), asp = 1)
with(subset(outline, type != "B"),
     points(x, y, pch = 16, col = rgb(70, 130, 180, 150, max = 255), cex = 3))
with(subset(outline, type == "B"),
     points(x, y, pch = 16, col = rgb(255, 0, 0, 150, max = 255), cex = 3))
par(op)
dev.off()

pdf("outline-3.pdf", 6, 6)
op = par(mar = c(0, 0, 0, 0))
plot(1, type = "n", xlim = range(seg$x, na.rm = TRUE),
     ylim = range(seg$y, na.rm = TRUE), asp = 1)
polypath(seg3$x, seg3$y, border = "steelblue")
points(seg3$x, seg3$y, pch = 16)
par(op)
dev.off()
