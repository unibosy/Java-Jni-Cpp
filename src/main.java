import headers.*;
import headers.EnumIndex.CHANNEL_PROFILE_TYPE;
class AgoraJavaRecording{

  class AgoraJavaEventHandler{

    void onWarning(int warn){
      System.out.println("java enent handler on warning!");
    }

  }

  static{
    //System.loadLibrary("/home/wangshiwei/ars/code_ars/javaDemo/bin/librecording.so");
    System.loadLibrary("recording");
  }

  public native boolean createChannel(String appid, String channelKey, String name,  int uid,
                RecordingConfig config);

  public static void main(String[] args) 
  {
		System.out.println(System.getProperty("java.library.path"));
    MixModeSettings settings;
    AgoraJavaRecording ars = new AgoraJavaRecording();
    String appid = "0c0b4b61adf94de1befd7cdd78a50444";
    String channelKey = "";
    String name = "video";
    int uid = 0;
    RecordingConfig config=new RecordingConfig();
		CHANNEL_PROFILE_TYPE profile = CHANNEL_PROFILE_TYPE.CHANNEL_PROFILE_LIVE_BROADCASTING;
		config.channelProfile = profile;
		config.idleLimitSec = 20;
		config.isMixingEnabled = false;
		config.appliteDir="./../native_layer/cppwrapper/bin";//contain Chines path????
    config.recordFileRootDir = ".";
		System.out.println("to create channel");
    ars.createChannel(appid, channelKey,name,uid,config);

    System.out.println("create channel end");
  }
}
