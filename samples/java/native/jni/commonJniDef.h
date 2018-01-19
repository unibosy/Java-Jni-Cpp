
//define signatures
#define LONG_SIGNATURE "J"
#define INT_SIGNATURE "I"
#define BOOL_SIGNATURE "Z"
#define DOUBLE_SIGNATURE "D"
#define LONG_PARA_VOID_RETURN "(J)V" 
#define EMPTY_PARA_INT_RETURN "()I"
#define SG_INT "I"
#define STRING_SIGNATURE "Ljava/lang/String;"
#define BYTEARRAY "Ljava/nio/ByteBuffer;"
#define CHANNEL_PROFILE_SIGNATURE "Lio/agora/recording/common/Common$CHANNEL_PROFILE_TYPE;"
#define REMOTE_VIDEO_STREAM_SIGNATURE "Lio/agora/recording/common/Common$REMOTE_VIDEO_STREAM_TYPE;"
//-----------------Video Begin-------------------
//#define VIDEOFRAME_SIGNATURE "io/agora/recording/common/Common$VideoFrame"
#define VIDEOFRAME_H264_SIGNATURE "Lio/agora/recording/common/Common$VideoH264Frame;"
#define VIDEOFRAME_YUV_SIGNATURE "Lio/agora/recording/common/Common$VideoYuvFrame;"
#define VIDEOFRAME_JPG_SIGNATURE "Lio/agora/recording/common/Common$VideoJpgFrame;" 
#define VIDEO_FRAME_TYPE_SIGNATURE "Lio/agora/recording/common/Common$VIDEO_FRAME_TYPE;"
#define AUDIO_FORMAT_TYPE_SIGNATURE "Lio/agora/recording/common/Common$AUDIO_FORMAT_TYPE;"
#define VIDEO_FORMAT_TYPE_SIGNATURE "Lio/agora/recording/common/Common$VIDEO_FORMAT_TYPE;"
#define VIDEOMIXLAYOUT_SIGNATURE "[Lio/agora/recording/common/Common$VideoMixingLayout$Region;"
//jmethodID signature
#define SN_MTD_COMMON_INIT "(Lio/agora/recording/common/Common;)V"
//video  init jmethodID 
#define SN_MTD_VIDEO_YUV_FRAME_INIT "(Lio/agora/recording/common/Common;JIIIII)V"
//class name
//VIDEO
#define CN_REP "io/agora/recording/common/RecordingEngineProperties"
#define CN_VIDEO_FRAME "io/agora/recording/common/Common$VideoFrame"
#define CN_VIDEO_YUV_FRAME "io/agora/recording/common/Common$VideoYuvFrame"
#define CN_VIDEO_JPG_FRAME "io/agora/recording/common/Common$VideoJpgFrame"
#define CN_VIDEO_H264_FRAME "io/agora/recording/common/Common$VideoH264Frame"

//video frame type clas
#define CN_VIDEO_FRAME_TYPE "io/agora/recording/common/Common$VIDEO_FRAME_TYPE"

//init jmethodID
#define SG_MTD_INIT "<init>"
#define SG_COMMON_INIT "<init>"
#define MTD_TYPE "type"

#define VIDEO_FRAME_SIGNATURE "(Lio/agora/recording/common/Common;)V"
#define MTD_INIT_VIDEO_FRAME "(Lio/agora/recording/common/Common;)V"

#define SN_CB_FUNC_RECEIVE_VIDEOFRAME "(JLio/agora/recording/common/Common$VideoFrame;)V"
//declare java callback function
#define CB_FUNC_RECEIVE_VIDEOFRAME "videoFrameReceived"

#define CB_FUNC_RECEIVE_AUDIOFRAME "audioFrameReceived"

//declare filedIDs
#define FID_VIDEO_YUV_FRAME_BUF "buf_"
#define FID_VIDEO_YUV_FRAME_BUFSIZE "bufSize_"
#define FID_VIDEO_FRAME_YUV "yuv"
#define FID_FRAME_MS "frame_ms_"

//static java class init jmethodIDs
static jmethodID mJavaVideoFrameInitMtd;
static jmethodID mJavaVideoYuvFrameInitMtd;

//-----------------Video End-------------------

//-----------------Audio Begin-------------------
//class name
//Audio
#define CN_AUDIO_FRAME "io/agora/recording/common/Common$AudioFrame"
#define CN_AUDIO_FRAME_TYPE "io/agora/recording/common/Common$AUDIO_FRAME_TYPE"
#define CN_AUDIO_AAC_FRAME "io/agora/recording/common/Common$AudioAacFrame"
#define CN_AUDIO_PCM_FRAME "io/agora/recording/common/Common$AudioPcmFrame"
#define SN_CB_FUNC_RECEIVE_AUDIOFRAME "(JLio/agora/recording/common/Common$AudioFrame;)V"

#define SN_AUDIO_FRAME_TYPE "Lio/agora/recording/common/Common$AUDIO_FRAME_TYPE;"
#define SN_INIT_MTD_AUDIO_FRAME "(Lio/agora/recording/common/Common;JJJ)V"

#define SN_AUDIO_PCM_FRAME "Lio/agora/recording/common/Common$AudioPcmFrame;"


//-----------------Audio End-------------------

//common
//audio pcm 
#define FID_CHANNELS "channels_"
#define FID_SAMPLE_BITS "sample_bits_"
#define FID_SAMPLE_RATES "sample_rates_"
#define FID_PCM_BUF "pcmBuf_"
#define FID_PCM_BUFFER_SIZE "pcmBufSize_"
#define FID_PCM "pcm"
//-----------------Callback Java method----------------
static jmethodID mJavaRecvVideoMtd = NULL;

static jmethodID mJavaRecvAudioMtd = NULL;

/*
struct JavaObjectMethod {
  int id;
  const char* name;
  const char* signature;
};
enum ObjectMethodId {
  MID_ON_EVENT = 0,
  MID_ON_LOG_EVENT = 1,
  MID_GET_NETWORK_TYPE = 2,
  MID_GET_NETWORK_INFO = 3,
  MID_GET_BATTERY_LIFE_PERCENT = 4,
  MID_OBJECT_NUM = 5,
};
enum ClassMethodId {
  MID_GET_RANDOM_UUID = 0,
  MID_GET_LOCAL_HOST = 1,
  MID_GET_LOCAL_HOST_LIST = 2,
  MID_CLASS_NUM = 3,
};
static JavaObjectMethod jObjectMethods[] = {
  { MID_ON_EVENT, "onEvent", "(I[B)V" },
  { MID_ON_LOG_EVENT, "onLogEvent", "(ILjava/lang/String;)V" },
  { MID_GET_NETWORK_TYPE, "getNetworkType", "()I" },
  { MID_GET_NETWORK_INFO, "getNetworkInfo", "()[B" },
  { MID_GET_BATTERY_LIFE_PERCENT, "getBatteryLifePercent", "()I" },
};
static JavaObjectMethod jVideoFrameClassMethods[] = {
  { MID_GET_RANDOM_UUID, "getRandomUUID", "()Ljava/lang/String;" },
  { MID_GET_LOCAL_HOST, "getLocalHost", "()Ljava/lang/String;" },
  { MID_GET_LOCAL_HOST_LIST, "getLocalHostList", "()[Ljava/lang/String;" },
};
*/
