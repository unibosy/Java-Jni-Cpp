CC = gcc
CFLAGS = -g
RM = rm -f 

TOPDIR=$(pwd)
INCLUDEPATH=-I. -I /usr/lib/jvm/java-9-openjdk-amd64/include/linux/

target:JavaRecorder

JavaRecorder :
	$(CC) $(CFLAGS) -o libRecording.so hello.c
clean veryclean:
	$(RM) Hello
