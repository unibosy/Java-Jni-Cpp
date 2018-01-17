#include "IAgoraLinuxSdkCommon.h"
#include "./jni/AgoraJavaRecording.h"
#include "AgoraJniProxy.h"
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include "helper.h"

using std::string;
using std::cout;
using std::cerr;
using std::endl;

using agora::base::log;
using namespace jniproxy;

atomic_bool_t g_bSignalStop;

void signal_handler(int signo) {
  (void)signo;
  cerr << "Signal " << signo<<endl;
  g_bSignalStop = true;
}

#ifdef __cplusplus
extern "C" {
#endif

JavaVM* g_jvm = NULL;//one eventhandler vs one?
//define signatures
#define LONG_SIGNATURE "J"
#define INT_SIGNATURE "I"
#define BOOL_SIGNATURE "Z"
#define DOUBLE_SIGNATURE "D"
#define LONG_PARA_VOID_RETURN "(J)V" 
#define EMPTY_PARA_INT_RETURN "()I"
#define STRING_SIGNATURE "Ljava/lang/String;"
#define BYTEARRAY "Ljava/nio/ByteBuffer;"
#define CHANNEL_PROFILE_SIGNATURE "Lio/agora/recording/common/Common$CHANNEL_PROFILE_TYPE;"
#define REMOTE_VIDEO_STREAM_SIGNATURE "Lio/agora/recording/common/Common$REMOTE_VIDEO_STREAM_TYPE;"

//#define VIDEOFRAME_SIGNATURE "io/agora/recording/common/Common$VideoFrame"
#define VIDEOFRAME_H264_SIGNATURE "Lio/agora/recording/common/Common$VideoH264Frame;"
#define VIDEOFRAME_YUV_SIGNATURE "Lio/agora/recording/common/Common$VideoYuvFrame;"
#define VIDEOFRAME_JPG_SIGNATURE "Lio/agora/recording/common/Common$VideoJpgFrame;" 

#define VIDEO_FRAME_TYPE_SIGNATURE "Lio/agora/recording/common/Common$VIDEO_FRAME_TYPE;"

#define AUDIO_FORMAT_TYPE_SIGNATURE "Lio/agora/recording/common/Common$AUDIO_FORMAT_TYPE;"
#define VIDEO_FORMAT_TYPE_SIGNATURE "Lio/agora/recording/common/Common$VIDEO_FORMAT_TYPE;"


#define VIDEOMIXLAYOUT_SIGNATURE "[Lio/agora/recording/common/Common$VideoMixingLayout$Region;"

//jmethodID signature
#define SN_MTD_COMMON_INIT "(Lio/agora/recording/common/Common;)V"
#define SN_MTD_VIDEO_YUV_FRAME "(Lio/agora/recording/common/Common;JIIIII)V"
//class name
//VIDEO
#define CN_REP "io/agora/recording/common/RecordingEngineProperties"
#define CN_VIDEO_FRAME "io/agora/recording/common/Common$VideoFrame"
#define CN_VIDEO_YUV_FRAME "io/agora/recording/common/Common$VideoYuvFrame"
#define CN_VIDEO_JPG_FRAME "io/agora/recording/common/Common$VideoJpgFrame"
#define CN_VIDEO_H264_FRAME "io/agora/recording/common/Common$VideoH264Frame"
//AUDIO
#define CN_AUDIO_FRAME "io/agora/recording/common/Common$AudioFrame"
#define CN_AUDIO_AAC_FRAME "io/agora/recording/common/Common$AudioAacFrame"
#define CN_AUDIO_PCM_FRAME "io/agora/recording/common/Common$AudioPcmFrame"

//jmethodID
#define INIT_SIGNATURE "<init>"
#define VIDEO_FRAME_SIGNATURE "(Lio/agora/recording/common/Common;)V"

//java callback function signature
#define SN_CB_FUNC_RECEIVE_VIDEOFRAME "(JLio/agora/recording/common/Common$VideoFrame;)V"

//declare java callback function
#define CB_FUNC_RECEIVE_VIDEOFRAME "videoFrameReceived"

//jclass
//audio
static jclass JC_AUDIO_FRAME_TYPE = NULL;
static jclass JC_AUDIO_AAC_FRAME = NULL;
static jclass JC_AUDIO_PCM_FRAME = NULL;

//video
static jclass JC_VIDEO_FRAME = NULL;
static jclass JC_VIDEO_YUV_FRAME = NULL;
static jclass JC_VIDEO_H264_FRAME = NULL;
static jclass JC_VIDEO_JPG_FRAME = NULL;

static jclass JC_RECORDING_ENGINE_PROPERTIES = NULL;

AgoraJniProxySdk::AgoraJniProxySdk():AgoraSdk(){
  LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk constructor");
  mJavaAgoraJavaRecordingClass = NULL;
  mJavaAgoraJavaRecordingClass = NULL;
  mJavaVideoFrameClass = NULL;
  mJavaVideoFrameObject = NULL;
  mJavaVideoYuvFrameClass = NULL;
  mJavaVideoYuvFrameObject = NULL;
  mJavaVideoJPGFrameClass = NULL;
  mJavaVideoJPGFrameObject = NULL;
  mJavaVideoH264FrameClass = NULL;
  mJavaVideoH264FrameObject = NULL;
  //audio
  mJavaAudioFrameClass = NULL;
  mJavaAudioFrameObject = NULL;
  mJavaAudioAacFrameClass = NULL;
  mJavaAudioAacFrameObject = NULL;
  mJavaAudioPcmFrameClass =NULL;
  mJavaAudioPcmFrameObject =NULL;
  //AgoraJavaRecording callback function jmethodIDs
  mJavaRecvVideoMtd = NULL;

  initJavaObjects(true);
  cacheJavaCBFuncMethodIDs(CN_VIDEO_FRAME);
}
AgoraJniProxySdk::~AgoraJniProxySdk(){
  cout<<"AgoraJniProxySdk destructor begin"<<endl;
  initJavaObjects(false);
  cout<<"AgoraJniProxySdk destructor end"<<endl;
}


#define CHECK_PTR_RETURN(PTR) \
  {            \
    if(!PTR)   \
      return;  \
  }

#define CHECK_PTR_RETURN_BOOL(PTR) \
  {            \
    if(!PTR)   \
      return false;  \
  }

#define CHECK_EXCEPTION(jni, message)                              \
  if (jni->ExceptionCheck()) {                                     \
    jni->ExceptionDescribe();                                      \
    jni->ExceptionClear();                                         \
  }

/*#if  0
#define CHECK_EXCEPTION(jni, message)                              \
  if (jni->ExceptionCheck()) {                                     \
    log(LOG_ERROR, "Check failed in: %s(%d)", __FILE__, __LINE__); \
    log(LOG_ERROR, "%s", message);                                 \
    jni->ExceptionDescribe();                                      \
    jni->ExceptionClear();                                         \
  }                                                                \

#define JNI_LOG(MSG,LEVEL) \
  LOG()

#endif
*/
class AttachThreadScoped
{
public:
  explicit AttachThreadScoped(JavaVM* jvm)
    : attached_(false), jvm_(jvm), env_(NULL) {
    jint ret_val = jvm->GetEnv(reinterpret_cast<void**>(&env_),JNI_VERSION_1_4);
    if (ret_val == JNI_EDETACHED) {
      // Attach the thread to the Java VM.
      ret_val = jvm_->AttachCurrentThread((void**)&env_, NULL);
      attached_ = ret_val >= 0;
      assert(attached_);
    }
  }
  /*~AttachThreadScoped() {
    if (attached_ && (jvm_->DetachCurrentThread() < 0)) {
      assert(false);
    }
  }*/
  void detach(){
    if (!attached_ && jvm_->DetachCurrentThread() < 0) {
      assert(false);
    }
  }
  JNIEnv* env() { return env_; }
private:
  bool attached_;
  JavaVM* jvm_;
  JNIEnv* env_;
};

jclass AgoraJniProxySdk::newGlobalJClass(JNIEnv* env, const char* className){
  jclass localRef = env->FindClass(className);
  if(!localRef) {
    LOG_DIR(m_logdir.c_str(), ERROR,"newGlobalJClass cannot find class:%s",className);
    return NULL;
  }
  jclass globalJc = static_cast<jclass>(env->NewGlobalRef(localRef));
  if(!globalJc){
    LOG_DIR(m_logdir.c_str(), ERROR,"newGlobalJClass cound not create global reference!",className);
    return NULL;
  }
  env->DeleteLocalRef(localRef);
  return globalJc;
}
jobject AgoraJniProxySdk::newGlobalJObject(JNIEnv* env, jclass jc, const char* signature){
  jmethodID initMid = env->GetMethodID(jc, INIT_SIGNATURE, signature);
  if(!initMid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"newGlobalJObject cannot get init method for this signature:%s", signature);
    return NULL;
  }
  jobject globalJob = env->NewGlobalRef(env->NewObject(mJavaVideoFrameClass, initMid));
  if(!globalJob){
    LOG_DIR(m_logdir.c_str(), ERROR,"newGlobalJObject new global reference failed ");
    return NULL;
  }
  return globalJob;
}
void AgoraJniProxySdk::cacheJavaCBFuncMethodIDs(const char* className){
  AttachThreadScoped ats(g_jvm);
  JNIEnv* env = ats.env();
  if (!env) return;
  jclass localRef = env->FindClass(className);
  if(!localRef) {
    LOG_DIR(m_logdir.c_str(), ERROR,"newGlobalJClass cannot find class:%s", className);
    return ;
  }
  mJavaRecvVideoMtd = Helper::safeGetMethodID(env, localRef, CB_FUNC_RECEIVE_VIDEOFRAME, SN_CB_FUNC_RECEIVE_VIDEOFRAME);
  if(!mJavaRecvVideoMtd) {
     LOG_DIR(m_logdir.c_str(), ERROR, "get receive video frame jmethodid failed ");
     return;
  }
}
void AgoraJniProxySdk::initJavaObjects(bool init){
  AttachThreadScoped ats(g_jvm);
  JNIEnv* env = ats.env();
  if (!env) return;

  mJavaVideoFrameClass = newGlobalJClass(env, CN_VIDEO_FRAME);
  mJavaVideoFrameObject = newGlobalJObject(env, mJavaVideoFrameClass, SN_MTD_COMMON_INIT);
  mJavaVideoYuvFrameClass = newGlobalJClass(env, CN_VIDEO_YUV_FRAME);
  mJavaVideoYuvFrameObject = newGlobalJObject(env, mJavaVideoYuvFrameClass, SN_MTD_VIDEO_YUV_FRAME);
  
  mJavaVideoJPGFrameClass = newGlobalJClass(env, CN_VIDEO_JPG_FRAME);
  CHECK_EXCEPTION(env,"");  
  mJavaVideoJPGFrameObject = newGlobalJObject(env, mJavaVideoJPGFrameClass, SN_MTD_COMMON_INIT);
  
  mJavaVideoH264FrameClass = newGlobalJClass(env, CN_VIDEO_H264_FRAME);
  mJavaVideoH264FrameObject = newGlobalJObject(env, mJavaVideoH264FrameClass, SN_MTD_COMMON_INIT);
}

