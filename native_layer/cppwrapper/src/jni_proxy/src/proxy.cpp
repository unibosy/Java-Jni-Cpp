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
	JNIEnv* jni_env = NULL;
  assert(jni_env);
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
    jclass objectClass = (jni_env)->FindClass("headers.EnumIndex$AudioPcmFrame"); 
    //jobject  objectNewEng = (env)->NewObject();
  }else if (frame->type == agora::linuxsdk::AUDIO_FRAME_AAC) {
    
  }
  jmethodID mid = jni_env->GetMethodID(javaClass, "audioPcmFrameReceived", "(ILheaders/EnumIndex$AudioPcmFrame;)V");

  assert(mid);
  cout <<"mid"<<endl;
  //jni_env->CallVoidMethod(javaClass, mid, );



  jni_env->DeleteLocalRef(javaClass);
  ((JavaVM*)g_jvm)->DetachCurrentThread();
}
void AgoraJniProxySdk::onLeaveChannel(agora::linuxsdk::LEAVE_PATH_CODE code) {
	cout<<"AgoraJniProxySdk onLeaveChannel"<<endl;

	JNIEnv* jni_env = NULL;
  assert(jni_env);
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

  //step 1:find class
  jclass jcLeave = (jni_env)->FindClass("headers/EnumIndex$LEAVE_PATH_CODE");
  cout << "jcleave ..."<<endl;
  if(jcLeave){
    cout << "find jcleave"<<endl;
  }else {
    cout <<"no jcleave???"<<endl;
  }
  assert(jcLeave);
  //step 2: create constructor
  jmethodID jconstructMid = NULL;
  jconstructMid = jni_env->GetMethodID(jcLeave, "<init>", "(Ljava/lang/String;II)V");
  assert(jconstructMid);
  if(jconstructMid){
  cout<<"jconstructMid"<<endl;}
  else{cout<<"jconstructMid -1"<<endl;}
  //step 3: new object
  jobject jobLeave = NULL;
  cout <<"NewObject..."<<endl;
  jint jLeaveValue = (int)(code);
  cout <<"jLeaveValue:"<<jLeaveValue<<endl;
  jint temp = 1;
  jobLeave = jni_env->NewObject(jcLeave, jconstructMid, temp,jLeaveValue);
  //jobLeave = (jni_env)->AllocObject(jcLeave,jint(1));
  cout << "jobleave ..."<<endl;
  //(jni_env)->SetIntField(jobLeave, (jni_env)->GetFieldID(jcLeave, "value", "()I"), (jint)(int(code)));

  //setp 4: get special method
	jmethodID jLeaveMid =  jni_env->GetStaticMethodID(javaClass,"onLeaveChannel","(I)V");
  assert(jcLeaveMid);
	cout <<"jLeaveMid"<<endl; 
  jni_env->CallStaticVoidMethod(javaClass, jLeaveMid, jobLeave);
  //jni_env->CallObjectMethod(javvaClassaClass, mid,jobLeave);
	//jni_env->CallStaticVoidMethod(javaClass, mid, jobLeave);
  cout<<"CallStaticVoidMethod end"<<endl;
  jni_env->DeleteLocalRef(javaClass);
	((JavaVM*)g_jvm)->DetachCurrentThread();
}
void AgoraJniProxySdk::onError(int error, agora::linuxsdk::STAT_CODE_TYPE stat_code)
{
 	cout<<"AgoraJniProxySdk onError"<<endl;
	JNIEnv* jni_env = NULL;
  assert(jni_env);
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

	jmethodID mid =  jni_env->GetMethodID(javaClass,"onError","(I)V");
  assert(mid);
	cout <<"mid"<<endl;
	jni_env->CallStaticVoidMethod(javaClass, mid, 2, jint((int)(stat_code)));
	cout<<"find onLeaveChannel method"<<endl;
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
