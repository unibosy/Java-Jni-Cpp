//#include "../agorasdk/AgoraSdk.h"
#include "IAgoraLinuxSdkCommon.h"
//#include "../cppwrapper.h"
#include "../jni_headers/AgoraJavaRecording.h"

#include "eventhandler.h"

#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

using std::string;
using std::cout;
using std::cerr;
using std::endl;

using agora::base::log;
using namespace jniproxy;

atomic_bool_t g_bSignalStop;

void signal_handler(int signo) {
  (void)signo;

  // cerr << "Signal " << signo << endl;
  g_bSignalStop = true;
}

#ifdef __cplusplus
extern "C" {
#endif

JavaVM* g_jvm = NULL;

void AgoraJniProxySdk::audioFrameReceived(unsigned int uid, const agora::linuxsdk::AudioFrame *frame) const
{
	cout<<"AgoraJniProxySdk::audioFrameReceived enter uid:"<<uid<<endl;
	//
#if 0
	JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
	if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK)
	{
		cout <<"GetEnv failed"<<endl;
		return;
	}
	assert(jni_env);
	jclass javaClass = jni_env->FindClass("AgoraJavaRecording");
  assert(javaClass);
	pid_t tid = getpid();
	cout << "this thread id is:"<<tid<<endl;
  //findClass -> constructor method ->NewObject->GetFieldID-> setXXXField
  if (frame->type == agora::linuxsdk::AUDIO_FRAME_RAW_PCM) {
    cout <<"jni receive raw data is pcm!"<<endl;
    jclass objectClass = (jni_env)->FindClass("headers/EnumIndex$AudioPcmFrame"); 
    //jobject  objectNewEng = (env)->NewObject();
  }else if (frame->type == agora::linuxsdk::AUDIO_FRAME_AAC) {
    
  }
  jmethodID mid = jni_env->GetMethodID(javaClass, "audioPcmFrameReceived", "(ILheaders/EnumIndex$AudioPcmFrame;)V");

  assert(mid);
  cout <<"mid"<<endl;
  //jni_env->CallVoidMethod(javaClass, mid, );
  jni_env->DeleteLocalRef(javaClass);
  ((JavaVM*)g_jvm)->DetachCurrentThread();
#endif
}

