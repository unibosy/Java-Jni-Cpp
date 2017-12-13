import headers.*;
import headers.EnumIndex.*;
import java.lang.InterruptedException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.File; //File
import java.io.FileNotFoundException;
import java.io.FileOutputStream; 
import java.util.*; //vector
import java.text.SimpleDateFormat;
import java.util.Date;//date
class AgoraJavaRecording{

  static{
    System.loadLibrary("recording");
  }
	//Native method declaration
  public native boolean createChannel(String appid, String channelKey, String name,  int uid,
                RecordingConfig config);
	public native boolean leaveChannel();
  public native int setVideoMixingLayout(VideoMixingLayout layout);
  //public native boolean checkJniStatus();

	//Called by C++
	public void onLeaveChannel(int reason){
    EnumIndex ei = new EnumIndex();
    EnumIndex.LEAVE_PATH_CODE lpc = ei.new LEAVE_PATH_CODE(reason);
		System.out.println("AgoraJavaRecording onLeaveChannel,code:"+lpc.getValue());
    stopped = true;
	}
  public void onError(int error, int stat_code) {
    System.out.println("AgoraJavaRecording onError,error:"+error+",stat code:"+stat_code);
    stopped = true;
  }
  public void onWarning(int warn) {
    //refer to "WARN_CODE_TYPE"
    System.out.println("AgoraJavaRecording onWarning,warn:"+warn);
  }
  public String storageDir = "./";
  public void onUserOffline(long uid, int reason) {
    System.out.println("AgoraJavaRecording onUserOffline uid:"+uid+",offline reason:"+reason);
    EnumIndex ei = new EnumIndex();
    EnumIndex.USER_OFFLINE_REASON_TYPE offline = ei.new USER_OFFLINE_REASON_TYPE(reason);
    System.out.println("AgoraJavaRecording onUserOffline,code:"+offline.getValue());
    SetVideoMixingLayout();
  }
  public void onUserJoined(long uid, String recordingDir){
    //recordingDir:recording file directory
    System.out.println("onUserJoined uid:"+uid+",recordingDir:"+recordingDir);
    storageDir = recordingDir;
    SetVideoMixingLayout();
  }
  public static int count = 0;
	public void audioFrameReceived(long uid, AudioFrame aFrame)
  { 
    if(aFrame.type.getValue() == 0) {//pcm
      count++;
      System.out.println("java demo audioFrameReceived,uid:"+uid+",AUDIO_FRAME_TYPE:"+aFrame.type.getValue()
                        +",frame_ms_:"+aFrame.pcm.frame_ms_+",bufSize:"+ aFrame.pcm.pcmBufSize_);
      System.out.println(count);

      String path = storageDir + Long.toString(uid)+".pcm";
			byte[] buf = aFrame.pcm.pcmBuf_;
			WriteBytesToFileClassic(buf, path);
    }else if(aFrame.type.getValue() == 1){//aac
      count++;
      System.out.println("java demo audioFrameReceived,uid:"+uid+",AUDIO_FRAME_TYPE:"+aFrame.type.getValue()
                        +",frame_ms_:"+aFrame.aac.frame_ms_+",bufSize:"+ aFrame.aac.aacBufSize_);
      System.out.println(count);
      String path = storageDir + Long.toString(uid)+".aac";
			byte[] buf = aFrame.aac.aacBuf_;
			WriteBytesToFileClassic(buf, path);
      }
  }
  public void videoFrameReceived(long uid, VideoFrame frame)
  {
    String path = storageDir + Long.toString(uid);
    //byte[] buf ;
    System.out.println("java demo audioFrameReceived,uid:"+uid+",type:"+frame.type.getValue());
    if(frame.type.getValue() == 0){//yuv
      path += ".yuv";
      byte[] buf = frame.yuv.buf_;
      WriteBytesToFileClassic(buf, path);
    }else if(frame.type.getValue() == 1) {//h264
      path +=  ".h264";
      byte[] buf = frame.h264.buf_;
      WriteBytesToFileClassic(buf, path);
    }else if(frame.type.getValue() == 2) { // jpg
      path += GetNowDate() + ".jpg";
      byte[] buf = frame.jpg.buf_;
      WriteBytesToFileClassic(buf, path);
    }
  }

  private void SetVideoMixingLayout(){
    System.out.println("setVideoMixingLayout");
    EnumIndex ei = new EnumIndex();
    EnumIndex.VideoMixingLayout layout = ei.new VideoMixingLayout();
    //layout.
    setVideoMixingLayout(layout);
  }
  
  public void stopCallBack() {
    System.out.println("java demo receive stop from JNI ");
  }
  private boolean stopped = false;

	private void WriteBytesToFileClassic(byte[] bFile, String fileDest) {
		FileOutputStream fileOuputStream = null;
    System.out.println("java writeBytesToFileClassic buf:"+bFile);
				try {
            fileOuputStream = new FileOutputStream(fileDest, true);
            fileOuputStream.write(bFile);
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            if (fileOuputStream != null) {
                try {
                    fileOuputStream.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
       }
  }
	private String GetNowDate(){
    String temp_str="";   
    Date dt = new Date();   
    SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss aa");   
    temp_str=sdf.format(dt);   
    return temp_str;   
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
    AUDIO_FORMAT_TYPE decodeAudio = AUDIO_FORMAT_TYPE.AUDIO_FORMAT_AAC_FRAME_TYPE;
    VIDEO_FORMAT_TYPE decodeVideo = VIDEO_FORMAT_TYPE.VIDEO_FORMAT_JPG_FRAME_TYPE;
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
		
		while(!ars.stopped)
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
