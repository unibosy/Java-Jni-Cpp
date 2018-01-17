#include "jni.h"
#include "string"
using namespace std;
/**
 *
 * Cache java objects' field/method ids
 *
 */
class Helper{
public:
  static jmethodID safeGetMethodID(JNIEnv* env, jclass clazz, const char* name, const char* sig);
};
