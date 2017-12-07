import headers.*;
import headers.EnumIndex.*;

class AgoraJavaRecording{

  class AgoraJavaEventHandler{

    void onWarning(int warn){
      System.out.println("java enent handler on warning!");
    }

  }

  static{
    System.loadLibrary("recording");
  }

  public native boolean createChannel(String appid, String channelKey, String name,  int uid,
                RecordingConfig config);
	public native boolean leaveChannel();
	
	//callback from C++
	public native void onLeaveChannel(){
		System.out.println("onLeaveChannel");
	}

  public static void main(String[] args) 
  {
		System.out.println(System.getProperty("java.library.path"));
    MixModeSettings settings;
    AgoraJavaRecording ars = new AgoraJavaRecording();
    String appid = "0c0b4b61adf94de1befd7cdd78a50444";
    String channelKey = "";
    String name = "video";
    int uid = 3333;
    RecordingConfig config= new RecordingConfig();
		CHANNEL_PROFILE_TYPE profile = CHANNEL_PROFILE_TYPE.CHANNEL_PROFILE_LIVE_BROADCASTING;
		REMOTE_VIDEO_STREAM_TYPE streamType = REMOTE_VIDEO_STREAM_TYPE.REMOTE_VIDEO_STREAM_HIGH;
		config.channelProfile = profile;
		config.streamType = streamType;
		config.idleLimitSec = 3;
		config.isMixingEnabled = false;
		config.appliteDir="./../native_layer/cppwrapper/bin";//contain Chines path????
    config.recordFileRootDir = ".";
		System.out.println("to create channel,profile:"+profile);
		System.out.println("to create channel,profile value:"+profile.getValue());
    ars.createChannel(appid, channelKey,name,uid,config);

    System.out.println("create channel end");
  }
}
