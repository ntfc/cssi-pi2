ENCODING=utf-8
LANG=pt_PT

REPORT_FILE=00relatz.tex
REPORT_NAME=00relatz

BIBTEX=
pdf:
	pdflatex $(REPORT_FILE)
	bibtex $(REPORT_NAME)
	pdflatex $(REPORT_FILE)
	pdflatex $(REPORT_FILE)
	mv -v $(REPORT_NAME).pdf report.pdf

# TODO: aspell

clean:
	@rm -v *.{aux,bbl,blg,log,nlo,out,toc,bak}

aspell:
	for i in *.tex ; do\
	 	aspell --encoding=$(ENCODING) --lang=$(LANG) check $$i; \
	done
