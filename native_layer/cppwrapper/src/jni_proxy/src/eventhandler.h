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
    ~AgoraJniProxySdk() {cout<<"AgoraJniProxySdk destructor"<<endl;}

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

    virtual int setVideoMixingLayout(const agora::linuxsdk::VideoMixingLayout &layout);
    void setJobAgoraJavaRecording(jobject job){
      m_jobAgoraJavaRecording = job;
    }
    void setJcAgoraJavaRecording(jclass jc){
      m_jcAgoraJavaRecording = jc;
    }
    void stopJavaProc();
    void setVideoMixLayoutTest();
    void setTmp(const std::string& tmp){ m_tmp = tmp;}
    std::string getTmp(){return m_tmp;}
private:
    //audio
    bool fillJAudioFrameByFields(JNIEnv* jni_env, const agora::linuxsdk::AudioFrame*& frame, jclass& jcAudioFrame, jobject& jobAudioFrame) const;
    //pcm
    bool fillAudioPcmFrame(JNIEnv* jni_env, const agora::linuxsdk::AudioFrame*& frame,jclass& jcAudioFrame, jobject& jobAudioFrame) const;
    bool fillPcmAllFields(JNIEnv* jni_env, jobject& job, jclass& jc, const agora::linuxsdk::AudioFrame*& frame) const;
    //aac
    bool fillAudioAacFrame(JNIEnv* jni_env, const agora::linuxsdk::AudioFrame*& frame,jclass& jcAudioFrame, jobject& jobAudioFrame) const;
    bool fillAacAllFields(JNIEnv* jni_env, jobject& job, jclass& jc, const agora::linuxsdk::AudioFrame*& frame) const;
    
    //video
    bool fillJVideoFrameByFields(JNIEnv* jni_env, const agora::linuxsdk::VideoFrame*& frame, jclass& jcVideoFrame, jobject& jobVideoFrame) const;
    bool fillJVideoOfYUV(JNIEnv* jni_env, const agora::linuxsdk::VideoFrame*& frame, jclass& jcVideoFrame, jobject& jobVideoFrame) const;
    bool fillJVideoOfJPG(JNIEnv* jni_env, const agora::linuxsdk::VideoFrame*& frame, jclass& jcVideoFrame, jobject& jobVideoFrame) const;
    bool fillJVideoOfH264(JNIEnv* jni_env, const agora::linuxsdk::VideoFrame*& frame, jclass& jcVideoFrame, jobject& jobVideoFrame) const;
private:
    std::string m_logdir;
private:
    JNIEnv* m_jni_env;
    jclass m_jcAgoraJavaRecording;
    jobject m_jobAgoraJavaRecording;

    std::string m_tmp;


};

#if 0 
int AgoraJniProxySdk::setVideoMixingLayout(const agora::linuxsdk::VideoMixingLayout &layout)
{
   int result = -agora::linuxsdk::ERR_INTERNAL_FAILED;
   if(m_server)
      result = m_engine->setVideoMixingLayout(layout);
   return result;
}
#endif

}//endnamespace
