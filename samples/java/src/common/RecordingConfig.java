import io.agora.recording.common.*;
import io.agora.recording.common.Common.*;

import io.agora.recording.common.Common.CHANNEL_PROFILE_TYPE;


class RecordingEngineProperties{
  private String storageDir;
  public String GetStorageDir(){
    return storageDir;
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
  public VIDEO_FORMAT_TYPE decodeVideo;
  public AUDIO_FORMAT_TYPE decodeAudio;
  public int lowUdpPort;
  public int highUdpPort;
  public int idleLimitSec;
  public int captureInterval;
	public CHANNEL_PROFILE_TYPE channelProfile;
  public REMOTE_VIDEO_STREAM_TYPE streamType;
  public int triggerMode;
}
