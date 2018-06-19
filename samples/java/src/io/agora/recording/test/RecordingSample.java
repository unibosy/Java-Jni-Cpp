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

public class RecordingSample implements RecordingEventHandler {
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
    private CHANNEL_PROFILE_TYPE profile_type;
    Vector<Long> m_peers = new Vector<Long>();
    private long mNativeHandle = 0;
    private RecordingSDK RecordingSDKInstance = null;
    
    private int layoutMode = 0;
    private long maxResolutionUid = -1;
    public static final int DEFAULT_LAYOUT = 0;
    public static final int BESTFIT_LAYOUT = 1;
    public static final int VERTICALPRESENTATION_LAYOUT = 2;

    public RecordingSample(RecordingSDK recording) {
        this.RecordingSDKInstance = recording;
        RecordingSDKInstance.registerOberserver(this);
    }

    public static void main(String[] args) {
        //should config -Djava.library.path to load library
        RecordingSDK RecordingSdk = new RecordingSDK();

        RecordingSample ars = new RecordingSample(RecordingSdk);
        ars.createChannel(args);
        ars.unRegister();
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
        System.out.println("RecordingSDK onError,error:" + error + ",stat code:" + stat_code);
    }

    public void onWarning(int warn) {
        System.out.println("RecordingSDK onWarning,warn:" + warn);
    }

    public void onJoinChannelSuccess(String channelId, long uid) {
        System.out.println("RecordingSDK joinChannel success, channelId:" + channelId +", uid:" + uid);
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
        int max_peers = profile_type == CHANNEL_PROFILE_TYPE.CHANNEL_PROFILE_COMMUNICATION ? 7:17;
        if(m_peers.size() > max_peers) {
            System.out.println("peers size is bigger than max m_peers:" + m_peers.size());
            return -1;
        }

        if (!IsMixMode())
            return -1;

        layout.canvasHeight = height;
        layout.canvasWidth = width;
        layout.backgroundColor = "#23b9dc";
        layout.regionCount = m_peers.size();

        if (!m_peers.isEmpty()) {
            System.out.println("java setVideoMixingLayout m_peers is not empty, start layout");
            Common.VideoMixingLayout.Region[] regionList = new Common.VideoMixingLayout.Region[m_peers.size()];
            System.out.println("mixing layout mode:"+layoutMode); 
            if(layoutMode == BESTFIT_LAYOUT) {
                adjustBestFitVideoLayout(regionList, layout);
            }else if(layoutMode == VERTICALPRESENTATION_LAYOUT) {
                adjustVerticalPresentationLayout(maxResolutionUid, regionList, layout);
            }else {
                adjustDefaultVideoLayout(regionList, layout);
            }

            layout.regions = regionList;

        } else {
            layout.regions = null;
        }
        return RecordingSDKInstance.setVideoMixingLayout(mNativeHandle, layout);
    }
    
    private void adjustVerticalPresentationLayout(long maxResolutionUid, Common.VideoMixingLayout.Region[] regionList, Common.VideoMixingLayout layout) {
       System.out.println("begin adjust vertical presentation layout,peers size:" + m_peers.size()+", maxResolutionUid:" + maxResolutionUid);
        if(m_peers.size() <= 5) {
            adjustVideo5Layout(maxResolutionUid, regionList, layout);
        }else if(m_peers.size() <= 7) {
            adjustVideo7Layout(maxResolutionUid, regionList, layout);
        }else if(m_peers.size() <= 9) {
            adjustVideo9Layout(maxResolutionUid, regionList, layout);
        }else {
            adjustVideo17Layout(maxResolutionUid, regionList, layout);
        }
    }

