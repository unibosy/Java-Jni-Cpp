import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.nio.ByteBuffer;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;

import io.agora.recording.common.Common;
import io.agora.recording.common.Common.AUDIO_FORMAT_TYPE;
import io.agora.recording.common.Common.AudioFrame;
import io.agora.recording.common.Common.CHANNEL_PROFILE_TYPE;
import io.agora.recording.common.Common.REMOTE_VIDEO_STREAM_TYPE;
import io.agora.recording.common.Common.VIDEO_FORMAT_TYPE;
import io.agora.recording.common.Common.VideoFrame;
import io.agora.recording.common.Common.VideoMixingLayout;
import io.agora.recording.common.RecordingConfig;
import io.agora.recording.common.RecordingEngineProperties;

import io.agora.recording.RecordingSDK;
import io.agora.recording.RecordingEventHandler;


import java.util.Scanner;

public class RecordingSample implements RecordingEventHandler{
	// java run status flag
	private boolean isMixMode = false;
	private int width = 0;
	private int height = 0;
	private int fps = 0;
	private int kbps = 0;
	private String storageDir = "./";
	private long aCount = 0;
	private long count = 0;
	private long size = 0;
    private boolean stopped = false;
	private CHANNEL_PROFILE_TYPE profile_type;
	Vector<Long> m_peers = new Vector<Long>();
	private long mNativeHandle = 0;
	private RecordingSDK RecordingSDKInstance = null;

    public RecordingSample(RecordingSDK recording) {
        this.RecordingSDKInstance = recording;
        RecordingSDKInstance.registerOberserver(this);
    }

    private static void Help(){
        System.out.println("Type \"start0\" to start thread 0 recording!(Only valid when \"triggerMode=1\")");
        System.out.println("Type \"stop0\" to stop thread 0 recording!(Only valid when \"triggerMode=1\")");
        System.out.println("Type \"getprop0\" to call thread 0 getProperties api!");
        System.out.println("Type \"quit0\" to leave thread 0 recording channel!");

        System.out.println("Type \"start1\" to start thread 1 recording!(Only valid when \"triggerMode=1\")");
        System.out.println("Type \"stop1\" to stop thread 1 recording!(Only valid when \"triggerMode=1\")");
        System.out.println("Type \"getprop1\" to call thread 1 getProperties api!");
        System.out.println("Type \"quit1\" to leave thread 1 recording channel!");

    }

    public static void main(String[] args) {
        // TODO Auto-generated method stub
        RecordingSDK RecordingSdk = new RecordingSDK();
        RecordingSDK RecordingSdk1 = new RecordingSDK();

        RecordingSample [] ars = new RecordingSample[2];
        ars[0] = new RecordingSample(RecordingSdk);
        ars[1] = new RecordingSample(RecordingSdk1);
      
        new Thread() {
            @Override
            public void run() {
                ars[0].createChannel(args);
            }
        }.start();

        new Thread() {
            @Override
            public void run() {
                ars[1].createChannel(args);
            }
        }.start();


       Scanner scn=new Scanner(System.in);
        while(true) {
            String input = scn.nextLine();
            if(ars[0].stopped && ars[1].stopped){
                System.out.println("Jni layer has been exited,now exiting Java...!");
                break;
            }
            if(input.equals("quit0")){
                ars[0].leaveChannel(ars[0].mNativeHandle);
            }else if(input.equals("start0")){
                ars[0].startService(ars[0].mNativeHandle);
            }else if(input.equals("stop0")){
                ars[0].stopService(ars[0].mNativeHandle);
            }else if(input.equals("getprop0")){
                System.out.println(ars[0].getProperties(ars[0].mNativeHandle).GetStorageDir());
            }else if(input.equals("quit1")){
                ars[1].leaveChannel(ars[1].mNativeHandle);
                break;
            }else if(input.equals("start1")){
                ars[1].startService(ars[1].mNativeHandle);
            }else if(input.equals("stop1")){
                ars[1].stopService(ars[1].mNativeHandle);
            }else if(input.equals("getprop1")){
                System.out.println(ars[1].getProperties(ars[1].mNativeHandle).GetStorageDir());
            }else if(input.equals("help")){
                Help();      
            }else{
                System.out.println("Undefined command:"+input+"  Try \"help\"");
            }
            try{
                Thread.currentThread().sleep(1000);//sleep 1s
            }catch(InterruptedException ie){
                System.out.println("exception throwed!");
            }
        }
        System.out.println("exit java process...");
        
        ars[0].unRegister();
        ars[1].unRegister();
    }