//do common!

bool AgoraJniProxySdk::fillAacAllFields(JNIEnv* env, jobject& job, jclass& jc, const agora::linuxsdk::AudioFrame*& frame) const {
  CHECK_PTR_RETURN_BOOL(mJavaAgoraJavaRecordingClass);
  if(frame->type != agora::linuxsdk::AUDIO_FRAME_AAC) return false;
  jfieldID fid = NULL;
  jobject jbArr = NULL;

  agora::linuxsdk::AudioAacFrame *f = frame->frame.aac;
  //frame_ms_
  fid = env->GetFieldID(jc, "frame_ms_", LONG_SIGNATURE);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get frame_ms_ value of audioAacFrame");
    return false;
  }
  long frame_ms_ = f->frame_ms_;
  env->SetLongField(job, fid, jlong(frame_ms_));
  //aacBuf_
  fid = env->GetFieldID(jc, "aacBuf_", BYTEARRAY);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get frame_ms_ value of audioAacFrame");
    return false;
  }
  const unsigned char* aacBuf_ = f->aacBuf_;
  long aacBufSize_ = f->aacBufSize_;
  jbArr = env->NewDirectByteBuffer((void*)aacBuf_, aacBufSize_); 
  env->SetObjectField(job, fid, jbArr);
  //aacBufSize_
  fid = env->GetFieldID(jc, "aacBufSize_", LONG_SIGNATURE);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO, "cannot get aacBufSize_ value of audioPcmFrame");
    return false;
  }
  env->SetLongField(job, fid, jlong(aacBufSize_));
  
  env->DeleteLocalRef(jc);
  env->DeleteLocalRef(jbArr);

  return true;
}

bool AgoraJniProxySdk::fillPcmAllFields(JNIEnv* env, jobject& job, jclass& jc, const agora::linuxsdk::AudioFrame*& frame) const {
  CHECK_PTR_RETURN_BOOL(mJavaAgoraJavaRecordingClass);
  if(frame->type != agora::linuxsdk::AUDIO_FRAME_RAW_PCM) return false;
  jfieldID fid = NULL;
  jobject jbArr = NULL;

  agora::linuxsdk::AudioPcmFrame *f = frame->frame.pcm;
  //frame_ms_
  fid = env->GetFieldID(jc, "frame_ms_", LONG_SIGNATURE);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get frame_ms_ value of audioPcmFrame");
    return false;
  }
  long frame_ms_ = f->frame_ms_;
  env->SetLongField(job, fid, jlong(frame_ms_));
  //channels_
  fid = env->GetFieldID(jc, "channels_", LONG_SIGNATURE);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get channels_ value of audioPcmFrame");
    return false;
  }
  long channels_ = f->channels_;
  env->SetLongField(job, fid, jlong(channels_));
  //sample_bits_
  fid = env->GetFieldID(jc, "sample_bits_", LONG_SIGNATURE);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get sample_bits_ value of audioPcmFrame");
    return false;
  }
  long sample_bits_ = f->sample_bits_;
  env->SetLongField(job, fid, jlong(sample_bits_));
  //sample_rates_
  fid = env->GetFieldID(jc, "sample_rates_", LONG_SIGNATURE);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get sample_rates_value of audioPcmFrame");
    return false;
  }
  long sample_rates_ = f->sample_rates_;
  env->SetLongField(job, fid, jlong(sample_rates_));
  //pcmBuf_
  fid = env->GetFieldID(jc, "pcmBuf_", BYTEARRAY);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get frame_ms_ value of audioPcmFrame");
    return false;
  }
  long pcmBufSize_ = f->pcmBufSize_;

  jbArr = env->NewDirectByteBuffer((void*)f->pcmBuf_, pcmBufSize_);
  env->SetObjectField(job, fid, jbArr);
  //pcmBufSize_
  fid = env->GetFieldID(jc, "pcmBufSize_", LONG_SIGNATURE);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get pcmBufSize_ value of audioPcmFrame");
    return false;
  }
  env->SetLongField(job, fid, jlong(pcmBufSize_));
  
  env->DeleteLocalRef(jc);

  return true;
}

bool AgoraJniProxySdk::fillJAudioFrameByFields(JNIEnv* env, const agora::linuxsdk::AudioFrame*& frame, jclass& jcAudioFrame, jobject& jobAudioFrame) const {
  //1.find main class
  if (frame->type == agora::linuxsdk::AUDIO_FRAME_RAW_PCM) {
    //call one function
    if(!fillAudioPcmFrame(env, frame, jcAudioFrame,jobAudioFrame))
    {
      LOG_DIR(m_logdir.c_str(), INFO,"Warning: fillAudioPcmFrame failed!!!!!");
      return false;
    }
  }else if (frame->type == agora::linuxsdk::AUDIO_FRAME_AAC) {
    //do things here
    if(!fillAudioAacFrame(env, frame, jcAudioFrame,jobAudioFrame))
    {
      LOG_DIR(m_logdir.c_str(), INFO,"Warning: fillAudioAacFrame failed!!!!!");
      return false;
    }

  }
  return true;
}

