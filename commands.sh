CLASSPATH=/home/wangshiwei/ars/code_ars/javaDemo/bin

export CLASSPATH
javac src/include/*.java -d bin
javac src/*.java -d bin
javah -d native_layer/jni -classpath bin AgoraJavaRecording

gcc -o native_layer/jni_library/librecording.so -shared -fPIC -I /usr/lib/jvm/java-9-openjdk-amd64/include/ native_layer/jni/AgoraJavaRecording.cpp -lc -I /usr/lib/jvm/java-9-openjdk-amd64/include/linux/ -std=c++11 -I/home/wangshiwei/ars/code_ars/javaDemo/native_layer/cppwrapper/include -I/home/wangshiwei/ars/code_ars/javaDemo/native_layer/cppwrapper

gcc -o native_layer/jni_library/librecording.so -shared -fPIC -I /usr/lib/jvm/java-9-openjdk-amd64/include/ native_layer/jni/AgoraJavaRecording.cpp -lc -I native_layer/cppwrapper/ -I /usr/lib/jvm/java-9-openjdk-amd64/include/linux/ -std=c++11 -I native_layer/cppwrapper/include/

gcc -o native_layer/jni_library/librecording.so -shared -fPIC -I /usr/lib/jvm/java-9-openjdk-amd64/include/ native_layer/jni/AgoraJavaRecording.cpp -lc -I native_layer/cppwrapper/ -I /usr/lib/jvm/java-9-openjdk-amd64/include/linux/ -std=c++11 -I native_layer/cppwrapper/include/
