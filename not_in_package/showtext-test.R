plottask = function()
{
    plot(1, type = "n")

    op = par(family = "sans", cex = 2)
    text(0.7, 1.3, "Sans-R", font = 1)
    text(0.7, 1.1, "Sans-B", font = 2)
    text(0.7, 0.9, "Sans-I", font = 3)
    text(0.7, 0.7, "Sans-BI", font = 4)
    par(op)
    
    op = par(family = "serif", cex = 2)
    text(1.0, 1.3, "Serif-R", font = 1)
    text(1.0, 1.1, "Serif-B", font = 2)
    text(1.0, 0.9, "Serif-I", font = 3)
    text(1.0, 0.7, "Serif-BI", font = 4)
    par(op)
    
    op = par(family = "mono", cex = 2)
    text(1.3, 1.3, "Mono-R", font = 1)
    text(1.3, 1.1, "Mono-B", font = 2)
    text(1.3, 0.9, "Mono-I", font = 3)
    text(1.3, 0.7, "Mono-BI", font = 4)
    par(op)
}



pdf("pdf-test.pdf")
plottask()
showtext.begin()
plottask()
showtext.end()
plottask()
dev.off()


png("png-test-%03d.png")
plottask()
showtext.begin(20)
plottask()
showtext.end()
plottask()
dev.off()


png("cairo-png-test-%03d.png", type = "cairo")
plottask()
showtext.begin(20)
plottask()
showtext.end()
plottask()
dev.off()


library(Cairo)
CairoPNG("cairo-test-%03d.png")
plottask()
showtext.begin(20)
plottask()
showtext.end()
plottask()
dev.off()


svg("svg-test-%03d.svg")
plottask()
showtext.begin()
plottask()
showtext.end()
plottask()
dev.off()
