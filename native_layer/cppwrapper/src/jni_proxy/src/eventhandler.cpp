#include "eventhandler.h"




void eventhandler::onError(int error, agora::linuxsdk::STAT_CODE_TYPE stat_code) {
  cout<<endl<<"AgoraJniProxySdk onError"<<endl;
	onErrorImpl(error, stat_code);
}

