ifeq (${CXX},)
CXX=g++
endif 
LINK=${CXX}

TOPDIR= `pwd`
INCPATH =-I. -I${TOPDIR}/include
SRC =$(wildcard ./src/*.cpp) #$(wildcard *.cpp)
LIBPATH=${TOPDIR}/.libs
OUTDIR = .libs_

target=exe
.PHONY: all clean

all: $(target)

$(OUTDIR):
	mkdir $@

$(target): shared
	g++ main.cpp -L. -lshared -o $@
#	$(LINK) $(LDFLAGS) $(INCPATH) $^ -o "$@" $(LIB)
shared: lib
	g++ -c -fPIC shared.cpp -L$(INCPATH)
	mv shared.o .libs
	g++ -shared ./.libs/shared.o -L./.libs -lcompute -o libshared.so

lib:
	gcc -c -fPIC $(SRC)
	mv add.o .libs
	mv mul.o .libs
	#gcc -c -fPIC add.cpp mul.pp
	#mv $(TOPDIR)/src/*.o .
#	gcc -c -fPIC mul.cpp
	ar -rc ./.libs/libcompute.a ./.libs/*.o

clean:
	rm -f *.o *.a *.so main exe target
	rm -rf ./.libs/*
