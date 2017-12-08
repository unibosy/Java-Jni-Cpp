package headers;

public class EnumIndex{
		public enum STAT_CODE_TYPE {
			STAT_ERR_FROM_ENGINE(1),
     	STAT_ERR_ARS_JOIN_CHANNEL(2),
     	STAT_ERR_CREATE_PROCESS(3),
     	STAT_ERR_MIXED_INVALID_VIDEO_PARAM(4),
     	STAT_POLL_ERR(0x8),
     	STAT_POLL_HANG_UP(0x10),
     	STAT_POLL_NVAL (0x20);
  		private int value;
			private STAT_CODE_TYPE(int value) {
				this.value = value;
			}
			public int getValue() {
				return value;
			}
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

		public enum REMOTE_VIDEO_STREAM_TYPE {
			REMOTE_VIDEO_STREAM_HIGH(0),
			REMOTE_VIDEO_STREAM_LOW(1);
			private int value;
			private REMOTE_VIDEO_STREAM_TYPE(int value)	{
				this.value = value;
			}
			public int getValue() {
				return value;
			}
		}
		public enum LEAVE_PATH_CODE {
    	LEAVE_CODE_INIT(0), 
    	LEAVE_CODE_SIG(1<<1),
   		LEAVE_CODE_NO_USERS(1<<2),
    	LEAVE_CODE_TIMER_CATCH(1<<3),
    	LEAVE_CODE_CLIENT_LEAVE(1 << 4);
			private int value;
			private LEAVE_PATH_CODE(int value) {
				this.value = value;
			}
			public int getValue() {
				return value;
			}
		}




};
