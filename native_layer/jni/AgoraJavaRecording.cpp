#include "AgoraJavaRecording.h"
#include "string"
#include "iostream"
using namespace std;

#include "../cppwrapper/agorasdk/AgoraSdk.h"


#ifdef __cplusplus
extern "C" {
#endif

  class agoraEventHandler : public  {
  
  };

  JNIEXPORT jboolean JNICALL Java_AgoraJavaRecording_createChannel(JNIEnv * env, jobject jobject1, jstring jni_appid, jstring jni_channelKey, 
      jstring jni_channelName, jint jni_uid, jobject jni_recordingConfig)
  {
    cout<<"enter Java_AgoraJavaRecording_createChannel"<<endl;
    const char* appid = NULL;
    appid = env->GetStringUTFChars(jni_appid,false);
    cout <<"appid:"<<appid<<endl;
    if(!appid)
      cerr<<"get appid is NULL"<<endl;
    const char* channelKey = NULL;
    channelKey = env->GetStringUTFChars(jni_channelKey,false);
    cout<<"channelKey:"<<channelKey<<endl;
    if(!channelKey)
      cerr<<"no channel key"<<endl;
    const char* channelName = env->GetStringUTFChars(jni_channelName,false);
    cout<<"channelName:"<<channelName<<endl;
    if(!channelName)
      cerr<<"channel name is empty!"<<endl;

    cout<<"end Java_AgoraJavaRecording_createChannel"<<endl;
    
  }


#ifdef __cplusplus
}
#endif