bool AgoraJniProxySdk::fillAudioAacFrame(JNIEnv* env, const agora::linuxsdk::AudioFrame*& frame, \
            jclass& jcAudioFrame, jobject& jobAudioFrame) const  {
  CHECK_PTR_RETURN_BOOL(mJavaAgoraJavaRecordingClass);
  if(frame->type != agora::linuxsdk::AUDIO_FRAME_AAC) return false;
  jmethodID jmid = NULL;
  jclass jc = NULL;
  jfieldID fid = NULL;
  jmethodID initMid = NULL;
  jobject job = NULL;

  //get pcm frame by find class
  jc = env->FindClass("Lio/agora/recording/common/Common$AudioAacFrame;");
  if(!jc) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get AudioAccFrame jclass");
    return false;
  }
  //new AudioAacFrame
  //TODO  place these to one function
  initMid = env->GetMethodID(jc,INIT_SIGNATURE,"(Lio/agora/recording/common/Common;J)V");
  if(!initMid) {
    LOG_DIR(m_logdir.c_str(), INFO,"get AudioAacFrame init methid failed!");
    return false;
  }
  job = env->NewObject(jc, initMid);
  if(!job){
    LOG_DIR(m_logdir.c_str(), INFO,"new AudioAacFrame failed! no memory?");
    return false;
  }
  fid = env->GetFieldID(jcAudioFrame, "aac", "Lio/agora/recording/common/Common$AudioAacFrame;");
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get aac AUDIO_FRAME_TYPE field");
    return false;
  }
  //fill all fields of AudioAacFrame jobject
  if(!fillAacAllFields(env, job, jc, frame)){
    LOG_DIR(m_logdir.c_str(), ERROR,"fillAacAllFields failed!");
    return false;
  }
  //Fill in the jobAdudioFrame
  env->SetObjectField(jobAudioFrame, fid, job);
  env->DeleteLocalRef(jc);
  env->DeleteLocalRef(job);

  return true;
}
bool AgoraJniProxySdk::fillAudioPcmFrame(JNIEnv* env, const agora::linuxsdk::AudioFrame*& frame, \
            jclass& jcAudioFrame, jobject& jobAudioFrame) const  {
  CHECK_PTR_RETURN_BOOL(mJavaAgoraJavaRecordingClass);
  if(frame->type != agora::linuxsdk::AUDIO_FRAME_RAW_PCM) return false;
  jclass jc = NULL;
  jfieldID fid = NULL;
  jmethodID initMid = NULL;
  jobject job = NULL;
  //get pcm frame by find class
  jc = env->FindClass("Lio/agora/recording/common/Common$AudioPcmFrame;");
  if(jc == NULL) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get AudioPcmFrame jclass");
    return false;
  }
  //new AudioPcmFrame
  //TODO  place these to one function
  initMid = env->GetMethodID(jc,INIT_SIGNATURE,"(Lio/agora/recording/common/Common;JJJ)V");
  if(initMid == NULL) {
    LOG_DIR(m_logdir.c_str(), ERROR,"get AudioPcmFrame init methid failed!");
    return false;
  }
  job = env->NewObject(jc, initMid);
  if(job == NULL){
    LOG_DIR(m_logdir.c_str(), ERROR,"new AudioPcmFrame failed! no memory?");
    return false;
  }
  fid = env->GetFieldID(jcAudioFrame, "pcm", "Lio/agora/recording/common/Common$AudioPcmFrame;");
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get pcm AUDIO_FRAME_TYPE field");
    return false;
  }
  //fill all fields of AudioPcmFrame jobject
  if(!fillPcmAllFields(env, job, jc, frame)){
    LOG_DIR(m_logdir.c_str(), INFO,"fillPcmAllFields failed!");
    return false;
  }
  //Fill in the jobAdudioFrame
  env->SetObjectField(jobAudioFrame, fid, job);
  env->DeleteLocalRef(jc);
  env->DeleteLocalRef(job);
  return true;
}
bool AgoraJniProxySdk::fillJVideoOfYUV(JNIEnv* env, const agora::linuxsdk::VideoFrame*& frame, jclass& jcVideoFrame, jobject& jobVideoFrame) const {
  CHECK_PTR_RETURN_BOOL(mJavaAgoraJavaRecordingClass);
  if(frame->type != agora::linuxsdk::VIDEO_FRAME_RAW_YUV) return false;
  if(!env || !frame) return false;
  jclass jc = NULL;
  jmethodID initMid = NULL;
  jobject job = NULL;
  jfieldID fid = NULL;
  jobject jbArr = NULL;
  //1.get subclass
  jc = env->FindClass("Lio/agora/recording/common/Common$VideoYuvFrame;");
  if(!jc) {
    LOG_DIR(m_logdir.c_str(), ERROR,"fillJVideoOfYUV cannot find subclass");
    return false;
  }
  //2.get subclass init method
  initMid = env->GetMethodID(jc,INIT_SIGNATURE,"(Lio/agora/recording/common/Common;JIIIII)V");
  if(!initMid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get init methodid");
    return false;
  }

  //3.new VideoXXXXXFrame object
  job = env->NewObject(jc, initMid);
  if(!job){
    LOG_DIR(m_logdir.c_str(), ERROR,"new subclass  failed! no memory?");
    return false;
  }
  agora::linuxsdk::VideoYuvFrame *f = frame->frame.yuv;
  if(!f) {
    LOG_DIR(m_logdir.c_str(), ERROR,"yuv frame is nullptr");
    return false;
  }
  //4.fill all fields
  //4.1 get & set of this subclass object
  //frame_ms_
  fid = env->GetFieldID(jc, "frame_ms_", LONG_SIGNATURE);
  if(!fid) {
    //TODO add field ID
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get field,field ID::%u",fid);
    return false;
  }
  long frame_ms_ = f->frame_ms_;
  env->SetLongField(job, fid, jlong(frame_ms_));
  //buf_
  fid = env->GetFieldID(jc, "buf_", BYTEARRAY);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get field,field ID::%u",fid);
    return false;
  }
  long bufSize_ = f->bufSize_;
  //jbArr  = env->NewDirectByteBuffer((void*)(char*)((const_cast<unsigned char*>(f->buf_))), bufSize_);
  jbArr  = env->NewDirectByteBuffer((void*)(f->buf_), bufSize_);
  env->SetObjectField(job, fid, jbArr);
  //bufSize_
  fid = env->GetFieldID(jc, "bufSize_", LONG_SIGNATURE);
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get field,field ID::%u",fid);
    return false;
  }
  env->SetLongField(job, fid, jlong(bufSize_));

  //5.get subclass field
  fid = env->GetFieldID(jcVideoFrame, "yuv", VIDEOFRAME_YUV_SIGNATURE);
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get VIDEOFRAME_YUV_SIGNATURE");
    return false;
  }
  //6.fill jobVideFrame
  env->SetObjectField(jobVideoFrame, fid, job);
   
  env->DeleteLocalRef(job);
  env->DeleteLocalRef(jc);
  env->DeleteLocalRef(jbArr);
  return  true;
}
bool AgoraJniProxySdk::fillJVideoOfJPG(JNIEnv* env, const agora::linuxsdk::VideoFrame*& frame, jclass& jcVideoFrame, jobject& jobVideoFrame) const {
  CHECK_PTR_RETURN_BOOL(mJavaAgoraJavaRecordingClass);
  LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk::fillJVideoOfJPG enter" );
  if(frame->type != agora::linuxsdk::VIDEO_FRAME_JPG) return false;
  if(!env || !frame) return false;
  jclass jc = NULL;
  jmethodID initMid = NULL;
  jobject job = NULL;
  jfieldID fid = NULL;
  int fieldId = 0; //TODO
  jobject jbArr = NULL;

  //1.get subclass
  jc = env->FindClass("Lio/agora/recording/common/Common$VideoJpgFrame;");
  if(!jc) {
    LOG_DIR(m_logdir.c_str(), ERROR,"fillJVideoOfJPG cannot find subclass");
    return false;
  }
  //2.get subclass init method
  initMid = env->GetMethodID(jc,INIT_SIGNATURE,"(Lio/agora/recording/common/Common;)V");
  if(!initMid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get init methodid");
    return false;
  }
  //3.new VideoXXXXXFrame object
  job = env->NewObject(jc, initMid);
  if(!job){
    LOG_DIR(m_logdir.c_str(), ERROR,"new subclass  failed! no memory?");
    return false;
  }
  agora::linuxsdk::VideoJpgFrame *f = frame->frame.jpg;
  if(!f) return false;
  //4.fill all fields
  //4.1 get & set of this subclass object
  //frame_ms_
  fid = env->GetFieldID(jc, "frame_ms_", LONG_SIGNATURE);
  if(!fid) {
    //TODO add field ID
    LOG_DIR(m_logdir.c_str(), ERROR,"fillJVideoOfJPG cannot get field,field ID:");
    return false;
  }
  long frame_ms_ = f->frame_ms_;
  env->SetLongField(job, fid, jlong(frame_ms_));
  //buf_
  fid = env->GetFieldID(jc, "buf_", BYTEARRAY);
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get field,field ID::%u",fid);
    return false;
  }
  const unsigned char* buf_ = f->buf_;
  long bufSize_ = f->bufSize_;

  jbArr = env->NewDirectByteBuffer((void*)f->buf_, bufSize_);

  env->SetObjectField(job, fid, jbArr);
  //bufSize_
  fid = env->GetFieldID(jc, "bufSize_", LONG_SIGNATURE);
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get field,field ID::%u",fid);
    return false;
  }
  env->SetLongField(job, fid, jlong(bufSize_));

  //5.get subclass field
  fid = env->GetFieldID(jcVideoFrame, "jpg", VIDEOFRAME_JPG_SIGNATURE);
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get VIDEOFRAME_JPG_SIGNATURE");
    return false;
  }
  //6.fill jobVideFrame
  env->SetObjectField(jobVideoFrame, fid, job);
  
  env->DeleteLocalRef(jc);
  env->DeleteLocalRef(job);
  env->DeleteLocalRef(jbArr);
  return  true;
}
bool AgoraJniProxySdk::fillJVideoOfH264(JNIEnv* env, const agora::linuxsdk::VideoFrame*& frame, jclass& jcVideoFrame, jobject& jobVideoFrame) const{
  CHECK_PTR_RETURN_BOOL(mJavaAgoraJavaRecordingClass);
  if(frame->type == agora::linuxsdk::VIDEO_FRAME_JPG || frame->type == agora::linuxsdk::VIDEO_FRAME_RAW_YUV) return false;
  if(!env || !frame) return false;
  jclass jc = NULL;
  jmethodID initMid = NULL;
  jobject job = NULL;
  jfieldID fid = NULL;
  int fieldId = 0; //TODO
  jobject jbArr = NULL;
  //1.get subclass
  jc = env->FindClass("Lio/agora/recording/common/Common$VideoH264Frame;");
  if(!jc) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot find subclass, type:%d",static_cast<int>(frame->type));
    return false;
  }
  //2.get subclass init method
  initMid = env->GetMethodID(jc,INIT_SIGNATURE,"(Lio/agora/recording/common/Common;)V");
  if(!initMid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get init methodid");
    return false;
  }
  //3.new VideoH264Frame object
  job = env->NewObject(jc, initMid);
  if(!job){
    LOG_DIR(m_logdir.c_str(), ERROR,"new subclass  failed! no memory?");
    return false;
  }
  agora::linuxsdk::VideoH264Frame *f = frame->frame.h264;
  if(!f) return false;
  //4.fill all fields
  //4.1 get & set of this subclass object
  //frame_ms_
  fid = env->GetFieldID(jc, "frame_ms_", LONG_SIGNATURE);
  if(!fid) {
    //TODO add field ID
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get field,field ID::%d",fid);
    return false;
  }
  long frame_ms_ = f->frame_ms_;
  env->SetLongField(job, fid, jlong(frame_ms_));
  //frame_num_
  fid = env->GetFieldID(jc, "frame_num_", LONG_SIGNATURE);
  if(!fid) {
    //TODO add field ID
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get field,field ID:%d",fid);
    return false;
  }
  long frame_num_ = f->frame_num_;
  env->SetLongField(job, fid, jlong(frame_num_));

  //buf_
  fid = env->GetFieldID(jc, "buf_", BYTEARRAY);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get field,field ID:%d",fieldId);
    return false;
  }
  const unsigned char* buf_ = f->buf_;
  long bufSize_ = f->bufSize_;
  
  jbArr = env->NewDirectByteBuffer((void*)f->buf_, bufSize_);

  env->SetObjectField(job, fid, jbArr);
  //bufSize_
  fid = env->GetFieldID(jc, "bufSize_", LONG_SIGNATURE);
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get field,field ID:%s",fieldId);
    return false;
  }
  env->SetLongField(job, fid, jlong(bufSize_));

  //5.get subclass field
  fid = env->GetFieldID(jcVideoFrame, "h264", VIDEOFRAME_H264_SIGNATURE);
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get VIDEOFRAME_H264_SIGNATURE");
    return false;
  }
  //6.fill jobVideFrame
  env->SetObjectField(jobVideoFrame, fid, job);
  
  env->DeleteLocalRef(jc);
  env->DeleteLocalRef(job);
  env->DeleteLocalRef(jbArr);
  return  true;
}
bool AgoraJniProxySdk::fillJVideoFrameByFields(JNIEnv* env, const agora::linuxsdk::VideoFrame*& frame, jclass jcVideoFrame, jobject jobVideoFrame) const {
  CHECK_PTR_RETURN_BOOL(mJavaAgoraJavaRecordingClass);
  bool ret = false;
  jclass jc = NULL;
  jmethodID initMid = NULL;
  jfieldID fid = NULL;
  jobject job = NULL;

  if(!env || !frame || !jcVideoFrame || !jobVideoFrame){
    LOG_DIR(m_logdir.c_str(), ERROR,"AgoraJniProxySdk::fillJVideoFrameByFields para error!");
    return ret;
  }

  if (frame->type == agora::linuxsdk::VIDEO_FRAME_RAW_YUV) {
    //3.1
    if(!fillJVideoOfYUV(env, frame, jcVideoFrame, jobVideoFrame)){
      LOG_DIR(m_logdir.c_str(), INFO,"fill subclass falied!");
      return false;
    }
  }else if(frame->type == agora::linuxsdk::VIDEO_FRAME_JPG){
    //3.2
    if(!fillJVideoOfJPG(env, frame, jcVideoFrame, jobVideoFrame)) {
      LOG_DIR(m_logdir.c_str(), INFO,"fill subclass falied!");
      return false;
    }
  }else{
    //3.3
    if(!fillJVideoOfH264(env, frame, jcVideoFrame, jobVideoFrame))
    {
      LOG_DIR(m_logdir.c_str(), INFO,"fillJVideoOfH264 failed!");
      return false;
    }
  }

  //fill VIDEO_FRAME_TYPE
  //set type of VIDEO_FRAME_TYPE by FindClass
  jc = env->FindClass("Lio/agora/recording/common/Common$VIDEO_FRAME_TYPE;");
  if(!jc) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get VIDEO_FRAME_TYPE class");
    return false;
  }
  //4.1.1
  initMid = env->GetMethodID(jc,INIT_SIGNATURE,"(Lio/agora/recording/common/Common;)V");
  if(!initMid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"get init methid failed!");
    return false;
  }
  //4.1.2
  job = env->NewObject(jc, initMid);
  //4.1.3 get field of this class
  fid = env->GetFieldID(jc, "type", "I");
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get value of VIDEO_FRAME_TYPE class");
    env->DeleteLocalRef(job);
    return false;
  }
  //4.1.4 fill this field
  int iVideoFrameType = static_cast<int>(frame->type);
  env->SetIntField(job, fid, jint(iVideoFrameType));
  //4.2
  //set this object into jobAudioFrame!
  //step 1:get this object field
  fid = env->GetFieldID(jcVideoFrame, "type", VIDEO_FRAME_TYPE_SIGNATURE);
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get VIDEO_FORMAT_TYPE field");
    env->DeleteLocalRef(job);
    return false;
  }
  //4.3
  env->SetObjectField(jobVideoFrame, fid, job);
  
  env->DeleteLocalRef(job);
  env->DeleteLocalRef(jc);
  
  return true;
}
void AgoraJniProxySdk::videoFrameReceived(unsigned int uid, const agora::linuxsdk::VideoFrame *frame) const {
  CHECK_PTR_RETURN(mJavaAgoraJavaRecordingClass);
  jmethodID jmid = NULL;

  AttachThreadScoped ats(g_jvm);
  JNIEnv* env = ats.env();
  if (!env) return;
  //3.get subclass or enumm details and fill these' fields
  if(!fillJVideoFrameByFields(env, frame, mJavaVideoFrameClass, mJavaVideoFrameObject))
  {
    LOG_DIR(m_logdir.c_str(), ERROR,"jni fillJVideoFrameByFields failed!" );
    return;
  }
  //4.find class where callback function in
  //5. find callback function
    //so far so well
  jmethodID mJavaRecvVideoMtd2 = env->GetMethodID(mJavaAgoraJavaRecordingClass, CB_FUNC_RECEIVE_VIDEOFRAME, SN_CB_FUNC_RECEIVE_VIDEOFRAME);
  env->CallVoidMethod(mJavaAgoraJavaRecordingObject, mJavaRecvVideoMtd2, jlong(long(uid)), mJavaVideoFrameObject);
  return;
}
//TODO  use the same parameter
void AgoraJniProxySdk::audioFrameReceived(unsigned int uid, const agora::linuxsdk::AudioFrame *frame) const {
  CHECK_PTR_RETURN(mJavaAgoraJavaRecordingClass);
  jfieldID fid;
  jmethodID mid;
  jclass jc = NULL;
  jmethodID initMid  = NULL;
  jobject job = NULL;
  int iAudioFrameType = 0; //default pcm?
  jclass jcAudioFrame = NULL;
  
  AttachThreadScoped ats(g_jvm);
  JNIEnv* env = ats.env();
  if (!env) return;


  jcAudioFrame = (env)->FindClass("io/agora/recording/common/Common$AudioFrame");
  if(!jcAudioFrame){
    LOG_DIR(m_logdir.c_str(), ERROR,"not find audioFrame");
    return ;
  }
  iAudioFrameType = static_cast<int>(frame->type);
  //2.get main class init methodid
  initMid = env->GetMethodID(jcAudioFrame,INIT_SIGNATURE,"(Lio/agora/recording/common/Common;)V");
  if(!initMid){
    LOG_DIR(m_logdir.c_str(), ERROR,"not find AudioFrameOfPcm initMid!");
    return;
  }
  //3.new main class object
  jobject jobAudioFrame = env->NewObject(jcAudioFrame, initMid);
  if(!jobAudioFrame){
    LOG_DIR(m_logdir.c_str(), ERROR,"new audio frame object failed!");
    return;
  }
  if(!fillJAudioFrameByFields(env, frame, jcAudioFrame,jobAudioFrame)) {
    LOG_DIR(m_logdir.c_str(), ERROR,"fillJAudioFrameByFields failed!" );
    return;
  }
  
  //4.get main class field
  //4.1.find subclass in main class
  //set type of AUDIO_FRAME_TYPE by FindClass
  jc = env->FindClass("Lio/agora/recording/common/Common$AUDIO_FRAME_TYPE;");
  if(!jc) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get AUDIO_FRAME_TYPE class");
    return;
  }
  //4.1.1
  initMid = env->GetMethodID(jc,INIT_SIGNATURE,"(Lio/agora/recording/common/Common;)V");
  if(!initMid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"get init methid failed!");
    return;
  }
  //4.1.2
  job = env->NewObject(jc, initMid);
  //4.1.3 get field of this class
  fid = env->GetFieldID(jc, "type", "I");
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get value of AUDIO_FRAME_TYPE class");
    return;
  }
  //4.1.4 fill this field
  env->SetIntField(job, fid, jint(iAudioFrameType));
  //4.2
  //set this object into jobAudioFrame!
  //step 1:get this object field
  fid = env->GetFieldID(jcAudioFrame, "type", "Lio/agora/recording/common/Common$AUDIO_FRAME_TYPE;");
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get AUDIO_FRAME_TYPE field");
    return;
  }
  //4.3
  env->SetObjectField(jobAudioFrame, fid, job);
 //6.find callback method in Java ---wrong, should use thisObj
  mid = env->GetMethodID(mJavaAgoraJavaRecordingClass, "audioFrameReceived", "(JLio/agora/recording/common/Common$AudioFrame;)V");
  if(!mid){
    LOG_DIR(m_logdir.c_str(), ERROR,"audioFrameReceived get method failed!");
    return ;
  }
  //7. callback java method
  env->CallVoidMethod(mJavaAgoraJavaRecordingObject, mid,jlong(long(uid)), jobAudioFrame);
  env->DeleteLocalRef(job);
  env->DeleteLocalRef(jc); 
  env->DeleteLocalRef(jobAudioFrame);
  return;
}

