//import headers.*;
//
class AgoraJavaRecording{

  class AgoraJavaEventHandler{

    void onWarning(int warn){
      System.out.println("java enent handler on warning!");
    }
  }



  static{
    //System.loadLibrary("/home/wangshiwei/ars/code_ars/javaDemo/bin/librecording.so");
    System.load("/home/wangshiwei/ars/code_ars/javaDemo/bin/librecording.so");
  }

  public native boolean createChannel(String appid, String channelKey, String name,  int uid,
                RecordingConfig config);
  public static void main(String[] args) 
  {
    MixModeSettings settings;
    AgoraJavaRecording ars = new AgoraJavaRecording();
    String appid = "0c0b4b61adf94de1befd7cdd78a50444";
    String channelKey = "";
    String name = "arsjava";
    int uid = 0;
    RecordingConfig config=new RecordingConfig();
		
    System.out.println("to create channel");
    ars.createChannel(appid, channelKey,name,uid,config);
    System.out.println("create channel end");
  }
}
