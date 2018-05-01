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

    String applitePath = "";
    String recordFileRootDir = "";
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
    if(args.length % 2 !=0){
        System.out.println("command line parameters error, should be '--key value' format!");
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
    Object ChannelKey = map.get("--channelKey");
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
        String usage = "java AgoraJavaRecording --appId STRING --uid UINTEGER32 --channel STRING --appliteDir STRING --channelKey STRING --channelProfile UINTEGER32 --isAudioOnly --isVideoOnly --isMixingEnabled --mixResolution STRING --mixedVideoAudio --decryptionMode STRING --secret STRING --idle INTEGER32 --recordFileRootDir STRING --lowUdpPort INTEGER32 --highUdpPort INTEGER32 --getAudioFrame UINTEGER32 --getVideoFrame UINTEGER32 --captureInterval INTEGER32 --cfgFilePath STRING --streamType UINTEGER32 --triggerMode INTEGER32 \r\n --appId     (App Id/must) \r\n --uid     (User Id default is 0/must)  \r\n --channel     (Channel Id/must) \r\n --appliteDir     (directory of app lite 'AgoraCoreService', Must pointer to 'Agora_Server_SDK_for_Linux_FULL/bin/' folder/must) \r\n --channelKey     (channelKey/option)\r\n --channelProfile     (channel_profile:(0:COMMUNICATION),(1:broadcast) default is 0/option)  \r\n --isAudioOnly     (Default 0:A/V, 1:AudioOnly (0:1)/option) \r\n --isVideoOnly     (Default 0:A/V, 1:VideoOnly (0:1)/option)\r\n --isMixingEnabled     (Mixing Enable? (0:1)/option)\r\n --mixResolution     (change default resolution for vdieo mix mode/option)                 \r\n --mixedVideoAudio     (mixVideoAudio:(0:seperated Audio,Video) (1:mixed Audio & Video), default is 0 /option)                 \r\n --decryptionMode     (decryption Mode, default is NULL/option)                 \r\n --secret     (input secret when enable decryptionMode/option)                 \r\n --idle     (Default 300s, should be above 3s/option)                 \r\n --recordFileRootDir     (recording file root dir/option)                 \r\n --lowUdpPort     (default is random value/option)                 \r\n --highUdpPort     (default is random value/option)                 \r\n --getAudioFrame     (default 0 (0:save as file, 1:aac frame, 2:pcm frame, 3:mixed pcm frame) (Can't combine with isMixingEnabled) /option)                 \r\n --getVideoFrame     (default 0 (0:save as file, 1:h.264, 2:yuv, 3:jpg buffer, 4:jpg file, 5:jpg file and video file) (Can't combine with isMixingEnabled) /option)              \r\n --captureInterval     (default 5 (Video snapshot interval (second)))                 \r\n --cfgFilePath     (config file path / option)                 \r\n --streamType     (remote video stream type(0:STREAM_HIGH,1:STREAM_LOW), default is 0/option)  \r\n --triggerMode     (triggerMode:(0: automatically mode, 1: manually mode) default is 0/option)";      

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
    if(MixedVideoAudio != null &&(Integer.parseInt(String.valueOf(MixedVideoAudio)) == 1)) mixedVideoAudio = true;
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
    /*
     * change log_config Facility per your specific purpose like agora::base::LOCAL5_LOG_FCLT
     * Default:USER_LOG_FCLT.
     *
     * ars.setFacility(LOCAL5_LOG_FCLT);
     */
    try {
        ars.loadLibrary();
    } catch (URISyntaxException e) {
        // TODO Auto-generated catch block
        e.printStackTrace();
    }
    System.out.println(System.getProperty("java.library.path"));

    ars.isMixMode = isMixingEnabled; 
    ars.profile_type = CHANNEL_PROFILE_TYPE.values()[channelProfile];
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
    //run jni event loop , or start a new thread to do it
    ars.createChannel(appId, channelKey,name,uid,config);
    System.out.println("jni layer has been exited...");
    System.exit(0);
}