void AgoraJniProxySdk::onUserJoined(agora::linuxsdk::uid_t uid, agora::linuxsdk::UserJoinInfos &infos) {
  LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk User:%u" ,uid , " joined, RecordingDir:%s" , (infos.storageDir? infos.storageDir:"NULL") );
  CHECK_PTR_RETURN(mJavaAgoraJavaRecordingClass);
  std::string store_dir = std::string(infos.storageDir);
  m_logdir = store_dir;
  
  AttachThreadScoped ats(g_jvm);
  JNIEnv* env = ats.env();
  if (!env) return;

  jmethodID jUserJoinedMid =  env->GetMethodID(mJavaAgoraJavaRecordingClass,"onUserJoined","(JLjava/lang/String;)V");
  assert(jUserJoinedMid);
  if(!jUserJoinedMid){
    LOG_DIR(m_logdir.c_str(), ERROR,"jUserJoinedMid is null");
    return;
  }
  jstring jstrRecordingDir = env->NewStringUTF(store_dir.c_str());
  env->CallVoidMethod(mJavaAgoraJavaRecordingObject, jUserJoinedMid, jlong((long)(uid)),jstrRecordingDir);
  return;
}
void AgoraJniProxySdk::onUserOffline(agora::linuxsdk::uid_t uid, agora::linuxsdk::USER_OFFLINE_REASON_TYPE reason) {
  LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk onUserOffline User:%u",uid, ",reason:%d",static_cast<int>(reason));
  CHECK_PTR_RETURN(mJavaAgoraJavaRecordingClass);
  AttachThreadScoped ats(g_jvm);
  JNIEnv* env = ats.env();
  if (!env) return;
  jmethodID jUserOfflineMid =  env->GetMethodID(mJavaAgoraJavaRecordingClass,"onUserOffline","(JI)V");
  if(!jUserOfflineMid){
    LOG_DIR(m_logdir.c_str(), ERROR,"cnnot find jUserOfflineMid" );
    return;
  }
  env->CallVoidMethod(mJavaAgoraJavaRecordingObject, jUserOfflineMid, jlong((long)(uid)),jint(int(reason)));

  return;
}
void AgoraJniProxySdk::onLeaveChannel(agora::linuxsdk::LEAVE_PATH_CODE code) {
  LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk onLeaveChannel");
  CHECK_PTR_RETURN(mJavaAgoraJavaRecordingClass);

  AttachThreadScoped ats(g_jvm);
  JNIEnv* env = ats.env();
  if (!env) return;

  jmethodID jLeaveMid =  env->GetMethodID(mJavaAgoraJavaRecordingClass,"onLeaveChannel","(I)V");
  env->CallVoidMethod(mJavaAgoraJavaRecordingObject, jLeaveMid, jint((int)(code)));
  return;
}
void AgoraJniProxySdk::onWarning(int warn) {
  CHECK_PTR_RETURN(mJavaAgoraJavaRecordingClass);
  
  AttachThreadScoped ats(g_jvm);
  JNIEnv* env = ats.env();
  if (!env) return;
  jmethodID jOnWarnMid =  env->GetMethodID(mJavaAgoraJavaRecordingClass, "onWarning","(I)V");
  assert(jOnWarnMid);
  env->CallVoidMethod(mJavaAgoraJavaRecordingObject, jOnWarnMid, warn);
  return;
}

