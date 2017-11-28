#include "cppwrapper.h"

AgoraRecorder::AgoraRecorder(): IRecordingEngineEventHandler() {
  m_recorder = NULL;
  m_stopped.store(false);
  m_recordingDir = "./";
}

AgoraRecorder::~AgoraRecorder() {
  if (m_recorder) {
    m_recorder->release();
  }
}

bool AgoraRecorder::stopped() const {
  return m_stopped;
}

bool AgoraRecorder::release() {
  if (m_recorder) {
    m_recorder->release();
    m_recorder = NULL;
  }

  return true;
}

bool AgoraRecorder::createChannel(const string &appid, const string &channelKey, const string &name,
    uint32_t uid, 
    agora::recording::RecordingConfig &config) 
{
  if ((m_recorder = agora::recording::IRecordingEngine::createAgoraRecordingEngine(appid.c_str(), this)) == NULL)
    return false;

  
  return 0 == m_recorder->joinChannel(channelKey.c_str(), name.c_str(), uid, config);
}

bool AgoraRecorder::leaveChannel() {
  if (m_recorder) {
    m_recorder->leaveChannel();
    m_stopped = true;
  }

  return true;
}

//Customize the layout of video under video mixing model
int AgoraRecorder::setVideoMixLayout()
{
    if(!m_mixRes.m_videoMix) return 0;

    LOG_DIR(m_logdir.c_str(), INFO, "setVideoMixLayout: user size: %d", m_peers.size());

    agora::recording::VideoMixingLayout layout;
    layout.canvasWidth = m_mixRes.m_width;
    layout.canvasHeight = m_mixRes.m_height;
    layout.backgroundColor = "#23b9dc";

    layout.regionCount = static_cast<uint32_t>(m_peers.size());

    if (!m_peers.empty()) {
        LOG_DIR(m_logdir.c_str(), INFO, "setVideoMixLayout: peers not empty");
        agora::recording::VideoMixingLayout::Region * regionList = new agora::recording::VideoMixingLayout::Region[m_peers.size()];

        regionList[0].uid = m_peers[0];
        regionList[0].x = 0.f;
        regionList[0].y = 0.f;
        regionList[0].width = 1.f;
        regionList[0].height = 1.f;
        regionList[0].zOrder = 0;
        regionList[0].alpha = 1.f;
        regionList[0].renderMode = 0;

        LOG_DIR(m_logdir.c_str(), INFO, "region 0 uid: %d, x: %f, y: %f, width: %f, height: %f, zOrder: %d, alpha: %f", regionList[0].uid, regionList[0].x, regionList[0].y, regionList[0].width, regionList[0].height, regionList[0].zOrder, regionList[0].alpha);


        float canvasWidth = static_cast<float>(m_mixRes.m_width);
        float canvasHeight = static_cast<float>(m_mixRes.m_height);

        float viewWidth = 0.3f;
        float viewHEdge = 0.025f;
        float viewHeight = viewWidth * (canvasWidth / canvasHeight);
        float viewVEdge = viewHEdge * (canvasWidth / canvasHeight);

        for (size_t i=1; i<m_peers.size(); i++) {
            if (i >= 7)
                break;

            regionList[i].uid = m_peers[i];

            float xIndex = static_cast<float>(i % 3);
            float yIndex = static_cast<float>(i / 3);
            regionList[i].x = xIndex * (viewWidth + viewHEdge) + viewHEdge;
            regionList[i].y = 1 - (yIndex + 1) * (viewHeight + viewVEdge);
            regionList[i].width = viewWidth;
            regionList[i].height = viewHeight;
            regionList[i].zOrder = 0;
            regionList[i].alpha = static_cast<double>(i + 1);
            regionList[i].renderMode = 0;
        }

        layout.regions = regionList;
//    LOG_DIR(m_logdir.c_str(), INFO, "region 0 uid: %d, x: %f, y: %f, width: %f, height: %f, zOrder: %d, alpha: %f", regionList[0].uid, regionList[0].x, regionList[0].y, regionList[0].width, regionList[0].height, regionList[0].zOrder, regionList[0].alpha);
    }
    else {
        layout.regions = NULL;
    }

    return m_recorder->setVideoMixingLayout(layout);
}

const agora::recording::RecordingEngineProperties* AgoraRecorder::getRecorderProperties(){
    return m_recorder->getProperties();
}

void AgoraRecorder::onError(int error, agora::recording::STAT_CODE_TYPE stat_code) {
    cerr << "Error: " << error <<",with stat_code:"<< stat_code << endl;
    leaveChannel();
}

void AgoraRecorder::onWarning(int warn) {
    cerr << "warn: " << warn << endl;
    //  leaveChannel();
}

void AgoraRecorder::onJoinChannelSuccess(const char * channelId, uid_t uid) {
    cout << "join channel Id: " << channelId << ", with uid: " << uid << endl;
}

