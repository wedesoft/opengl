.SUFFIXES: .pdf .tex .svg .png .jpg

IMAGES = $(wildcard *.png) $(wildcard *.jpg)

opengl.pdf: opengl.tex $(IMAGES)
	pdflatex -shell-escape $<

images: $(IMAGES)

.svg.pdf:
	rsvg-convert -f pdf -o $@ $<
