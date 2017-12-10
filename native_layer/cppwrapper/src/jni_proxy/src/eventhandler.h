#include "../../agorasdk/AgoraSdk.h"
#include "string.h"
#include "iostream"
using namespace std;

namespace jniproxy
{

class AgoraJniProxySdk : public agora::AgoraSdk {
public:
    typedef void (*callback_t ) (void);
    AgoraJniProxySdk():AgoraSdk(){cout<<"AgoraJniProxySdk constructor"<<endl;}
    ~AgoraJniProxySdk() {cout<<"AgoraJniProxySdk destructor"<<endl;}

    virtual void onError(int error, agora::linuxsdk::STAT_CODE_TYPE stat_code);
    virtual void onWarning(int warn);

    virtual void onJoinChannelSuccess(const char * channelId, agora::linuxsdk::uid_t uid) {
        cout<<"AgoraJniProxySdk onJoinChannelSuccess"<<endl;
    }
    virtual void onLeaveChannel(agora::linuxsdk::LEAVE_PATH_CODE code);
		/*{
        cout<<"AgoraJniProxySdk onLeaveChannel"<<endl;
				//call java code via jni
				onLeaveChannelImpl(code);
    }*/

    virtual void onUserJoined(agora::linuxsdk::uid_t uid, agora::linuxsdk::UserJoinInfos &infos);
    virtual void onUserOffline(agora::linuxsdk::uid_t uid, agora::linuxsdk::USER_OFFLINE_REASON_TYPE reason);

    virtual void audioFrameReceived(unsigned int uid, const agora::linuxsdk::AudioFrame *frame) const;
    //{
    //    cout<<"AgoraJniProxySdk audioFrameReceived"<<endl;
    //}
    virtual void videoFrameReceived(unsigned int uid, const agora::linuxsdk::VideoFrame *frame) const {
        cout<<"AgoraJniProxySdk videoFrameReceived"<<endl;
    }
private:

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
