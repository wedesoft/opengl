.SUFFIXES: .pdf .tex .svg .png .jpg

SVGS = $(wildcard *.svg)

IMAGES = $(wildcard *.png) $(wildcard *.jpg) $(SVGS:.svg=.pdf)

opengl.pdf: opengl.tex $(IMAGES)
	pdflatex -shell-escape $<

images: $(IMAGES)

.svg.pdf:
	rsvg-convert -f pdf -o $@ $<
