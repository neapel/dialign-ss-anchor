#MK=latexmk -shell-escape -f -pdf -pdflatex=lualatex

#all :
#	$(MK) report
#cont :
#	$(MK) -pvc report

all : report.pdf

%.tex : %.Rnw
	R CMD Sweave $^

%.pdf : %.tex
	R CMD lualatex $^
	R CMD lualatex $^
