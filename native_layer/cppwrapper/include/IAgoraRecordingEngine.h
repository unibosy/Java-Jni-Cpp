#pragma once

namespace agora {
namespace recording {

typedef unsigned char uchar_t;
typedef unsigned int uint_t;
typedef unsigned int uid_t;

enum ERROR_CODE_TYPE {
    ERR_OK = 0,
    //1~1000
    ERR_FAILED = 1,
    ERR_INVALID_ARGUMENT = 2,
    ERR_INTERNAL_FAILED = 3,
};

enum STAT_CODE_TYPE {
    STAT_OK = 0,
    STAT_ERR_FROM_ENGINE = 1,
    STAT_ERR_ARS_JOIN_CHANNEL = 2,
    STAT_ERR_CREATE_PROCESS = 3,
    STAT_ERR_MIXED_INVALID_VIDEO_PARAM = 4,

    STAT_POLL_ERR = 0x8,
    STAT_POLL_HANG_UP = 0x10,
    STAT_POLL_NVAL = 0x20,
};

enum LEAVE_PATH_CODE {
    LEAVE_CODE_INIT = 0, 
    LEAVE_CODE_SIG = 1<<1,
    LEAVE_CODE_NO_USERS = 1<<2,
    LEAVE_CODE_TIMER_CATCH = 1<<3,
    LEAVE_CODE_CLIENT_LEAVE = 1 << 4,
};


enum WARN_CODE_TYPE {
    WARN_NO_AVAILABLE_CHANNEL = 103,
    WARN_LOOKUP_CHANNEL_TIMEOUT = 104,
    WARN_LOOKUP_CHANNEL_REJECTED = 105,
    WARN_OPEN_CHANNEL_TIMEOUT = 106,
    WARN_OPEN_CHANNEL_REJECTED = 107,
};

enum CHANNEL_PROFILE_TYPE
{
    CHANNEL_PROFILE_COMMUNICATION = 0,
    CHANNEL_PROFILE_LIVE_BROADCASTING = 1,
};

enum USER_OFFLINE_REASON_TYPE
{
    USER_OFFLINE_QUIT = 0,
    USER_OFFLINE_DROPPED = 1,
    USER_OFFLINE_BECOME_AUDIENCE = 2,
};

enum REMOTE_VIDEO_STREAM_TYPE
{
    REMOTE_VIDEO_STREAM_HIGH = 0,
    REMOTE_VIDEO_STREAM_LOW = 1,
};

enum VIDEO_FORMAT_TYPE {
    VIDEO_FORMAT_DEFAULT_TYPE = 0,
    VIDEO_FORMAT_H264_FRAME_TYPE = 1,
    VIDEO_FORMAT_YUV_FRAME_TYPE = 2,
    VIDEO_FORMAT_JPG_FRAME_TYPE = 3,
    VIDEO_FORMAT_JPG_FILE_TYPE = 4,
};

enum AUDIO_FORMAT_TYPE {
    AUDIO_FORMAT_DEFAULT_TYPE = 0,
    AUDIO_FORMAT_AAC_FRAME_TYPE = 1,
    AUDIO_FORMAT_PCM_FRAME_TYPE = 2,
};

enum AUDIO_FRAME_TYPE {
  AUDIO_FRAME_RAW_PCM = 0,
  AUDIO_FRAME_AAC = 1
};

enum VIDEO_FRAME_TYPE {
  VIDEO_FRAME_RAW_YUV = 0,
  VIDEO_FRAME_H264 = 1,
  VIDEO_FRAME_JPG = 2,
};


class AudioPcmFrame {
  friend class RecordingEngineImpl;
 public:
  AudioPcmFrame(uint_t frame_ms, uint_t sample_rates, uint_t samples);
  ~AudioPcmFrame();
 public:
  uint_t frame_ms_;
  uint_t channels_; // 1
  uint_t sample_bits_; // 16
  uint_t sample_rates_; // 8k, 16k, 32k
  uint_t samples_;

  uchar_t *pcmBuf_;
  uint_t pcmBufSize_;

 private:
  std::string buf_; // samples * sample_bits_ / CHAR_BIT * channels_
};

class AudioAacFrame {
  friend class RecordingEngineImpl;
 public:
  explicit AudioAacFrame(uint_t frame_ms);
  ~AudioAacFrame();
  
