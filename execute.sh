#!/bin/sh
CLASSPATH=`pwd`/bin
export CLASSPATH
javac src/include/*.java -d bin
javac src/*.java -d bin
javah -d ./native_layer/cppwrapper/src/jni/ -classpath ./bin AgoraJavaRecording

