//package headers;
/*
enum STAT_CODE_TYPE {
		 STAT_ERR_FROM_ENGINE(1),
     STAT_ERR_ARS_JOIN_CHANNEL(2),
     STAT_ERR_CREATE_PROCESS(3)
     STAT_ERR_MIXED_INVALID_VIDEO_PARAM(4),
     STAT_POLL_ERR(0x8),
     STAT_POLL_HANG_UP(0x10),
     STAT_POLL_NVAL (0x20)
 };
*/

class MixModeSettings{
  public int m_height;
  public int m_width;
  public boolean m_videoMix;
  {
    m_height = 0;
    m_width = 0;
    m_videoMix = false;
  }
}

class RecordingConfig {
  boolean isAudioOnly;
  boolean isVideoOnly;
  boolean isMixingEnabled;
  boolean mixedVideoAudio;
  String mixResolution;
  String decryptionMode;
  String secret;
  String appliteDir;
  String recordFileRootDir;
  String cfgFilePath;
  //VIDEO_FORMAT_TYPE decodeVideo;
  //AUDIO_FORMAT_TYPE decodeAudio;
  int lowUdpPort;
  int highUdpPort;
  int idleLimitSec;
  int captureInterval;
  //CHANNEL_PROFILE_TYPE channelProfile;
  //REMOTE_VIDEO_STREAM_TYPE streamType;
}