  uchar_t *aacBuf_;
  uint_t frame_ms_;
  uint_t aacBufSize_;
 private:
  std::string buf_;
};

struct AudioFrame {
  AUDIO_FRAME_TYPE type;
  union {
    AudioPcmFrame *pcm;
    AudioAacFrame *aac;
  } frame;
};

class VideoYuvFrame {
  friend class RecordingEngineImpl;
public:
  VideoYuvFrame(uint_t frame_ms, uint_t width, uint_t height, uint_t ystride,
      uint_t ustride, uint_t vstride);
  ~VideoYuvFrame();

  uint_t frame_ms_;

  uchar_t *ybuf_;
  uchar_t *ubuf_;
  uchar_t *vbuf_;

  uint_t width_;
  uint_t height_;

  uint_t ystride_;
  uint_t ustride_;
  uint_t vstride_;

  //all
  uchar_t *buf_;
  uint_t bufSize_;

private:
  std::string data_;
};

struct VideoH264Frame {
  friend class RecordingEngineImpl;
public:
  uint_t frame_ms;
  uint_t frame_num;

  //all
  uchar_t *buf_;
  uint_t bufSize_;

private:
  std::string payload;
};

struct VideoJpgFrame {
    friend class RecordingEngineImpl;
  public:
    uint_t frame_ms;
  
    //all
    uchar_t *buf_;
    uint_t bufSize_;
  
  private:
    std::string payload;
};

struct VideoFrame {
  VIDEO_FRAME_TYPE type;
  union {
    VideoYuvFrame *yuv;
    VideoH264Frame *h264;
    VideoJpgFrame *jpg;
  } frame;

  int rotation; // 0, 90, 180, 270
};



typedef struct VideoMixingLayout
{
    struct Region {
        uid_t uid;
        double x;//[0,1]
        double y;//[0,1]
        double width;//[0,1]
        double height;//[0,1]
        int zOrder; //optional, [0, 100] //0 (default): bottom most, 100: top most

        //  Optional
        //  [0, 1.0] where 0 denotes throughly transparent, 1.0 opaque
        double alpha;

        int renderMode;//RENDER_MODE_HIDDEN: Crop, RENDER_MODE_FIT: Zoom to fit
        Region()
        :uid(0)
        , x(0)
        , y(0)
        , width(0)
        , height(0)
        , zOrder(0)
        , alpha(1.0)
        , renderMode(1)
        {}

    };
    int canvasWidth;
    int canvasHeight;
    const char* backgroundColor;//e.g. "#C0C0C0" in RGB
    uint32_t regionCount;
    const Region* regions;
    const char* appData;
    int appDataLength;
    VideoMixingLayout()
    :canvasWidth(0)
    , canvasHeight(0)
    , backgroundColor(NULL)
    , regionCount(0)
    , regions(NULL)
    , appData(NULL)
    , appDataLength(0)
    {}
} VideoMixingLayout;

typedef struct UserJoinInfos {
    const char* recordingDir;
    //new attached info add below

    UserJoinInfos():
    recordingDir(NULL)
    {}
}UserJoinInfos;


class IRecordingEngineEventHandler {
public:
    virtual ~IRecordingEngineEventHandler() {}
    
    /**
     *  Callback when an error occurred during the runtime of recording engine
     *
     *
     *  @param error        Error code
     *  @param stat_code    state code
     *
     */
    virtual void onError(int error, STAT_CODE_TYPE stat_code) = 0;

     /**
     *  Callback when an warning occurred during the runtime of recording engine
     *
     *
     *  @param warn         warning code
     *
     */
    virtual void onWarning(int warn) = 0;
   
     /**
     *  Callback when the user hase successfully joined the specified channel
     *
     *
     *  @param channelID    channel ID 
     *  @param uid          User ID
     *
     */
    virtual void onJoinChannelSuccess(const char * channelId, uid_t uid) = 0;
   
    /**
     *  Callback when recording application successfully left the channel
     *
     *
     *  @param code        leave path code
     *
     */
    virtual void onLeaveChannel(LEAVE_PATH_CODE code) = 0;