void AgoraRecorder::onLeaveChannel(agora::recording::LEAVE_PATH_CODE code) {
    cout << "leave channel with code:" << code << endl;
}

void AgoraRecorder::onUserJoined(unsigned uid, agora::recording::UserJoinInfos &infos) {
    cout << "User " << uid << " joined, RecordingDir:" << (infos.recordingDir? infos.recordingDir:"NULL") <<endl;
    if(infos.recordingDir)
    {
        m_recordingDir = std::string(infos.recordingDir);
        m_logdir = m_recordingDir;
    }

    m_peers.push_back(uid);
    
    //When the user joined, we can re-layout the canvas
    setVideoMixLayout();
}

void AgoraRecorder::onUserOffline(unsigned uid, agora::recording::USER_OFFLINE_REASON_TYPE reason) {
    cout << "User " << uid << " offline, reason: " << reason << endl;
    m_peers.erase(std::remove(m_peers.begin(), m_peers.end(), uid), m_peers.end());
    
    //When the user is offline, we can re-layout the canvas
    setVideoMixLayout();
}

void AgoraRecorder::audioFrameReceived(unsigned int uid, const AudioFrame *pframe) const 
{
  char uidbuf[65];
  snprintf(uidbuf, sizeof(uidbuf),"%u", uid);
  std::string info_name = m_recordingDir + std::string(uidbuf) /*+ timestamp_per_join_*/;

  uint8_t* data = NULL; 
  uint32_t size = 0;
  if (pframe->type == agora::recording::AUDIO_FRAME_RAW_PCM) {
    info_name += ".pcm";  
    
    cout << "User " << uid << ", received a raw PCM frame" << endl;

    agora::recording::AudioPcmFrame *f = pframe->frame.pcm;
    data = f->pcmBuf_;
    size = f->pcmBufSize_;

  } else if (pframe->type == agora::recording::AUDIO_FRAME_AAC) {
    info_name += ".aac";

    cout << "User " << uid << ", received an AAC frame" << endl;

    agora::recording::AudioAacFrame *f = pframe->frame.aac;
    data = f->aacBuf_;
    size = f->aacBufSize_;
  }

  FILE *fp = fopen(info_name.c_str(), "a+b");
  ::fwrite(data, 1, size, fp);
  ::fclose(fp);
}

void AgoraRecorder::videoFrameReceived(unsigned int uid, const VideoFrame *pframe) const {
  char uidbuf[65];
  snprintf(uidbuf, sizeof(uidbuf),"%u", uid);
  const char * suffix=".vtmp";
  if (pframe->type == agora::recording::VIDEO_FRAME_RAW_YUV) {
    agora::recording::VideoYuvFrame *f = pframe->frame.yuv;
    suffix=".yuv";

    cout << "User " << uid << ", received a yuv frame, width: "
        << f->width_ << ", height: " << f->height_ ;
    cout<<",ystide:"<<f->ystride_<< ",ustride:"<<f->ustride_<<",vstride:"<<f->vstride_;
    cout<< endl;
  } else if(pframe->type == agora::recording::VIDEO_FRAME_JPG) {
    suffix=".jpg";
    agora::recording::VideoJpgFrame *f = pframe->frame.jpg;

    cout << "User " << uid << ", received an jpg frame, timestamp: "
    << f->frame_ms << endl;

    struct tm date;
    time_t t = time(NULL);
    localtime_r(&t, &date);
    char timebuf[128];
    sprintf(timebuf, "%04d%02d%02d%02d%02d%02d", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday, date.tm_hour, date.tm_min, date.tm_sec);
    std::string file_name = m_recordingDir + std::string(uidbuf) + "_" + std::string(timebuf) + suffix;
    FILE *fp = fopen(file_name.c_str(), "w");
    ::fwrite(f->buf_, 1, f->bufSize_, fp);
    ::fclose(fp);
    return;
  } else {
    suffix=".h264";
    agora::recording::VideoH264Frame *f = pframe->frame.h264;

    cout << "User " << uid << ", received an h264 frame, timestamp: "
        << f->frame_ms << ", frame no: " << f->frame_num << endl;
  }

  std::string info_name = m_recordingDir + std::string(uidbuf) /*+ timestamp_per_join_ */+ std::string(suffix);
  FILE *fp = fopen(info_name.c_str(), "a+b");

  //store it as file
  if (pframe->type == agora::recording::VIDEO_FRAME_RAW_YUV) {
      agora::recording::VideoYuvFrame *f = pframe->frame.yuv;
      ::fwrite(f->buf_, 1, f->bufSize_, fp);
  }
  else {
      agora::recording::VideoH264Frame *f = pframe->frame.h264;
      ::fwrite(f->buf_, 1, f->bufSize_, fp);
  }
  ::fclose(fp);

}
