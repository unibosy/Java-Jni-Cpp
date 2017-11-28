ifeq (${CXX},)
CXX=g++
endif 
LINK=${CXX}

TOPDIR= `pwd`
INCPATH =-I. -I${TOPDIR}/include
SRC =$(wildcard ./src/*.cpp)
target=exe
exe:shared
	g++ main.cpp -L. -lshared -o $@
shared:lib
	g++ -c -fPIC shared.cpp -L$(INCPATH)
	g++ -shared shared.o -L. -lcompute -o libshared.so

lib:
	gcc -c -fPIC add.cpp
	#mv $(TOPDIR)/src/*.o .
	gcc -c -fPIC mul.cpp
	ar -rc libcompute.a add.o mul.o
#	mkdir libs
#	cp libcompute.a libs

clean:
	rm -f *.o *.a *.so main exe
	rm -rf libs