void AgoraJniProxySdk::onError(int error, agora::linuxsdk::STAT_CODE_TYPE stat_code) {
  LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk onError");
  CHECK_PTR_RETURN(mJavaAgoraJavaRecordingClass);
  cout<<"on error -1"<<endl;
  AttachThreadScoped ats(g_jvm);
  JNIEnv* env = ats.env();
  if (!env) return;

  cout<<"on error -2"<<endl;
  jmethodID jLeaveMid =  env->GetMethodID(mJavaAgoraJavaRecordingClass,"onError","(II)V");
  if(!jLeaveMid) {
    LOG_DIR(m_logdir.c_str(), INFO,"get method onError failed!");
    return;
  }
  cout<<"on error -3"<<endl;
  env->CallVoidMethod(mJavaAgoraJavaRecordingObject, jLeaveMid, error, jint((int)(stat_code)));

  cout<<"on error -4"<<endl;
  leaveChannel();
  cout<<"on error -2"<<endl;

  ats.detach();
  return;
}
//void AgoraJniProxySdk::setJavaVM(JavaVM* jvm){ 
//  m_jvm = jvm;
//}
JNIEXPORT jint JNI_OnLoad(JavaVM* jvm, void* reserved) {
  //jniproxy::AgoraJniProxySdk::setJavaVM(jvm);
  g_jvm = jvm;
  return JNI_VERSION_1_4;
}

/*
 * Class:     AgoraJavaRecording
 * Method:    leaveChannel
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_AgoraJavaRecording_leaveChannel
  (JNIEnv *, jobject, jlong nativeObjectRef) {
  cout<<"java call leaveChannel"<<endl;
  jniproxy::AgoraJniProxySdk* nativeHandle = reinterpret_cast<jniproxy::AgoraJniProxySdk*>(nativeObjectRef);
  /*if(!nativeHandle){
    return JNI_FALSE;
  }
  nativeHandle->leaveChannel();*/
  g_bSignalStop = true;
  return JNI_TRUE;
}


/*
 * Class:     AgoraJavaRecording
 * Method:    setVideoMixingLayout
 * Signature: (JLio/agora/recording/common/Common/VideoMixingLayout;)I
 */
