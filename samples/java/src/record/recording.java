import io.agora.recording.common.*;
import io.agora.recording.common.Common.*;
import java.lang.InterruptedException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.File; 
import java.io.FileNotFoundException;
import java.io.FileOutputStream; 
import java.io.BufferedOutputStream;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Vector;
import java.util.HashMap;
import java.util.Map;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;
import java.net.URI;
import java.net.URISyntaxException;

class AgoraJavaRecording{
  //java run status flag
  private boolean stopped = false;
  private boolean isMixMode = false;
  private int width = 0;
  private int height = 0;
  private int fps = 0;
  private int kbps = 0;
  private String storageDir = "./";
  private long aCount = 0;
  private long count = 0;
  private long size = 0;
  private CHANNEL_PROFILE_TYPE profile_type;
  Vector<Long> m_peers = new Vector<Long>();
  private long mNativeHandle = 0;
  private boolean IsMixMode(){
    return isMixMode;
  }

  //load Cpp library
  void loadLibrary() throws URISyntaxException{
      URI uri = getClass().getResource("./librecording.so").toURI() ; 
      String realPath = new File(uri).getAbsolutePath() ;
      System.load(realPath);
  }


  //Native method declaration
  private native boolean createChannel(String appId, String channelKey, String name,  int uid, RecordingConfig config);
  private native boolean leaveChannel(long nativeHandle);
  private native int setVideoMixingLayout(long nativeHandle, VideoMixingLayout layout);
  private native int startService(long nativeHandle);
  private native int stopService(long nativeHandle);
  private native RecordingEngineProperties getProperties(long nativeHandle);
  //Called by C++
  //get native handle
  private void nativeObjectRef(long nativeHandle){
    mNativeHandle = nativeHandle;
  }
  private void onLeaveChannel(int reason){
    System.out.println("AgoraJavaRecording onLeaveChannel,code:"+reason);
    stopped = true;
  }
  private void onError(int error, int stat_code) {
    System.out.println("AgoraJavaRecording onError,error:"+error+",stat code:"+stat_code);
    stopped = true;
  }
  private void onWarning(int warn) {
    //refer to "WARN_CODE_TYPE"
    System.out.println("AgoraJavaRecording onWarning,warn:"+warn);
  }
  private void onUserOffline(long uid, int reason) {
    System.out.println("AgoraJavaRecording onUserOffline uid:"+uid+",offline reason:"+reason);
    m_peers.remove(uid);
    PrintUsersInfo(m_peers);
    SetVideoMixingLayout();
  }
  private void onUserJoined(long uid, String recordingDir){
    //recordingDir:recording file directory
    System.out.println("onUserJoined uid:"+uid+",recordingDir:"+recordingDir);
    storageDir = recordingDir;
    m_peers.add(uid);
    PrintUsersInfo(m_peers);
    //When the user joined, we can re-layout the canvas
    SetVideoMixingLayout();
  }
  private void audioFrameReceived(long uid, int type, AudioFrame frame)
  {
    //System.out.println("java demo audioFrameReceived,uid:"+uid+",type:"+type);
    ByteBuffer buf = null;
    String path = storageDir + Long.toString(uid);
    if(type == 0) {//pcm
      path += ".pcm";
      buf = frame.pcm.pcmBuf;
    }else if(type == 1){//aac
      path += ".aac";
      buf = frame.aac.aacBuf;
    }
    WriteBytesToFileClassic(buf, path,false);
    buf = null;
    path = null;
    frame = null;
  }
  private void videoFrameReceived(long uid, int type, VideoFrame frame, int rotation)//rotation:0, 90, 180, 270
  {
    String path = storageDir + Long.toString(uid);
    ByteBuffer buf = null;
    //System.out.println("java demo videoFrameReceived,uid:"+uid+",VIDEO_FRAME_TYPE:"+frame.type.getValue());
    if(type == 0){//yuv
      path += ".yuv";
      buf = frame.yuv.buf;
      if(buf == null){
        System.out.println("java demo videoFrameReceived null");
      }
    }else if(type == 1) {//h264
      path +=  ".h264";
      buf = frame.h264.buf;
    }else if(type == 2) { // jpg
      path += "_"+GetNowDate() + ".jpg";
      buf = frame.jpg.buf;
    }
    WriteBytesToFileClassic(buf, path, true);
    buf = null;
    path = null;
    frame = null;
  }

