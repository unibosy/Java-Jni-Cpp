#include "AgoraJavaRecording.h"
#include "string"
#include "iostream"
using namespace std;

#include "../cppwrapper/agorasdk/AgoraSdk.h"
using namespace agora;

#ifdef __cplusplus
extern "C" {
#endif

  class agoraEventHandler {
  
  };

  JNIEXPORT jboolean JNICALL Java_AgoraJavaRecording_createChannel(JNIEnv * env, jobject jobject1, jstring jni_appid, jstring jni_channelKey, 
      jstring jni_channelName, jint jni_uid, jobject jni_recordingConfig)
  {
    cout<<"enter Java_AgoraJavaRecording_createChannel"<<endl;
    const char* appId = NULL;
    appId = env->GetStringUTFChars(jni_appid,false);
    cout <<"appId:"<<appId<<endl;
    if(!appId)
      cerr<<"get appId is NULL"<<endl;
    const char* channelKey = NULL;
    channelKey = env->GetStringUTFChars(jni_channelKey,false);
    cout<<"channelKey:"<<channelKey<<endl;
    if(!channelKey)
      cerr<<"no channel key"<<endl;
    const char* channelName = env->GetStringUTFChars(jni_channelName,false);
    cout<<"channelName:"<<channelName<<endl;
    if(!channelName)
      cerr<<"channel name is empty!"<<endl;
    int uid = 0;
    recording::RecordingConfig config;
    AgoraSdk recorder;
    int ret = recorder.createChannel(appId, channelKey, channelName, uid, config);
		
    cout<<"agora sdk createChannel ret:"<<ret<<endl;
    cout<<"end Java_AgoraJavaRecording_createChannel"<<endl;
    
  }


#ifdef __cplusplus
}
#endif
