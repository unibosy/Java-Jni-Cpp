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
  public native int setVideoMixingLayout(VideoMixingLayout layout);

	//Called by C++
	public static void onLeaveChannel(int reason){
    EnumIndex ei = new EnumIndex();
    EnumIndex.LEAVE_PATH_CODE lpc = ei.new LEAVE_PATH_CODE(reason);
		System.out.println("AgoraJavaRecording onLeaveChannel,code:"+lpc.getValue());
		//stopped = true;
    System.out.println("AgoraJavaRecording onLeaveChannel,stopped"+stopped);
	}
  public static void onError(int error, int stat_code) {
    System.out.println("AgoraJavaRecording onError,error:"+error+",stat code:"+stat_code);
    stopped = true;
  }
  public static void onWarning(int warn) {
    //refer to "WARN_CODE_TYPE"
    System.out.println("AgoraJavaRecording onWarning,warn:"+warn);
  }
  public static void onUserOffline(long uid, int reason) {
    System.out.println("AgoraJavaRecording onUserOffline uid:"+uid+",offline reason:"+reason);
    EnumIndex ei = new EnumIndex();
    EnumIndex.USER_OFFLINE_REASON_TYPE offline = ei.new USER_OFFLINE_REASON_TYPE(reason);
    System.out.println("AgoraJavaRecording onUserOffline,code:"+offline.getValue());
  }
  public static void onUserJoined(long uid, String recordingDir){
    //recordingDir:recording file directory
    System.out.println("onUserJoined uid:"+uid+",recordingDir:"+recordingDir);
  }
	public static void audioPcmFrameReceived(long uid, AudioPcmFrame pcmFrame)
  {
    System.out.println("java demo audioFrameReceived,uid:"+uid);
  }
	public static void audioAacFrameReceived(long uid, AudioPcmFrame pcmFrame)
  {
    System.out.println("java demo audioAacFrameReceived ");
  }
  public static void stopCallBack() {
    System.out.println("java demo receive stop from JNI ");
    stopped = true;
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
		CHANNEL_PROFILE_TYPE profile = CHANNEL_PROFILE_TYPE.CHANNEL_PROFILE_COMMUNICATION;
		REMOTE_VIDEO_STREAM_TYPE streamType = REMOTE_VIDEO_STREAM_TYPE.REMOTE_VIDEO_STREAM_HIGH;
    AUDIO_FORMAT_TYPE decodeAudio = AUDIO_FORMAT_TYPE.AUDIO_FORMAT_PCM_FRAME_TYPE;
    VIDEO_FORMAT_TYPE decodeVideo = VIDEO_FORMAT_TYPE.VIDEO_FORMAT_H264_FRAME_TYPE;
		config.channelProfile = profile;
		config.streamType = streamType;
		config.idleLimitSec = 3;
		config.isMixingEnabled = false;
		config.appliteDir="./../native_layer/cppwrapper/bin";//contain Chines path????
    config.recordFileRootDir = ".";
    config.decodeAudio = decodeAudio;
    config.decodeVideo = decodeVideo;
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
