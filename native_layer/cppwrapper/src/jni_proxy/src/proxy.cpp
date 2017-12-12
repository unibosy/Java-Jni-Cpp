#include "IAgoraLinuxSdkCommon.h"
#include "../jni_headers/AgoraJavaRecording.h"
#include "eventhandler.h"
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

using std::string;
using std::cout;
using std::cerr;
using std::endl;

using agora::base::log;
using namespace jniproxy;

atomic_bool_t g_bSignalStop;

void signal_handler(int signo) {
  (void)signo;

  // cerr << "Signal " << signo << endl;
  g_bSignalStop = true;
}

#ifdef __cplusplus
extern "C" {
#endif

JavaVM* g_jvm = NULL;//one eventhandler vs one?
//define signatures
#define LONG_SIGNATURE "J"
#define INT_SIGNATURE "I"
#define STRING_SIGNATURE "Ljava/lang/String;"
#define BYTEARRAY "[B"
#define VIDEOFRAME_H264_SIGNATURE "Lheaders/EnumIndex$VideoH264Frame;"
#define VIDEO_FORMAT_TYPE_SIGNATURE "Lheaders/EnumIndex$VIDEO_FRAME_TYPE;"
//do common!
bool AgoraJniProxySdk::fillAllFields(JNIEnv* jni_env, jobject& job, jclass& jc, const agora::linuxsdk::AudioFrame*& frame) const
{
#if 1
  if(frame->type != agora::linuxsdk::AUDIO_FRAME_RAW_PCM) return false;
  jfieldID fid = NULL;
  jbyteArray jbArr = NULL;

  agora::linuxsdk::AudioPcmFrame *f = frame->frame.pcm;
  //frame_ms_
  fid = jni_env->GetFieldID(jc, "frame_ms_", LONG_SIGNATURE);
  if(fid == NULL) {
    cout <<"cannot get frame_ms_ value of audioPcmFrame"<<endl;
    return false;
  }
  long frame_ms_ = f->frame_ms_;
  cout<<"get frame_ms_ value of AUDIO_FRAME_TYPE ok:"<<frame_ms_<<endl;
  jni_env->SetLongField(job, fid, jlong(frame_ms_));
#if 1
  //channels_
  fid = jni_env->GetFieldID(jc, "channels_", LONG_SIGNATURE);
  if(fid == NULL) {
    cout <<"cannot get channels_ value of audioPcmFrame"<<endl;
    return false;
  }
  long channels_ = f->channels_;
  cout<<"get channels_ value of AUDIO_FRAME_TYPE ok:"<<channels_<<endl;
  jni_env->SetLongField(job, fid, jlong(channels_));
  //sample_bits_
  fid = jni_env->GetFieldID(jc, "sample_bits_", LONG_SIGNATURE);
  if(fid == NULL) {
    cout <<"cannot get sample_bits_ value of audioPcmFrame"<<endl;
    return false;
  }
  long sample_bits_ = f->sample_bits_;
  cout<<"get sample_bits_ value of AUDIO_FRAME_TYPE ok:"<<sample_bits_<<endl;
  jni_env->SetLongField(job, fid, jlong(sample_bits_));
  //sample_rates_
  fid = jni_env->GetFieldID(jc, "sample_rates_", LONG_SIGNATURE);
  if(fid == NULL) {
    cout <<"cannot get sample_rates_value of audioPcmFrame"<<endl;
    return false;
  }
  long sample_rates_ = f->sample_rates_;
  cout<<"get sample_rates_ value of AUDIO_FRAME_TYPE ok:"<<sample_rates_<<endl;
  jni_env->SetLongField(job, fid, jlong(sample_rates_));
  //pcmBuf_
  fid = jni_env->GetFieldID(jc, "pcmBuf_", BYTEARRAY);
  if(fid == NULL) {
    cout <<"cannot get frame_ms_ value of audioPcmFrame"<<endl;
    return false;
  }
  unsigned char* pcmBuf_ = f->pcmBuf_;
  long pcmBufSize_ = f->pcmBufSize_;

  jbArr = jni_env->NewByteArray(pcmBufSize_);
  jbyte *jby = jni_env->GetByteArrayElements(jbArr,0); 
  memcpy(jby, pcmBuf_, pcmBufSize_);
  jni_env->SetByteArrayRegion(jbArr, 0, pcmBufSize_, jby);
  jni_env->SetObjectField(job, fid, jbArr);
  cout<<"set pcmBuf_  value of AUDIO_FRAME_TYPE ok:"<<pcmBufSize_<<endl;
  //pcmBufSize_
  fid = jni_env->GetFieldID(jc, "pcmBufSize_", LONG_SIGNATURE);
  if(fid == NULL) {
    cout <<"cannot get pcmBufSize_ value of audioPcmFrame"<<endl;
    return false;
  }
  cout<<"get pcmBufSize_ value of AUDIO_FRAME_TYPE ok:"<<pcmBufSize_<<endl;
  jni_env->SetLongField(job, fid, jlong(pcmBufSize_));

  //buf_
  fid = jni_env->GetFieldID(jc, "buf_", STRING_SIGNATURE);
  if(fid == NULL) {
    cout <<"cannot get buf_ value of audioPcmFrame"<<endl;
    return false;
  }
  std::string buf_ = f->buf_;
  jstring jstrBuf = jni_env->NewStringUTF(buf_.c_str());
  cout<<"get buf_ value of AUDIO_FRAME_TYPE ok:"<<buf_<<endl;
  jni_env->SetObjectField(job, fid, jstrBuf);
#endif 
  //fill AudioPcmFrame field
  return true;
#endif
}
bool AgoraJniProxySdk::fillAudioPcmFrame(JNIEnv* jni_env, const agora::linuxsdk::AudioFrame*& frame, \
            jclass& jcAudioFrame, jobject& jobAudioFrame) const 
{
#if 1
  if(frame->type != agora::linuxsdk::AUDIO_FRAME_RAW_PCM) return false;
  jmethodID jmid = NULL;
  jclass jc = NULL;
  jfieldID fid = NULL;
  jmethodID initMid = NULL;
  jobject job = NULL;

  //get pcm frame by find class
  jc = jni_env->FindClass("Lheaders/EnumIndex$AudioPcmFrame;");
  if(jc == NULL) {
    cout<<"cannot get AudioPcmFrame jclass"<<endl;
    //((JavaVM*)g_jvm)->DetachCurrentThread();
    return false;
  }
  cout << "get pcm jclass ok"<<endl;
  //new AudioPcmFrame
  //TODO  place these to one function
  initMid = jni_env->GetMethodID(jc,"<init>","(Lheaders/EnumIndex;JJJ)V");
  if(initMid == NULL) {
    cout <<"get AudioPcmFrame init methid failed!"<<endl;
    //((JavaVM*)g_jvm)->DetachCurrentThread();
    return false;
  }
  job = jni_env->NewObject(jc, initMid);
  if(job == NULL){
    cout<<"new AudioPcmFrame failed! no memory?"<<endl;
    //((JavaVM*)g_jvm)->DetachCurrentThread();
    return false;
  }
#if 0
  //get AudioPcmFrame field
 fid = jni_env->GetFieldID(jc, "frame_ms_", "J");
  if(fid == NULL) {
    cout <<"cannot get frame_ms_ value of AUDIO_FRAME_TYPE "<<endl;
    //((JavaVM*)g_jvm)->DetachCurrentThread();
    return false;
  }
  agora::linuxsdk::AudioPcmFrame *f = frame->frame.pcm;
  long frame_ms_ = f->frame_ms_;
  cout<<"get frame_ms_ value of AUDIO_FRAME_TYPE ok:"<<frame_ms_<<endl;
  //fill AudioPcmFrame field
  jni_env->SetLongField(job, fid, jlong(frame_ms_));
  //set 
  //fill pcm data
  fid = jni_env->GetFieldID(jcAudioFrame, "pcm", "Lheaders/EnumIndex$AudioPcmFrame;");
  if(fid == NULL) {
    cout <<"cannot get pcm AUDIO_FRAME_TYPE field"<<endl;
    //((JavaVM*)g_jvm)->DetachCurrentThread();
    return false;
  }
 
  jni_env->SetObjectField(jobAudioFrame, fid, job);
#else
  fid = jni_env->GetFieldID(jcAudioFrame, "pcm", "Lheaders/EnumIndex$AudioPcmFrame;");
  if(fid == NULL) {
    cout <<"cannot get pcm AUDIO_FRAME_TYPE field"<<endl;
    //((JavaVM*)g_jvm)->DetachCurrentThread();
    return false;
  }
  //fill all fields of AudioPcmFrame jobject
  if(fillAllFields(jni_env, job, jc, frame)){
    //Fill in the jobAdudioFrame
    jni_env->SetObjectField(jobAudioFrame, fid, job);
    return true;
  }
  return false;
#endif
  return true;
#endif
}
bool AgoraJniProxySdk::fillJVideoOfYUV(JNIEnv* jni_env, const agora::linuxsdk::VideoFrame*& frame, jclass& jcVideoFrame, jobject& jobVideoFrame) const {
  return true;
}
bool AgoraJniProxySdk::fillJVideoOfJPG(JNIEnv* jni_env, const agora::linuxsdk::VideoFrame*& frame, jclass& jcVideoFrame, jobject& jobVideoFrame) const {
  return true;
}
bool AgoraJniProxySdk::fillJVideoOfH264(JNIEnv* jni_env, const agora::linuxsdk::VideoFrame*& frame, jclass& jcVideoFrame, jobject& jobVideoFrame) const{
//bool AgoraJniProxySdk::fillJVideoOfH264(JNIEnv* jni_env, const agora::linuxsdk::VideoFrame*& frame, jobject& job) const{
  if(frame->type == agora::linuxsdk::VIDEO_FRAME_JPG || frame->type == agora::linuxsdk::VIDEO_FRAME_RAW_YUV) return false;
  if(!jni_env || !frame) return false;
  jclass jc = NULL;
  jmethodID initMid = NULL;
  jobject job = NULL;
  jfieldID fid = NULL;
  int fieldId = 0; //TODO
  jbyteArray jbArr = NULL;

  //1.get subclass
  jc = jni_env->FindClass("Lheaders/EnumIndex$VideoH264Frame;");
  if(!jc) {
    cout << "cannot find subclass, type:"<<frame->type <<endl;
    return false;
  }
  //2.get subclass init method
  initMid = jni_env->GetMethodID(jc,"<init>","(Lheaders/EnumIndex;)V");
  if(!initMid) {
    cout<<"cannot get init methodid"<<endl;
    return false;
  }
  //3.new VideoH264Frame object
  job = jni_env->NewObject(jc, initMid);
  if(!job){
    cout<<"new subclass  failed! no memory?"<<endl;
    return false;
  }
  agora::linuxsdk::VideoH264Frame *f = frame->frame.h264;
  if(!f) return false;
  //4.fill all fields
  //4.1 get & set of this subclass object
  //frame_ms_
  fid = jni_env->GetFieldID(jc, "frame_ms_", LONG_SIGNATURE);
  if(!fid) {
    //TODO add field ID
    cout <<"cannot get field,field ID:"<<fieldId<<endl;
    return false;
  }
  long frame_ms_ = f->frame_ms_;
  cout<<"get frame_ms_ value of h264 ok:"<<frame_ms_<<endl;
  jni_env->SetLongField(job, fid, jlong(frame_ms_));
  //frame_num_
  fid = jni_env->GetFieldID(jc, "frame_num_", LONG_SIGNATURE);
  if(!fid) {
    //TODO add field ID
    cout <<"cannot get field,field ID:"<<fieldId<<endl;
    return false;
  }
  long frame_num_ = f->frame_num_;
  jni_env->SetLongField(job, fid, jlong(frame_num_));

  //buf_
  fid = jni_env->GetFieldID(jc, "buf_", BYTEARRAY);
  if(fid == NULL) {
    cout <<"cannot get field,field ID:"<<fieldId<<endl;
    return false;
  }
  unsigned char* buf_ = f->buf_;
  long bufSize_ = f->bufSize_;

  jbArr = jni_env->NewByteArray(bufSize_);
  if(!jbArr) {
    cout <<"NewByteArray failed, no memory??"<<endl;
    return false;
  }
  jbyte *jby = jni_env->GetByteArrayElements(jbArr,0); 
  memcpy(jby, buf_, bufSize_);
  jni_env->SetByteArrayRegion(jbArr, 0, bufSize_, jby);
  jni_env->SetObjectField(job, fid, jbArr);
  cout<<"set buf_ value  ok:"<<bufSize_<<endl;
  //bufSize_
  fid = jni_env->GetFieldID(jc, "bufSize_", LONG_SIGNATURE);
  if(!fid) {
    cout <<"cannot get field,field ID:"<<fieldId<<endl;
    return false;
  }
  cout<<"get bufSize_ value ok:"<<bufSize_<<endl;
  jni_env->SetLongField(job, fid, jlong(bufSize_));
  //payload
  fid = jni_env->GetFieldID(jc, "payload", STRING_SIGNATURE);
  if(!fid) {
    cout <<"cannot get field,field ID:"<<endl;
    return false;
  }
  std::string payload = f->payload;
  jstring jstrPayload = jni_env->NewStringUTF(payload.c_str());
  cout<<"get payload value ok"<<endl;
  jni_env->SetObjectField(job, fid, jstrPayload);

  //5.get subclass field
  fid = jni_env->GetFieldID(jcVideoFrame, "h264", VIDEOFRAME_H264_SIGNATURE);
  if(fid == NULL) {
    cout <<"cannot get VIDEOFRAME_H264_SIGNATURE"<<endl;
    //((JavaVM*)g_jvm)->DetachCurrentThread();
    return false;
  }
  //6.fill jobVideFrame
  jni_env->SetObjectField(jobVideoFrame, fid, job);
  return  true;
}
bool AgoraJniProxySdk::fillJVideoFrameByFields(JNIEnv* jni_env, const agora::linuxsdk::VideoFrame*& frame, jclass& jcVideoFrame, jobject& jobVideoFrame) const {
  bool ret = false;
  jfieldID jfid = NULL;
  jclass jc = NULL;
  jmethodID initMid = NULL;
  jfieldID fid = NULL;

  if(!jni_env || !frame || !jcVideoFrame || !jobVideoFrame){
    cout <<"AgoraJniProxySdk::fillJVideoFrameByFields para error!"<<endl;
    return ret;
  }
  cout <<"AgoraJniProxySdk::fillJVideoFrameByFields enter , video type:"<<frame->type<<endl;
  jobject job = NULL;
#if 0
  switch(frame->type) {
    case agora::linuxsdk::VIDEO_FRAME_RAW_YUV: {
       
    }
    break;
    case agora::linuxsdk::VIDEO_FRAME_JPG: {
    }
    break;
    default: {
      cout<<"videoFrameReceived raw h264!"<<endl;
      if(!fillJVideoOfYUV(jni_env, frame, job)){
        cout << "fill subclass falied!"<<endl;
       }
    }
    break;
  }
  if(job)
    jni_env->DeleteLocalRef(javaClass);
  return ret;
#endif 
  #if 1
  if (frame->type == agora::linuxsdk::VIDEO_FRAME_RAW_YUV) {
    //3.1
    cout<<"videoFrameReceived raw yuv!"<<endl;
    //3.1.1 
    if(!fillJVideoOfYUV(jni_env, frame, jcVideoFrame, jobVideoFrame)){
      cout << "fill subclass falied!"<<endl;
      if(job)
        jni_env->DeleteLocalRef(job);
      return false;
    }
  }else if(frame->type == agora::linuxsdk::VIDEO_FRAME_JPG){
    //3.2
    cout<<"videoFrameReceived raw jpg!"<<endl;
    fillJVideoOfJPG(jni_env, frame, jcVideoFrame, jobVideoFrame);
  }else if(frame->type == agora::linuxsdk::VIDEO_FORMAT_H264_FRAME_TYPE){
    //3.3
    cout<<"videoFrameReceived raw h264!"<<endl;
    if(!fillJVideoOfH264(jni_env, frame, jcVideoFrame, jobVideoFrame))
    {
      cout << "fillJVideoOfH264 failed!"<<endl;
      return false;
    }
  }else if(frame->type == agora::linuxsdk::VIDEO_FORMAT_JPG_FRAME_TYPE){
    cout << "videoFrameReceived VIDEO_FORMAT_JPG_FRAME_TYPE" <<endl;
  }else if(frame->type == agora::linuxsdk::VIDEO_FORMAT_JPG_FILE_TYPE){
    cout << "videoFrameReceived VIDEO_FORMAT_JPG_FILE_TYPE" <<endl;
  }
  #endif 
  //fill VIDEO_FRAME_TYPE
  //set type of VIDEO_FRAME_TYPE by FindClass
  jc = jni_env->FindClass("Lheaders/EnumIndex$VIDEO_FRAME_TYPE;");
  if(!jc) {
    cout<<"cannot get VIDEO_FRAME_TYPE class"<<endl;
    return false;
  }
  cout <<"get AUDIO_FORMAT_TYPE ok!"<<endl;
  //4.1.1
  initMid = jni_env->GetMethodID(jc,"<init>","(Lheaders/EnumIndex;)V");
  if(initMid == NULL) {
    cout <<"get init methid failed!"<<endl;
    return false;
  }
  //4.1.2
  job = jni_env->NewObject(jc, initMid);
  //4.1.3 get field of this class
  fid = jni_env->GetFieldID(jc, "type", "I");
  if(fid == NULL) {
    cout <<"cannot get value of VIDEO_FRAME_TYPE class"<<endl;
    return false;
  }
  cout<<"get value of VIDEO_FRAME_TYPE ok"<<endl;
  //4.1.4 fill this field
  int iVideoFrameType = static_cast<int>(frame->type);
  jni_env->SetIntField(job, fid, jint(iVideoFrameType));
  //4.2
  //set this object into jobAudioFrame!
  //step 1:get this object field
  fid = jni_env->GetFieldID(jcVideoFrame, "type", VIDEO_FORMAT_TYPE_SIGNATURE);
  if(!fid) {
    cout <<"cannot get VIDEO_FORMAT_TYPE field"<<endl;
    return false;
  }
  //4.3
  jni_env->SetObjectField(jobVideoFrame, fid, job);
  cout<<"set GetObjectField ok"<<endl;

}
void AgoraJniProxySdk::videoFrameReceived(unsigned int uid, const agora::linuxsdk::VideoFrame *frame) const {
  static int count = 0;
  cout<<"AgoraJniProxySdk::videoFrameReceived enter uid:"<<uid<<",count:"<<++count<<endl;
#if 1
  jmethodID jmid = NULL;
  jobject job = NULL; //should be deleted
  jclass jczz = NULL; //should be deleted,findClass need to free?
  jfieldID jfid = NULL;
  jmethodID initMid = NULL;
  jclass jcVideoFrame = NULL;
  jobject jobVideoFrame = NULL;


  JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
	if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK) {
		cout <<"GetEnv failed"<<endl;
		return;
	}
	assert(jni_env);
  assert(javaClass);
	pid_t tid = getpid();
	cout << "this thread id is:"<<tid<<endl;
  //1.find VideoFrame class
  jcVideoFrame = jni_env->FindClass("headers/EnumIndex$VideoFrame");
  if(jcVideoFrame == NULL) {
    cout << "videoFrameReceived cannot find class VideoFrame!"<<endl;
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  //2.get init methodid to new VideoFrame object
  initMid = jni_env->GetMethodID(jcVideoFrame,"<init>","(Lheaders/EnumIndex;)V");
  if(initMid == NULL) {
    cout << "cannot get VideoFrmae init method "<<endl;
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  //new VideoFrame object
  jobVideoFrame = jni_env->NewObject(jcVideoFrame, initMid);
  if(jobVideoFrame == NULL) {
    cout <<"new video frame object failed!"<<endl;
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  //3.get subclass or enumm details and fill these' fields
#if 0
  if (frame->type == agora::linuxsdk::VIDEO_FRAME_RAW_YUV) {
    //3.1
    cout<<"videoFrameReceived raw yuv!"<<endl;
    handleRawYUV(jni_env, frame, jcVideoFrame, jobVideoFrame);
    
  }else if(frame->type == agora::linuxsdk::VIDEO_FRAME_JPG){
    //3.2
    cout<<"videoFrameReceived raw jpg!"<<endl;
    handleJPG(jni_env, frame, jcVideoFrame, jobVideoFrame);
  }else{
    //3.3
    cout<<"videoFrameReceived raw h264!"<<endl;
    handleH264(jni_env, frame, jcVideoFrame, jobVideoFrame);
  }
#else
  if(!fillJVideoFrameByFields(jni_env, frame, jcVideoFrame, jobVideoFrame))
  {
    cout << "jni fillJVideoFrameByFields failed!" <<endl;
    //TODO delete ref
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
#endif
  //4.find class where callback function in
  cout <<"AgoraJniProxySdk::videoFrameReceived end"<<endl;
  jclass jcCallBack = jni_env->FindClass("AgoraJavaRecording");
  if(jcCallBack == NULL) {
    cout << "cannot find callback class"<<endl;
    //TODO delete local ref
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  //5. find callback function
  jmid = jni_env->GetStaticMethodID(jcCallBack, "videoFrameReceived", "(JLheaders/EnumIndex$VideoFrame;)V");
  if(jmid == NULL) {
    cout << "cannot get callback function"<<endl;
    //TODO memory leak..
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  //so far so well
  //6. call back java method, para is jobVideoFrame
  jni_env->CallStaticVoidMethod(jcVideoFrame, jmid,jlong(long(uid)), jobVideoFrame);
  
  jni_env->DeleteLocalRef(jcVideoFrame);
  ((JavaVM*)g_jvm)->DetachCurrentThread();
#endif
#if 0
 char uidbuf[65];
  snprintf(uidbuf, sizeof(uidbuf),"%u", uid);
  const char * suffix=".vtmp";
  if (frame->type == agora::linuxsdk::VIDEO_FRAME_RAW_YUV) {
    agora::linuxsdk::VideoYuvFrame *f = frame->frame.yuv;
    suffix=".yuv";

    cout << "User " << uid << ", received a yuv frame, width: "
        << f->width_ << ", height: " << f->height_ ;
    cout<<",ystride:"<<f->ystride_<< ",ustride:"<<f->ustride_<<",vstride:"<<f->vstride_;
    cout<< endl;
  } else if(frame->type == agora::linuxsdk::VIDEO_FRAME_JPG) {
    suffix=".jpg";
    agora::linuxsdk::VideoJpgFrame *f = frame->frame.jpg;

    cout << "User " << uid << ", received an jpg frame, timestamp: "
    << f->frame_ms_ << endl;

    struct tm date;
    time_t t = time(NULL);
    localtime_r(&t, &date);
    char timebuf[128];
    sprintf(timebuf, "%04d%02d%02d%02d%02d%02d", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday, date.tm_hour, date.tm_min, date.tm_sec);
    std::string file_name = m_storage_dir + std::string(uidbuf) + "_" + std::string(timebuf) + suffix;
    FILE *fp = fopen(file_name.c_str(), "w");
    ::fwrite(f->buf_, 1, f->bufSize_, fp);
    ::fclose(fp);
    return;
  } else {
    suffix=".h264";
    agora::linuxsdk::VideoH264Frame *f = frame->frame.h264;

    cout << "User " << uid << ", received an h264 frame, timestamp: "
        << f->frame_ms_ << ", frame no: " << f->frame_num_ << endl;
  }

  std::string info_name = m_storage_dir + std::string(uidbuf) /*+ timestamp_per_join_ */+ std::string(suffix);
  FILE *fp = fopen(info_name.c_str(), "a+b");

  //store it as file
  if (frame->type == agora::linuxsdk::VIDEO_FRAME_RAW_YUV) {
      agora::linuxsdk::VideoYuvFrame *f = frame->frame.yuv;
      ::fwrite(f->buf_, 1, f->bufSize_, fp);
  }
  else {
      agora::linuxsdk::VideoH264Frame *f = frame->frame.h264;
      ::fwrite(f->buf_, 1, f->bufSize_, fp);
  }
  ::fclose(fp);
#endif 
}
//TODO  use the same parameter
void AgoraJniProxySdk::audioFrameReceived(unsigned int uid, const agora::linuxsdk::AudioFrame *frame) const
{
  static int count = 0;
	cout<<"AgoraJniProxySdk::audioFrameReceived enter uid:"<<uid<<",count:"<<++count<<endl;
  jfieldID fid;
  jmethodID mid;
  jclass jc = NULL;
  jmethodID initMid  = NULL;
  jobject job = NULL;
  int iAudioFrameType = 0; //default pcm?

	JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
	if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK) {
		cout <<"GetEnv failed"<<endl;
		return;
	}
	assert(jni_env);
	pid_t tid = getpid();
	cout << "this thread id is:"<<tid<<endl;
  jclass jcAudioFrame = NULL;

  jcAudioFrame = (jni_env)->FindClass("headers/EnumIndex$AudioFrame");
  if(jcAudioFrame == NULL){
    cout<<"not find audioFrame"<<endl;
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return ;
  }
  //1.find main class
  if (frame->type == agora::linuxsdk::AUDIO_FRAME_RAW_PCM) {
    cout <<"jni receive raw data is pcm!"<<endl;
    cout<<"----------1"<<endl;
    //do thngs here
  }else if (frame->type == agora::linuxsdk::AUDIO_FRAME_AAC) {
    //do things here
  }
  iAudioFrameType = static_cast<int>(frame->type);
  cout<<"----------2,iAudioFrameType:"<<iAudioFrameType<<endl;
  //2.get main class init methodid
  initMid = jni_env->GetMethodID(jcAudioFrame,"<init>","(Lheaders/EnumIndex;)V");
  if(initMid == NULL){
    cout <<"not find AudioFrameOfPcm initMid!"<<endl;
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  cout<<"----------3"<<endl;
  //3.new main class object
  jobject jobAudioFrame = jni_env->NewObject(jcAudioFrame, initMid);
  if(jobAudioFrame == NULL){
    cout<<"new audio frame object failed!"<<endl;
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  cout<<"----------4"<<endl;
  //4.get main class field
#if 1
  //4.1.find subclass in main class
  //set type of AUDIO_FRAME_TYPE by FindClass
  jc = jni_env->FindClass("Lheaders/EnumIndex$AUDIO_FRAME_TYPE;");
  if(jc == NULL) {
    cout<<"cannot get AUDIO_FRAME_TYPE class"<<endl;
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  cout <<"get AUDIO_FORMAT_TYPE ok!"<<endl;
  //4.1.1
  initMid = jni_env->GetMethodID(jc,"<init>","(Lheaders/EnumIndex;)V");
  if(initMid == NULL) {
    cout <<"get init methid failed!"<<endl;
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  //4.1.2
  job = jni_env->NewObject(jc, initMid);
  //4.1.3 get field of this class
  fid = jni_env->GetFieldID(jc, "type", "I");
  if(fid == NULL) {
    cout <<"cannot get value of AUDIO_FRAME_TYPE class"<<endl;
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  cout<<"get value of AUDIO_FRAME_TYPE ok"<<endl;
  //4.1.4 fill this field
  jni_env->SetIntField(job, fid, jint(iAudioFrameType));
  //4.2
  //set this object into jobAudioFrame!
  //step 1:get this object field
  fid = jni_env->GetFieldID(jcAudioFrame, "type", "Lheaders/EnumIndex$AUDIO_FRAME_TYPE;");
  if(fid == NULL) {
    cout <<"cannot get AUDIO_FRAME_TYPE field"<<endl;
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  //4.3
  jni_env->SetObjectField(jobAudioFrame, fid, job);
  cout<<"set GetObjectField ok"<<endl;
#endif 
#if 0
  fid = jni_env->GetFieldID(jcAudioFrame, "Type", "I");
  if(fid == NULL){
    cout <<"cannot get field of type "<<endl;
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  //5.set object all fields
  //if this is a static parameter,then shoud set field to jclass
  jni_env->SetIntField(jobAudioFrame, fid, jint(1));
#endif 
  //call one function
  if(!fillAudioPcmFrame(jni_env, frame, jcAudioFrame,jobAudioFrame))
  {
    cout <<"Warning: fillAudioPcmFrame failed!!!!!"<<endl;
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  cout<<"----------7"<<endl;
  //6.find callback method in Java
  jclass javaClass = jni_env->FindClass("AgoraJavaRecording");
  assert(javaClass);

  mid = jni_env->GetStaticMethodID(javaClass, "audioFrameReceived", "(JLheaders/EnumIndex$AudioFrame;)V");
  cout<<"----------8"<<endl;
  if(mid == NULL){
    cout<<"audioFrameReceived get method failed!"<<endl;
    jni_env->DeleteLocalRef(javaClass);
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return ;
  }
  cout<<"----------9"<<endl;
  //7. callback java method
  jni_env->CallStaticVoidMethod(javaClass, mid,jlong(long(uid)), jobAudioFrame);
  cout<<"----------10"<<endl;
  jni_env->DeleteLocalRef(javaClass);
  ((JavaVM*)g_jvm)->DetachCurrentThread();
#if 0
  char uidbuf[65];
  snprintf(uidbuf, sizeof(uidbuf),"%u", uid);
  std::string info_name = m_storage_dir + std::string(uidbuf) /*+ timestamp_per_join_*/;

  uint8_t* data = NULL; 
  uint32_t size = 0;
  if (frame->type == agora::linuxsdk::AUDIO_FRAME_RAW_PCM) {
    info_name += ".pcm";  
    
    cout << "User " << uid << ", received a raw PCM frame" << endl;

    agora::linuxsdk::AudioPcmFrame *f = frame->frame.pcm;
    long tmp = f->frame_ms_;
    data = f->pcmBuf_;
    size = f->pcmBufSize_;

  } else if (frame->type == agora::linuxsdk::AUDIO_FRAME_AAC) {
    info_name += ".aac";

    cout << "User " << uid << ", received an AAC frame" << endl;

    agora::linuxsdk::AudioAacFrame *f = frame->frame.aac;
    data = f->aacBuf_;
    size = f->aacBufSize_;
  }

  FILE *fp = fopen(info_name.c_str(), "a+b");
  ::fwrite(data, 1, size, fp);
  ::fclose(fp); 
#endif 
}

void AgoraJniProxySdk::onUserJoined(agora::linuxsdk::uid_t uid, agora::linuxsdk::UserJoinInfos &infos) {
  cout << "AgoraJniProxySdk User " << uid << " joined, RecordingDir:" << (infos.storageDir? infos.storageDir:"NULL") <<endl;
  std::string store_dir = std::string(infos.storageDir);
	JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
	if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK) {
		cout <<"GetEnv failed"<<endl;
		return;
	}
  assert(jni_env);
	jclass javaClass = jni_env->FindClass("AgoraJavaRecording");
  assert(javaClass);
	//int tid = GetCurrentThreadId();
	pid_t tid = getpid();
	cout << "this thread id is:"<<tid<<endl;
  jmethodID jUserJoinedMid =  jni_env->GetStaticMethodID(javaClass,"onUserJoined","(JLjava/lang/String;)V");
  assert(jUserJoinedMid);

  jstring jstrRecordingDir = jni_env->NewStringUTF(store_dir.c_str());
  jni_env->CallStaticVoidMethod(javaClass, jUserJoinedMid, jlong((long)(uid)),jstrRecordingDir);
  cout << "AgoraJniProxySdk::onUserJoined call end"<<endl;

  jni_env->DeleteLocalRef(javaClass);
	((JavaVM*)g_jvm)->DetachCurrentThread();
}
void AgoraJniProxySdk::onUserOffline(agora::linuxsdk::uid_t uid, agora::linuxsdk::USER_OFFLINE_REASON_TYPE reason)
{
  cout << "AgoraJniProxySdk onUserOffline User:" << uid << ",reason" << reason<<endl;
	JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
	if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK) {
		cout <<"GetEnv failed"<<endl;
		return;
	}
  assert(jni_env);
	jclass javaClass = jni_env->FindClass("AgoraJavaRecording");
  assert(javaClass);
	//int tid = GetCurrentThreadId();
	pid_t tid = getpid();
	cout << "this thread id is:"<<tid<<endl;
  jmethodID jUserOfflineMid =  jni_env->GetStaticMethodID(javaClass,"onUserOffline","(JI)V");
  assert(jUserOfflineMid);
  jni_env->CallStaticVoidMethod(javaClass, jUserOfflineMid, jlong((long)(uid)),jint(int(reason)));
  cout << "AgoraJniProxySdk::onUserOffline call end"<<endl;

  jni_env->DeleteLocalRef(javaClass);
	((JavaVM*)g_jvm)->DetachCurrentThread();
}
void AgoraJniProxySdk::onLeaveChannel(agora::linuxsdk::LEAVE_PATH_CODE code) {
	cout<<"AgoraJniProxySdk onLeaveChannel"<<endl;
	JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
	if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK)
	{
		cout <<"GetEnv failed"<<endl;
		return;
	}
  assert(jni_env);
	jclass javaClass = jni_env->FindClass("AgoraJavaRecording");
  assert(javaClass);
	//int tid = GetCurrentThreadId();
	pid_t tid = getpid();
	cout << "this thread id is:"<<tid<<endl;
  jmethodID jLeaveMid =  jni_env->GetStaticMethodID(javaClass,"onLeaveChannel","(I)V");
  assert(jcLeave);
  jni_env->CallStaticVoidMethod(javaClass, jLeaveMid, jint((int)(code)));
  cout << "AgoraJniProxySdk::onLeaveChannel call end"<<endl;

  jni_env->DeleteLocalRef(javaClass);
	((JavaVM*)g_jvm)->DetachCurrentThread();
}
void AgoraJniProxySdk::onWarning(int warn)
{
 	cout<<"AgoraJniProxySdk onWarning,warn:"<<warn<<endl;
	JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
	if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK) {
		cout <<"GetEnv failed"<<endl;
		return;
	}
  assert(jni_env);
	jclass javaClass = jni_env->FindClass("AgoraJavaRecording");
  assert(javaClass);
	//int tid = GetCurrentThreadId();
	pid_t tid = getpid();
	cout << "onWarning this thread id is:"<<tid<<endl;
  jmethodID jOnWarnMid =  jni_env->GetStaticMethodID(javaClass,"onWarning","(I)V");
  assert(jOnWarnMid);
  jni_env->CallStaticVoidMethod(javaClass, jOnWarnMid, warn);
  jni_env->DeleteLocalRef(javaClass);
	((JavaVM*)g_jvm)->DetachCurrentThread();
}

void AgoraJniProxySdk::onError(int error, agora::linuxsdk::STAT_CODE_TYPE stat_code)
{
 	cout<<"AgoraJniProxySdk onError"<<endl;
	JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
  if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK) {
		cout <<"GetEnv failed"<<endl;
		return;
	}
  assert(jni_env);
	jclass javaClass = jni_env->FindClass("AgoraJavaRecording");
  assert(javaClass);
	//int tid = GetCurrentThreadId();
	pid_t tid = getpid();
	cout << "this thread id is:"<<tid<<endl;
  jmethodID jLeaveMid =  jni_env->GetStaticMethodID(javaClass,"onError","(II)V");
  assert(jcLeave);
  jni_env->CallStaticVoidMethod(javaClass, jLeaveMid, error, jint((int)(stat_code)));
  leaveChannel();

  jni_env->DeleteLocalRef(javaClass);
	((JavaVM*)g_jvm)->DetachCurrentThread();
}
//TODO
#if 0
  convert jobject jni_recordingConfig to c++ recordingConfig
#endif
#if 1
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	g_jvm = vm;
	assert(g_jvm);
	cout<<"jint JNI_OnLoad enter"<<endl;
	return JNI_VERSION_1_4;
}
#endif


/*
 * Class:     AgoraJavaRecording
 * Method:    leaveChannel
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_AgoraJavaRecording_leaveChannel
  (JNIEnv *, jobject)
{
  cout<<"java call leaveChannel"<<endl;
  g_bSignalStop = true;
  return JNI_TRUE;
}

void stopJavaProc()
{
  cout<<"AgoraJniProxySdk stopJavaProc"<<endl;
	JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
  if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK) {
		cout <<"GetEnv failed"<<endl;
		return;
	}
  assert(jni_env);
	jclass javaClass = jni_env->FindClass("AgoraJavaRecording");
  assert(javaClass);
	pid_t tid = getpid();
	cout << "this thread id is:"<<tid<<endl;
  jmethodID jStopCB =  jni_env->GetStaticMethodID(javaClass,"stopCallBack","()V");
  assert(jStopCB);
  jni_env->CallStaticVoidMethod(javaClass, jStopCB);

  jni_env->DeleteLocalRef(javaClass);
	((JavaVM*)g_jvm)->DetachCurrentThread();

}

JNIEXPORT jboolean JNICALL Java_AgoraJavaRecording_createChannel(JNIEnv * env, jobject jobject1, jstring jni_appid, jstring jni_channelKey, 
      jstring jni_channelName, jint jni_uid, jobject jni_recordingConfig)
{
		pid_t tid = getpid();
	  cout<<"enter Java_AgoraJavaRecording_createChannel,pid:"<<tid<<endl;
		g_bSignalStop = false;
	  signal(SIGQUIT, signal_handler);
  	signal(SIGABRT, signal_handler);
  	signal(SIGINT, signal_handler);
  	signal(SIGPIPE, SIG_IGN);

    const char* appId = NULL;
    appId = env->GetStringUTFChars(jni_appid,false);
    if(!appId)
      cerr<<"get appId is NULL"<<endl;
    const char* channelKey = NULL;
    channelKey = env->GetStringUTFChars(jni_channelKey,false);
    if(!channelKey)
      cerr<<"no channel key"<<endl;
    const char* channelName = env->GetStringUTFChars(jni_channelName,false);
    if(!channelName)
      cerr<<"channel name is empty!"<<endl;
    int uid = (int)jni_uid;
		if(uid < 0){
			cout<<"jni uid is smaller than 0, set 0!"<<endl;
			uid = 0;
		}
   	jclass jRecordingConfig =env->GetObjectClass(jni_recordingConfig); 
		if(jRecordingConfig == NULL){
			cout<<"jni_recordingConfig is NULL"<<endl;
			return JNI_FALSE;
		}
    //get parameters field ID
		jfieldID idleLimitSecFieldID = env->GetFieldID(jRecordingConfig, "idleLimitSec", "I");
		jfieldID appliteDirFieldID = env->GetFieldID(jRecordingConfig, "appliteDir", "Ljava/lang/String;");
		jfieldID channelProfileFieldID = env->GetFieldID(jRecordingConfig, "channelProfile", "Lheaders/EnumIndex$CHANNEL_PROFILE_TYPE;");
		jfieldID streamTypeFieldID = env->GetFieldID(jRecordingConfig, "streamType", "Lheaders/EnumIndex$REMOTE_VIDEO_STREAM_TYPE;");
		jfieldID decodeAudioFieldID = env->GetFieldID(jRecordingConfig, "decodeAudio", "Lheaders/EnumIndex$AUDIO_FORMAT_TYPE;");
		jfieldID decodeVideoFieldID = env->GetFieldID(jRecordingConfig, "decodeVideo", "Lheaders/EnumIndex$VIDEO_FORMAT_TYPE;");

    if (idleLimitSecFieldID == NULL ||appliteDirFieldID == NULL || channelProfileFieldID == NULL 
						|| streamTypeFieldID == NULL ||decodeAudioFieldID ==NULL ||decodeVideoFieldID ==NULL) { 
            cout <<"get fieldID failed!"<<endl;return JNI_FALSE;}
		//idle
		jint idleValue = env->GetIntField(jni_recordingConfig, idleLimitSecFieldID); 
		//appliteDir
		jstring appliteDir = (jstring)env->GetObjectField(jni_recordingConfig, appliteDirFieldID);
		const char * c_appliteDir = env->GetStringUTFChars(appliteDir ,NULL);
		std::string str_appliteDir = c_appliteDir;
		env->ReleaseStringUTFChars(appliteDir,c_appliteDir);
		//CHANNEL_PROFILE_TYPE
		jobject channelProfileObject = (env)->GetObjectField(jni_recordingConfig, channelProfileFieldID);
		//assert(channelProfileObject);
		jclass enumClass = env->GetObjectClass(channelProfileObject);
		if(enumClass == NULL) {  
			cout <<"enumClass is null"<<endl;
		}
		jmethodID getValue = env->GetMethodID(enumClass, "getValue", "()I");
		if (getValue == NULL) {
			cout <<"method not found"<<endl;
			return JNI_FALSE; /* method not found */
		}
		jint channelProfileValue = env->CallIntMethod(channelProfileObject, getValue);
    //streamType
		jobject streamTypeObject = (env)->GetObjectField(jni_recordingConfig, streamTypeFieldID);
		jclass streamTypeClass = env->GetObjectClass(streamTypeObject);
		assert(streamTypeObject);
		if(streamTypeObject == NULL){cout <<"streamTypeEnum is NULL"; return JNI_FALSE;}
		jmethodID getValue2 = env->GetMethodID(streamTypeClass, "getValue", "()I");
		jint streamTypeValue = env->CallIntMethod(streamTypeObject, getValue2);
		//decodeAudio
		jobject jobDecodeAudio = (env)->GetObjectField(jni_recordingConfig, decodeAudioFieldID);
		jclass jcdecodeAudio = env->GetObjectClass(jobDecodeAudio);
		if(jcdecodeAudio == NULL) {
			cout <<"jcdecodeAudio is null"<<endl;
		}
		jmethodID jmidGetValue = env->GetMethodID(jcdecodeAudio, "getValue", "()I");
		if (jmidGetValue == NULL) {
			cout <<"jmidGetValue not found"<<endl;
			return JNI_FALSE; /* method not found */
		}
		jint decodeAudioValue = env->CallIntMethod(jobDecodeAudio, jmidGetValue);
    //decodeVideo
		jobject jobDecodeVideo = (env)->GetObjectField(jni_recordingConfig, decodeVideoFieldID);
		jclass jcdecodeVideo = env->GetObjectClass(jobDecodeVideo);
		if(jcdecodeVideo == NULL) {
			cout <<"jcdecodeVideo is null"<<endl;
		}
		jmidGetValue = env->GetMethodID(jcdecodeVideo, "getValue", "()I");
		if (jmidGetValue == NULL) {
			cout <<"jmidGetValue not found"<<endl;
			return JNI_FALSE; /* method not found */
		}
		jint decodeVideoValue = env->CallIntMethod(jobDecodeVideo, jmidGetValue);
    
    //paser parameters end

		agora::recording::RecordingConfig config;
		jniproxy::AgoraJniProxySdk jniRecorder;
		config.appliteDir = const_cast<char*>(str_appliteDir.c_str());	
		config.idleLimitSec = (int)idleValue;
    cout <<"appId:"<<appId<<",uid:"<<uid<<",channelKey:"<<channelKey<<",channelName:"<<channelName<<",str_appliteDir:"
          <<str_appliteDir<<",channelProfileValue:"<<channelProfileValue<<",decodeAudio:"
          <<decodeAudioValue<<",decodeVideoValue:"<<decodeVideoValue<<endl;
	  config.decodeAudio = static_cast<agora::linuxsdk::AUDIO_FORMAT_TYPE>(decodeAudioValue);
    config.decodeVideo = static_cast<agora::linuxsdk::VIDEO_FORMAT_TYPE>(decodeVideoValue);
		config.streamType = static_cast<agora::linuxsdk::REMOTE_VIDEO_STREAM_TYPE>(streamTypeValue);
    if(!jniRecorder.createChannel(appId, channelKey, channelName, uid, config))
    {
      cerr << "Failed to create agora channel: " << channelName << endl;
      return JNI_FALSE;
    }
    //sava jni_env,the same thread?
 		while (!jniRecorder.stopped() && !g_bSignalStop) {
  		usleep(10000); //10ms
		}
  	if (g_bSignalStop) {
    	jniRecorder.leaveChannel();
      cout << "jni layer stopped!"<<endl;
      stopJavaProc();
    	jniRecorder.release();
  	}
    cout<<"Java_AgoraJavaRecording__recordingConfigcreateChannel end"<<endl;
 
}
#ifdef __cplusplus
}
#endif
