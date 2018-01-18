//define signatures
#define LONG_SIGNATURE "J"
#define INT_SIGNATURE "I"
#define BOOL_SIGNATURE "Z"
#define DOUBLE_SIGNATURE "D"
#define LONG_PARA_VOID_RETURN "(J)V" 
#define EMPTY_PARA_INT_RETURN "()I"
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
//init jmethodID
#define MTD_INIT_SIGNATURE "<init>"
#define VIDEO_FRAME_SIGNATURE "(Lio/agora/recording/common/Common;)V"
#define MTD_INIT_VIDEO_FRAME "(Lio/agora/recording/common/Common;)V"

#define SN_CB_FUNC_RECEIVE_VIDEOFRAME "(JLio/agora/recording/common/Common$VideoFrame;)V"
//declare java callback function
#define CB_FUNC_RECEIVE_VIDEOFRAME "videoFrameReceived"

//declare filedIDs
#define FID_VIDEO_YUV_FRAME_BUF "buf_"
#define FID_VIDEO_YUV_FRAME_BUFSIZE "bufSize_"
#define FID_VIDEO_FRAME_YUV "yuv"
#define FID_VIDEO_YUV_FRAME_FRAMEMS "frame_ms_"

//static java class init jmethodIDs
static jmethodID mJavaVideoFrameInitMtd;
static jmethodID mJavaVideoYuvFrameInitMtd;

//-----------------Video End-------------------

//-----------------Audio Begin-------------------
//class name
//Audio
#define CN_AUDIO_FRAME "io/agora/recording/common/Common$AudioFrame"
#define CN_AUDIO_AAC_FRAME "io/agora/recording/common/Common$AudioAacFrame"
#define CN_AUDIO_PCM_FRAME "io/agora/recording/common/Common$AudioPcmFrame"

//-----------------Audio End-------------------


//-----------------Callback Java method----------------
static jmethodID mJavaRecvVideoMtd = NULL;

static jmethodID mJavaRecvAudioMtd = NULL;
 

