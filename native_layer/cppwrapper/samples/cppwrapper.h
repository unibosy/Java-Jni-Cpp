#include <csignal>
#include <cstdint>
#include <iostream>
#include <sstream> 
#include <string>
#include <vector>
#include <algorithm>

#include "IAgoraRecordingEngine.h"

#include "base/atomic.h"
#include "base/log.h" 
#include "base/opt_parser.h" 

using std::string;
using std::cout;
using std::cerr;
using std::endl;

using agora::base::opt_parser;
using agora::recording::VideoFrame;
using agora::recording::AudioFrame;

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


class AgoraRecorder : public agora::recording::IRecordingEngineEventHandler {
 public:
 // appId
  AgoraRecorder();
  ~AgoraRecorder();

  bool createChannel(const string &appid, const string &channelKey, const string &name,  uid_t uid,
          agora::recording::RecordingConfig &config);

  int setVideoMixLayout();

  bool leaveChannel();
  bool release();

  bool stopped() const;
  inline void updateMixModeSetting(int width, int height, bool isVideoMix) {
      m_mixRes.m_width = width;
      m_mixRes.m_height = height;
      m_mixRes.m_videoMix = isVideoMix;
  }
  
  const agora::recording::RecordingEngineProperties* getRecorderProperties();

 private:
  virtual void onError(int error, agora::recording::STAT_CODE_TYPE stat_code);
  virtual void onWarning(int warn);

  virtual void onJoinChannelSuccess(const char * channelId, uid_t uid);
  virtual void onLeaveChannel(agora::recording::LEAVE_PATH_CODE code);

  virtual void onUserJoined(uid_t uid, agora::recording::UserJoinInfos &infos);
  virtual void onUserOffline(uid_t uid, agora::recording::USER_OFFLINE_REASON_TYPE reason);

  virtual void audioFrameReceived(unsigned int uid, const AudioFrame *frame) const;
  virtual void videoFrameReceived(unsigned int uid, const VideoFrame *frame) const;
 private:
  atomic_bool_t m_stopped;
  agora::recording::IRecordingEngine *m_recorder;
  std::vector<agora::recording::uid_t> m_peers;
  std::string m_recordingDir;
  std::string m_logdir;
  MixModeSettings m_mixRes;
};