    private void adjustBestFitVideoLayout(Common.VideoMixingLayout.Region[] regionList, Common.VideoMixingLayout layout) {
        if(m_peers.size() == 1) {
            adjustBestFitLayout_Square(regionList,1, layout);
        }else if(m_peers.size() == 2) {
            adjustBestFitLayout_2(regionList, layout);
        }else if( 2 < m_peers.size() && m_peers.size() <=4) {
            adjustBestFitLayout_Square(regionList,2, layout);
        }else if(5<=m_peers.size() && m_peers.size() <=9) {
            adjustBestFitLayout_Square(regionList,3, layout);
        }else if(10<=m_peers.size() && m_peers.size() <=16) {
            adjustBestFitLayout_Square(regionList,4, layout);
        }else if(m_peers.size() ==17) {
            adjustBestFitLayout_17(regionList, layout);
        }else {
           System.out.println("adjustBestFitVideoLayout is more than 17 users");
        }
    }

    private void adjustBestFitLayout_2(Common.VideoMixingLayout.Region[] regionList, Common.VideoMixingLayout layout) {
        float canvasWidth = (float)width;
        float canvasHeight = (float)height;
        float viewWidth = 0.235f;
        float viewHEdge = 0.012f;
        float viewHeight = viewWidth * (canvasWidth/canvasHeight);
        float viewVEdge = viewHEdge * (canvasWidth/canvasHeight);
        int peersCount = m_peers.size();
        for (int i=0; i < peersCount; i++) {
            regionList[i] = layout.new Region();
            regionList[i].uid = m_peers.get(i);
            regionList[i].x = (((i+1)%2) == 0) ?0:0.5;
            regionList[i].y =  0.f;
            regionList[i].width = 0.5f;
            regionList[i].height = 1.f;
            regionList[i].zOrder = 0;
            regionList[i].alpha = i+1;
            regionList[i].renderMode = 0;
        }
    }

    private void adjustDefaultVideoLayout(Common.VideoMixingLayout.Region[] regionList, Common.VideoMixingLayout layout) {
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
    }