void AgoraJniProxySdk::onUserJoined(agora::linuxsdk::uid_t uid, agora::linuxsdk::UserJoinInfos &infos) {
  cout << "AgoraJniProxySdk User " << uid << " joined, RecordingDir:" << (infos.storageDir? infos.storageDir:"NULL") <<endl;
  std::string store_dir = std::string(infos.storageDir);
	JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
	if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK) {
		cout <<"GetEnv failed"<<endl;
		return;
	}
  assert(jni_env);
	jclass javaClass = jni_env->FindClass("AgoraJavaRecording");
  assert(javaClass);
	//int tid = GetCurrentThreadId();
	pid_t tid = getpid();
	cout << "this thread id is:"<<tid<<endl;
  jmethodID jUserJoinedMid =  jni_env->GetStaticMethodID(javaClass,"onUserJoined","(JLjava/lang/String;)V");
  assert(jUserJoinedMid);

  jstring jstrRecordingDir = jni_env->NewStringUTF(store_dir.c_str());
  jni_env->CallStaticVoidMethod(javaClass, jUserJoinedMid, jlong((long)(uid)),jstrRecordingDir);
  cout << "AgoraJniProxySdk::onUserJoined call end"<<endl;

  jni_env->DeleteLocalRef(javaClass);
	((JavaVM*)g_jvm)->DetachCurrentThread();
}
void AgoraJniProxySdk::onUserOffline(agora::linuxsdk::uid_t uid, agora::linuxsdk::USER_OFFLINE_REASON_TYPE reason)
{
  cout << "AgoraJniProxySdk onUserOffline User:" << uid << ",reason" << reason<<endl;
	JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
	if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK) {
		cout <<"GetEnv failed"<<endl;
		return;
	}
  assert(jni_env);
	jclass javaClass = jni_env->FindClass("AgoraJavaRecording");
  assert(javaClass);
	//int tid = GetCurrentThreadId();
	pid_t tid = getpid();
	cout << "this thread id is:"<<tid<<endl;
  jmethodID jUserOfflineMid =  jni_env->GetStaticMethodID(javaClass,"onUserOffline","(JI)V");
  assert(jUserOfflineMid);
  jni_env->CallStaticVoidMethod(javaClass, jUserOfflineMid, jlong((long)(uid)),jint(int(reason)));
  cout << "AgoraJniProxySdk::onUserOffline call end"<<endl;

  jni_env->DeleteLocalRef(javaClass);
	((JavaVM*)g_jvm)->DetachCurrentThread();
}
void AgoraJniProxySdk::onLeaveChannel(agora::linuxsdk::LEAVE_PATH_CODE code) {
	cout<<"AgoraJniProxySdk onLeaveChannel"<<endl;
	JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
	if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK)
	{
		cout <<"GetEnv failed"<<endl;
		return;
	}
  assert(jni_env);
	jclass javaClass = jni_env->FindClass("AgoraJavaRecording");
  assert(javaClass);
	//int tid = GetCurrentThreadId();
	pid_t tid = getpid();
	cout << "this thread id is:"<<tid<<endl;
  jmethodID jLeaveMid =  jni_env->GetStaticMethodID(javaClass,"onLeaveChannel","(I)V");
  assert(jcLeave);
  jni_env->CallStaticVoidMethod(javaClass, jLeaveMid, jint((int)(code)));
  cout << "AgoraJniProxySdk::onLeaveChannel call end"<<endl;

  jni_env->DeleteLocalRef(javaClass);
	((JavaVM*)g_jvm)->DetachCurrentThread();
}
void AgoraJniProxySdk::onWarning(int warn)
{
 	cout<<"AgoraJniProxySdk onWarning,warn:"<<warn<<endl;
	JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
	if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK) {
		cout <<"GetEnv failed"<<endl;
		return;
	}
  assert(jni_env);
	jclass javaClass = jni_env->FindClass("AgoraJavaRecording");
  assert(javaClass);
	//int tid = GetCurrentThreadId();
	pid_t tid = getpid();
	cout << "onWarning this thread id is:"<<tid<<endl;
  jmethodID jOnWarnMid =  jni_env->GetStaticMethodID(javaClass,"onWarning","(I)V");
  assert(jOnWarnMid);
  jni_env->CallStaticVoidMethod(javaClass, jOnWarnMid, warn);
  jni_env->DeleteLocalRef(javaClass);
	((JavaVM*)g_jvm)->DetachCurrentThread();
}

void AgoraJniProxySdk::onError(int error, agora::linuxsdk::STAT_CODE_TYPE stat_code)
{
 	cout<<"AgoraJniProxySdk onError"<<endl;
	JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
  if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK) {
		cout <<"GetEnv failed"<<endl;
		return;
	}
  assert(jni_env);
	jclass javaClass = jni_env->FindClass("AgoraJavaRecording");
  assert(javaClass);
	//int tid = GetCurrentThreadId();
	pid_t tid = getpid();
	cout << "this thread id is:"<<tid<<endl;
  jmethodID jLeaveMid =  jni_env->GetStaticMethodID(javaClass,"onError","(II)V");
  assert(jcLeave);
  jni_env->CallStaticVoidMethod(javaClass, jLeaveMid, error, jint((int)(stat_code)));
  leaveChannel();

  jni_env->DeleteLocalRef(javaClass);
	((JavaVM*)g_jvm)->DetachCurrentThread();
}
//TODO
#if 0
  convert jobject jni_recordingConfig to c++ recordingConfig
#endif
#if 1
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	g_jvm = vm;
	assert(g_jvm);
	cout<<"jint JNI_OnLoad enter"<<endl;
	return JNI_VERSION_1_4;
}
#endif


