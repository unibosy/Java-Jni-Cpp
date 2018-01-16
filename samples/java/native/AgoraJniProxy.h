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
    m_jobAgoraJavaRecording = job;
  }
  void setJcAgoraJavaRecording(jclass jc){
    m_jcAgoraJavaRecording = jc;
  }
  void stopJavaProc(JNIEnv* env);
  void setJavaRecordingPath(JNIEnv* env, std::string& storeDir);
private:
  void initJavaObjects(bool init);
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
  jclass mJavaVideoYUVFrameClass;
  jobject mJavaVideoYUVFrameObject;
  jclass mJavaVideoFrameJPGClass;
  jobject mJavaVideoFrameJPGObject;
  jclass mJavaVideoFrameH264Class;
  jobject mJavaVideoFrameH264Object;
  //audio
  jclass mJavaAudioFrameClass;
  jobject mJavaAudioFrameObject;

  jclass mJavaAudioAacFrameClass;
  jobject mJavaAudioAacFrameObject;

  jclass mJavaAudioPcmFrameClass;
  jobject mJavaAudioPcmFrameObject;

private:
  JNIEnv* m_jni_env;
  jclass m_jcAgoraJavaRecording;
  jobject m_jobAgoraJavaRecording;
};

}//endnamespace