	public void unRegister(){
		RecordingSDKInstance.unRegisterOberserver(this);	
	}

	private boolean IsMixMode() {
		return isMixMode;
	}

	public void nativeObjectRef(long nativeHandle) {
		mNativeHandle = nativeHandle;
	}

	public void onLeaveChannel(int reason) {
		System.out.println("RecordingSDK onLeaveChannel,code:" + reason);
	}

	public void onError(int error, int stat_code) {
        stopped =true;
		System.out.println("RecordingSDK onError,error:" + error + ",stat code:" + stat_code);
	}

	public void onWarning(int warn) {
		System.out.println("RecordingSDK onWarning,warn:" + warn);
	}

	public void onUserOffline(long uid, int reason) {
		System.out.println("RecordingSDK onUserOffline uid:" + uid + ",offline reason:" + reason);
		m_peers.remove(uid);
		PrintUsersInfo(m_peers);
		SetVideoMixingLayout();
	}

	public void onUserJoined(long uid, String recordingDir) {
		System.out.println("onUserJoined uid:" + uid + ",recordingDir:" + recordingDir);
		storageDir = recordingDir;
		m_peers.add(uid);
		PrintUsersInfo(m_peers);
		// When the user joined, we can re-layout the canvas
		SetVideoMixingLayout();
	}

    public void onActiveSpeaker(long uid) {
        System.out.println("User:"+uid+"is speaking");
    }

	public void audioFrameReceived(long uid, int type, AudioFrame frame) {
		// System.out.println("java demo
		// audioFrameReceived,uid:"+uid+",type:"+type);
		ByteBuffer buf = null;
		String path = storageDir + Long.toString(uid);
		if (type == 0) {// pcm
			path += ".pcm";
			buf = frame.pcm.pcmBuf;
		} else if (type == 1) {// aac
			path += ".aac";
			buf = frame.aac.aacBuf;
		}
		WriteBytesToFileClassic(buf, path);
		buf = null;
		path = null;
		frame = null;
	}

	public void videoFrameReceived(long uid, int type, VideoFrame frame, int rotation)// rotation:0,90,180,270
	{
		String path = storageDir + Long.toString(uid);
		ByteBuffer buf = null;
		// System.out.println("java demovideoFrameReceived,uid:"+uid+",type:"+type);
		if (type == 0) {// yuv
			path += ".yuv";
            buf = frame.yuv.buf;
			if (buf == null) {
				System.out.println("java demo videoFrameReceived null");
			}
		} else if (type == 1) {// h264
			path += ".h264";
			buf = frame.h264.buf;
		} else if (type == 2) { // jpg
			path += "_" + GetNowDate() + ".jpg";
			buf = frame.jpg.buf;
		}
		WriteBytesToFileClassic(buf, path);
		buf = null;
		path = null;
		frame = null;
	}

	/*
	 * Brief: Callback when JNI layer exited
	 */
	public void stopCallBack() {
        stopped =true;
		System.out.println("java demo receive stop from JNI ");
	}

	/*
	 * Brief: Callback when call createChannel successfully
	 * 
	 * @param path recording file directory
	 */
	public void recordingPathCallBack(String path) {
		storageDir = path;
	}

