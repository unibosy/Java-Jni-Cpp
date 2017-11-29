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
  public boolean isAudioOnly;
  public boolean isVideoOnly;
 	public boolean isMixingEnabled;
  public boolean mixedVideoAudio;
  public String mixResolution;
  public String decryptionMode;
  public String secret;
  public String appliteDir;
  public String recordFileRootDir;
  public String cfgFilePath;
  //VIDEO_FORMAT_TYPE decodeVideo;
  //AUDIO_FORMAT_TYPE decodeAudio;
  public int lowUdpPort;
  public int highUdpPort;
  public int idleLimitSec;
  public int captureInterval;
  //CHANNEL_PROFILE_TYPE channelProfile;
  //REMOTE_VIDEO_STREAM_TYPE streamType;
}
