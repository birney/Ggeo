
GGEO_OBJ = ggeo_feature.o \
	ggeo_inmemory.o

CC = cc
CFLAGS =  -Wall -pthread -g3 -O2 -c  -DUNIX -I../base/ `pkg-config --cflags cairo`


test_inmemory : test_inmemory.o $(GGEO_OBJ)
	cc -o test_inmemory test_inmemory.o $(GGEO_OBJ) -lm
