ifeq (${CXX},)
CXX=g++
endif
LINK=${CXX}

TOPDIR=`pwd`/..
LIBPATH=${TOPDIR}/libs
CPP11 = -std=c++11
LDFLAGS= -shared
FPIC = -fPIC
CXXFLAGS  = -pipe -std=c++0x -fPIC -g -fno-omit-frame-pointer \
			-DNDEBUG=1 -Wconversion -O3 -Wall -W #-fvisibility=hidden
					
LIB	   = -pthread -lpthread -L$(LIBPATH) -lrecorder -lrt
INCPATH =-I. -I${TOPDIR}/include -I${TOPDIR}/include/base -I${TOPDIR}/src/base -I${TOPDIR}/src/agorasdk -I${TOPDIR}/src/jni
JNIPATH = -I. -I/usr/lib/jvm/java-9-openjdk-amd64/include/ -I/usr/lib/jvm/java-9-openjdk-amd64/include/linux/
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
	g++ -c $(CXXFLAGS) $(INCPATH) ./agorasdk/AgoraSdk.cpp
	g++ ./jni_proxy/src/proxy.cpp -o $@ $(LDFLAGS) $(INCPATH) $(JNIPATH) $(CPP11) $(CXXFLAGS) AgoraSdk.o opt_parser.o $(LIB)
	mv $@ ../../../bin
	#g++ $(CXXFLAGS) $(LDFLAGS) AgoraSdk.o $^ -o "$@" $(LIB)

$(OBJ):
	g++ -c $(CXXFLAGS) $(INCPATH) ./base/opt_parser.cpp
	
#%.o: %.cpp
#	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

clean:
	rm -f $(TARGET)
	rm -f ${OBJ}
	rm -f *.o
	rm -f libWrapper.so