	private int SetVideoMixingLayout() {
		Common ei = new Common();
		Common.VideoMixingLayout layout = ei.new VideoMixingLayout();

		if (!IsMixMode())
			return -1;

		layout.canvasHeight = height;
		layout.canvasWidth = width;
		layout.backgroundColor = "#23b9dc";
		layout.regionCount = (int) (m_peers.size());

		if (!m_peers.isEmpty()) {
			System.out.println("java setVideoMixingLayout m_peers is not empty, start layout");
			int max_peers = (profile_type == CHANNEL_PROFILE_TYPE.CHANNEL_PROFILE_COMMUNICATION ? 7 : 17);
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
			for (int i = 1; i < m_peers.size(); i++) {
				if (i >= max_peers)
					break;
				regionList[i] = layout.new Region();

				regionList[i].uid = m_peers.get(i);
				float f_x = (i - 1) % 4;
				float f_y = (i - 1) / 4;
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
		} else {
			layout.regions = null;
		}
		return RecordingSDKInstance.setVideoMixingLayout(mNativeHandle, layout);
	}

	private void WriteBytesToFileClassic(ByteBuffer byteBuffer, String fileDest) {
		if (byteBuffer == null) {
			System.out.println("WriteBytesToFileClassic but byte buffer is null!");
			return;
		}
		byte[] data = new byte[byteBuffer.capacity()];
		((ByteBuffer) byteBuffer.duplicate().clear()).get(data);
		try {
			FileOutputStream fos = new FileOutputStream(fileDest, true);
			BufferedOutputStream bos = new BufferedOutputStream(fos);
			bos.write(data);
			bos.flush();
			bos.close();
			fos = null;
			bos = null;
			data = null;
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	private String GetNowDate() {
		String temp_str = "";
		Date dt = new Date();
		SimpleDateFormat sdf = new SimpleDateFormat("yyyyMMddHHmmss");
		temp_str = sdf.format(dt);
		return temp_str;
	}

	private void PrintUsersInfo(Vector vector) {
		System.out.println("user size:" + vector.size());
		for (Long l : m_peers) {
			System.out.println("user:" + l);
		}
	}

	public void createChannel(String[] args) {
		int uid = 0;
		String appId = "";
		String channelKey = "";
		String name = "";
		int channelProfile = 0;

		String decryptionMode = "";
		String secret = "";
		String mixResolution = "360,640,15,500";

		int idleLimitSec = 5 * 60;// 300s

		String applitePath = "";
        String recordFileRootDir = "";
        String cfgFilePath = "";
        String proxyServer = "";
        String defaultVideoBgPath = "";
        String defaultUserBgPath = "";


		int lowUdpPort = 0;// 40000;
		int highUdpPort = 0;// 40004;

		boolean isAudioOnly = false;
		boolean isVideoOnly = false;
		boolean isMixingEnabled = false;
		boolean mixedVideoAudio = false;

		int getAudioFrame = AUDIO_FORMAT_TYPE.AUDIO_FORMAT_DEFAULT_TYPE.ordinal();
		int getVideoFrame = VIDEO_FORMAT_TYPE.VIDEO_FORMAT_DEFAULT_TYPE.ordinal();
		int streamType = REMOTE_VIDEO_STREAM_TYPE.REMOTE_VIDEO_STREAM_HIGH.ordinal();
        int captureInterval = 5;
        int triggerMode = 0;

        int audioIndicationInterval = 0;
        int logLevel = 6;
        int logModule = 0xFF;

        int width = 0;
        int height = 0;
		int fps = 0;
        int kbps = 0;
        int count = 0;
        int audioProfile = 0;

		// paser command line parameters
		if (args.length % 2 != 0) {
			System.out.println("command line parameters error, should be '--key value' format!");
			return;
		}
		String key = "";
		String value = "";
		Map<String, String> map = new HashMap<String, String>();
		if (0 < args.length) {
			for (int i = 0; i < args.length - 1; i++) {
				key = args[i];
				value = args[i + 1];
				map.put(key, value);
			}
		}
		// prefer to use CmdLineParser or annotation
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

        Object ProxyServer = map.get("--proxyServer");
        Object AudioProfile = map.get("--audioProfile");
        Object DefaultVideoBg = map.get("--defaultVideoBg");
        Object DefaultUserBg = map.get("--defaultUserBg");
        Object LogLevel = map.get("--logLevel");
        Object LogModule = map.get("--logModule");
        Object AudioIndicationInterval = map.get("--audioIndicationInterval");


		if (Appid == null || Uid == null || Channel == null || AppliteDir == null) {
			// print usage
            String usage = "java RecordingSDK --appId STRING --uid UINTEGER32 --channel STRING --appliteDir STRING --channelKey STRING --channelProfile UINTEGER32 --isAudioOnly --isVideoOnly --isMixingEnabled --mixResolution STRING --mixedVideoAudio --decryptionMode STRING --secret STRING --idle INTEGER32 --recordFileRootDir STRING --lowUdpPort INTEGER32 --highUdpPort INTEGER32 --getAudioFrame UINTEGER32 --getVideoFrame UINTEGER32 --captureInterval INTEGER32 --cfgFilePath STRING --streamType UINTEGER32 --triggerMode INTEGER32 \r\n --appId     (App Id/must) \r\n --uid     (User Id default is 0/must)  \r\n --channel     (Channel Id/must) \r\n --appliteDir     (directory of app lite 'AgoraCoreService', Must pointer to 'Agora_Server_SDK_for_Linux_FULL/bin/' folder/must) \r\n --channelKey     (channelKey/option)\r\n --channelProfile     (channel_profile:(0:COMMUNICATION),(1:broadcast) default is 0/option)  \r\n --isAudioOnly     (Default 0:A/V, 1:AudioOnly (0:1)/option) \r\n --isVideoOnly     (Default 0:A/V, 1:VideoOnly (0:1)/option)\r\n --isMixingEnabled     (Mixing Enable? (0:1)/option)\r\n --mixResolution     (change default resolution for vdieo mix mode/option)                 \r\n --mixedVideoAudio     (mixVideoAudio:(0:seperated Audio,Video) (1:mixed Audio & Video), default is 0 /option)                 \r\n --decryptionMode     (decryption Mode, default is NULL/option)                 \r\n --secret     (input secret when enable decryptionMode/option)                 \r\n --idle     (Default 300s, should be above 3s/option)                 \r\n --recordFileRootDir     (recording file root dir/option)                 \r\n --lowUdpPort     (default is random value/option)                 \r\n --highUdpPort     (default is random value/option)                 \r\n --getAudioFrame     (default 0 (0:save as file, 1:aac frame, 2:pcm frame, 3:mixed pcm frame) (Can't combine with isMixingEnabled) /option)                 \r\n --getVideoFrame     (default 0 (0:save as file, 1:h.264, 2:yuv, 3:jpg buffer, 4:jpg file, 5:jpg file and video file) (Can't combine with isMixingEnabled) /option)              \r\n --captureInterval     (default 5 (Video snapshot interval (second)))                 \r\n --cfgFilePath     (config file path / option)                 \r\n --streamType     (remote video stream type(0:STREAM_HIGH,1:STREAM_LOW), default is 0/option)  \r\n --triggerMode     (triggerMode:(0: automatically mode, 1: manually mode) default is 0/option) \r\n --proxyServer     proxyServer:format ip:port, eg,\"127.0.0.1:1080\"/option \r\n --defaultVideoBg    (default user background image path/option) \r\n --defaultUserBg (default user background image path/option))  \r\n --audioProfile (audio profile(0: standard single channel, 1: high quality single channel, 2: high quality two channels) defualt is 0/option)   \r\n --logLevel (log level default INFO/option) \r\n --logModule (log modules. default ALL/option) \r\n --audioIndicationInterval(0: no indication, audio indication interval(ms))efault is 0/option)";      
            System.out.println("Usage:" + usage);
            return;
        }
        appId = String.valueOf(Appid);
		uid = Integer.parseInt(String.valueOf(Uid));
		appId = String.valueOf(Appid);
		name = String.valueOf(Channel);
		applitePath = String.valueOf(AppliteDir);

		if (ChannelKey != null)
			channelKey = String.valueOf(ChannelKey);
		if (ChannelProfile != null)
			channelProfile = Integer.parseInt(String.valueOf(ChannelProfile));
		if (DecryptionMode != null)
			decryptionMode = String.valueOf(DecryptionMode);
		if (Secret != null)
			secret = String.valueOf(Secret);
		if (MixResolution != null)
			mixResolution = String.valueOf(MixResolution);
		if (Idle != null)
			idleLimitSec = Integer.parseInt(String.valueOf(Idle));
		if (RecordFileRootDir != null)
			recordFileRootDir = String.valueOf(RecordFileRootDir);
		if (CfgFilePath != null)
			cfgFilePath = String.valueOf(CfgFilePath);
		if (LowUdpPort != null)
			lowUdpPort = Integer.parseInt(String.valueOf(LowUdpPort));
		if (HighUdpPort != null)
			highUdpPort = Integer.parseInt(String.valueOf(HighUdpPort));
		if (IsAudioOnly != null && (Integer.parseInt(String.valueOf(IsAudioOnly)) == 1))
			isAudioOnly = true;
		if (IsVideoOnly != null && (Integer.parseInt(String.valueOf(IsVideoOnly)) == 1))
			isVideoOnly = true;
		if (IsMixingEnabled != null && (Integer.parseInt(String.valueOf(IsMixingEnabled)) == 1))
			isMixingEnabled = true;
		if (MixedVideoAudio != null && (Integer.parseInt(String.valueOf(MixedVideoAudio)) == 1))
			mixedVideoAudio = true;
		if (GetAudioFrame != null)
			getAudioFrame = Integer.parseInt(String.valueOf(GetAudioFrame));
		if (GetVideoFrame != null)
			getVideoFrame = Integer.parseInt(String.valueOf(GetVideoFrame));
		if (StreamType != null)
            streamType = Integer.parseInt(String.valueOf(StreamType));
        if (CaptureInterval != null)
            captureInterval = Integer.parseInt(String.valueOf(CaptureInterval));
        if(AudioIndicationInterval != null) audioIndicationInterval = Integer.parseInt(String.valueOf(AudioIndicationInterval));
        if(TriggerMode != null) triggerMode = Integer.parseInt(String.valueOf(TriggerMode));
        if(ProxyServer != null) proxyServer = String.valueOf(ProxyServer);
        if(AudioProfile != null) audioProfile = Integer.parseInt(String.valueOf(AudioProfile));
        if(DefaultVideoBg != null) defaultVideoBgPath = String.valueOf(DefaultVideoBg);
        if(DefaultUserBg != null) defaultUserBgPath = String.valueOf(DefaultUserBg);
        if(LogLevel != null) logLevel = Integer.parseInt(String.valueOf(LogLevel));
        if(LogModule != null) logModule = Integer.parseInt(String.valueOf(LogModule));

        if(audioProfile > 2) audioProfile = 2;
        if(audioProfile < 0) audioProfile = 0;

        RecordingConfig config = new RecordingConfig();
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
        config.audioIndicationInterval = audioIndicationInterval;
        config.decodeAudio = AUDIO_FORMAT_TYPE.values()[getAudioFrame];
        config.decodeVideo = VIDEO_FORMAT_TYPE.values()[getVideoFrame];
        config.streamType = REMOTE_VIDEO_STREAM_TYPE.values()[streamType];
        config.triggerMode = triggerMode;
        config.proxyServer = proxyServer;
        config.audioProfile = audioProfile;
        config.defaultVideoBgPath = defaultVideoBgPath;
        config.defaultUserBgPath = defaultUserBgPath;

		/*
		 * change log_config Facility per your specific purpose like
		 * agora::base::LOCAL5_LOG_FCLT Default:USER_LOG_FCLT.
		 *
		 * ars.setFacility(LOCAL5_LOG_FCLT);
		 */

        System.out.println(System.getProperty("java.library.path"));

        if(logLevel < 2) logLevel = 2;
        if(logLevel > 7) logLevel = 7;

		this.isMixMode = isMixingEnabled;
		this.profile_type = CHANNEL_PROFILE_TYPE.values()[channelProfile];
		if (isMixingEnabled && !isAudioOnly) {
			String[] sourceStrArray = mixResolution.split(",");
			if (sourceStrArray.length != 4) {
				System.out.println("Illegal resolution:" + mixResolution);
				return;
			}
			this.width = Integer.valueOf(sourceStrArray[0]).intValue();
			this.height = Integer.valueOf(sourceStrArray[1]).intValue();
			this.fps = Integer.valueOf(sourceStrArray[2]).intValue();
			this.kbps = Integer.valueOf(sourceStrArray[3]).intValue();
		}
		// run jni event loop , or start a new thread to do it
		RecordingSDKInstance.createChannel(appId, channelKey, name, uid, config, logLevel, logModule);
		System.out.println("jni layer has been exited...");
	}

    public boolean leaveChannel(long nativeHandle) {
        return RecordingSDKInstance.leaveChannel(nativeHandle);
    }

    public int startService(long nativeHandle) {
        return RecordingSDKInstance.startService(nativeHandle);
    }

    public int stopService(long nativeHandle) {
        return RecordingSDKInstance.stopService(nativeHandle);
    }

    public RecordingEngineProperties getProperties(long nativeHandle) {
        return RecordingSDKInstance.getProperties(nativeHandle);
    }

    public void onJoinChannelSuccess(String channelId, long uid) {
		System.out.println("RecordingSDK onJoinChannelSuccess:" + channelId+", uid:" + uid);
    }


  
}
