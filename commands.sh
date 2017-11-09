javac src/include/*.java -d bin
javac src/*.java -d bin
javah -d native_layer/jni -classpath bin AgoraJavaRecording


gcc -o native_layer/jni_library/librecording.so -shared -fPIC -I /usr/lib/jvm/java-9-openjdk-amd64/include/ native_layer/jni/AgoraJavaRecording.cpp -lc -I /usr/lib/jvm/java-9-openjdk-amd64/include/linux/
