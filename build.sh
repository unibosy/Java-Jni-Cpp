#!/bin/sh

CLASSPATH=`pwd`/bin
export CLASSPATH

binDir="bin"
[ -d "$binDir" ] && rmdir "$binDir"
mkdir "$binDir"

#jni layer
jniLayer="./native_layer/cppwrapper/src/jni_proxy/jni_headers"

javac java/headers/*.java -d bin
javac java/*.java -d bin
javah -d $jniLayer -classpath ./bin AgoraJavaRecording

