package headers;

public class EnumIndex{
  public enum ERROR_CODE_TYPE {
    ERR_OK(0),
    //1~1000
    ERR_FAILED(1),
    ERR_INVALID_ARGUMENT(2),
    ERR_INTERNAL_FAILED(3);
    private ERROR_CODE_TYPE(int value){}
  }

  public class STAT_CODE_TYPE {
    public final int STAT_ERR_FROM_ENGINE = 1;
    public final int STAT_ERR_ARS_JOIN_CHANNEL = 2;
    public final int STAT_ERR_CREATE_PROCESS = 3;
    public final int STAT_ERR_MIXED_INVALID_VIDEO_PARAM= 4;
    public final int STAT_POLL_ERR= 0x8;
    public final int STAT_POLL_HANG_UP= 0x10;
    public final int STAT_POLL_NVAL = 0x20;
    private int value;
    public STAT_CODE_TYPE(int value) {
      this.value = value;
    }
    public int getValue() {
      return value;
    }
  }
  public class LEAVE_PATH_CODE {
    public final int LEAVE_CODE_INIT = 0; 
    public final int LEAVE_CODE_SIG = 1<<1;
    public final int LEAVE_CODE_NO_USERS= 1<<2 ;
    public final int LEAVE_CODE_TIMER_CATCH = 1<<3;
    public final int LEAVE_CODE_CLIENT_LEAVE = 1 << 4;
    private int value;
    public LEAVE_PATH_CODE(int code) {
      this.value = code;
    }
    public int getValue() {
      return value;
    }
    /*public void setValue(int code) {
      value = code;
    }*/
  }


  
    public enum WARN_CODE_TYPE {
    WARN_NO_AVAILABLE_CHANNEL(103),
    WARN_LOOKUP_CHANNEL_TIMEOUT(104),
    WARN_LOOKUP_CHANNEL_REJECTED(105),
    WARN_OPEN_CHANNEL_TIMEOUT(106),
    WARN_OPEN_CHANNEL_REJECTED(107);
        private WARN_CODE_TYPE(int value){}
    }

    public enum CHANNEL_PROFILE_TYPE {
        
        CHANNEL_PROFILE_COMMUNICATION(0),
        CHANNEL_PROFILE_LIVE_BROADCASTING(1);
        private int value;
        private CHANNEL_PROFILE_TYPE(int value) {
            this.value = value;
        }
        public int getValue() {
            return value;
        }
    }
    public class USER_OFFLINE_REASON_TYPE  {
      public static final int USER_OFFLINE_QUIT = 0;
      public static final int USER_OFFLINE_DROPPED = 1;
      public static final int USER_OFFLINE_BECOME_AUDIENCE = 2;
      private int value;
      public USER_OFFLINE_REASON_TYPE(int code) {
        this.value = code;
      }
       public int getValue() {
        return value;
      }
      /*public void setValue(int code) {
        value = code;
      }*/
    }
    public enum REMOTE_VIDEO_STREAM_TYPE {
        REMOTE_VIDEO_STREAM_HIGH(0),
        REMOTE_VIDEO_STREAM_LOW(1);
        private int value;
        private REMOTE_VIDEO_STREAM_TYPE(int value) {
            this.value = value;
        }
        public int getValue() {
            return value;
        }
    }
    public enum VIDEO_FORMAT_TYPE {
    VIDEO_FORMAT_DEFAULT_TYPE(0),
    VIDEO_FORMAT_H264_FRAME_TYPE(1),
    VIDEO_FORMAT_YUV_FRAME_TYPE(2),
    VIDEO_FORMAT_JPG_FRAME_TYPE(3),
    VIDEO_FORMAT_JPG_FILE_TYPE(4);
        private VIDEO_FORMAT_TYPE(int value){}
    }

    
    public enum AUDIO_FORMAT_TYPE {
    AUDIO_FORMAT_DEFAULT_TYPE(0),
    AUDIO_FORMAT_AAC_FRAME_TYPE(1),
    AUDIO_FORMAT_PCM_FRAME_TYPE(2);
        private AUDIO_FORMAT_TYPE(int value){}
    }

    public enum AUDIO_FRAME_TYPE {
    AUDIO_FRAME_RAW_PCM(0),
    AUDIO_FRAME_AAC(1);
        private AUDIO_FRAME_TYPE(int value){}
    }

    public enum MEMORY_TYPE {
    STACK_MEM_TYPE(0),
    HEAP_MEM_TYPE(1);
        private MEMORY_TYPE(int value){}
    }

