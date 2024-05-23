.SUFFIXES: .pdf .tex .svg .png .jpg .c

SVGS = $(wildcard *.svg)

IMAGES = $(wildcard *.png) $(wildcard *.jpg) $(SVGS:.svg=.pdf)

all: opengl-example opengl.pdf

opengl.pdf: opengl.tex $(IMAGES)
	pdflatex -shell-escape $<

opengl-example: opengl-example.c
	gcc -o $@ $< -lglfw -lGLEW -lGL -lm

clean:
	rm -Rf *.pdf opengl-example _minted-opengl *.aux *.log *.out

.svg.pdf:
	rsvg-convert -f pdf -o $@ $<
