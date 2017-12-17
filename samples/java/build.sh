#!/bin/bash
#set jni path,you can get this by 'locate jni.h'
JNI_PATH='/usr/lib/jvm/java-9-openjdk-amd64/include'

build_java()
{
  CLASSPATH=`pwd`/bin
  export CLASSPATH
  
  LD_LIBRARY_PATH=`pwd`/bin
  export LD_LIBRARY_PATH
  
  jniLayer="./jni_code/jni_headers"
  javac java_code/headers/*.java -d bin
  javac java_code/*.java -d bin -Xlint:unchecked
  javah -d $jniLayer -classpath ./bin AgoraJavaRecording
}

build_cpp()
{
  make -f ./jni_code/.makefile JNIINCLUDEPATH=$JNI_PATH
}
clean_java()
{
  rm -f bin/*.class
  rm -rf bin/headers
}
clean_cpp()
{
  make clean -f ./jni_code/.makefile
}
build()
{
  build_java
  build_cpp
  echo "build all done!"
}
clean()
{ 
  clean_java
  clean_cpp
  echo "clean all done!"
}

cmdhelp()
{
  echo "Usage:"
  echo "$1 build"
  echo "$1 clean"

  #echo "$1 build_java"
  #echo "$1 build_cpp"
  #echo "$1 clean_java"
  #echo "$1 clean_cpp"
}
run()
{
  for param in $@
  do
    case $param in
       "build")
          build
          ;;
       "clean")
          clean
          ;;
       "build_java")
          #build_java
          ;;
       "build_cpp")
          #build_cpp
          ;;
        "clean_java")
          #clean_java
          ;;
        "clean_cpp")
          #clean_cpp
          ;;
       *)
       cmdhelp $0
       ;;
    esac
  done
}
if [ $# -eq "0" ];then
  cmdhelp $0
else
  run $1
fi
