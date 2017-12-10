#include <jni.h>



class JniHelper
{
public:
	JniHelper();
	~JniHelper();
public:
	void init();
	void destory();

public:
	JNIEnv *env;
	JavaVM *jvm;

};
