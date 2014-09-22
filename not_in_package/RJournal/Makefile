.PHONY: all clean

all: RJwrapper.pdf

RJwrapper.pdf: RJwrapper.tex qiu.tex
	pdflatex RJwrapper.tex
	bibtex RJwrapper
	pdflatex RJwrapper.tex
	pdflatex RJwrapper.tex

qiu.tex: qiu.Rnw
	Rscript script.R

qiu.Rnw: qiu.lyx
	lyx -e knitr qiu.lyx

clean:
	rm *.aux *.bbl *.blg *.brf *.log *.out
