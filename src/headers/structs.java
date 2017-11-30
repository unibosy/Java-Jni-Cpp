import headers.*;
import headers.EnumIndex.CHANNEL_PROFILE_TYPE;
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
	public CHANNEL_PROFILE_TYPE channelProfile;
  //REMOTE_VIDEO_STREAM_TYPE streamType;
}
