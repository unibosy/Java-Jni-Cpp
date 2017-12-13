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
import java.util.Vector;
class AgoraJavaRecording{
  //java run status flag
  private boolean stopped = false;
  private boolean isMixMode = false;
  private String mixResolution= "360,640,15,500";
  private String storageDir = "./";
  private Vector m_peers = new Vector();
  private MixModeSettings m_mixRes;
  private long mNativeObjectRef = 0;
  private boolean IsMixMode(){
    return isMixMode;
  }

  //load Cpp library
  static{
    System.loadLibrary("recording");
  }

	//Native method declaration
  public native boolean createChannel(String appid, String channelKey, String name,  int uid,
                RecordingConfig config);
	public native boolean leaveChannel(long nativeObject);
  public native int setVideoMixingLayout(long nativeObject, VideoMixingLayout layout);

	//Called by C++
  //get native object
  public void nativeObjectRef(long nativeObject){
    mNativeObjectRef = nativeObject;
  }
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
  public void onUserOffline(long uid, int reason) {
    System.out.println("AgoraJavaRecording onUserOffline uid:"+uid+",offline reason:"+reason);
    EnumIndex ei = new EnumIndex();
    EnumIndex.USER_OFFLINE_REASON_TYPE offline = ei.new USER_OFFLINE_REASON_TYPE(reason);
    System.out.println("AgoraJavaRecording onUserOffline,code:"+offline.getValue());
    //m_peers.erase(std::remove(m_peers.begin(), m_peers.end(), uid), m_peers.end());
    SetVideoMixingLayout();
  }
  public void onUserJoined(long uid, String recordingDir){
    //recordingDir:recording file directory
    System.out.println("onUserJoined uid:"+uid+",recordingDir:"+recordingDir);
    storageDir = recordingDir;
    /*if(infos.storageDir) {
        m_storage_dir = std::string(infos.storageDir);
        m_logdir = m_storage_dir;
    }
    m_peers.push_back(uid);
    //When the user joined, we can re-layout the canvas
    setVideoMixLayout();
    */
    SetVideoMixingLayout();
  }
	public void audioFrameReceived(long uid, AudioFrame aFrame)
  { 
    System.out.println("java demo audioFrameReceived,uid:"+uid+",AUDIO_FRAME_TYPE:"+aFrame.type.getValue());
    byte[] buf = null;
    String path = storageDir + Long.toString(uid);
    if(aFrame.type.getValue() == 0) {//pcm
      path += ".pcm";
			buf = aFrame.pcm.pcmBuf_;
    }else if(aFrame.type.getValue() == 1){//aac
      path += ".aac";
			buf = aFrame.aac.aacBuf_;
    }
	  WriteBytesToFileClassic(buf, path);
  }
  public void videoFrameReceived(long uid, VideoFrame frame)
  {
    String path = storageDir + Long.toString(uid);
    byte[] buf = null;
    System.out.println("java demo videoFrameReceived,uid:"+uid+",type:"+frame.type.getValue());
    if(frame.type.getValue() == 0){//yuv
      path += ".yuv";
      buf = frame.yuv.buf_;
    }else if(frame.type.getValue() == 1) {//h264
      path +=  ".h264";
      buf = frame.h264.buf_;
    }else if(frame.type.getValue() == 2) { // jpg
      path += GetNowDate() + ".jpg";
      buf = frame.jpg.buf_;
    }
    WriteBytesToFileClassic(buf, path);
  }

  private int SetVideoMixingLayout(){
    System.out.println("java setVideoMixingLayout");
    EnumIndex ei = new EnumIndex();
    EnumIndex.VideoMixingLayout layout = ei.new VideoMixingLayout();

   	if(!IsMixMode()) return -1;
    layout.canvasHeight = m_mixRes.m_height;
    layout.backgroundColor = "#23b9dc";

/*
    layout.regionCount = (int)(m_peers.size());
/*
    if (!m_peers.empty()) {
        LOG_DIR(m_logdir.c_str(), INFO, "setVideoMixLayout: peers not empty");
        agora::linuxsdk::VideoMixingLayout::Region * regionList = new agora::linuxsdk::VideoMixingLayout::Region[m_peers.size()];

        regionList[0].uid = m_peers[0];
        regionList[0].x = 0.f;
        regionList[0].y = 0.f;
        regionList[0].width = 1.f;
        regionList[0].height = 1.f;
        regionList[0].zOrder = 0;
        regionList[0].alpha = 1.f;
        regionList[0].renderMode = 0;

        float canvasWidth = static_cast<float>(m_mixRes.m_width);
        float canvasHeight = static_cast<float>(m_mixRes.m_height);
        float viewWidth = 0.3f;
        float viewHEdge = 0.025f;
        float viewHeight = viewWidth * (canvasWidth / canvasHeight);
        float viewVEdge = viewHEdge * (canvasWidth / canvasHeight);
        for (size_t i=1; i<m_peers.size(); i++) {
            if (i >= 7)
                break;
            regionList[i].uid = m_peers[i];
            float xIndex = static_cast<float>(i % 3);
            float yIndex = static_cast<float>(i / 3);
            regionList[i].x = xIndex * (viewWidth + viewHEdge) + viewHEdge;
            regionList[i].y = 1 - (yIndex + 1) * (viewHeight + viewVEdge);
            regionList[i].width = viewWidth;
            regionList[i].height = viewHeight;
            regionList[i].zOrder = 0;
            regionList[i].alpha = static_cast<double>(i + 1);
            regionList[i].renderMode = 0;
        }
        layout.regions = regionList;
    }
    else {
        layout.regions = NULL;
    }
*/
    return setVideoMixingLayout(mNativeObjectRef, layout); 
  }
  
  public void stopCallBack() {
    System.out.println("java demo receive stop from JNI ");
    stopped = true;
  }
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
    int idleLimitSec=5*60;//300s

    boolean isAudioOnly=false;
    boolean isVideoOnly=false;
    boolean isMixingEnabled=false;
    boolean mixedVideoAudio=false;

    //MixModeSettings settings;
    AgoraJavaRecording ars = new AgoraJavaRecording();
    RecordingConfig config= new RecordingConfig();
    
    String appid = "0c0b4b61adf94de1befd7cdd78a50444";
    String channelKey = "";
    String name = "video";
    int uid = 0;
		CHANNEL_PROFILE_TYPE profile = CHANNEL_PROFILE_TYPE.CHANNEL_PROFILE_COMMUNICATION;
		REMOTE_VIDEO_STREAM_TYPE streamType = REMOTE_VIDEO_STREAM_TYPE.REMOTE_VIDEO_STREAM_HIGH;
    AUDIO_FORMAT_TYPE decodeAudio = AUDIO_FORMAT_TYPE.AUDIO_FORMAT_AAC_FRAME_TYPE;
    VIDEO_FORMAT_TYPE decodeVideo = VIDEO_FORMAT_TYPE.VIDEO_FORMAT_YUV_FRAME_TYPE;
    
    config.channelProfile = profile;
		config.streamType = streamType;
		config.idleLimitSec = 3;
		config.isMixingEnabled = false;
		config.appliteDir="./../native_layer/cppwrapper/bin";//contain Chines path????
    config.recordFileRootDir = ".";
    config.decodeAudio = decodeAudio;
    config.decodeVideo = decodeVideo;
		
    System.out.println(System.getProperty("java.library.path"));
    
    ars.createChannel(appid, channelKey,name,uid,config);
		System.out.println("############ars.createChannel end###########");
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
