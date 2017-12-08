class EnumTest{
		public enum LEAVE_PATH_CODE {
    	LEAVE_CODE_INIT(0), 
    	LEAVE_CODE_SIG(1<<1),
    	LEAVE_CODE_NO_USERS(1<<2),
    	LEAVE_CODE_TIMER_CATCH(1<<3),
    	LEAVE_CODE_CLIENT_LEAVE(1 << 4);
			private LEAVE_PATH_CODE(int value){}
		}

}
