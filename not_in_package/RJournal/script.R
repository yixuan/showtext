cleanfile = function(filename)
{
    doc = readLines(filename)
    first = grep("^\\\\title", doc)
    delete1 = grep("^\\\\IfFileExists", doc)
    doc[(delete1 - 1):(delete1 + 1)] = ""
    delete2 = grep("^\\\\bibliographystyle", doc)
    doc[delete2:(delete2 + 2)] = ""
    writeLines(doc[first:(length(doc) - 1)], filename)
}

library(knitr)
knit("qiu.Rnw")
cleanfile("qiu.tex")
