#LD_LIBRARY_PATH=`pwd`/bin
#export LD_LIBRARY_PATH

#CLASSPATH=`pwd`/bin

#export CLASSPATH

javac *.java
g++ TestJNI.cpp -o test -I/usr/lib/jvm/java-9-openjdk-amd64/include/ -I/usr/lib/jvm/java-9-openjdk-amd64/include/linux/
