#include "TestJNI.h"

TestJNI::TestJNI()
{
    options[0].optionString = "-Djava.class.path=.";
    vm_args.version = JNI_VERSION_1_2;
    vm_args.nOptions = 1;
    vm_args.options = options;
    vm_args.ignoreUnrecognized = JNI_TRUE;
    status = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
}

jstring TestJNI::NewJString(const char *str)
{
    if(status == JNI_ERR || str == NULL)
        return 0;
    jclass strClass = env->FindClass("Ljava/lang/String;");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jbyteArray bytes = env->NewByteArray(strlen(str));
    (env)->SetByteArrayRegion(bytes, 0, strlen(str), (jbyte*)str);
    jstring encoding = env->NewStringUTF("GB2312");
    return (jstring)(env)->NewObject(strClass, ctorID, bytes, encoding);
}

void TestJNI::say(string name)
{
    if(status != JNI_ERR){
        jclass cls = env->FindClass("leixf/Demo");
        jmethodID mid = env->GetMethodID(cls, "<init>", "(Ljava/lang/String;)V");
        jstring arg = NewJString(name.c_str());
        jobject demo = env->NewObject(cls, mid, arg);
        mid = env->GetMethodID(cls, "myCall", "()V");
        env->CallObjectMethod(demo, mid);
    }
}

int main()
{
    TestJNI test;
    test.say("xiaofeng");
		return 0;
}
