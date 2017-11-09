#include "Hello.h"


#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     Hello
 * Method:    add
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_Hello_add
  (JNIEnv * env, jclass cls, jint i, jint j){
  
    return i+j;
  }

#ifdef __cplusplus
}
#endif

