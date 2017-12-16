#!/bin/sh

build()
{
  CLASSPATH=`pwd`/bin
  export CLASSPATH

  binDir="bin"
  [ -d "$binDir" ] && rmdir "$binDir"
  mkdir "$binDir"

  #jni layer
  jniLayer="./jni_code/jni_headers"

  javac java_code/headers/*.java -d bin
  javac java_code/*.java -d bin -Xlint:unchecked
  javah -d $jniLayer -classpath ./bin AgoraJavaRecording
  #make -f ./jni_code/.makefile
   
  echo "build ok!"
}
clean()
{
  make clean -f ./jni_code/.makefile
  rm -f bin/*.class
  rm -rf bin/headers
  echo "clean ok!"
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
