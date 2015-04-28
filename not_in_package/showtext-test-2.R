library(showtext)
font.add.google("Gochi Hand", "gochi")
font.add.google("Schoolbell", "bell")
font.add.google("Covered By Your Grace", "grace")
font.add.google("Rock Salt", "rock")


showtext.auto()

plot_fun = function()
{
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
}


png("demo.png", 700, 600, res = 96)
plot_fun()
dev.off()

jpeg("demo.jpeg", 700, 600, res = 96)
plot_fun()
dev.off()

tiff("demo.tiff", 700, 600, res = 96)
plot_fun()
dev.off()

pdf("demo.pdf", 7, 6)
plot_fun()
dev.off()

svg("demo.svg", 7, 6)
plot_fun()
dev.off()

setEPS()
postscript("demo.ps", width = 7, height = 6)
plot_fun()
dev.off()

x11()
plot_fun()