/*
 * Class:     AgoraJavaRecording
 * Method:    leaveChannel
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_AgoraJavaRecording_leaveChannel
  (JNIEnv *, jobject)
{
  cout<<"java call leaveChannel"<<endl;
  g_bSignalStop = true;
  return JNI_TRUE;
}

void stopJavaProc()
{
  cout<<"AgoraJniProxySdk stopJavaProc"<<endl;
	JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
  if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK) {
		cout <<"GetEnv failed"<<endl;
		return;
	}
  assert(jni_env);
	jclass javaClass = jni_env->FindClass("AgoraJavaRecording");
  assert(javaClass);
	pid_t tid = getpid();
	cout << "this thread id is:"<<tid<<endl;
  jmethodID jStopCB =  jni_env->GetStaticMethodID(javaClass,"stopCallBack","()V");
  assert(jStopCB);
  jni_env->CallStaticVoidMethod(javaClass, jStopCB);

  jni_env->DeleteLocalRef(javaClass);
	((JavaVM*)g_jvm)->DetachCurrentThread();

}

JNIEXPORT jboolean JNICALL Java_AgoraJavaRecording_createChannel(JNIEnv * env, jobject jobject1, jstring jni_appid, jstring jni_channelKey, 
      jstring jni_channelName, jint jni_uid, jobject jni_recordingConfig)
{
		pid_t tid = getpid();
	  cout<<"enter Java_AgoraJavaRecording_createChannel,pid:"<<tid<<endl;
		g_bSignalStop = false;
	  signal(SIGQUIT, signal_handler);
  	signal(SIGABRT, signal_handler);
  	signal(SIGINT, signal_handler);
  	signal(SIGPIPE, SIG_IGN);

    const char* appId = NULL;
    appId = env->GetStringUTFChars(jni_appid,false);
    if(!appId)
      cerr<<"get appId is NULL"<<endl;
    const char* channelKey = NULL;
    channelKey = env->GetStringUTFChars(jni_channelKey,false);
    if(!channelKey)
      cerr<<"no channel key"<<endl;
    const char* channelName = env->GetStringUTFChars(jni_channelName,false);
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
		//appliteDir
		jstring appliteDir = (jstring)env->GetObjectField(jni_recordingConfig, appliteDirFieldID);
		const char * c_appliteDir = env->GetStringUTFChars(appliteDir ,NULL);
		std::string str_appliteDir = c_appliteDir;
		env->ReleaseStringUTFChars(appliteDir,c_appliteDir);
		//CHANNEL_PROFILE_TYPE
		jobject channelProfileObject = (env)->GetObjectField(jni_recordingConfig, channelProfileFieldID);
		//assert(channelProfileObject);
		jclass enumClass = env->GetObjectClass(channelProfileObject);
		if(enumClass == NULL) {
			cout <<"enumClass is null"<<endl;
		}
		jmethodID getValue = env->GetMethodID(enumClass, "getValue", "()I");
		if (getValue == NULL) {
			cout <<"method not found"<<endl;
			return JNI_FALSE; /* method not found */
		}
		jint channelProfileValue = env->CallIntMethod(channelProfileObject, getValue);
		jobject streamTypeObject = (env)->GetObjectField(jni_recordingConfig, streamTypeFieldID);
		jclass streamTypeClass = env->GetObjectClass(streamTypeObject);
		assert(streamTypeObject);
		if(streamTypeObject == NULL){cout <<"streamTypeEnum is NULL"; return JNI_FALSE;}
		jmethodID getValue2 = env->GetMethodID(streamTypeClass, "getValue", "()I");
		jint streamTypeValue = env->CallIntMethod(streamTypeObject, getValue2);
		
		agora::recording::RecordingConfig config;
		jniproxy::AgoraJniProxySdk jniRecorder;
		config.appliteDir = const_cast<char*>(str_appliteDir.c_str());	
		config.idleLimitSec = (int)idleValue;
    cout <<"appId:"<<appId<<",uid:"<<uid<<",channelKey:"<<channelKey<<",channelName:"<<channelName<<",str_appliteDir:"
          <<str_appliteDir<<",channelProfileValue:"<<channelProfileValue<<endl;
	
		config.streamType = static_cast<agora::linuxsdk::REMOTE_VIDEO_STREAM_TYPE>(streamTypeValue);
    int ret = jniRecorder.createChannel(appId, channelKey, channelName, uid, config);

    cout<<"agora sdk createChannel ret:"<<ret<<endl;
 		while (!jniRecorder.stopped() && !g_bSignalStop) {
  		usleep(10000); //10ms
		}


  	if (g_bSignalStop) {
    	jniRecorder.leaveChannel();
      cout << "jni layer stopped!"<<endl;
      stopJavaProc();
    	jniRecorder.release();
  	}
    cout<<"Java_AgoraJavaRecording__recordingConfigcreateChannel end"<<endl;
 
}
#ifdef __cplusplus
}
#endif