    private void setMaxResolutionUid(int number, long maxResolutionUid, Common.VideoMixingLayout.Region[] regionList, double weight_ratio) {
        regionList[number].uid = maxResolutionUid;
        regionList[number].x = 0.f;
        regionList[number].y = 0.f;
        regionList[number].width = 1.f * weight_ratio;
        regionList[number].height = 1.f;
        regionList[number].zOrder = number;
        regionList[number].alpha = 1.f;
        regionList[number].renderMode = 1;
    }
    private void changeToVideo7Layout(long maxResolutionUid, Common.VideoMixingLayout.Region[] regionList, Common.VideoMixingLayout layout) {
        System.out.println("changeToVideo7Layout");
        adjustVideo7Layout(maxResolutionUid, regionList, layout);
    }
    private void changeToVideo9Layout(long maxResolutionUid, Common.VideoMixingLayout.Region[] regionList, Common.VideoMixingLayout layout) {
        System.out.println("changeToVideo9Layout");
        adjustVideo9Layout(maxResolutionUid, regionList, layout);
    }
    private void changeToVideo17Layout(long maxResolutionUid, Common.VideoMixingLayout.Region[] regionList, Common.VideoMixingLayout layout) {
        System.out.println("changeToVideo17Layout");
        adjustVideo17Layout(maxResolutionUid, regionList, layout);
    }
    private void adjustBestFitLayout_Square(Common.VideoMixingLayout.Region[] regionList, int nSquare, Common.VideoMixingLayout layout) {
        float canvasWidth = (float)width;
        float canvasHeight = (float)height;
        float viewWidth = (float)(1.f * 1.0/nSquare);
        float viewHEdge = (float)(1.f * 1.0/nSquare);
        float viewHeight = viewWidth * (canvasWidth/canvasHeight);
        float viewVEdge = viewHEdge * (canvasWidth/canvasHeight);
        int peersCount = m_peers.size();
        for (int i=0; i < peersCount; i++) {
            regionList[i] = layout.new Region();
            float xIndex =(float)(i%nSquare);
            float yIndex = (float)(i/nSquare);
            regionList[i].uid = m_peers.get(i);
            regionList[i].x = 1.f * 1.0/nSquare * xIndex;
            regionList[i].y = 1.f * 1.0/nSquare * yIndex;
            regionList[i].width = viewWidth;
            regionList[i].height = viewHEdge;
            regionList[i].zOrder = 0;
            regionList[i].alpha = (double)(i+1);
            regionList[i].renderMode = 0;
        }
    }
    private void adjustBestFitLayout_17(Common.VideoMixingLayout.Region[] regionList, Common.VideoMixingLayout layout) {
        float canvasWidth = (float)width;
        float canvasHeight = (float)height;
        int n = 5;
        float viewWidth = (float)(1.f * 1.0/n);
        float viewHEdge = (float)(1.f * 1.0/n);
        float totalWidth = (float)(1.f - viewWidth);
        float viewHeight = viewWidth * (canvasWidth/canvasHeight);
        float viewVEdge = viewHEdge * (canvasWidth/canvasHeight);
        int peersCount = m_peers.size();
        for (int i = 0; i < peersCount; i++) {
            regionList[i] = layout.new Region();
            float xIndex = (float)(i%(n-1));
            float yIndex = (float)(i/(n-1));
            regionList[i].uid = m_peers.get(i);
            regionList[i].width = viewWidth;
            regionList[i].height = viewHEdge;
            regionList[i].zOrder = 0;
            regionList[i].alpha = i+1;
            regionList[i].renderMode = 0;
            if(i == 16) {
                regionList[i].x = (1-viewWidth)*(1.f/2) * 1.f;
                System.out.println( "special layout for 17 x is:"+regionList[i].x);
            }else {
                regionList[i].x = 0.5f * viewWidth +  viewWidth * xIndex;
            }
            regionList[i].y =  (1.0/n) * yIndex;
        }
    }
    private void adjustVideo5Layout(long maxResolutionUid, Common.VideoMixingLayout.Region[] regionList, Common.VideoMixingLayout layout) {
        boolean flag = false;

        float canvasWidth = (float)width;
        float canvasHeight = (float)height;

        float viewWidth = 0.235f;
        float viewHEdge = 0.012f;
        float viewHeight = viewWidth * (canvasWidth/canvasHeight);
        float viewVEdge = viewHEdge * (canvasWidth/canvasHeight);
        int number = 0;

        int i=0;
        for (; i<m_peers.size(); i++) {
            regionList[i] = layout.new Region();
            if(maxResolutionUid == m_peers.get(i)){
                System.out.println("adjustVideo5Layout equal with configured user uid:" + maxResolutionUid);
                flag = true;
                setMaxResolutionUid(number,  maxResolutionUid, regionList,0.8);
                number++;
                continue;
            }
            regionList[number].uid = m_peers.get(i);
            //float xIndex = ;
            float yIndex = flag?((float)(number-1 % 4)):((float)(number % 4));
            regionList[number].x = 1.f * 0.8;
            regionList[number].y = (0.25) * yIndex;
            regionList[number].width = 1.f*(1-0.8);
            regionList[number].height = 1.f * (0.25);
            regionList[number].zOrder = 0;
            regionList[number].alpha = (double)number;
            regionList[number].renderMode = 0;
            number++;
            if(i == 4 && !flag){
                changeToVideo7Layout(maxResolutionUid, regionList, layout);
            }
        }

    }
    private void adjustVideo7Layout(long maxResolutionUid, Common.VideoMixingLayout.Region[] regionList, Common.VideoMixingLayout layout) {
        boolean flag = false;
        float canvasWidth = (float)width;
        float canvasHeight = (float)height;

        float viewWidth = 0.235f;
        float viewHEdge = 0.012f;
        float viewHeight = viewWidth * (canvasWidth/canvasHeight);
        float viewVEdge = viewHEdge * (canvasWidth/canvasHeight);
        int number = 0;

        int i=0;
        for (; i<m_peers.size(); i++) {
            regionList[i] = layout.new Region();
            if(maxResolutionUid == m_peers.get(i)){
                System.out.println("adjustVideo7Layout equal with configured user uid:" + maxResolutionUid);
                flag = true;
                setMaxResolutionUid(number,  maxResolutionUid, regionList,6.f/7);
                number++;
                continue;
            }
            regionList[number].uid = m_peers.get(i);
            float yIndex = flag?((float)number-1 % 6):((float)(number % 6));
            regionList[number].x = 6.f/7;
            regionList[number].y = (1.f/6) * yIndex;
            regionList[number].width = (1.f/7);
            regionList[number].height = (1.f/6);
            regionList[number].zOrder = 0;
            regionList[number].alpha = (double)number;
            regionList[number].renderMode = 0;
            number++;
            if(i == 6 && !flag){
                changeToVideo9Layout(maxResolutionUid, regionList, layout);
            }
        }

    }
    private void adjustVideo9Layout(long maxResolutionUid, Common.VideoMixingLayout.Region[] regionList, Common.VideoMixingLayout layout) {
        boolean flag = false;

        float canvasWidth = (float)width;
        float canvasHeight = (float)height;

        float viewWidth = 0.235f;
        float viewHEdge = 0.012f;
        float viewHeight = viewWidth * (canvasWidth/canvasHeight);
        float viewVEdge = viewHEdge * (canvasWidth/canvasHeight);
        int number = 0;

        int i=0;
        for (; i<m_peers.size(); i++) {
            regionList[i] = layout.new Region();
            if(maxResolutionUid == m_peers.get(i)){
                System.out.println("adjustVideo9Layout equal with configured user uid:" + maxResolutionUid);
                flag = true;
                setMaxResolutionUid(number,  maxResolutionUid, regionList,9.f/5);
                number++;
                continue;
            }
            regionList[number].uid = m_peers.get(i);
            float yIndex = flag?((float)(number-1 % 8)):((float)(number % 8));
            regionList[number].x = 8.f/9;
            regionList[number].y = (1.f/8) * yIndex;
            regionList[number].width = 1.f/9 ;
            regionList[number].height = 1.f/8;
            regionList[number].zOrder = 0;
            regionList[number].alpha = (double)number;
            regionList[number].renderMode = 0;
            number++;
            if(i == 8 && !flag){
                changeToVideo17Layout(maxResolutionUid, regionList, layout);
            }
        }
    }

