#include "helper.h"


jmethodID Helper::safeGetMethodID(JNIEnv* env, jclass clazz, const char* name, const char* sig)
{
  //env->ExceptionClear();
  jmethodID mid = env->GetMethodID(clazz, name, sig);
  if (env->ExceptionCheck() || !mid) {
    //log(LOG_ERROR, "exception occurred at jni call GetMethodID('%s')", name);
    env->ExceptionClear();
    mid = 0;
  }
  return mid;
}
