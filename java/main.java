import headers.*;
import headers.EnumIndex.*;
import java.lang.InterruptedException;
class AgoraJavaRecording{

  static{
    System.loadLibrary("recording");
  }
	//Native method declaration
  public native boolean createChannel(String appid, String channelKey, String name,  int uid,
                RecordingConfig config);
	public native boolean leaveChannel();

	//Called by C++
	public static void onLeaveChannel(LEAVE_PATH_CODE reason){
		System.out.println("AgoraJavaRecording onLeaveChannel,code:"+reason.getValue());
		stopped = true;
	}
  public void onError(int error, STAT_CODE_TYPE stat_code) {
    System.out.println("AgoraJavaRecording onLeaveChannel,code:"+error+",stat code:"+stat_code);
    stopped = true;
  }
	public void audioPcmFrameReceived(int uid, AudioPcmFrame pcmFrame)
  {
    System.out.println("java demo audioFrameReceived ");
  }
	public void audioAacFrameReceived(int uid, AudioPcmFrame pcmFrame)
  {
    System.out.println("java demo audioAacFrameReceived ");
  }


  
	//stop java process flag
	private static boolean stopped = false;
	public static boolean stopped()
	{
		return stopped;
	}

  public static void main(String[] args) 
  {
		System.out.println(System.getProperty("java.library.path"));
    //MixModeSettings settings;
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
		
		while(!stopped())
		{
			try{
				Thread.currentThread().sleep(10);//sleep 10 ms
				//System.out.println("in a loop");
			}
			catch(InterruptedException ie){
				System.out.println("exception throwed!");
			}
		}

    System.out.println("jni layer has been exited");
		


  }
}