  private int SetVideoMixingLayout(){
    Common ei = new Common();
    Common.VideoMixingLayout layout = ei.new VideoMixingLayout();
   	
    if(!IsMixMode()) return -1;
    
    layout.canvasHeight = height;
    layout.canvasWidth = width;
    layout.backgroundColor = "#23b9dc";
    layout.regionCount = (int)(m_peers.size());

    if (!m_peers.isEmpty()) {
        System.out.println("java setVideoMixingLayout m_peers is not empty, start layout");
        int max_peers = (profile_type == CHANNEL_PROFILE_TYPE.CHANNEL_PROFILE_COMMUNICATION ? 7:16);
        Common.VideoMixingLayout.Region[] regionList = new Common.VideoMixingLayout.Region[m_peers.size()];
        regionList[0] = layout.new Region();
        regionList[0].uid = m_peers.get(0);
        regionList[0].x = 0.f;
        regionList[0].y = 0.f;
        regionList[0].width = 1.f;
        regionList[0].height = 1.f;
        regionList[0].zOrder = 0;
        regionList[0].alpha = 1.f;
        regionList[0].renderMode = 0;
        float f_width = width;
        float f_height = height;
        float canvasWidth = f_width;
        float canvasHeight = f_height;
        float viewWidth = 0.235f;
        float viewHEdge = 0.012f;
        float viewHeight = viewWidth * (canvasWidth / canvasHeight);
        float viewVEdge = viewHEdge * (canvasWidth / canvasHeight);
        for (int i=1; i<m_peers.size(); i++) {
            if (i >= max_peers)
                break;
            regionList[i] = layout.new Region();

            regionList[i].uid = m_peers.get(i);
            float f_x = (i-1) % 4;
            float f_y = (i-1) / 4;
            float xIndex = f_x;
            float yIndex = f_y;
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
    return setVideoMixingLayout(mNativeHandle,layout); 
  }
  
  private void stopCallBack() {
    System.out.println("java demo receive stop from JNI ");
    stopped = true;
  }
  private void recordingPathCallBack(String path){
    storageDir =  path;
  }
  private void WriteBytesToFileClassic(ByteBuffer byteBuffer,String fileDest, boolean v) {
    if(byteBuffer == null){
      System.out.println("WriteBytesToFileClassic but byte buffer is null!");
      return;
    }
    /*if(v)
      count=count+1;
    if(count > 5000){
      return;
    }*/
    long startTime = System.nanoTime();
    byte[] data = new byte[byteBuffer.capacity()];
    ((ByteBuffer) byteBuffer.duplicate().clear()).get(data);
    try {
      FileOutputStream fos = new FileOutputStream(fileDest, true);
      BufferedOutputStream bos = new BufferedOutputStream(fos);
      bos.write(data);
      bos.flush();
      bos.close();
      fos = null;bos=null;data=null;
      /*long duration = System.nanoTime() - startTime;
      size = size+duration;
      if(v)
        System.out.println("BufferedWriter lost time:" +duration+",total size:"+ size +",count:"+count+",byteBuffer.size:"+byteBuffer.capacity());
        */
    } catch (IOException e) {
      e.printStackTrace();
    }
    /*
    long startTime = System.nanoTime();
    byte[] data = new byte[byteBuffer.capacity()];
    ((ByteBuffer) byteBuffer.duplicate().clear()).get(data);
    FileOutputStream fileOuputStream = null;
    try {
      fileOuputStream = new FileOutputStream(fileDest, true);
      fileOuputStream.write(data);
    } catch (IOException e) {
      e.printStackTrace();
    } finally {
      if (fileOuputStream != null) {
        try {
          fileOuputStream.close();
          long duration = System.nanoTime() - startTime;
          size = size+duration;
          if(v)
            System.out.println("BufferedWriter lost time:" +duration+",total size:"+ size +",count:"+count+",byteBuffer.size:"+byteBuffer.capacity());
        } catch (IOException e) {
          e.printStackTrace();
        }
      }
    }
    */
  }
  private String GetNowDate(){
    String temp_str="";   
    Date dt = new Date();   
    SimpleDateFormat sdf = new SimpleDateFormat("yyyyMMddHHmmss");  
    temp_str=sdf.format(dt);   
    return temp_str;   
    }

  private void PrintUsersInfo(Vector vector){
    System.out.println("user size:"+vector.size());
    for(Long l : m_peers){  
      System.out.println("user:"+l);
    }
  }
  
