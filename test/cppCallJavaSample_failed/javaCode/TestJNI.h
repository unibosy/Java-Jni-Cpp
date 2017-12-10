#ifndef _TESTJNI_
#define _TESTJNI_

#include <jni.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;

class TestJNI
{
    public:
        TestJNI();
        jstring NewJString(const char *str);
        void say(string name);
    private:
        JavaVMOption options[1];
        JNIEnv *env;
        JavaVM *jvm;
        JavaVMInitArgs vm_args;
        long status;
};
#endif
