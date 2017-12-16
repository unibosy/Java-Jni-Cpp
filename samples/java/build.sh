#!/bin/bash

build_java()
{
  CLASSPATH=`pwd`/bin
  export CLASSPATH
  
  LD_LIBRARY_PATH=`pwd`/bin
  export LD_LIBRARY_PATH

  binDir="bin"
  [ -d "$binDir" ] && rmdir "$binDir"
  mkdir "$binDir"

  #jni layer
  jniLayer="./jni_code/jni_headers"

  javac java_code/headers/*.java -d bin
  javac java_code/*.java -d bin -Xlint:unchecked
  javah -d $jniLayer -classpath ./bin AgoraJavaRecording
}

build_cpp()
{
  make -f ./jni_code/.makefile
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
}

if [ $# -eq "0" ];then
  cmdhelp $0
  exit
fi

for param in $@
do
  case $param in
       "build")
          build
          ;;
       "clean")
          clean
          ;;
       *)
       cmdhelp $0
       exit 1
       ;;
  esac
done
