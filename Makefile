.SUFFIXES: .pdf .tex .svg .png .jpg .c

SVGS = $(wildcard *.svg)

IMAGES = $(wildcard *.png) $(wildcard *.jpg) $(SVGS:.svg=.pdf)

all: opengl.pdf opengl-example

opengl.pdf: opengl.tex $(IMAGES)
	pdflatex -shell-escape $<

opengl-example: opengl-example.c
	gcc -o $@ $< -lglfw -lGL

images: $(IMAGES)

.svg.pdf:
	rsvg-convert -f pdf -o $@ $<