    /**
     *  Callback when another user successfully joined the channel
     *
     *
     *  @param uid          user ID
     *  @param infos        user join information    
     *
     */
    virtual void onUserJoined(uid_t uid, UserJoinInfos &infos) = 0;
   
     /**
     *  Callback when a user left the channel or gone offline
     *
     *
     *  @param uid          user ID
     *  @param reason       offline reason    
     *
     */
    virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) = 0;

     /**
     *  Callback when received a audio frame
     *
     *
     *  @param uid          user ID
     *  @param frame        pointer to received audio frame    
     *
     */
    virtual void audioFrameReceived(unsigned int uid, const AudioFrame *frame) const = 0;

      /**
     *  Callback when received a video frame
     *
     *
     *  @param uid          user ID
     *  @param frame        pointer to received video frame    
     *
     */
    virtual void videoFrameReceived(unsigned int uid, const VideoFrame *frame) const = 0;
};

typedef struct RecordingConfig {
    bool isAudioOnly;
    bool isVideoOnly;
    bool isMixingEnabled;
    bool mixedVideoAudio;
    char * mixResolution;
    char * decryptionMode;
    char * secret;
    char * appliteDir;
    char * recordFileRootDir;
    char * cfgFilePath;
    VIDEO_FORMAT_TYPE decodeVideo;
    AUDIO_FORMAT_TYPE decodeAudio; 
    int lowUdpPort;
    int highUdpPort;  
    int idleLimitSec;
    int captureInterval;
    CHANNEL_PROFILE_TYPE channelProfile;
    REMOTE_VIDEO_STREAM_TYPE streamType;

    RecordingConfig(): channelProfile(CHANNEL_PROFILE_COMMUNICATION),
        isAudioOnly(false),
        isVideoOnly(false),
        isMixingEnabled(false),
        mixResolution(NULL),
        decryptionMode(NULL),
        secret(NULL),
        idleLimitSec(300),
        appliteDir(NULL),
        recordFileRootDir(NULL),
        cfgFilePath(NULL),
        lowUdpPort(0),
        highUdpPort(0),
        captureInterval(5),
        decodeAudio(AUDIO_FORMAT_DEFAULT_TYPE),
        decodeVideo(VIDEO_FORMAT_DEFAULT_TYPE),
        mixedVideoAudio(false),
        streamType(REMOTE_VIDEO_STREAM_HIGH)
    {}
} RecordingConfig;

typedef struct RecordingEngineProperties {
    char* recordingDir;
    RecordingEngineProperties(): recordingDir(NULL)
                          {}
}RecordingEngineProperties;

class IRecordingEngine{
public:

    /**
     *  create a new recording engine instance
     *
     *  @param appId        The App ID issued to the application developers by Agora.io.
     *  @param eventHandler the callback interface
     *
     *  @return a recording engine instance pointer
     */
    static IRecordingEngine* createAgoraRecordingEngine(const char * appId, IRecordingEngineEventHandler *eventHandler);

    virtual ~IRecordingEngine() {}

    /**
     *  This method lets the recording engine join a channel, and start recording
     *
     *  @param channelKey This parameter is optional if the user uses a static key, or App ID. In this case, pass NULL as the parameter value. More details refer to http://docs-origin.agora.io/en/user_guide/Component_and_Others/Dynamic_Key_User_Guide.html
     *  @param channelId  A string providing the unique channel id for the AgoraRTC session
     *  @param uid        The uid of recording client
     *  @param config     The config of current recording
     *
     *  @return 0: Method call succeeded. <0: Method call failed.
     */
    virtual int joinChannel(const char * channelKey, const char *channelId, uid_t uid, const RecordingConfig &config) = 0;

    /**
     *  set the layout of video mixing
     *
     *  @param layout layout setting
     *
     *  @return 0: Method call succeeded. <0: Method call failed.
     */
    virtual int setVideoMixingLayout(const VideoMixingLayout &layout) = 0;

    /**
     *  Stop recording
     *
     *  @return 0: Method call succeeded. <0: Method call failed.
     */
    virtual int leaveChannel() = 0;

    /**
     *  release recording engine
     *
     *  @return 0: Method call succeeded. <0: Method call failed.
     */
    virtual int release() = 0;

    /**
     * Get recording properties
     */
    virtual const RecordingEngineProperties* getProperties() = 0;
};

}
}
