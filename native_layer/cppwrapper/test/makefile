ifeq (${CXX},)
CXX=g++
endif
LINK=${CXX}
AR=ar
TOPDIR=`pwd`
LIBPATH=${TOPDIR}/libs
#-static-libstdc++ 
LDFLAGS= -static-libgcc -std=c++11
CXXFLAGS  = -pipe -std=c++0x -fPIC -g -fno-omit-frame-pointer \
			-DNDEBUG=1 -Wconversion -O3 -Wall -W -fvisibility=hidden
					
LIB	   = -pthread -lpthread -L$(LIBPATH) -lRecordEngine -lrt

INCPATH =-I. -I${TOPDIR}/include

SRC =$(wildcard ./base/*.cpp) $(wildcard ./agorasdk/*.cpp)
OBJ=$(addsuffix .o, $(basename $(SRC)))
TARGET=libcppwrapper.a

.PHONY: all clean
all: $(TARGET)

$(TARGET): $(OBJ)
	ar rcs $(TARGET) $(OBJ)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

clean:
	rm -f $(TARGET)
	rm -f ${OBJ}
