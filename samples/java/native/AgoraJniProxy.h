#include "../../agorasdk/AgoraSdk.h"
#include "string.h"
#include "iostream"
#include "jni.h"
using namespace std;

namespace jniproxy
{

class AgoraJniProxySdk : public agora::AgoraSdk {
public:
  typedef void (*callback_t ) (void);
  AgoraJniProxySdk();
  ~AgoraJniProxySdk(); 
  void initialize();
  virtual void onError(int error, agora::linuxsdk::STAT_CODE_TYPE stat_code);
  virtual void onWarning(int warn);

  virtual void onJoinChannelSuccess(const char * channelId, agora::linuxsdk::uid_t uid) {
    cout<<"AgoraJniProxySdk onJoinChannelSuccess"<<endl;
  }
  virtual void onLeaveChannel(agora::linuxsdk::LEAVE_PATH_CODE code);
  virtual void onUserJoined(agora::linuxsdk::uid_t uid, agora::linuxsdk::UserJoinInfos &infos);
  virtual void onUserOffline(agora::linuxsdk::uid_t uid, agora::linuxsdk::USER_OFFLINE_REASON_TYPE reason);
  virtual void audioFrameReceived(unsigned int uid, const agora::linuxsdk::AudioFrame *frame) const;
  virtual void videoFrameReceived(unsigned int uid, const agora::linuxsdk::VideoFrame *frame) const;

  void setJobAgoraJavaRecording(jobject job){
    mJavaAgoraJavaRecordingObject = job;
  }
  void setJcAgoraJavaRecording(jclass jc){
    mJavaAgoraJavaRecordingClass = jc;
  }
  void stopJavaProc(JNIEnv* env);
  void setJavaRecordingPath(JNIEnv* env, std::string& storeDir);

  jobject newJObject(JNIEnv* env) const;
  jobject newJObject2(JNIEnv* env) const;

  jclass newGlobalJClass(JNIEnv* env, const char* className);
  jobject newGlobalJObject(JNIEnv* env, jclass jc, const char* signature);
  //cache field ids & method ids
  void cacheJavaCBFuncMethodIDs4Video(JNIEnv* env, const char* className);
  void cacheJavaCBFuncMethodIDs4YUV(JNIEnv* env, const char* className);
  jmethodID safeGetMethodID(JNIEnv* env, jclass clazz, const char* name, const char* sig) const;

public:
   
private:
  void initJavaObjects(JNIEnv* env, bool init);
private:
  //audio
  bool fillJAudioFrameByFields(JNIEnv* env, const agora::linuxsdk::AudioFrame*& frame, jclass& jcAudioFrame, jobject& jobAudioFrame) const;
  //pcm
  bool fillAudioPcmFrame(JNIEnv* env, const agora::linuxsdk::AudioFrame*& frame,jclass& jcAudioFrame, jobject& jobAudioFrame) const;
  bool fillPcmAllFields(JNIEnv* env, jobject& job, jclass& jc, const agora::linuxsdk::AudioFrame*& frame) const;
  //aac
  bool fillAudioAacFrame(JNIEnv* env, const agora::linuxsdk::AudioFrame*& frame,jclass& jcAudioFrame, jobject& jobAudioFrame) const;
  bool fillAacAllFields(JNIEnv* env, jobject& job, jclass& jc, const agora::linuxsdk::AudioFrame*& frame) const;

  //video
  bool fillJVideoFrameByFields(JNIEnv* env, const agora::linuxsdk::VideoFrame*& frame, jclass jcVideoFrame, jobject jobVideoFrame) const;
  bool fillJVideoOfYUV(JNIEnv* env, const agora::linuxsdk::VideoFrame*& frame, jclass& jcVideoFrame, jobject& jobVideoFrame) const;
  bool fillJVideoOfJPG(JNIEnv* env, const agora::linuxsdk::VideoFrame*& frame, jclass& jcVideoFrame, jobject& jobVideoFrame) const;
  bool fillJVideoOfH264(JNIEnv* env, const agora::linuxsdk::VideoFrame*& frame, jclass& jcVideoFrame, jobject& jobVideoFrame) const;
private:
  std::string m_logdir;
private:
  //define java object
  //video
  jclass mJavaVideoFrameClass;
  jobject mJavaVideoFrameObject;
  jclass mJavaVideoYuvFrameClass;
  jobject mJavaVideoYuvFrameObject;
  jclass mJavaVideoJPGFrameClass;
  jobject mJavaVideoJPGFrameObject;
  jclass mJavaVideoH264FrameClass;
  jobject mJavaVideoH264FrameObject;
  //audio
  jclass mJavaAudioFrameClass;
  jobject mJavaAudioFrameObject;

  jclass mJavaAudioAacFrameClass;
  jobject mJavaAudioAacFrameObject;

  jclass mJavaAudioPcmFrameClass;
  jobject mJavaAudioPcmFrameObject;
  //callback function jmethodIDs

  //java class jmethod IDs
  //static jmethodID mJavaVideoFrameInitMtd;
  //static jmethodID mJavaVideoYuvFrameInitMtd;
  jmethodID mJavaVideoJPGFrameInitMtd;
  jmethodID mJavaVideoH264FrameInitMtd;
  jmethodID mJavaAudioFrameInitMtd;
  jmethodID mJavaAudioAacFrameInitMtd;
  jmethodID mJavaAudioPcmFrameInitMtd;
  //video frame field
  //different jobject can or cannotshare one field?
  jfieldID mJavaVideoFrameMsFid;
  jfieldID mJavaVideoFrameBufFid;
  jfieldID mJavaVideoFrameBufSizeFid;
  jfieldID mJavaVideoFrameYuvFid;

private:
  jclass mJavaAgoraJavaRecordingClass;
  jobject mJavaAgoraJavaRecordingObject;
};

}//endnamespace