JNIEXPORT jint JNICALL Java_AgoraJavaRecording_setVideoMixingLayout
  (JNIEnv * env, jobject job, jlong nativeObjectRef, jobject jVideoMixLayout)
{
  jniproxy::AgoraJniProxySdk* nativeHandle = reinterpret_cast<jniproxy::AgoraJniProxySdk*>(nativeObjectRef);
  if(!nativeHandle){
    cout<<"setVideoMixingLayout nativeHandle is null"<<endl;
    return JNI_FALSE;
  }
  //convert jVideoMixLayout to c++ VideoMixLayout
	jclass jcVideoMixingLayout  = env->GetObjectClass(jVideoMixLayout); 
  if(!jcVideoMixingLayout){
    cout<<"jcVideoMixingLayout is NULL";
    return JNI_FALSE;
	}
  jfieldID jCanvasWidthID = env->GetFieldID(jcVideoMixingLayout, "canvasWidth", INT_SIGNATURE);
  jfieldID jCanvasHeightID = env->GetFieldID(jcVideoMixingLayout, "canvasHeight", INT_SIGNATURE);
  jfieldID jBackgroundColorID = env->GetFieldID(jcVideoMixingLayout, "backgroundColor", STRING_SIGNATURE);
  jfieldID jRegionCountID = env->GetFieldID(jcVideoMixingLayout, "regionCount", INT_SIGNATURE);
  jfieldID jRegionsID = env->GetFieldID(jcVideoMixingLayout, "regions", VIDEOMIXLAYOUT_SIGNATURE);
  jfieldID jAppDataID = env->GetFieldID(jcVideoMixingLayout, "appData", STRING_SIGNATURE);
  jfieldID jAppDataLengthID = env->GetFieldID(jcVideoMixingLayout, "appDataLength", INT_SIGNATURE);
  
  if(!jCanvasWidthID || !jCanvasHeightID || !jBackgroundColorID || !jRegionCountID || !jRegionsID){
    cout<<"Java_AgoraJavaRecording_setVideoMixingLayout get fields failed!"<<endl;
    return JNI_FALSE;
  }
  //convert into cpp value
  jint canvasWidth = env->GetIntField(jVideoMixLayout,jCanvasWidthID);
  jint canvasHeight = env->GetIntField(jVideoMixLayout,jCanvasWidthID);
  
  jstring jstrBackgroundColor = (jstring)env->GetObjectField(jVideoMixLayout, jBackgroundColorID);
  const char* c_backgroundColor = env->GetStringUTFChars(jstrBackgroundColor, NULL);
  
  jint regionCount = env->GetIntField(jVideoMixLayout,jRegionCountID);
  
  //jstring jstrAppData = (jstring)env->GetObjectField(jVideoMixLayout,jAppDataID);
  //const char* c_jstrAppData = env->GetStringUTFChars(jstrAppData, NULL);
  //jint appDataLength = env->GetIntField(jVideoMixLayout,jAppDataLengthID);

  agora::linuxsdk::VideoMixingLayout layout;
 
  layout.canvasWidth = int(canvasWidth);
  layout.canvasHeight = int(canvasHeight);
  layout.backgroundColor = c_backgroundColor;
  layout.regionCount = int(regionCount);

  //layout.appData = c_jstrAppData;
  //layout.appDataLength = int(appDataLength);
  //regions begin
  
  if(0<regionCount)
  {
   jobjectArray jobRegions =  (jobjectArray)env->GetObjectField(jVideoMixLayout, jRegionsID);
   if(!jobRegions) {
    cout<<"[ERROR]regionCount is bigger than zero,but cannot find Regions in jVideoMixLayout!"<<endl;
    return JNI_FALSE;
   }
   jint arrLen = env->GetArrayLength(jobRegions);
   if(arrLen != regionCount){
    cout<<"regionCount is not equal with arrLen"<<endl;
    return JNI_FALSE;//return ??
   }  
    agora::linuxsdk::VideoMixingLayout::Region* regionList = new agora::linuxsdk::VideoMixingLayout::Region[arrLen];
    for(int i=0; i<arrLen;++i){
      jobject region = env->GetObjectArrayElement(jobRegions, i);
      jclass jcRegion =env->GetObjectClass(region);
      if(!jcRegion){
        cout<<"cannot get jclass Region!"<<endl;
        break;
      }
      jfieldID jfidUid = env->GetFieldID(jcRegion, "uid", LONG_SIGNATURE);
      if(!jfidUid){
        cout<<"connot get region uid"<<endl;
        continue;
      }
      //uid
      jlong uidValue = env->GetLongField(region,jfidUid);
      //C++ uid is uint32
      regionList[i].uid = static_cast<uint32_t>(uidValue);
      //x
      jfieldID jxID = env->GetFieldID(jcRegion, "x", DOUBLE_SIGNATURE);
      if(!jxID){
        cout<<"connot get region x,uid:"<<uint32_t(uidValue)<<endl;
        continue;
      }
      jdouble jx = env->GetDoubleField(region,jxID);
      regionList[i].x = static_cast<float>(jx);

      //y
      jfieldID jyID = env->GetFieldID(jcRegion, "y", DOUBLE_SIGNATURE);
      if(!jyID){
        cout<<"connot get region y,uid:"<<int(uidValue)<<endl;
        continue;
      }
      jdouble jy = env->GetDoubleField(region,jyID);
      regionList[i].y = static_cast<float>(jy);

      //width
      jfieldID jwidthID = env->GetFieldID(jcRegion, "width", DOUBLE_SIGNATURE);
      if(!jwidthID){
        cout<<"connot get region width, uid:"<<int(uidValue)<<endl;
        continue;
      }
      jdouble jwidth = env->GetDoubleField(region,jwidthID);
      regionList[i].width = static_cast<float>(jwidth);

      //height
      jfieldID jheightID = env->GetFieldID(jcRegion, "height", DOUBLE_SIGNATURE);
      if(!jheightID){
        cout<<"connot get region height, uid:"<<int(uidValue)<<endl;
        continue;
      }
      jdouble jheight = env->GetDoubleField(region,jheightID);
      regionList[i].height = static_cast<float>(jheight);
      cout<<"user id:"<<static_cast<uint32_t>(uidValue)<<",x:"<<static_cast<float>(jx)<<",y:"<<static_cast<float>(jy)<<",width:"<<static_cast<float>(jwidth)<<",height:"<<static_cast<float>(jheight)<<",alpha:"<<static_cast<double>(i + 1)<<endl;

      //zOrder
      regionList[i].zOrder = 0;
      //alpha
      regionList[i].alpha = static_cast<double>(i + 1);
      //renderMode
      regionList[i].renderMode = 0;
    }  
    layout.regions = regionList;
  }
  else
    layout.regions = NULL;
  //regions end
  nativeHandle->setVideoMixingLayout(layout);
  return jint(0);
}

/*                                                                                                                                                                           * Class:     AgoraJavaRecording
 * Method:    getProperties
 * Signature: (J)LRecordingEngineProperties;
 */
JNIEXPORT jobject JNICALL Java_AgoraJavaRecording_getProperties(JNIEnv * env, jobject, jlong nativeObjectRef){
  jniproxy::AgoraJniProxySdk* nativeHandle = reinterpret_cast<jniproxy::AgoraJniProxySdk*>(nativeObjectRef);
  if(!nativeHandle){
    return JNI_FALSE;
  }
  jclass jc = env->FindClass(CN_REP);
  if(!jc){
    cout<<"cannot get jclass RecordingEngineProperties!"<<endl;    
    return JNI_FALSE;
  }
  jmethodID initMid = env->GetMethodID(jc,INIT_SIGNATURE,"(Lio/agora/recording/common/Common;)V");
  if(!initMid){
    cout<<"cannot get RecordingEngineProperties init!"<<endl;
    return JNI_FALSE;
  }
  jobject job = NULL;
  job = env->NewObject(jc, initMid);
  if(!job){
    cout<<"new object failed!"<<endl;
    return JNI_FALSE;
  }
  jfieldID storageFid = env->GetFieldID(jc, "storageDir", "Ljava/lang/String;");
  if(!storageFid){
    cout<<"storageDir fid not found!"<<endl;
    return JNI_FALSE;
  }
  char* storageDir = nativeHandle->getRecorderProperties()->storageDir;
  jstring jstrStorageDir = env->NewStringUTF(storageDir);
  env->SetObjectField(job, storageFid, jstrStorageDir);
  env->DeleteLocalRef(jc);
  return job;
}
/*
 * Class:     AgoraJavaRecording
 * Method:    startService
 * Signature: (J)V
 */
 JNIEXPORT void JNICALL Java_AgoraJavaRecording_startService(JNIEnv * env, jobject job, jlong nativeObjectRef){
   jniproxy::AgoraJniProxySdk* nativeHandle = reinterpret_cast<jniproxy::AgoraJniProxySdk*>(nativeObjectRef);
   if(nativeHandle){
     nativeHandle->startService();
   }
 }
/*
 * Class:     AgoraJavaRecording
 * Method:    stopService
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_AgoraJavaRecording_stopService(JNIEnv * env, jobject job, jlong nativeObjectRef){
  jniproxy::AgoraJniProxySdk* nativeHandle = reinterpret_cast<jniproxy::AgoraJniProxySdk*>(nativeObjectRef);
  if(nativeHandle){
    nativeHandle->stopService();
  }
}


void AgoraJniProxySdk::stopJavaProc(JNIEnv* env) {
  LOG_DIR(m_logdir.c_str(), WARN,"AgoraJniProxySdk stopJavaProc");
  CHECK_PTR_RETURN(mJavaAgoraJavaRecordingClass);
  jmethodID jStopCB =  env->GetMethodID(mJavaAgoraJavaRecordingClass,"stopCallBack","()V");
  assert(jStopCB);
  env->CallVoidMethod(mJavaAgoraJavaRecordingObject, jStopCB);
}

void AgoraJniProxySdk::setJavaRecordingPath(JNIEnv* env, std::string& storeDir){
  CHECK_PTR_RETURN(mJavaAgoraJavaRecordingClass);
  
  jmethodID jRecordingPathCB =  env->GetMethodID(mJavaAgoraJavaRecordingClass,"recordingPathCallBack","(Ljava/lang/String;)V");
  jstring jstrRecordingDir = env->NewStringUTF(storeDir.c_str());

  env->CallVoidMethod(mJavaAgoraJavaRecordingObject, jRecordingPathCB, jstrRecordingDir);
}
/*void AgoraJniProxySdk::setJavaObjects(bool init, jobject job){
    
}*/

