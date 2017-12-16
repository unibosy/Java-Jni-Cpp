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
import java.util.HashMap;
import java.util.Map;

class AgoraJavaRecording{
  //java run status flag
  private boolean stopped = false;
  private boolean isMixMode = false;
  private int width = 0;
  private int height = 0;
  private int fps = 0;
  private int kbps = 0;
  private String storageDir = "./";
  Vector<Long> m_peers = new Vector<Long>();
  //private MixModeSettings m_mixRes;
  private long mNativeObjectRef = 0;
  private boolean IsMixMode(){
    return isMixMode;
  }

  //load Cpp library
  static{
    System.loadLibrary("recording");
  }

	//Native method declaration
  public native boolean createChannel(String appId, String channelKey, String name,  int uid,
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
    m_peers.remove(uid);
    print(m_peers);
    SetVideoMixingLayout();
  }
  public void onUserJoined(long uid, String recordingDir){
    //recordingDir:recording file directory
    System.out.println("onUserJoined uid:"+uid+",recordingDir:"+recordingDir);
    storageDir = recordingDir;
    m_peers.add(uid);
    print(m_peers);
    //When the user joined, we can re-layout the canvas
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

  public int SetVideoMixingLayout(){
    System.out.println("java setVideoMixingLayout");
    EnumIndex ei = new EnumIndex();
    EnumIndex.VideoMixingLayout layout = ei.new VideoMixingLayout();

    //EnumIndex.Region regionList = ei.new Region(/*m_peers.size()*/);
   	
    if(!IsMixMode()) return -1;
    
    System.out.println("java setVideoMixingLayout -0");
    layout.canvasHeight = height;
    layout.backgroundColor = "#23b9dc";
    layout.regionCount = (int)(m_peers.size());
    System.out.println("java setVideoMixingLayout -1");

    if (!m_peers.isEmpty()) {
        System.out.println("java setVideoMixingLayout m_peers is not empty");
        EnumIndex.VideoMixingLayout.Region[] regionList = new EnumIndex.VideoMixingLayout.Region[m_peers.size()];
        regionList[0] = layout.new Region();
        regionList[0].uid = m_peers.get(0);
        regionList[0].x = 0.f;
        regionList[0].y = 0.f;
        regionList[0].width = 1.f;
        regionList[0].height = 1.f;
        regionList[0].zOrder = 0;
        regionList[0].alpha = 1.f;
        regionList[0].renderMode = 0;

        float canvasWidth = width;
        float canvasHeight = height;
        float viewWidth = 0.3f;
        float viewHEdge = 0.025f;
        float viewHeight = viewWidth * (canvasWidth / canvasHeight);
        float viewVEdge = viewHEdge * (canvasWidth / canvasHeight);
        for (int i=1; i<m_peers.size(); i++) {
            if (i >= 7)
                break;
            regionList[i] = layout.new Region();

            regionList[i].uid = m_peers.get(i);
            float xIndex = (i % 3);
            float yIndex = (i / 3);
            regionList[i].x = xIndex * (viewWidth + viewHEdge) + viewHEdge;
            regionList[i].y = 1 - (yIndex + 1) * (viewHeight + viewVEdge);
            regionList[i].width = viewWidth;
            regionList[i].height = viewHeight;
            regionList[i].zOrder = 0;
            regionList[i].alpha = (i + 1);
            regionList[i].renderMode = 0;
        }
        layout.regions = regionList;
    }
    else {
        layout.regions = null;
    }
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

  private void print(Vector vector){
    System.out.println("print vector size:"+vector.size());
    for(Long l : m_peers){  
      System.out.println("iter:"+l);
    }
  }
  public static void main(String[] args) 
  {
		int uid = 0;
  	String appId = "";
  	String channelKey = "";
  	String name = "";
  	int channelProfile = 0;

  	String decryptionMode = "";
  	String secret = "";
  	String mixResolution = "360,640,15,500";

  	int idleLimitSec=5*60;//300s

  	String applitePath = "./../native_layer/cppwrapper/bin";
  	String recordFileRootDir = ".";
  	String cfgFilePath = "";

  	int lowUdpPort = 0;//40000;
  	int highUdpPort = 0;//40004;

  	boolean isAudioOnly=false;
  	boolean isVideoOnly=false;
  	boolean isMixingEnabled=false;
  	boolean mixedVideoAudio=false;

  	int getAudioFrame = AUDIO_FORMAT_TYPE.AUDIO_FORMAT_DEFAULT_TYPE.ordinal();
  	int getVideoFrame = VIDEO_FORMAT_TYPE.VIDEO_FORMAT_DEFAULT_TYPE.ordinal();
  	int streamType = REMOTE_VIDEO_STREAM_TYPE.REMOTE_VIDEO_STREAM_HIGH.ordinal();
  	int captureInterval = 5;
    int triggerMode = 0;

  	int width = 0;
  	int height = 0;
  	int fps = 0;
  	int kbps = 0;
  	int count = 0;
    
    //paser command line parameters
		/*Args myArgs = new Args();
		CmdLineParser parser = new CmdLineParser(myArgs);
    //CommandLineParser parser = new BasicParser();
    Options options = new Options();
    parser.parseArgument(myArgs);
		parser.printUsage(System.out);*/
    System.out.println("args length:"+args.length);
    if(args.length % 2 !=0){
      System.out.println("set parameters error, key-value format!");
      return;
    }
    String key = "";
    String value = "";
    Map<String,String> map = new HashMap<String,String>();
    if(0 < args.length ){
      for(int i = 0; i<args.length-1; i++){
        key = args[i];
        value = args[i+1];
        map.put(key, value);
      }
    }
    //prefer to use CmdLineParser or annotation
    Object Appid = map.get("--appId");
    Object Uid = map.get("--uid");
    Object Channel = map.get("--channel");
    Object AppliteDir = map.get("--appliteDir");
    Object ChannelKey = map.get("----channelKey");
    Object ChannelProfile = map.get("--channelProfile");
    Object IsAudioOnly = map.get("--isAudioOnly");
    Object IsVideoOnly = map.get("--isVideoOnly");
    Object IsMixingEnabled = map.get("--isMixingEnabled");
    Object MixResolution = map.get("--mixResolution");
    Object MixedVideoAudio = map.get("--mixedVideoAudio");
    Object DecryptionMode = map.get("--decryptionMode");
    Object Secret = map.get("--secret");
    Object Idle = map.get("--idle");
    Object RecordFileRootDir = map.get("--recordFileRootDir");
    Object LowUdpPort = map.get("--lowUdpPort");
    Object HighUdpPort = map.get("--highUdpPort");
    Object GetAudioFrame = map.get("--getAudioFrame");
    Object GetVideoFrame = map.get("--getVideoFrame");
    Object CaptureInterval = map.get("--captureInterval");
    Object CfgFilePath = map.get("--cfgFilePath");
    Object StreamType = map.get("--streamType");
    Object TriggerMode = map.get("--triggerMode");

    if( Appid ==null || Uid==null ||Channel==null || AppliteDir == null){
      //print usage
      String usage = "./server_local --appId STRING --uid UINTEGER32 --channel STRING --appliteDir STRING --channelKey STRING --channelProfile UINTEGER32 --isAudioOnly --isVideoOnly --isMixingEnabled --mixResolution STRING --mixedVideoAudio --decryptionMode STRING --secret STRING --idle INTEGER32 --recordFileRootDir STRING --lowUdpPort INTEGER32 --highUdpPort INTEGER32 --getAudioFrame UINTEGER32 --getVideoFrame UINTEGER32 --captureInterval INTEGER32 --cfgFilePath STRING --streamType UINTEGER32 --triggerMode INTEGER32 \r\n --appId     (App Id/must) \r\n --uid     (User Id default is 0/must)  \r\n--channel     (Channel Id/must) \r\n --appliteDir     (directory of app lite 'AgoraCoreService', Must pointer to 'Agora_Server_SDK_for_Linux_FULL/bin/' folder/must) \r\n --channelKey     (channelKey/option)\r\n --channelProfile     (channel_profile:(0:COMMUNICATION),(1:broadcast) default is 0/option)  \r\n --isAudioOnly     (Default 0:A/V, 1:AudioOnly (0:1)/option) \r\n--isVideoOnly     (Default 0:A/V, 1:VideoOnly (0:1)/option)\r\n --isMixingEnabled     (Mixing Enable? (0:1)/option)\r\n --mixResolution     (change default resolution for vdieo mix mode/option)                 \r\n --mixedVideoAudio     (mixVideoAudio:(0:seperated Audio,Video) (1:mixed Audio & Video), default is 0 /option)                 \r\n --decryptionMode     (decryption Mode, default is NULL/option)                 \r\n --secret     (input secret when enable decryptionMode/option)                 \r\n --idle     (Default 300s, should be above 3s/option)                 \r\n --recordFileRootDir     (recording file root dir/option)                 \r\n --lowUdpPort     (default is random value/option)                 \r\n --highUdpPort     (default is random value/option)                 \r\n --getAudioFrame     (default 0 (0:save as file, 1:aac frame, 2:pcm frame) /option)                 \r\n --getVideoFrame     (default 0 (0:save as file, 1:h.264, 2:yuv, 3.jpg buffer, 4,jpg file) /option)              \r\n --captureInterval     (default 5 (Video snapshot interval (second)))                 \r\n --cfgFilePath     (config file path / option)                 \r\n --streamType     (remote video stream type(0:STREAM_HIGH,1:STREAM_LOW), default is 0/option)  \r\n --triggerMode     (triggerMode:(0: automatically mode, 1: manually mode) default is 0/option)";      
      
      System.out.println("Usage:"+usage);
      return;
    }
    appId = String.valueOf(Appid);
    uid = Integer.parseInt(String.valueOf(Uid));
    appId = String.valueOf(Appid);
    name = String.valueOf(Channel);
    applitePath = String.valueOf(AppliteDir);

    if(ChannelKey != null) channelKey = String.valueOf(ChannelKey);
    if(ChannelProfile != null) channelProfile = Integer.parseInt(String.valueOf(ChannelProfile));
    if(DecryptionMode != null) decryptionMode = String.valueOf(DecryptionMode);
    if(Secret != null) secret = String.valueOf(Secret);
    if(MixResolution != null) mixResolution = String.valueOf(MixResolution);
    if(Idle != null) idleLimitSec = Integer.parseInt(String.valueOf(Idle));
    if(RecordFileRootDir != null) recordFileRootDir = String.valueOf(RecordFileRootDir);
    if(CfgFilePath != null) cfgFilePath = String.valueOf(CfgFilePath);
    if(LowUdpPort != null) lowUdpPort = Integer.parseInt(String.valueOf(LowUdpPort));
    if(HighUdpPort != null) highUdpPort = Integer.parseInt(String.valueOf(HighUdpPort));
    if(IsAudioOnly != null &&(Integer.parseInt(String.valueOf(IsAudioOnly)) == 1)) isAudioOnly = true;
    if(IsVideoOnly != null &&(Integer.parseInt(String.valueOf(IsVideoOnly)) == 1)) isVideoOnly = true;
    if(IsMixingEnabled != null &&(Integer.parseInt(String.valueOf(IsMixingEnabled))==1)) isMixingEnabled = true;
    if(MixedVideoAudio != null &&(Integer.parseInt(String.valueOf(IsVideoOnly)) == 1)) mixedVideoAudio = true;
    if(GetAudioFrame != null) getAudioFrame = Integer.parseInt(String.valueOf(GetAudioFrame));
    if(GetVideoFrame != null) getVideoFrame = Integer.parseInt(String.valueOf(GetVideoFrame));
    if(StreamType != null) streamType = Integer.parseInt(String.valueOf(StreamType));
    if(CaptureInterval != null) captureInterval = Integer.parseInt(String.valueOf(CaptureInterval));
    if(TriggerMode != null) triggerMode = Integer.parseInt(String.valueOf(TriggerMode));
    
    AgoraJavaRecording ars = new AgoraJavaRecording();
    RecordingConfig config= new RecordingConfig();
    
    config.channelProfile = CHANNEL_PROFILE_TYPE.values()[channelProfile];
		config.idleLimitSec = idleLimitSec;
    config.isVideoOnly = isVideoOnly;
    config.isAudioOnly = isAudioOnly;
    config.isMixingEnabled = isMixingEnabled;
    config.mixResolution = mixResolution;
    config.mixedVideoAudio = mixedVideoAudio;
    config.appliteDir = applitePath;
    config.recordFileRootDir = recordFileRootDir;
    config.cfgFilePath = cfgFilePath;
    config.secret = secret;
    config.decryptionMode = decryptionMode;
    config.lowUdpPort = lowUdpPort;
    config.highUdpPort = highUdpPort;
    config.captureInterval = captureInterval;
    config.decodeAudio = AUDIO_FORMAT_TYPE.values()[getAudioFrame];
    config.decodeVideo = VIDEO_FORMAT_TYPE.values()[getVideoFrame];
		config.streamType = REMOTE_VIDEO_STREAM_TYPE.values()[streamType];
    config.triggerMode = triggerMode;
		
    System.out.println(map.get("java.library.path"));

    ars.isMixMode = isMixingEnabled; 
    if(isMixingEnabled && !isAudioOnly) {
      String[] sourceStrArray=mixResolution.split(",");
      if(sourceStrArray.length != 4) {
		    System.out.println("Illegal resolution:"+mixResolution);
        return;
      }
      ars.width = Integer.valueOf(sourceStrArray[0]).intValue();
      ars.height = Integer.valueOf(sourceStrArray[1]).intValue();
      ars.fps = Integer.valueOf(sourceStrArray[2]).intValue();
      ars.kbps = Integer.valueOf(sourceStrArray[3]).intValue();
    }
    System.out.println("############ars.createChannel begin:###########:isMixingEnabled"+isMixingEnabled);
    ars.createChannel(appId, channelKey,name,uid,config);
		System.out.println("############ars.createChannel end###########");
		/*while(!ars.stopped)
		{
			try{
				Thread.currentThread().sleep(10);//sleep 10 ms
				//System.out.println("in a loop");
			}
			catch(InterruptedException ie){
				System.out.println("exception throwed!");
			}
		}*/
    System.out.println("jni layer has been exited");
  }
}
