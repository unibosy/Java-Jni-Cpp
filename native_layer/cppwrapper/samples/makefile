ifeq (${CXX},)
CXX=g++
endif
LINK=${CXX}

TOPDIR=`pwd`/..
LIBPATH=${TOPDIR}/libs
#-static-libstdc++
CPP11 = -std=c++11
LDFLAGS= -shared
FPIC = -fPIC
CXXFLAGS  = -pipe -std=c++0x -fPIC -g -fno-omit-frame-pointer \
			-DNDEBUG=1 -Wconversion -O3 -Wall -W -fvisibility=hidden
					
LIB	   = -pthread -lpthread -L$(LIBPATH) -lRecordEngine -lrt
INCPATH =-I. -I${TOPDIR}/include -I${TOPDIR}/include/base -I${TOPDIR}/samples/base -I${TOPDIR}/samples/agorasdk

OBJ = opt_parser.o

TARGET=libWrapper.so

.PHONY: all clean
all: $(TARGET)

$(TARGET): $(OBJ)
	g++ -c $(FPIC) $(CPP11) ./agorasdk/AgoraSdk.cpp $(INCPATH)
	g++ AgoraSdk.o $(LDFLAGS) $^ -o "$@" $(LIB)

$(OBJ):
	gcc -c $(FPIC) $(CPP11) ./base/opt_parser.cpp $(INCPATH)
	
#%.o: %.cpp
#	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

clean:
	rm -f $(TARGET)
	rm -f ${OBJ}
	rm -f *.o
