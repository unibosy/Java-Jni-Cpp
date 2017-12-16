ifeq (${CXX},)
CXX=g++
endif
LINK=${CXX}

TOPDIR=`pwd`
COMMONPATH=`pwd`/../..
LIBPATH=${COMMONPATH}/libs
#set your jvm path!!!
JNIINCLUDE = /usr/lib/jvm/java-9-openjdk-amd64/include/
CPP11 = -std=c++11
LDFLAGS= -shared
FPIC = -fPIC
CXXFLAGS  = -pipe -std=c++0x -fPIC -g -fno-omit-frame-pointer \
			-DNDEBUG=1 -Wconversion -O3 -Wall -W #-fvisibility=hidden
					
LIB	   = -pthread -lpthread -L$(LIBPATH) -lrecorder -lrt
INCPATH =-I. -I${COMMONPATH}/include -I${COMMONPATH}/include/base -I${COMMONPATH}/samples/base -I${COMMONPATH}/samples/agorasdk -I${COMMONPATH}/samples
#-I:q:${TOPDIR}/src/jni
JNIPATH = -I. -I/$(JNIINCLUDE) -I/$(JNIINCLUDE)/linux/
OBJ = opt_parser.o

REALTARGET = exe
TARGET=librecording.so

.PHONY: all clean
#all: $(REALTARGET)
all: $(TARGET)
#$(REALTARGET): $(TARGET)
#	g++ cppwrapper.cpp -o $@ $(LDFLAGS) $(INCPATH) $(CPP11) $(CXXFLAGS) AgoraSdk.o opt_parser.o $(LIB)

#$(REALTARGET): $(TARGET)
#	g++ main.cpp -L. -lWrapper -o exe

$(TARGET): $(OBJ)
	g++ -c $(CXXFLAGS) $(INCPATH) ${COMMONPATH}/samples/agorasdk/AgoraSdk.cpp -d .
	g++ ./jni_code/proxy.cpp -o $@ $(LDFLAGS) $(INCPATH) $(JNIPATH) $(CPP11) $(CXXFLAGS) AgoraSdk.o opt_parser.o $(LIB) -I.
	mv $@ ../bin

$(OBJ):
	g++ -c $(CXXFLAGS) $(INCPATH) ${COMMONPATH}/samples/base/opt_parser.cpp

clean:
	rm -f ../bin/$(TARGET)
	rm -f ${OBJ}
	rm -f *.o