JNIEXPORT jboolean JNICALL Java_AgoraJavaRecording_createChannel(JNIEnv * env, jobject thisObj, jstring jni_appid, jstring jni_channelKey, 
      jstring jni_channelName, jint jni_uid, jobject jni_recordingConfig)
{
  uint32_t uid = 0;
  string appId;
  string channelKey;
  string channelName;
  uint32_t channelProfile = 0;

  string decryptionMode;
  string secret;
  string mixResolution("360,640,15,500");

  int idleLimitSec=5*60;//300s

  string applitePath;
  string appliteLogPath;
  string recordFileRootDir = "";
  string cfgFilePath = "";

  int lowUdpPort = 0;//40000;
  int highUdpPort = 0;//40004;

  bool isAudioOnly=0;
  bool isVideoOnly=0;
  bool isMixingEnabled=0;
  bool mixedVideoAudio=0;

  uint32_t getAudioFrame = agora::linuxsdk::AUDIO_FORMAT_DEFAULT_TYPE;
  uint32_t getVideoFrame = agora::linuxsdk::VIDEO_FORMAT_DEFAULT_TYPE;
  uint32_t streamType = agora::linuxsdk::REMOTE_VIDEO_STREAM_HIGH;
  int captureInterval = 5;
  int triggerMode = 0;
  
  g_bSignalStop = false;

  signal(SIGQUIT, signal_handler);
  signal(SIGABRT, signal_handler);
  signal(SIGINT, signal_handler);
  signal(SIGPIPE, SIG_IGN);

  //const char* appId = NULL;
  const char* c_appId = env->GetStringUTFChars(jni_appid,false);
  appId = c_appId;
  env->ReleaseStringUTFChars(jni_appid, c_appId);
  if(appId.empty()){
    cout<<"get appId is NULL"<<endl;
    return JNI_FALSE;
  }
  
  //const char* channelKey = NULL;
  const char* c_channelKey = env->GetStringUTFChars(jni_channelKey,false);
  channelKey = c_channelKey;
  env->ReleaseStringUTFChars(jni_channelKey, c_channelKey);
  if(channelKey.empty()){
     cout<<"get channel key is NULL"<<endl;
  }
  //const char* channelName = NULL; 
  const char* c_channelName = env->GetStringUTFChars(jni_channelName,false);
  channelName = c_channelName;
  env->ReleaseStringUTFChars(jni_channelName, c_channelName);
  if(channelName.empty()){
    cout<<"channel name is empty!"<<endl;
    return JNI_FALSE;
  }
  uid = (int)jni_uid;
  if(uid < 0){
    cout<<"jni uid is smaller than 0, set 0!"<<endl;
    uid = 0;
  }
  jclass jRecordingJavaConfig =env->GetObjectClass(jni_recordingConfig); 
  if(!jRecordingJavaConfig){
    cout<<"jni_recordingConfig is NULL"<<endl;
    return JNI_FALSE;
  }
  //get parameters field ID
  jfieldID idleLimitSecFieldID = env->GetFieldID(jRecordingJavaConfig, "idleLimitSec", INT_SIGNATURE);
  jfieldID channelProfileFieldID = env->GetFieldID(jRecordingJavaConfig, "channelProfile", CHANNEL_PROFILE_SIGNATURE);
  jfieldID isVideoOnlyFid = env->GetFieldID(jRecordingJavaConfig, "isVideoOnly", BOOL_SIGNATURE);
  jfieldID isAudioOnlyFid = env->GetFieldID(jRecordingJavaConfig, "isAudioOnly", BOOL_SIGNATURE);
  jfieldID isMixingEnabledFid = env->GetFieldID(jRecordingJavaConfig, "isMixingEnabled", BOOL_SIGNATURE);
		
  jfieldID mixResolutionFid = env->GetFieldID(jRecordingJavaConfig, "mixResolution", STRING_SIGNATURE);
  jfieldID mixedVideoAudioFid = env->GetFieldID(jRecordingJavaConfig, "mixedVideoAudio", BOOL_SIGNATURE);
  jfieldID appliteDirFieldID = env->GetFieldID(jRecordingJavaConfig, "appliteDir", STRING_SIGNATURE);
  jfieldID recordFileRootDirFid = env->GetFieldID(jRecordingJavaConfig, "recordFileRootDir", STRING_SIGNATURE);
  jfieldID cfgFilePathFid = env->GetFieldID(jRecordingJavaConfig, "cfgFilePath", STRING_SIGNATURE);
  jfieldID secretFid = env->GetFieldID(jRecordingJavaConfig, "secret", STRING_SIGNATURE);
  jfieldID decryptionModeFid = env->GetFieldID(jRecordingJavaConfig, "decryptionMode", STRING_SIGNATURE);
  jfieldID lowUdpPortFid = env->GetFieldID(jRecordingJavaConfig, "lowUdpPort", INT_SIGNATURE);
  jfieldID highUdpPortFid = env->GetFieldID(jRecordingJavaConfig, "highUdpPort", INT_SIGNATURE);
  jfieldID captureIntervalFid = env->GetFieldID(jRecordingJavaConfig, "captureInterval", INT_SIGNATURE);
  jfieldID streamTypeFieldID = env->GetFieldID(jRecordingJavaConfig, "streamType", REMOTE_VIDEO_STREAM_SIGNATURE);
  jfieldID decodeAudioFieldID = env->GetFieldID(jRecordingJavaConfig, "decodeAudio", AUDIO_FORMAT_TYPE_SIGNATURE);
  jfieldID decodeVideoFieldID = env->GetFieldID(jRecordingJavaConfig, "decodeVideo", VIDEO_FORMAT_TYPE_SIGNATURE);
  jfieldID triggerModeFid = env->GetFieldID(jRecordingJavaConfig, "triggerMode", INT_SIGNATURE);
  if (idleLimitSecFieldID == NULL ||appliteDirFieldID == NULL || channelProfileFieldID == NULL 
						|| streamTypeFieldID == NULL ||decodeAudioFieldID ==NULL ||decodeVideoFieldID ==NULL || !isMixingEnabledFid) { 
            cout<<"get fieldID failed!";return JNI_FALSE;}
  //1.idle
  idleLimitSec = (int)env->GetIntField(jni_recordingConfig, idleLimitSecFieldID); 
  //2.appliteDir
  jstring appliteDir = (jstring)env->GetObjectField(jni_recordingConfig, appliteDirFieldID);
  const char * c_appliteDir = env->GetStringUTFChars(appliteDir ,NULL);
  applitePath = c_appliteDir;
  env->ReleaseStringUTFChars(appliteDir,c_appliteDir);
  env->DeleteLocalRef(appliteDir);
  //3.CHANNEL_PROFILE_TYPE
  jobject channelProfileObject = (env)->GetObjectField(jni_recordingConfig, channelProfileFieldID);
  //assert(channelProfileObject);
  jclass enumClass = env->GetObjectClass(channelProfileObject);
  if(!enumClass) {  
    cout<<"enumClass is null";
    return JNI_FALSE;
  }
  jmethodID getValue = env->GetMethodID(enumClass, "getValue", EMPTY_PARA_INT_RETURN);
  if (!getValue) {
    cout<<"method not found";
    return JNI_FALSE; /* method not found */
  }
  jint channelProfileValue = env->CallIntMethod(channelProfileObject, getValue);
  channelProfile=int(channelProfileValue);
  //4.streamType
  jobject streamTypeObject = (env)->GetObjectField(jni_recordingConfig, streamTypeFieldID);
  jclass streamTypeClass = env->GetObjectClass(streamTypeObject);
  assert(streamTypeObject);
  if(streamTypeObject == NULL){cout<<"streamTypeEnum is NULL"; return JNI_FALSE;}
  jmethodID getValue2 = env->GetMethodID(streamTypeClass, "getValue", EMPTY_PARA_INT_RETURN);
  jint streamTypeValue = env->CallIntMethod(streamTypeObject, getValue2);
  streamType = int(streamTypeValue);
  //5.decodeAudio
  jobject jobDecodeAudio = (env)->GetObjectField(jni_recordingConfig, decodeAudioFieldID);
  jclass jcdecodeAudio = env->GetObjectClass(jobDecodeAudio);
  if(!jcdecodeAudio) {
    cout<<"jcdecodeAudio is null";
  }
  jmethodID jmidGetValue = env->GetMethodID(jcdecodeAudio, "getValue", EMPTY_PARA_INT_RETURN);
  if (!jmidGetValue) {
    cout<<"jmidGetValue not found";
    return JNI_FALSE; /* method not found */
  }
  jint decodeAudioValue = env->CallIntMethod(jobDecodeAudio, jmidGetValue);
  getAudioFrame = int(decodeAudioValue);
  env->DeleteLocalRef(jobDecodeAudio);
  env->DeleteLocalRef(jcdecodeAudio);
  //6.decodeVideo
  jobject jobDecodeVideo = (env)->GetObjectField(jni_recordingConfig, decodeVideoFieldID);
  jclass jcdecodeVideo = env->GetObjectClass(jobDecodeVideo);
  if(!jcdecodeVideo) {
    cout<<"jcdecodeVideo is null";
  }
  jmidGetValue = env->GetMethodID(jcdecodeVideo, "getValue", EMPTY_PARA_INT_RETURN);
  if (!jmidGetValue) {
    cout<<"jmidGetValue not found";
    return JNI_FALSE; /* method not found */
  }
  jint decodeVideoValue = env->CallIntMethod(jobDecodeVideo, jmidGetValue);
  getVideoFrame = int(decodeVideoValue);
  //7.isMixingEnabled
  jboolean isMixingEnabledValue = env->GetBooleanField(jni_recordingConfig, isMixingEnabledFid);
  isMixingEnabled = bool(isMixingEnabledValue);
  //8.isVideoOnly
  jboolean jisVideoOnly = (int)env->GetIntField(jni_recordingConfig, isVideoOnlyFid); 
  isVideoOnly = bool(jisVideoOnly);
  //9.isAudioOnly
  jboolean jisAudioOnly = (int)env->GetIntField(jni_recordingConfig, isAudioOnlyFid); 
  isAudioOnly = bool(jisAudioOnly);
  //10.mixResolution
  jstring jmixResolution = (jstring)env->GetObjectField(jni_recordingConfig, mixResolutionFid);
  const char * c_mixResolution = env->GetStringUTFChars(jmixResolution ,NULL);
  mixResolution = c_mixResolution;
  env->ReleaseStringUTFChars(jmixResolution, c_mixResolution);
  env->DeleteLocalRef(jmixResolution);

  //11.mixedVideoAudio
  jboolean jmixedVideoAudio = env->GetBooleanField(jni_recordingConfig, mixedVideoAudioFid);
  mixedVideoAudio = bool(jmixedVideoAudio);

  //12.recordFileRootDir
  jstring jrecordFileRootDir = (jstring)env->GetObjectField(jni_recordingConfig, recordFileRootDirFid);
  const char * c_recordFileRootDir = env->GetStringUTFChars(jrecordFileRootDir, NULL);
  recordFileRootDir = c_recordFileRootDir;
  env->ReleaseStringUTFChars(jrecordFileRootDir, c_recordFileRootDir);
  env->DeleteLocalRef(jrecordFileRootDir);
  //14.cfgFilePath
  jstring jcfgFilePath = (jstring)env->GetObjectField(jni_recordingConfig, cfgFilePathFid);
  const char * c_cfgFilePath = env->GetStringUTFChars(jcfgFilePath, NULL);
  cfgFilePath = c_cfgFilePath;
  env->ReleaseStringUTFChars(jcfgFilePath, c_cfgFilePath);
  env->DeleteLocalRef(jcfgFilePath);
  //15.secret
  jstring jsecret = (jstring)env->GetObjectField(jni_recordingConfig, secretFid);
  const char * c_secret = env->GetStringUTFChars(jsecret, NULL);
  secret = c_secret;
  env->ReleaseStringUTFChars(jsecret, c_secret);
  env->DeleteLocalRef(jsecret);

  //16.decryptionMode
  jstring jdecryptionMode = (jstring)env->GetObjectField(jni_recordingConfig, decryptionModeFid);
  const char * c_decryptionMode = env->GetStringUTFChars(jdecryptionMode, NULL);
  decryptionMode = c_decryptionMode;
  env->ReleaseStringUTFChars(jdecryptionMode, c_decryptionMode);
  env->DeleteLocalRef(jdecryptionMode);
  //17.lowUdpPort
  lowUdpPort = (int)env->GetIntField(jni_recordingConfig, lowUdpPortFid); 
  //18.highUdpPort
  highUdpPort = (int)env->GetIntField(jni_recordingConfig, highUdpPortFid); 
  //19.captureInterval
  captureInterval = (int)env->GetIntField(jni_recordingConfig, captureIntervalFid); 
  //20.triggerMode
  triggerMode = (int)env->GetIntField(jni_recordingConfig, triggerModeFid); 
  //paser parameters end
  env->DeleteLocalRef(jni_recordingConfig);

  agora::recording::RecordingConfig config;
  jniproxy::AgoraJniProxySdk jniRecorder;
  //important! Get a reference to this object's class

  jclass thisJcInstance = NULL;
  thisJcInstance = env->GetObjectClass(thisObj);
  if(!thisJcInstance) {
    cout<<"Jni cannot get java instance, error!!!";
    return JNI_FALSE;
  }
  jniRecorder.setJcAgoraJavaRecording(thisJcInstance);
  jniRecorder.setJobAgoraJavaRecording(thisObj);

  config.idleLimitSec = idleLimitSec;
  config.channelProfile = static_cast<agora::linuxsdk::CHANNEL_PROFILE_TYPE>(channelProfile);

  config.isVideoOnly = isVideoOnly;
  config.isAudioOnly = isAudioOnly;
  config.isMixingEnabled = isMixingEnabled;
  config.mixResolution = (isMixingEnabled && !isAudioOnly)? const_cast<char*>(mixResolution.c_str()):NULL;
  config.mixedVideoAudio = mixedVideoAudio;

  config.appliteDir = const_cast<char*>(applitePath.c_str());	
  config.recordFileRootDir = const_cast<char*>(recordFileRootDir.c_str());
  config.cfgFilePath = const_cast<char*>(cfgFilePath.c_str());

  config.secret = secret.empty()? NULL:const_cast<char*>(secret.c_str());
  config.decryptionMode = decryptionMode.empty()? NULL:const_cast<char*>(decryptionMode.c_str());

  config.lowUdpPort = lowUdpPort;
  config.highUdpPort = highUdpPort;
  config.captureInterval = captureInterval;

  config.decodeAudio = static_cast<agora::linuxsdk::AUDIO_FORMAT_TYPE>(getAudioFrame);
  config.decodeVideo = static_cast<agora::linuxsdk::VIDEO_FORMAT_TYPE>(getVideoFrame);
  config.streamType = static_cast<agora::linuxsdk::REMOTE_VIDEO_STREAM_TYPE>(streamType);
  config.triggerMode = static_cast<agora::linuxsdk::TRIGGER_MODE_TYPE>(triggerMode);

  cout<<"appId:"<<appId<<",uid:"<<uid<<",channelKey:"<<channelKey<<",channelName:"<<channelName<<",applitePath:"
    <<applitePath<<",channelProfile:"<<channelProfile<<",getAudioFrame:"
    <<getAudioFrame<<",getVideoFrame:"<<getVideoFrame<<endl<<",idle:"<<idleLimitSec<<",lowUdpPort:"<<lowUdpPort<<",highUdpPort:"<<highUdpPort
    <<",captureInterval:"<<captureInterval<<",mixedVideoAudio:"<<mixedVideoAudio<<",mixResolution:"<<mixResolution<<",isVideoOnly:"<<isVideoOnly
    <<",isAudioOnly:"<<isAudioOnly<<",isMixingEnabled:"<<isMixingEnabled<<",triggerMode:"<<triggerMode<<endl;

  /**
   * change log_config Facility per your specific purpose like agora::base::LOCAL5_LOG_FCLT
   * Default:USER_LOG_FCLT. 
   * agora::base::log_config::setFacility(agora::base::LOCAL5_LOG_FCLT);  
   */
  if(!jniRecorder.createChannel(appId, channelKey, channelName, uid, config))
  {
    cerr << "Failed to create agora channel: " << channelName <<endl;
    return JNI_FALSE;
  }

  //tell java this para pointer
  jlong nativeObjectRef = jlong(&jniRecorder);
  //find java callback function and set this value
  jmethodID jmid = env->GetMethodID(thisJcInstance, "nativeObjectRef", LONG_PARA_VOID_RETURN);
  if(!jmid) {
    cout << "cannot find nativeObjectRef method " <<endl;
    return JNI_FALSE;
  }
  env->CallIntMethod(thisObj, jmid, nativeObjectRef);
  
  std::string recordingDir = jniRecorder.getRecorderProperties()->storageDir;
  cout<<"Recording directory is "<<jniRecorder.getRecorderProperties()->storageDir<<endl;
  jniRecorder.setJavaRecordingPath(env, recordingDir);
  while (!jniRecorder.stopped() && !g_bSignalStop) {
    usleep(1*1000*1000); //1s
  }
  if (g_bSignalStop) {
    jniRecorder.leaveChannel();
    jniRecorder.release();
    cout<<"jni layer stopped!";
  }
  jniRecorder.stopJavaProc(env);
  cout<<"Java_AgoraJavaRecording_createChannel  end"<<endl;
  return JNI_TRUE;

}
#ifdef __cplusplus
}
#endif
