//#include "../agorasdk/AgoraSdk.h"
#include "IAgoraLinuxSdkCommon.h"
//#include "../cppwrapper.h"
#include "../jni_headers/AgoraJavaRecording.h"

#include "jniEventHandler.h"

using std::string;
using std::cout;
using std::cerr;
using std::endl;

using agora::base::log;

atomic_bool_t g_bSignalStop;

void signal_handler(int signo) {
  (void)signo;

  // cerr << "Signal " << signo << endl;
  g_bSignalStop = true;
}

#ifdef __cplusplus
extern "C" {
#endif

//TODO
#if 0
  convert jobject jni_recordingConfig to c++ recordingConfig
#endif

JNIEXPORT jboolean JNICALL Java_AgoraJavaRecording_createChannel(JNIEnv * env, jobject jobject1, jstring jni_appid, jstring jni_channelKey, 
      jstring jni_channelName, jint jni_uid, jobject jni_recordingConfig)
{
	  cout<<"enter Java_AgoraJavaRecording_createChannel"<<endl;

		g_bSignalStop = false;
	  signal(SIGQUIT, signal_handler);
  	signal(SIGABRT, signal_handler);
  	signal(SIGINT, signal_handler);
  	signal(SIGPIPE, SIG_IGN);

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
    int uid = (int)jni_uid;
		if(uid < 0){
			cout<<"jni uid is smaller than 0, set 0!"<<endl;
			uid = 0;
		}
   		jclass jRecordingConfig =env->GetObjectClass(jni_recordingConfig); 
		if(jRecordingConfig == NULL){
			cout<<"jni_recordingConfig is NULL"<<endl;
			return JNI_FALSE;
		}

		jfieldID idleLimitSecFieldID = env->GetFieldID(jRecordingConfig, "idleLimitSec", "I");
		jfieldID appliteDirFieldID = env->GetFieldID(jRecordingConfig, "appliteDir", "Ljava/lang/String;");
		jfieldID channelProfileFieldID = env->GetFieldID(jRecordingConfig, "channelProfile", "Lheaders/EnumIndex$CHANNEL_PROFILE_TYPE;");
		jfieldID streamTypeFieldID = env->GetFieldID(jRecordingConfig, "streamType", "Lheaders/EnumIndex$REMOTE_VIDEO_STREAM_TYPE;");

		if (idleLimitSecFieldID == NULL ||appliteDirFieldID == NULL || channelProfileFieldID == NULL 
						|| streamTypeFieldID == NULL ){ cout <<"get fieldID failed!"<<endl;return JNI_FALSE;}
		//idle
		jint idleValue = env->GetIntField(jni_recordingConfig, idleLimitSecFieldID); 
		cout<<"idleLimitSec:"<<idleValue<<endl;
		//appliteDir
		jstring appliteDir = (jstring)env->GetObjectField(jni_recordingConfig, appliteDirFieldID);
		const char * c_appliteDir = env->GetStringUTFChars(appliteDir ,NULL);
		std::string str_appliteDir = c_appliteDir;
		env->ReleaseStringUTFChars(appliteDir,c_appliteDir);
		//CHANNEL_PROFILE_TYPE
#if 1
		jobject channelProfileObject = (env)->GetObjectField(jni_recordingConfig, channelProfileFieldID);
		//assert(channelProfileObject);
		jclass enumClass = env->GetObjectClass(channelProfileObject);
		if(enumClass == NULL)
		{
			cout <<"enumClass is null"<<endl;
		}
		jmethodID getValue = env->GetMethodID(enumClass, "getValue", "()I");
		if (getValue == NULL) {
			cout <<"method not found"<<endl;
			return JNI_FALSE; /* method not found */
		}
		jint channelProfileValue = env->CallIntMethod(channelProfileObject, getValue);
		cout <<"channelProfileValue is:"<<channelProfileValue<<endl;
		//assert(enumClass != NULL);
#endif
		//streamType
		jobject streamTypeObject = (env)->GetObjectField(jni_recordingConfig, streamTypeFieldID);
		jclass streamTypeClass = env->GetObjectClass(streamTypeObject);
		assert(streamTypeObject);
		if(streamTypeObject == NULL){cout <<"streamTypeEnum is NULL"; return JNI_FALSE;}
		jmethodID getValue2 = env->GetMethodID(streamTypeClass, "getValue", "()I");
		jint streamTypeValue = env->CallIntMethod(streamTypeObject, getValue2);
		cout << "streamTypeValue:"<<streamTypeValue<<endl;
		
		cout<<"c_appliteDir:"<<c_appliteDir<<",str_appliteDir:"<<str_appliteDir<<endl;
		
		agora::recording::RecordingConfig config;
		jniproxy::AgoraJniProxySdk jniRecorder;
		config.appliteDir = const_cast<char*>(str_appliteDir.c_str());	
		config.idleLimitSec = (int)idleValue;
	
		config.streamType = static_cast<agora::linuxsdk::REMOTE_VIDEO_STREAM_TYPE>(streamTypeValue);
    int ret = jniRecorder.createChannel(appId, channelKey, channelName, uid, config);

    cout<<"agora sdk createChannel ret:"<<ret<<endl;
 		while (!jniRecorder.stopped() && !g_bSignalStop) {
  		usleep(10000); //10ms
		}


  	if (g_bSignalStop) {
    	jniRecorder.leaveChannel();
    	jniRecorder.release();
  	}
    cout<<"Java_AgoraJavaRecording__recordingConfigcreateChannel end"<<endl;
 
}
#ifdef __cplusplus
}
#endif