    public enum VIDEO_FRAME_TYPE {
    VIDEO_FRAME_RAW_YUV(0),
    VIDEO_FRAME_H264(1),
    VIDEO_FRAME_JPG(2);
        private VIDEO_FRAME_TYPE(int value){}
    }

    public enum SERVICE_MODE {
    	RECORDING_MODE(0),//down stream
    	SERVER_MODE(1),//up-down stream
    	IOT_MODE(2);//up-down stream
        private SERVICE_MODE(int value){}
    }
    public class AudioFrameOfPcm {
      public AUDIO_FRAME_TYPE type;
      public AudioPcmFrame apf;
      public MEMORY_TYPE mType;
    }
		public class AudioFrameOfAac {
    	public AUDIO_FRAME_TYPE type;
    	public MEMORY_TYPE mType;
		}
    public class AudioPcmFrame {
         public AudioPcmFrame(long frame_ms, long sample_rates, long samples) {
        }
        public long frame_ms_;
        public long channels_; // 1
        public long sample_bits_; // 16
        public long sample_rates_; // 8k, 16k, 32k
        public long samples_;

        public int pcmBuf_;
        public long pcmBufSize_;

        public String buf_; // samples * sample_bits_ / CHAR_BIT * channels_
    }
    public class AudioAacFrame {
    public AudioAacFrame(long frame_ms) {
      aacBuf_ = 0;
      frame_ms_ = frame_ms;
      aacBufSize_ = 0;
    }
    public int aacBuf_;
    public long frame_ms_;
    public long aacBufSize_;
		public String buf_;
	}
  public class VideoYuvFrame {
    VideoYuvFrame(long frame_ms, long width, long height, long ystride,long ustride, long vstride){
         frame_ms_ = frame_ms;
         width_ = width;
         height_ = height;
         ystride_ = ystride;
         ustride_ = ustride;
         vstride_ = vstride;
         ybuf_ = "";
         ubuf_ = "";
         vbuf_ = "";
         buf_ = "";
         data_ = "";
    }
    long frame_ms_;
/*
    byte[] ybuf_;
    byte[] ubuf_;
    byte[] vbuf_;
*/
    String ybuf_;
    String ubuf_;
    String vbuf_;
    long width_;
    long height_;

    long ystride_;
    long ustride_;
    long vstride_;
    //all
    String buf_;
    long bufSize_;
    String data_;
	}
	public class VideoH264Frame {
    VideoH264Frame(){
        frame_ms_ = 0;
        frame_num_ = 0;
        buf_ = "";
        bufSize_ = 0; 
		}
    long 	frame_ms_;
    long  frame_num_;
    //all
    //byte[] buf_;
    String buf_;
    long 	bufSize_;
		//byte[] payload;
    String payload;
	}
	public class  VideoJpgFrame {
    VideoJpgFrame(){
        frame_ms_ = 0;
        buf_ = "";
        bufSize_ = 0;
        payload = "";
		}
    long frame_ms_;
    //all
		//byte[] buf_;
    String buf_;
    long bufSize_;
    String payload;
	}
	public class VideoFrame {
    /*VIDEO_FRAME_TYPE type;
        VideoYuvFrame *yuv;
        VideoH264Frame *h264;
        VideoJpgFrame *jpg;

    int rotation_; // 0, 90, 180, 270
    VideoFrame();
    ~VideoFrame();
		*/
    MEMORY_TYPE mType;
	}
	
	public class UserJoinInfos {
    String storageDir;
    UserJoinInfos(){
        storageDir = "";
    }
	}
	
	public class VideoMixingLayout
	{
    public class Region {
        long uid;
        double x;//[0,1]
        double y;//[0,1]
        double width;//[0,1]
        double height;//[0,1]
        int zOrder; //optional, [0, 100] //0 (default): bottom most, 100: top most

        //  Optional
        //  [0, 1.0] where 0 denotes throughly transparent, 1.0 opaque
        double alpha;

        int renderMode;//RENDER_MODE_HIDDEN: Crop, RENDER_MODE_FIT: Zoom to fit
        Region(){
					uid = 0;
          x = 0;
          y = 0;
					width = 0;
					height = 0;
					zOrder = 0;
					alpha = 1.0;
					renderMode = 0;
        }

    };
    int canvasWidth;
    int canvasHeight;
    String backgroundColor;//e.g. "#C0C0C0" in RGB
    long regionCount;
    Region regions;
    String appData;
    int appDataLength;
    VideoMixingLayout() {
        canvasWidth = 0;
				canvasHeight = 0;
        backgroundColor = "";
        regionCount = 0;
        //regions = 0;
        appData = "";
        appDataLength = 0;
    }
	} 






	

}
