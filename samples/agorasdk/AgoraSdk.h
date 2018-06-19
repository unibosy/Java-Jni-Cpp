#include <csignal>
#include <cstdint>
#include <iostream>
#include <sstream> 
#include <string>
#include <vector>
#include <algorithm>

#include "IAgoraLinuxSdkCommon.h"
#include "IAgoraRecordingEngine.h"

#include "base/atomic.h"
#include "base/log.h" 
#include "base/opt_parser.h" 

namespace agora {

using std::string;
using std::cout;
using std::cerr;
using std::endl;

using agora::base::opt_parser;
using agora::linuxsdk::VideoFrame;
using agora::linuxsdk::AudioFrame;

enum LAYOUT_MODE_TYPE {
    DEFAULT_LAYOUT = 0,
    BESTFIT_LAYOUT = 1,
    VERTICALPRESENTATION_LAYOUT = 2,
};


struct MixModeSettings {
    int m_height;
    int m_width;
    bool m_videoMix;
    MixModeSettings():
        m_height(0),
        m_width(0),
        m_videoMix(false)
    {};
};


class AgoraSdk : virtual public agora::recording::IRecordingEngineEventHandler {
    public:
        AgoraSdk();
        virtual ~AgoraSdk();

        virtual bool createChannel(const string &appid, const string &channelKey, const string &name,  agora::linuxsdk::uid_t uid,
                agora::recording::RecordingConfig &config);
        virtual int setVideoMixLayout();
        virtual bool leaveChannel();
        virtual bool release();
        virtual bool stopped() const;
        virtual void updateMixModeSetting(int width, int height, bool isVideoMix) {
            m_mixRes.m_width = width;
            m_mixRes.m_height = height;
            m_mixRes.m_videoMix = isVideoMix;
        }
        virtual const agora::recording::RecordingEngineProperties* getRecorderProperties();
        virtual void updateStorageDir(const char* dir) { m_storage_dir = dir? dir:"./"; }
        virtual void updateLayoutSetting(int layoutMode, int maxVertPreLayoutUid) {
            m_layoutMode = static_cast<LAYOUT_MODE_TYPE >(layoutMode);
            m_maxVertPreLayoutUid = maxVertPreLayoutUid;
        }

        virtual int startService();
        virtual int stopService();

        virtual int setVideoMixingLayout(const agora::linuxsdk::VideoMixingLayout &layout);
        virtual agora::recording::RecordingConfig* getConfigInfo() { return &m_config;}
        virtual int setUserBackground(agora::linuxsdk::uid_t uid, const char* image_path);
        virtual void setLogLevel(agora::linuxsdk::agora_log_level level);
        virtual void enableLogModule(uint32_t modules, bool enable);
    protected:
        virtual void onError(int error, agora::linuxsdk::STAT_CODE_TYPE stat_code) {
            onErrorImpl(error, stat_code);
        }
        virtual void onWarning(int warn) {
            onWarningImpl(warn);
        }

        virtual void onJoinChannelSuccess(const char * channelId, agora::linuxsdk::uid_t uid) {
            onJoinChannelSuccessImpl(channelId, uid);
        }
        virtual void onLeaveChannel(agora::linuxsdk::LEAVE_PATH_CODE code) {
            onLeaveChannelImpl(code);
        }

        virtual void onUserJoined(agora::linuxsdk::uid_t uid, agora::linuxsdk::UserJoinInfos &infos) {
            onUserJoinedImpl(uid, infos);
        }
        virtual void onUserOffline(agora::linuxsdk::uid_t uid, agora::linuxsdk::USER_OFFLINE_REASON_TYPE reason) {
            onUserOfflineImpl(uid, reason);
        }

        virtual void audioFrameReceived(unsigned int uid, const agora::linuxsdk::AudioFrame *frame) const {
            audioFrameReceivedImpl(uid, frame);
        }
        virtual void videoFrameReceived(unsigned int uid, const agora::linuxsdk::VideoFrame *frame) const {
            videoFrameReceivedImpl(uid, frame);
        }
        virtual void onActiveSpeaker(uid_t uid) {
            onActiveSpeakerImpl(uid);
        }
    private:
        void adjustDefaultVideoLayout(agora::linuxsdk::VideoMixingLayout::Region * regionList);
        void adjustBestFitVideoLayout(agora::linuxsdk::VideoMixingLayout::Region * regionList);
        void adjustVerticalPresentationLayout(unsigned int maxResolutionUid, agora::linuxsdk::VideoMixingLayout::Region * regionList);

        void adjustVideo5Layout(unsigned int maxResolutionUid, agora::linuxsdk::VideoMixingLayout::Region * regionList);
        void adjustVideo7Layout(unsigned int maxResolutionUid, agora::linuxsdk::VideoMixingLayout::Region * regionList);
        void adjustVideo9Layout(unsigned int maxResolutionUid, agora::linuxsdk::VideoMixingLayout::Region * regionList);
        void adjustVideo17Layout(unsigned int maxResolutionUid, agora::linuxsdk::VideoMixingLayout::Region * regionList);

        void changeToVideo7Layout(unsigned int maxResolutionUid, agora::linuxsdk::VideoMixingLayout::Region * regionList);
        void changeToVideo9Layout(unsigned int maxResolutionUid, agora::linuxsdk::VideoMixingLayout::Region * regionList);
        void changeToVideo17Layout(unsigned int maxResolutionUid, agora::linuxsdk::VideoMixingLayout::Region * regionList);

        void adjustBestFitLayout_2(agora::linuxsdk::VideoMixingLayout::Region * regionList);
        void adjustBestFitLayout_Square(agora::linuxsdk::VideoMixingLayout::Region * regionList, int nSquare);
        void adjustBestFitLayout_17(agora::linuxsdk::VideoMixingLayout::Region * regionList);
        void setMaxResolutionUid(int number, unsigned int maxResolutionUid, agora::linuxsdk::VideoMixingLayout::Region * regionList, double weight_ratio);


    protected:
        void onErrorImpl(int error, agora::linuxsdk::STAT_CODE_TYPE stat_code);
        void onWarningImpl(int warn);

        void onJoinChannelSuccessImpl(const char * channelId, agora::linuxsdk::uid_t uid);
        void onLeaveChannelImpl(agora::linuxsdk::LEAVE_PATH_CODE code);

        void onUserJoinedImpl(agora::linuxsdk::uid_t uid, agora::linuxsdk::UserJoinInfos &infos);
        void onUserOfflineImpl(agora::linuxsdk::uid_t uid, agora::linuxsdk::USER_OFFLINE_REASON_TYPE reason);

        void audioFrameReceivedImpl(unsigned int uid, const agora::linuxsdk::AudioFrame *frame) const;
        void videoFrameReceivedImpl(unsigned int uid, const agora::linuxsdk::VideoFrame *frame) const;
        void onActiveSpeakerImpl(uid_t uid);

    protected:
        atomic_bool_t m_stopped;
        std::vector<agora::linuxsdk::uid_t> m_peers;
        std::string m_logdir;
        std::string m_storage_dir;
        MixModeSettings m_mixRes;
        agora::recording::RecordingConfig m_config;
        agora::recording::IRecordingEngine *m_engine;
        agora::linuxsdk::agora_log_level m_level;
        uint32_t m_logModules;
        LAYOUT_MODE_TYPE m_layoutMode;
        int m_maxVertPreLayoutUid;

};


}
