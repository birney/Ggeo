
GGEO_CAIRO = ggeo_cairo_render.o

CC = cc
CFLAGS =  -Wall -pthread -g3 -O2 -c -I../ `pkg-config --cflags cairo`


test_cairo_render : test_cairo_render.o $(GGEO_CAIRO)
	cc -g -o test_cairo test_cairo_render.o $(GGEO_CAIRO) ../ggeo_inmemory.o `pkg-config --libs cairo`