    private void adjustVideo17Layout(long maxResolutionUid, Common.VideoMixingLayout.Region[] regionList, Common.VideoMixingLayout layout) {
        boolean flag = false;
        float canvasWidth = (float)width;
        float canvasHeight = (float)height;

        float viewWidth = 0.235f;
        float viewHEdge = 0.012f;
        float viewHeight = viewWidth * (canvasWidth/canvasHeight);
        float viewVEdge = viewHEdge * (canvasWidth/canvasHeight);
        int number = 0;
        System.out.println("adjustVideo17Layoutenter m_peers size is:" + m_peers.size() + ", maxResolutionUid:" + maxResolutionUid);
        for (int i=0; i<m_peers.size(); i++) {
            regionList[i] = layout.new Region();
            if(maxResolutionUid == m_peers.get(i)){
                flag = true;
                setMaxResolutionUid(number,  maxResolutionUid, regionList,0.8);
                number++;
                continue;
            }
            if(!flag && i == 16) {
                System.out.println("Not the configured uid, and small regions is sixteen, so ignore this user:" + m_peers.get(i));
                break;
            }
            regionList[number].uid = m_peers.get(i);
            //float xIndex = 0.833f;
            float yIndex = flag?((float)((number-1) % 8)):((float)(number % 8));
            regionList[number].x = ((flag && i>8) || (!flag && i >=8)) ? (9.f/10):(8.f/10);
            regionList[number].y = (1.f/8) * yIndex;
            regionList[number].width =  1.f/10 ;
            regionList[number].height = 1.f/8;
            regionList[number].zOrder = 0;
            regionList[number].alpha = (double)number;
            regionList[number].renderMode = 0;
            number++;
        }
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
        Object LayoutMode = map.get("--layoutMode");
        Object MaxResolutionUid = map.get("--maxResolutionUid");



        if (Appid == null || Uid == null || Channel == null || AppliteDir == null) {
            // print usage
            String usage = "java RecordingSDK --appId STRING --uid UINTEGER32 --channel STRING --appliteDir STRING --channelKey STRING --channelProfile UINTEGER32 --isAudioOnly --isVideoOnly --isMixingEnabled --mixResolution STRING --mixedVideoAudio --decryptionMode STRING --secret STRING --idle INTEGER32 --recordFileRootDir STRING --lowUdpPort INTEGER32 --highUdpPort INTEGER32 --getAudioFrame UINTEGER32 --getVideoFrame UINTEGER32 --captureInterval INTEGER32 --cfgFilePath STRING --streamType UINTEGER32 --triggerMode INTEGER32 \r\n --appId     (App Id/must) \r\n --uid     (User Id default is 0/must)  \r\n --channel     (Channel Id/must) \r\n --appliteDir     (directory of app lite 'AgoraCoreService', Must pointer to 'Agora_Server_SDK_for_Linux_FULL/bin/' folder/must) \r\n --channelKey     (channelKey/option)\r\n --channelProfile     (channel_profile:(0:COMMUNICATION),(1:broadcast) default is 0/option)  \r\n --isAudioOnly     (Default 0:A/V, 1:AudioOnly (0:1)/option) \r\n --isVideoOnly     (Default 0:A/V, 1:VideoOnly (0:1)/option)\r\n --isMixingEnabled     (Mixing Enable? (0:1)/option)\r\n --mixResolution     (change default resolution for vdieo mix mode/option)                 \r\n --mixedVideoAudio     (mixVideoAudio:(0:seperated Audio,Video) (1:mixed Audio & Video), default is 0 /option)                 \r\n --decryptionMode     (decryption Mode, default is NULL/option)                 \r\n --secret     (input secret when enable decryptionMode/option)                 \r\n --idle     (Default 300s, should be above 3s/option)                 \r\n --recordFileRootDir     (recording file root dir/option)                 \r\n --lowUdpPort     (default is random value/option)                 \r\n --highUdpPort     (default is random value/option)                 \r\n --getAudioFrame     (default 0 (0:save as file, 1:aac frame, 2:pcm frame, 3:mixed pcm frame) (Can't combine with isMixingEnabled) /option)                 \r\n --getVideoFrame     (default 0 (0:save as file, 1:h.264, 2:yuv, 3:jpg buffer, 4:jpg file, 5:jpg file and video file) (Can't combine with isMixingEnabled) /option)              \r\n --captureInterval     (default 5 (Video snapshot interval (second)))                 \r\n --cfgFilePath     (config file path / option)                 \r\n --streamType     (remote video stream type(0:STREAM_HIGH,1:STREAM_LOW), default is 0/option)  \r\n --triggerMode     (triggerMode:(0: automatically mode, 1: manually mode) default is 0/option) \r\n --proxyServer     proxyServer:format ip:port, eg,\"127.0.0.1:1080\"/option \r\n --defaultVideoBg    (default user background image path/option) \r\n --defaultUserBg (default user background image path/option))  \r\n --audioProfile (audio profile(0: standard single channel, 1: high quality single channel, 2: high quality two channels) defualt is 0/option)   \r\n --logLevel (log level default INFO/option) \r\n --logModule (log modules. default ALL/option) \r\n --audioIndicationInterval(0: no indication, audio indication interval(ms) default is 0/option) \r\n --layoutMode    (mix video layout mode:(0: default layout, 1:bestFit Layout mode, 2:vertical presentation Layout mode, default is 0/option)(combine with isMixingEnabled)) \r\n --maxResolutionUid    (max resolution uid (uid with maxest resolution under vertical presentation Layout mode  ( default is -1 /option))";      
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
        if(LayoutMode != null) layoutMode = Integer.parseInt(String.valueOf(LayoutMode));
        if(MaxResolutionUid != null) maxResolutionUid = Long.parseLong(String.valueOf(MaxResolutionUid));
        System.out.println(" maxResolutionUid = "+maxResolutionUid);

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

}
