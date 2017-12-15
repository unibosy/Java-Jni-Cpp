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

  // cerr << "Signal " << signo ";
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
#define STRING_SIGNATURE "Ljava/lang/String;"
#define BYTEARRAY "[B"
#define CHANNEL_PROFILE_SIGNATURE "Lheaders/EnumIndex$CHANNEL_PROFILE_TYPE;"
#define REMOTE_VIDEO_STREAM_SIGNATURE "Lheaders/EnumIndex$REMOTE_VIDEO_STREAM_TYPE;"

#define VIDEOFRAME_H264_SIGNATURE "Lheaders/EnumIndex$VideoH264Frame;"
#define VIDEOFRAME_YUV_SIGNATURE "Lheaders/EnumIndex$VideoYuvFrame;"
#define VIDEOFRAME_JPG_SIGNATURE "Lheaders/EnumIndex$VideoJpgFrame;" 

#define VIDEO_FRAME_TYPE_SIGNATURE "Lheaders/EnumIndex$VIDEO_FRAME_TYPE;"

#define AUDIO_FORMAT_TYPE_SIGNATURE "Lheaders/EnumIndex$AUDIO_FORMAT_TYPE;"
#define VIDEO_FORMAT_TYPE_SIGNATURE "Lheaders/EnumIndex$VIDEO_FORMAT_TYPE;"


#define VIDEOMIXLAYOUT_SIGNATURE "[Lheaders/EnumIndex$VideoMixingLayout$Region;"

AgoraJniProxySdk::AgoraJniProxySdk():AgoraSdk(){
  LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk constructor");
  m_jobAgoraJavaRecording = NULL;
  m_jcAgoraJavaRecording = NULL;
  m_tmp = "";
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

#if 0
#define JNI_LOG(MSG,LEVEL) \
  LOG()
#endif

//do common!

bool AgoraJniProxySdk::fillAacAllFields(JNIEnv* jni_env, jobject& job, jclass& jc, const agora::linuxsdk::AudioFrame*& frame) const {
  CHECK_PTR_RETURN_BOOL(m_jobAgoraJavaRecording);
#if 1
  if(frame->type != agora::linuxsdk::AUDIO_FRAME_AAC) return false;
  jfieldID fid = NULL;
  jbyteArray jbArr = NULL;

  agora::linuxsdk::AudioAacFrame *f = frame->frame.aac;
  //frame_ms_
  fid = jni_env->GetFieldID(jc, "frame_ms_", LONG_SIGNATURE);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get frame_ms_ value of audioPcmFrame");
    return false;
  }
  long frame_ms_ = f->frame_ms_;
  LOG_DIR(m_logdir.c_str(), INFO, "get frame_ms_ value of AUDIO_FRAME_TYPE ok");
  jni_env->SetLongField(job, fid, jlong(frame_ms_));
  //aacBuf_
  fid = jni_env->GetFieldID(jc, "aacBuf_", BYTEARRAY);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get frame_ms_ value of audioAacFrame");
    return false;
  }
  unsigned char* aacBuf_ = f->aacBuf_;
  long aacBufSize_ = f->aacBufSize_;

  jbArr = jni_env->NewByteArray(aacBufSize_);
  jbyte *jby = jni_env->GetByteArrayElements(jbArr,0); 
  memcpy(jby, aacBuf_, aacBufSize_);
  jni_env->SetByteArrayRegion(jbArr, 0, aacBufSize_, jby);
  jni_env->SetObjectField(job, fid, jbArr);
  LOG_DIR(m_logdir.c_str(), INFO,"set aacBufSize_  value of AUDIO_FRAME_TYPE ok:%ld",aacBufSize_);
  //aacBufSize_
  fid = jni_env->GetFieldID(jc, "aacBufSize_", LONG_SIGNATURE);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO, "cannot get aacBufSize_ value of audioPcmFrame");
    return false;
  }
  LOG_DIR(m_logdir.c_str(), INFO, "get aacBufSize_ value of AUDIO_FRAME_TYPE ok");
  jni_env->SetLongField(job, fid, jlong(aacBufSize_));

  //buf_
  fid = jni_env->GetFieldID(jc, "buf_", STRING_SIGNATURE);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get buf_ value of audioPcmFrame");
    return false;
  }
  std::string buf_ = f->buf_;
  jstring jstrBuf = jni_env->NewStringUTF(buf_.c_str());
  LOG_DIR(m_logdir.c_str(), INFO,"get buf_ value of AUDIO_FRAME_TYPE ok");
  jni_env->SetObjectField(job, fid, jstrBuf);
#endif 
  //fill AudioPcmFrame field
  return true;
}

bool AgoraJniProxySdk::fillPcmAllFields(JNIEnv* jni_env, jobject& job, jclass& jc, const agora::linuxsdk::AudioFrame*& frame) const {
  CHECK_PTR_RETURN_BOOL(m_jobAgoraJavaRecording);
#if 1
  if(frame->type != agora::linuxsdk::AUDIO_FRAME_RAW_PCM) return false;
  jfieldID fid = NULL;
  jbyteArray jbArr = NULL;

  agora::linuxsdk::AudioPcmFrame *f = frame->frame.pcm;
  //frame_ms_
  fid = jni_env->GetFieldID(jc, "frame_ms_", LONG_SIGNATURE);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get frame_ms_ value of audioPcmFrame");
    return false;
  }
  long frame_ms_ = f->frame_ms_;
  LOG_DIR(m_logdir.c_str(), INFO,"get frame_ms_ value of AUDIO_FRAME_TYPE ok:%u",frame_ms_);
  jni_env->SetLongField(job, fid, jlong(frame_ms_));
#if 1
  //channels_
  fid = jni_env->GetFieldID(jc, "channels_", LONG_SIGNATURE);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get channels_ value of audioPcmFrame");
    return false;
  }
  long channels_ = f->channels_;
  LOG_DIR(m_logdir.c_str(), INFO,"get channels_ value of AUDIO_FRAME_TYPE ok:%ld",channels_);
  jni_env->SetLongField(job, fid, jlong(channels_));
  //sample_bits_
  fid = jni_env->GetFieldID(jc, "sample_bits_", LONG_SIGNATURE);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get sample_bits_ value of audioPcmFrame");
    return false;
  }
  long sample_bits_ = f->sample_bits_;
  LOG_DIR(m_logdir.c_str(), INFO,"get sample_bits_ value of AUDIO_FRAME_TYPE ok:%ld",sample_bits_);
  jni_env->SetLongField(job, fid, jlong(sample_bits_));
  //sample_rates_
  fid = jni_env->GetFieldID(jc, "sample_rates_", LONG_SIGNATURE);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get sample_rates_value of audioPcmFrame");
    return false;
  }
  long sample_rates_ = f->sample_rates_;
  LOG_DIR(m_logdir.c_str(), INFO,"get sample_rates_ value of AUDIO_FRAME_TYPE ok:%ld",sample_rates_);
  jni_env->SetLongField(job, fid, jlong(sample_rates_));
  //pcmBuf_
  fid = jni_env->GetFieldID(jc, "pcmBuf_", BYTEARRAY);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get frame_ms_ value of audioPcmFrame");
    return false;
  }
  unsigned char* pcmBuf_ = f->pcmBuf_;
  long pcmBufSize_ = f->pcmBufSize_;

  jbArr = jni_env->NewByteArray(pcmBufSize_);
  jbyte *jby = jni_env->GetByteArrayElements(jbArr,0); 
  memcpy(jby, pcmBuf_, pcmBufSize_);
  jni_env->SetByteArrayRegion(jbArr, 0, pcmBufSize_, jby);
  jni_env->SetObjectField(job, fid, jbArr);
  LOG_DIR(m_logdir.c_str(), INFO,"set pcmBuf_  value of AUDIO_FRAME_TYPE ok:%ld",pcmBufSize_);
  //pcmBufSize_
  fid = jni_env->GetFieldID(jc, "pcmBufSize_", LONG_SIGNATURE);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get pcmBufSize_ value of audioPcmFrame");
    return false;
  }
  LOG_DIR(m_logdir.c_str(), INFO,"get pcmBufSize_ value of AUDIO_FRAME_TYPE ok:%ld",pcmBufSize_);
  jni_env->SetLongField(job, fid, jlong(pcmBufSize_));

  //buf_
  fid = jni_env->GetFieldID(jc, "buf_", STRING_SIGNATURE);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get buf_ value of audioPcmFrame");
    return false;
  }
  std::string buf_ = f->buf_;
  jstring jstrBuf = jni_env->NewStringUTF(buf_.c_str());
  LOG_DIR(m_logdir.c_str(), INFO,"get buf_ value of AUDIO_FRAME_TYPE ok");
  jni_env->SetObjectField(job, fid, jstrBuf);
#endif 
  //fill AudioPcmFrame field
  return true;
#endif
}

bool AgoraJniProxySdk::fillJAudioFrameByFields(JNIEnv* jni_env, const agora::linuxsdk::AudioFrame*& frame, jclass& jcAudioFrame, jobject& jobAudioFrame) const {
  //1.find main class
  if (frame->type == agora::linuxsdk::AUDIO_FRAME_RAW_PCM) {
    LOG_DIR(m_logdir.c_str(), INFO,"jni receive raw data is pcm!!!!!!!!");
    //call one function
    if(!fillAudioPcmFrame(jni_env, frame, jcAudioFrame,jobAudioFrame))
    {
      LOG_DIR(m_logdir.c_str(), INFO,"Warning: fillAudioPcmFrame failed!!!!!");
      ((JavaVM*)g_jvm)->DetachCurrentThread();
      return false;
    }
  }else if (frame->type == agora::linuxsdk::AUDIO_FRAME_AAC) {
    //do things here
    if(!fillAudioAacFrame(jni_env, frame, jcAudioFrame,jobAudioFrame))
    {
      LOG_DIR(m_logdir.c_str(), INFO,"Warning: fillAudioAacFrame failed!!!!!");
      ((JavaVM*)g_jvm)->DetachCurrentThread();
      return false;
    }

  }
  
  LOG_DIR(m_logdir.c_str(), INFO,"fillJAudioFrameByFields enter" );
  return true;
}

bool AgoraJniProxySdk::fillAudioAacFrame(JNIEnv* jni_env, const agora::linuxsdk::AudioFrame*& frame, \
            jclass& jcAudioFrame, jobject& jobAudioFrame) const  {
  CHECK_PTR_RETURN_BOOL(m_jobAgoraJavaRecording);
#if 1
  if(frame->type != agora::linuxsdk::AUDIO_FRAME_AAC) return false;
  jmethodID jmid = NULL;
  jclass jc = NULL;
  jfieldID fid = NULL;
  jmethodID initMid = NULL;
  jobject job = NULL;

  //get pcm frame by find class
  jc = jni_env->FindClass("Lheaders/EnumIndex$AudioAacFrame;");
  if(!jc) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get AudioAccFrame jclass");
    //((JavaVM*)g_jvm)->DetachCurrentThread();
    return false;
  }
  LOG_DIR(m_logdir.c_str(), INFO,"get aac jclass ok");
  //new AudioAacFrame
  //TODO  place these to one function
  initMid = jni_env->GetMethodID(jc,"<init>","(Lheaders/EnumIndex;J)V");
  if(!initMid) {
    LOG_DIR(m_logdir.c_str(), INFO,"get AudioAacFrame init methid failed!");
    //((JavaVM*)g_jvm)->DetachCurrentThread();
    return false;
  }
  job = jni_env->NewObject(jc, initMid);
  if(!job){
    LOG_DIR(m_logdir.c_str(), INFO,"new AudioAacFrame failed! no memory?");
    //((JavaVM*)g_jvm)->DetachCurrentThread();
    return false;
  }
#if 0
  //get AudioPcmFrame field
 fid = jni_env->GetFieldID(jc, "frame_ms_", "J");
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get frame_ms_ value of AUDIO_FRAME_TYPE ");
    //((JavaVM*)g_jvm)->DetachCurrentThread();
    return false;
  }
  agora::linuxsdk::AudioPcmFrame *f = frame->frame.pcm;
  long frame_ms_ = f->frame_ms_;
  LOG_DIR(m_logdir.c_str(), INFO,"get frame_ms_ value of AUDIO_FRAME_TYPE ok:%u",frame_ms_);
  //fill AudioPcmFrame field
  jni_env->SetLongField(job, fid, jlong(frame_ms_));
  //set 
  //fill pcm data
  fid = jni_env->GetFieldID(jcAudioFrame, "pcm", "Lheaders/EnumIndex$AudioPcmFrame;");
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get pcm AUDIO_FRAME_TYPE field");
    //((JavaVM*)g_jvm)->DetachCurrentThread();
    return false;
  }
 
  jni_env->SetObjectField(jobAudioFrame, fid, job);
#else
  fid = jni_env->GetFieldID(jcAudioFrame, "aac", "Lheaders/EnumIndex$AudioAacFrame;");
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get aac AUDIO_FRAME_TYPE field");
    //((JavaVM*)g_jvm)->DetachCurrentThread();
    return false;
  }
  //fill all fields of AudioAacFrame jobject
  if(fillAacAllFields(jni_env, job, jc, frame)){
    //Fill in the jobAdudioFrame
    jni_env->SetObjectField(jobAudioFrame, fid, job);
    return true;
  }
  return false;
#endif
  return true;
#endif
}
bool AgoraJniProxySdk::fillAudioPcmFrame(JNIEnv* jni_env, const agora::linuxsdk::AudioFrame*& frame, \
            jclass& jcAudioFrame, jobject& jobAudioFrame) const  {
  CHECK_PTR_RETURN_BOOL(m_jobAgoraJavaRecording);
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
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get AudioPcmFrame jclass");
    //((JavaVM*)g_jvm)->DetachCurrentThread();
    return false;
  }
  LOG_DIR(m_logdir.c_str(), INFO,"get pcm jclass ok");
  //new AudioPcmFrame
  //TODO  place these to one function
  initMid = jni_env->GetMethodID(jc,"<init>","(Lheaders/EnumIndex;JJJ)V");
  if(initMid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"get AudioPcmFrame init methid failed!");
    //((JavaVM*)g_jvm)->DetachCurrentThread();
    return false;
  }
  job = jni_env->NewObject(jc, initMid);
  if(job == NULL){
    LOG_DIR(m_logdir.c_str(), INFO,"new AudioPcmFrame failed! no memory?");
    //((JavaVM*)g_jvm)->DetachCurrentThread();
    return false;
  }
#if 0
  //get AudioPcmFrame field
 fid = jni_env->GetFieldID(jc, "frame_ms_", "J");
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get frame_ms_ value of AUDIO_FRAME_TYPE ");
    //((JavaVM*)g_jvm)->DetachCurrentThread();
    return false;
  }
  agora::linuxsdk::AudioPcmFrame *f = frame->frame.pcm;
  long frame_ms_ = f->frame_ms_;
  LOG_DIR(m_logdir.c_str(), INFO,"get frame_ms_ value of AUDIO_FRAME_TYPE ok:%u",frame_ms_);
  //fill AudioPcmFrame field
  jni_env->SetLongField(job, fid, jlong(frame_ms_));
  //set 
  //fill pcm data
  fid = jni_env->GetFieldID(jcAudioFrame, "pcm", "Lheaders/EnumIndex$AudioPcmFrame;");
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get pcm AUDIO_FRAME_TYPE field");
    //((JavaVM*)g_jvm)->DetachCurrentThread();
    return false;
  }
 
  jni_env->SetObjectField(jobAudioFrame, fid, job);
#else
  fid = jni_env->GetFieldID(jcAudioFrame, "pcm", "Lheaders/EnumIndex$AudioPcmFrame;");
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get pcm AUDIO_FRAME_TYPE field");
    //((JavaVM*)g_jvm)->DetachCurrentThread();
    return false;
  }
  //fill all fields of AudioPcmFrame jobject
  if(fillPcmAllFields(jni_env, job, jc, frame)){
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
  CHECK_PTR_RETURN_BOOL(m_jobAgoraJavaRecording);
  LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk::fillJVideo");
  if(frame->type != agora::linuxsdk::VIDEO_FRAME_RAW_YUV) return false;
  if(!jni_env || !frame) return false;
  jclass jc = NULL;
  jmethodID initMid = NULL;
  jobject job = NULL;
  jfieldID fid = NULL;
  int fieldId = 0; //TODO
  jbyteArray jbArr = NULL;

  //1.get subclass
  jc = jni_env->FindClass("Lheaders/EnumIndex$VideoYuvFrame;");
  if(!jc) {
    LOG_DIR(m_logdir.c_str(), INFO,"fillJVideoOfYUV cannot find subclass");
    return false;
  }
  //2.get subclass init method
  initMid = jni_env->GetMethodID(jc,"<init>","(Lheaders/EnumIndex;JIIIII)V");
  if(!initMid) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get init methodid");
    return false;
  }
  //3.new VideoXXXXXFrame object
  job = jni_env->NewObject(jc, initMid);
  if(!job){
    LOG_DIR(m_logdir.c_str(), INFO,"new subclass  failed! no memory?");
    return false;
  }
  agora::linuxsdk::VideoYuvFrame *f = frame->frame.yuv;
  if(!f) return false;
  //4.fill all fields
  //4.1 get & set of this subclass object
  //frame_ms_
  fid = jni_env->GetFieldID(jc, "frame_ms_", LONG_SIGNATURE);
  if(!fid) {
    //TODO add field ID
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get field,field ID::%u",fid);
    return false;
  }
  long frame_ms_ = f->frame_ms_;
  LOG_DIR(m_logdir.c_str(), INFO,"get frame_ms_ value of h264 ok:%u",frame_ms_);
  jni_env->SetLongField(job, fid, jlong(frame_ms_));
  //buf_
  fid = jni_env->GetFieldID(jc, "buf_", BYTEARRAY);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get field,field ID::%u",fid);
    return false;
  }
  unsigned char* buf_ = f->buf_;
  long bufSize_ = f->bufSize_;

  jbArr = jni_env->NewByteArray(bufSize_);
  if(!jbArr) {
    LOG_DIR(m_logdir.c_str(), INFO,"NewByteArray failed, no memory??");
    return false;
  }
  jbyte *jby = jni_env->GetByteArrayElements(jbArr,0); 
  memcpy(jby, buf_, bufSize_);
  jni_env->SetByteArrayRegion(jbArr, 0, bufSize_, jby);
  jni_env->SetObjectField(job, fid, jbArr);
  LOG_DIR(m_logdir.c_str(), INFO,"set buf_ value  ok:%ld",bufSize_);
  //bufSize_
  fid = jni_env->GetFieldID(jc, "bufSize_", LONG_SIGNATURE);
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get field,field ID::%u",fid);
    return false;
  }
  LOG_DIR(m_logdir.c_str(), INFO,"get bufSize_ value ok:%ld",bufSize_);
  jni_env->SetLongField(job, fid, jlong(bufSize_));

  //5.get subclass field
  fid = jni_env->GetFieldID(jcVideoFrame, "yuv", VIDEOFRAME_YUV_SIGNATURE);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get VIDEOFRAME_YUV_SIGNATURE");
    //((JavaVM*)g_jvm)->DetachCurrentThread();
    return false;
  }
  //6.fill jobVideFrame
  jni_env->SetObjectField(jobVideoFrame, fid, job);
  return  true;
}
bool AgoraJniProxySdk::fillJVideoOfJPG(JNIEnv* jni_env, const agora::linuxsdk::VideoFrame*& frame, jclass& jcVideoFrame, jobject& jobVideoFrame) const {
  CHECK_PTR_RETURN_BOOL(m_jobAgoraJavaRecording);
  LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk::fillJVideoOfJPG enter" );
  if(frame->type != agora::linuxsdk::VIDEO_FRAME_JPG) return false;
  if(!jni_env || !frame) return false;
  jclass jc = NULL;
  jmethodID initMid = NULL;
  jobject job = NULL;
  jfieldID fid = NULL;
  int fieldId = 0; //TODO
  jbyteArray jbArr = NULL;

  //1.get subclass
  jc = jni_env->FindClass("Lheaders/EnumIndex$VideoJpgFrame;");
  if(!jc) {
    LOG_DIR(m_logdir.c_str(), INFO,"fillJVideoOfJPG cannot find subclass");
    return false;
  }
  //2.get subclass init method
  initMid = jni_env->GetMethodID(jc,"<init>","(Lheaders/EnumIndex;)V");
  if(!initMid) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get init methodid");
    return false;
  }
  //3.new VideoXXXXXFrame object
  job = jni_env->NewObject(jc, initMid);
  if(!job){
    LOG_DIR(m_logdir.c_str(), INFO,"new subclass  failed! no memory?");
    return false;
  }
  agora::linuxsdk::VideoJpgFrame *f = frame->frame.jpg;
  if(!f) return false;
  //4.fill all fields
  //4.1 get & set of this subclass object
  //frame_ms_
  fid = jni_env->GetFieldID(jc, "frame_ms_", LONG_SIGNATURE);
  if(!fid) {
    //TODO add field ID
    LOG_DIR(m_logdir.c_str(), INFO,"fillJVideoOfJPG cannot get field,field ID:");
    return false;
  }
  long frame_ms_ = f->frame_ms_;
  LOG_DIR(m_logdir.c_str(), INFO,"get frame_ms_ value of h264 ok!");
  jni_env->SetLongField(job, fid, jlong(frame_ms_));
  //buf_
  fid = jni_env->GetFieldID(jc, "buf_", BYTEARRAY);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get field,field ID::%u",fid);
    return false;
  }
  unsigned char* buf_ = f->buf_;
  long bufSize_ = f->bufSize_;

  jbArr = jni_env->NewByteArray(bufSize_);
  if(!jbArr) {
    LOG_DIR(m_logdir.c_str(), INFO,"NewByteArray failed, no memory??");
    return false;
  }
  jbyte *jby = jni_env->GetByteArrayElements(jbArr,0); 
  memcpy(jby, buf_, bufSize_);
  jni_env->SetByteArrayRegion(jbArr, 0, bufSize_, jby);
  jni_env->SetObjectField(job, fid, jbArr);
  LOG_DIR(m_logdir.c_str(), INFO,"set buf_ value  ok:%ld",bufSize_);
  //bufSize_
  fid = jni_env->GetFieldID(jc, "bufSize_", LONG_SIGNATURE);
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get field,field ID::%u",fid);
    return false;
  }
  LOG_DIR(m_logdir.c_str(), INFO,"get bufSize_ value ok:%ld",bufSize_);
  jni_env->SetLongField(job, fid, jlong(bufSize_));
  //payload
  fid = jni_env->GetFieldID(jc, "payload", STRING_SIGNATURE);
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get field,field ID:");
    return false;
  }
  std::string payload = f->payload;
  jstring jstrPayload = jni_env->NewStringUTF(payload.c_str());
  LOG_DIR(m_logdir.c_str(), INFO,"get payload value ok");
  jni_env->SetObjectField(job, fid, jstrPayload);

  //5.get subclass field
  fid = jni_env->GetFieldID(jcVideoFrame, "jpg", VIDEOFRAME_JPG_SIGNATURE);
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get VIDEOFRAME_JPG_SIGNATURE");
    return false;
  }
  //6.fill jobVideFrame
  jni_env->SetObjectField(jobVideoFrame, fid, job);
  return  true;
}
bool AgoraJniProxySdk::fillJVideoOfH264(JNIEnv* jni_env, const agora::linuxsdk::VideoFrame*& frame, jclass& jcVideoFrame, jobject& jobVideoFrame) const{
//bool AgoraJniProxySdk::fillJVideoOfH264(JNIEnv* jni_env, const agora::linuxsdk::VideoFrame*& frame, jobject& job) const{
  LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk::fillJVideo");
  CHECK_PTR_RETURN_BOOL(m_jobAgoraJavaRecording);
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
    LOG_DIR(m_logdir.c_str(), INFO,"cannot find subclass, type:%d",static_cast<int>(frame->type));
    return false;
  }
  //2.get subclass init method
  initMid = jni_env->GetMethodID(jc,"<init>","(Lheaders/EnumIndex;)V");
  if(!initMid) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get init methodid");
    return false;
  }
  //3.new VideoH264Frame object
  job = jni_env->NewObject(jc, initMid);
  if(!job){
    LOG_DIR(m_logdir.c_str(), INFO,"new subclass  failed! no memory?");
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
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get field,field ID::%d",fid);
    return false;
  }
  long frame_ms_ = f->frame_ms_;
  LOG_DIR(m_logdir.c_str(), INFO,"get frame_ms_ value of h264 ok:%u",frame_ms_);
  jni_env->SetLongField(job, fid, jlong(frame_ms_));
  //frame_num_
  fid = jni_env->GetFieldID(jc, "frame_num_", LONG_SIGNATURE);
  if(!fid) {
    //TODO add field ID
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get field,field ID:%d",fid);
    return false;
  }
  long frame_num_ = f->frame_num_;
  jni_env->SetLongField(job, fid, jlong(frame_num_));

  //buf_
  fid = jni_env->GetFieldID(jc, "buf_", BYTEARRAY);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get field,field ID:%d",fieldId);
    return false;
  }
  unsigned char* buf_ = f->buf_;
  long bufSize_ = f->bufSize_;

  jbArr = jni_env->NewByteArray(bufSize_);
  if(!jbArr) {
    LOG_DIR(m_logdir.c_str(), INFO,"NewByteArray failed, no memory??");
    return false;
  }
  jbyte *jby = jni_env->GetByteArrayElements(jbArr,0); 
  memcpy(jby, buf_, bufSize_);
  jni_env->SetByteArrayRegion(jbArr, 0, bufSize_, jby);
  jni_env->SetObjectField(job, fid, jbArr);
  LOG_DIR(m_logdir.c_str(), INFO,"set buf_ value  ok");
  //bufSize_
  fid = jni_env->GetFieldID(jc, "bufSize_", LONG_SIGNATURE);
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get field,field ID:%s",fieldId);
    return false;
  }
  LOG_DIR(m_logdir.c_str(), INFO,"get bufSize_ value ok:%ld",bufSize_);
  jni_env->SetLongField(job, fid, jlong(bufSize_));
  //payload
  fid = jni_env->GetFieldID(jc, "payload", STRING_SIGNATURE);
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get field,field ID:");
    return false;
  }
  std::string payload = f->payload;
  jstring jstrPayload = jni_env->NewStringUTF(payload.c_str());
  LOG_DIR(m_logdir.c_str(), INFO,"get payload value ok");
  jni_env->SetObjectField(job, fid, jstrPayload);

  //5.get subclass field
  fid = jni_env->GetFieldID(jcVideoFrame, "h264", VIDEOFRAME_H264_SIGNATURE);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get VIDEOFRAME_H264_SIGNATURE");
    //((JavaVM*)g_jvm)->DetachCurrentThread();
    return false;
  }
  //6.fill jobVideFrame
  jni_env->SetObjectField(jobVideoFrame, fid, job);
  return  true;
}
bool AgoraJniProxySdk::fillJVideoFrameByFields(JNIEnv* jni_env, const agora::linuxsdk::VideoFrame*& frame, jclass& jcVideoFrame, jobject& jobVideoFrame) const {
  LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk::fillJVideoFrameByFields enter");
  CHECK_PTR_RETURN_BOOL(m_jobAgoraJavaRecording);
  bool ret = false;
  jfieldID jfid = NULL;
  jclass jc = NULL;
  jmethodID initMid = NULL;
  jfieldID fid = NULL;

  if(!jni_env || !frame || !jcVideoFrame || !jobVideoFrame){
    LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk::fillJVideoFrameByFields para error!");
    return ret;
  }
  LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk::fillJVideoFrameByFields enter , video type:%d",frame->type);
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
      LOG_DIR(m_logdir.c_str(), INFO,"videoFrameReceived raw h264!");
      if(!fillJVideoOfYUV(jni_env, frame, job)){
        LOG_DIR(m_logdir.c_str(), INFO,"fill subclass falied!");
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
    LOG_DIR(m_logdir.c_str(), INFO,"videoFrameReceived raw yuv! frame type:%d",static_cast<int>(frame->type));
    //3.1.1 
    if(!fillJVideoOfYUV(jni_env, frame, jcVideoFrame, jobVideoFrame)){
      LOG_DIR(m_logdir.c_str(), INFO,"fill subclass falied!");
      if(job)
        jni_env->DeleteLocalRef(job);
      return false;
    }
  }else if(frame->type == agora::linuxsdk::VIDEO_FRAME_JPG){
    //3.2
    LOG_DIR(m_logdir.c_str(), INFO,"videoFrameReceived raw jpg!");
    if(!fillJVideoOfJPG(jni_env, frame, jcVideoFrame, jobVideoFrame)) {
      LOG_DIR(m_logdir.c_str(), INFO,"fill subclass falied!");
      if(job)
        jni_env->DeleteLocalRef(job);
      return false;
    }
  }else if(frame->type == agora::linuxsdk::VIDEO_FORMAT_H264_FRAME_TYPE){
    //3.3
    LOG_DIR(m_logdir.c_str(), INFO,"videoFrameReceived raw h264!");
    if(!fillJVideoOfH264(jni_env, frame, jcVideoFrame, jobVideoFrame))
    {
      LOG_DIR(m_logdir.c_str(), INFO,"fillJVideoOfH264 failed!");
      return false;
    }
  }else if(frame->type == agora::linuxsdk::VIDEO_FORMAT_JPG_FRAME_TYPE){
    LOG_DIR(m_logdir.c_str(), INFO,"videoFrameReceived VIDEO_FORMAT_JPG_FRAME_TYPE" );
  }else if(frame->type == agora::linuxsdk::VIDEO_FORMAT_JPG_FILE_TYPE){
    LOG_DIR(m_logdir.c_str(), INFO,"videoFrameReceived VIDEO_FORMAT_JPG_FILE_TYPE" );
  }
  #endif 
  //fill VIDEO_FRAME_TYPE
  //set type of VIDEO_FRAME_TYPE by FindClass
  jc = jni_env->FindClass("Lheaders/EnumIndex$VIDEO_FRAME_TYPE;");
  if(!jc) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get VIDEO_FRAME_TYPE class");
    return false;
  }
  LOG_DIR(m_logdir.c_str(), INFO,"get AUDIO_FORMAT_TYPE ok!");
  //4.1.1
  initMid = jni_env->GetMethodID(jc,"<init>","(Lheaders/EnumIndex;)V");
  if(initMid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"get init methid failed!");
    return false;
  }
  //4.1.2
  job = jni_env->NewObject(jc, initMid);
  //4.1.3 get field of this class
  fid = jni_env->GetFieldID(jc, "type", "I");
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get value of VIDEO_FRAME_TYPE class");
    return false;
  }
  //4.1.4 fill this field
  int iVideoFrameType = static_cast<int>(frame->type);
  LOG_DIR(m_logdir.c_str(), INFO,"get value of VIDEO_FRAME_TYPE ok,iVideoFrameType:%d",iVideoFrameType);
  jni_env->SetIntField(job, fid, jint(iVideoFrameType));
  //4.2
  //set this object into jobAudioFrame!
  //step 1:get this object field
  fid = jni_env->GetFieldID(jcVideoFrame, "type", VIDEO_FRAME_TYPE_SIGNATURE);
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get VIDEO_FORMAT_TYPE field");
    return false;
  }
  //4.3
  jni_env->SetObjectField(jobVideoFrame, fid, job);
  LOG_DIR(m_logdir.c_str(), INFO,"set GetObjectField ok");
  return true;
}
void AgoraJniProxySdk::videoFrameReceived(unsigned int uid, const agora::linuxsdk::VideoFrame *frame) const {
  static int count = 0;
  LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk::videoFrameReceived enter uid:%u",uid, ",count:%d", ++count, ",frame->type:%d",frame->type);
  CHECK_PTR_RETURN(m_jobAgoraJavaRecording);
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
		LOG_DIR(m_logdir.c_str(), INFO,"GetEnv failed");
		return;
	}
	assert(jni_env);
  assert(javaClass);
	pid_t tid = getpid();
	LOG_DIR(m_logdir.c_str(), INFO,"videoFrameReceived this thread id is:%u",tid);
  //1.find VideoFrame class
  jcVideoFrame = jni_env->FindClass("headers/EnumIndex$VideoFrame");
  if(jcVideoFrame == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"videoFrameReceived cannot find class VideoFrame!");
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  //2.get init methodid to new VideoFrame object
  initMid = jni_env->GetMethodID(jcVideoFrame,"<init>","(Lheaders/EnumIndex;)V");
  if(initMid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get VideoFrmae init method ");
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  //new VideoFrame object
  jobVideoFrame = jni_env->NewObject(jcVideoFrame, initMid);
  if(jobVideoFrame == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"new video frame object failed!");
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  //3.get subclass or enumm details and fill these' fields
#if 0
  if (frame->type == agora::linuxsdk::VIDEO_FRAME_RAW_YUV) {
    //3.1
    LOG_DIR(m_logdir.c_str(), INFO,"videoFrameReceived raw yuv!");
    handleRawYUV(jni_env, frame, jcVideoFrame, jobVideoFrame);
    
  }else if(frame->type == agora::linuxsdk::VIDEO_FRAME_JPG){
    //3.2
    LOG_DIR(m_logdir.c_str(), INFO,"videoFrameReceived raw jpg!");
    handleJPG(jni_env, frame, jcVideoFrame, jobVideoFrame);
  }else{
    //3.3
    LOG_DIR(m_logdir.c_str(), INFO,"videoFrameReceived raw h264!");
    handleH264(jni_env, frame, jcVideoFrame, jobVideoFrame);
  }
#else
  if(!fillJVideoFrameByFields(jni_env, frame, jcVideoFrame, jobVideoFrame))
  {
    LOG_DIR(m_logdir.c_str(), INFO,"jni fillJVideoFrameByFields failed!" );
    //TODO delete ref
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
#endif
  //4.find class where callback function in
  LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk::videoFrameReceived end");
  //5. find callback function
  jmid = jni_env->GetMethodID(m_jcAgoraJavaRecording, "videoFrameReceived", "(JLheaders/EnumIndex$VideoFrame;)V");
  if(jmid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get callback function");
    //TODO memory leak..
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  //so far so well
  //6. call back java method, para is jobVideoFrame
  jni_env->CallVoidMethod(m_jobAgoraJavaRecording, jmid,jlong(long(uid)), jobVideoFrame);
  
  ((JavaVM*)g_jvm)->DetachCurrentThread();
#endif
#if 0
 char uidbuf[65];
  snprintf(uidbuf, sizeof(uidbuf),"%u", uid);
  const char * suffix=".vtmp";
  if (frame->type == agora::linuxsdk::VIDEO_FRAME_RAW_YUV) {
    agora::linuxsdk::VideoYuvFrame *f = frame->frame.yuv;
    suffix=".yuv";

    LOG_DIR(m_logdir.c_str(), INFO,"User " << uid << ", received a yuv frame, width: "
        << f->width_ << ", height: " << f->height_ ;
    LOG_DIR(m_logdir.c_str(), INFO,",ystride:"<<f->ystride_<< ",ustride:"<<f->ustride_<<",vstride:"<<f->vstride_;
    cout";
  } else if(frame->type == agora::linuxsdk::VIDEO_FRAME_JPG) {
    suffix=".jpg";
    agora::linuxsdk::VideoJpgFrame *f = frame->frame.jpg;

    LOG_DIR(m_logdir.c_str(), INFO,"User " << uid << ", received an jpg frame, timestamp: "
    << f->frame_ms_ ";

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

    LOG_DIR(m_logdir.c_str(), INFO,"User " << uid << ", received an h264 frame, timestamp: "
        << f->frame_ms_ << ", frame no: " << f->frame_num_ ";
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
void AgoraJniProxySdk::audioFrameReceived(unsigned int uid, const agora::linuxsdk::AudioFrame *frame) const {
  static int count = 0;
	LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk::audioFrameReceived enter uid:%u",uid,",count:%d",++count);
  CHECK_PTR_RETURN(m_jobAgoraJavaRecording);
  jfieldID fid;
  jmethodID mid;
  jclass jc = NULL;
  jmethodID initMid  = NULL;
  jobject job = NULL;
  int iAudioFrameType = 0; //default pcm?

	JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
	if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK) {
		LOG_DIR(m_logdir.c_str(), INFO,"GetEnv failed");
		return;
	}
	assert(jni_env);
	pid_t tid = getpid();
	LOG_DIR(m_logdir.c_str(), INFO,"audioFrameReceived this thread id is:%u",tid);
  jclass jcAudioFrame = NULL;

  jcAudioFrame = (jni_env)->FindClass("headers/EnumIndex$AudioFrame");
  if(jcAudioFrame == NULL){
    LOG_DIR(m_logdir.c_str(), INFO,"not find audioFrame");
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return ;
  }
    iAudioFrameType = static_cast<int>(frame->type);
  //2.get main class init methodid
  initMid = jni_env->GetMethodID(jcAudioFrame,"<init>","(Lheaders/EnumIndex;)V");
  if(initMid == NULL){
    LOG_DIR(m_logdir.c_str(), INFO,"not find AudioFrameOfPcm initMid!");
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  //3.new main class object
  jobject jobAudioFrame = jni_env->NewObject(jcAudioFrame, initMid);
  if(jobAudioFrame == NULL){
    LOG_DIR(m_logdir.c_str(), INFO,"new audio frame object failed!");
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  if(!fillJAudioFrameByFields(jni_env, frame, jcAudioFrame,jobAudioFrame)) {
    LOG_DIR(m_logdir.c_str(), INFO,"fillJAudioFrameByFields failed!" );
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  
  //4.get main class field
#if 1
  //4.1.find subclass in main class
  //set type of AUDIO_FRAME_TYPE by FindClass
  jc = jni_env->FindClass("Lheaders/EnumIndex$AUDIO_FRAME_TYPE;");
  if(jc == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get AUDIO_FRAME_TYPE class");
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  LOG_DIR(m_logdir.c_str(), INFO,"get AUDIO_FORMAT_TYPE ok!");
  //4.1.1
  initMid = jni_env->GetMethodID(jc,"<init>","(Lheaders/EnumIndex;)V");
  if(initMid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"get init methid failed!");
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  //4.1.2
  job = jni_env->NewObject(jc, initMid);
  //4.1.3 get field of this class
  fid = jni_env->GetFieldID(jc, "type", "I");
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get value of AUDIO_FRAME_TYPE class");
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  LOG_DIR(m_logdir.c_str(), INFO,"get value of AUDIO_FRAME_TYPE ok");
  //4.1.4 fill this field
  jni_env->SetIntField(job, fid, jint(iAudioFrameType));
  //4.2
  //set this object into jobAudioFrame!
  //step 1:get this object field
  fid = jni_env->GetFieldID(jcAudioFrame, "type", "Lheaders/EnumIndex$AUDIO_FRAME_TYPE;");
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get AUDIO_FRAME_TYPE field");
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  //4.3
  jni_env->SetObjectField(jobAudioFrame, fid, job);
  LOG_DIR(m_logdir.c_str(), INFO,"set GetObjectField ok");
#endif 
 //6.find callback method in Java ---wrong, should use thisObj
  mid = jni_env->GetMethodID(m_jcAgoraJavaRecording, "audioFrameReceived", "(JLheaders/EnumIndex$AudioFrame;)V");
  if(mid == NULL){
    LOG_DIR(m_logdir.c_str(), INFO,"audioFrameReceived get method failed!");
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return ;
  }
  //7. callback java method
  jni_env->CallVoidMethod(m_jobAgoraJavaRecording, mid,jlong(long(uid)), jobAudioFrame);
#if 0
  char uidbuf[65];
  snprintf(uidbuf, sizeof(uidbuf),"%u", uid);
  std::string info_name = m_storage_dir + std::string(uidbuf) /*+ timestamp_per_join_*/;

  uint8_t* data = NULL; 
  uint32_t size = 0;
  if (frame->type == agora::linuxsdk::AUDIO_FRAME_RAW_PCM) {
    info_name += ".pcm";  
    
    LOG_DIR(m_logdir.c_str(), INFO,"User " << uid << ", received a raw PCM frame" ";

    agora::linuxsdk::AudioPcmFrame *f = frame->frame.pcm;
    long tmp = f->frame_ms_;
    data = f->pcmBuf_;
    size = f->pcmBufSize_;

  } else if (frame->type == agora::linuxsdk::AUDIO_FRAME_AAC) {
    info_name += ".aac";

    LOG_DIR(m_logdir.c_str(), INFO,"User " << uid << ", received an AAC frame" ";

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
  LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk User:%u" ,uid , " joined, RecordingDir:%s" , (infos.storageDir? infos.storageDir:"NULL") );
  CHECK_PTR_RETURN(m_jobAgoraJavaRecording);
  LOG_DIR(m_logdir.c_str(), INFO,"on user joined");
  std::string store_dir = std::string(infos.storageDir);
  m_logdir = store_dir;
  LOG_DIR(m_logdir.c_str(), INFO, "[mydebug]AgoraJniProxySdk::onUserJoined");
	JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
	if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK) {
		LOG_DIR(m_logdir.c_str(), INFO,"GetEnv failed");
		return;
	}
  assert(jni_env);
	//int tid = GetCurrentThreadId();
	pid_t tid = getpid();
	LOG_DIR(m_logdir.c_str(), INFO,"onUserJoined this thread id is:",tid);
  jmethodID jUserJoinedMid =  jni_env->GetMethodID(m_jcAgoraJavaRecording,"onUserJoined","(JLjava/lang/String;)V");
  LOG_DIR(m_logdir.c_str(), INFO,"on user joined-----1" );
  assert(jUserJoinedMid);
  if(!jUserJoinedMid){
    LOG_DIR(m_logdir.c_str(), INFO,"jUserJoinedMid is null");
  }
  jstring jstrRecordingDir = jni_env->NewStringUTF(store_dir.c_str());
  jni_env->CallVoidMethod(m_jobAgoraJavaRecording, jUserJoinedMid, jlong((long)(uid)),jstrRecordingDir);
	((JavaVM*)g_jvm)->DetachCurrentThread();
}
void AgoraJniProxySdk::onUserOffline(agora::linuxsdk::uid_t uid, agora::linuxsdk::USER_OFFLINE_REASON_TYPE reason) {
  LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk onUserOffline User:%u",uid, ",reason:%d",static_cast<int>(reason));
  CHECK_PTR_RETURN(m_jobAgoraJavaRecording);
	JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
	if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK) {
		LOG_DIR(m_logdir.c_str(), INFO,"GetEnv failed");
		return;
	}
  assert(jni_env);
	//int tid = GetCurrentThreadId();
	pid_t tid = getpid();
	LOG_DIR(m_logdir.c_str(), INFO,"onUserOffline this thread id is:%u",tid);
  jmethodID jUserOfflineMid =  jni_env->GetMethodID(m_jcAgoraJavaRecording,"onUserOffline","(JI)V");
  if(!jUserOfflineMid){
    LOG_DIR(m_logdir.c_str(), INFO,"cnnot find jUserOfflineMid" );
    return;
  }
  jni_env->CallVoidMethod(m_jobAgoraJavaRecording, jUserOfflineMid, jlong((long)(uid)),jint(int(reason)));
  LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk::onUserOffline call end");

	((JavaVM*)g_jvm)->DetachCurrentThread();
}
void AgoraJniProxySdk::onLeaveChannel(agora::linuxsdk::LEAVE_PATH_CODE code) {
	LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk onLeaveChannel");
  CHECK_PTR_RETURN(m_jobAgoraJavaRecording);
	JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
	if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK)
	{
		LOG_DIR(m_logdir.c_str(), INFO,"GetEnv failed");
		return;
	}
  assert(jni_env);
	//int tid = GetCurrentThreadId();
	pid_t tid = getpid();
	LOG_DIR(m_logdir.c_str(), INFO,"onLeaveChannel this thread id is:%u",tid);
  jmethodID jLeaveMid =  jni_env->GetMethodID(m_jcAgoraJavaRecording,"onLeaveChannel","(I)V");
  assert(jcLeave);
  jni_env->CallVoidMethod(m_jobAgoraJavaRecording, jLeaveMid, jint((int)(code)));
  LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk::onLeaveChannel call end");
	((JavaVM*)g_jvm)->DetachCurrentThread();
}
void AgoraJniProxySdk::onWarning(int warn) {
 	LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk onWarning,warn:%d",warn);
  CHECK_PTR_RETURN(m_jobAgoraJavaRecording);
	JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
	if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK) {
		LOG_DIR(m_logdir.c_str(), INFO,"GetEnv failed");
		return;
	}
  assert(jni_env);
	jclass javaClass = jni_env->FindClass("AgoraJavaRecording");
  assert(javaClass);
	//int tid = GetCurrentThreadId();
	pid_t tid = getpid();
	LOG_DIR(m_logdir.c_str(), INFO,"onWarning this thread id is:%u",tid);
  jmethodID jOnWarnMid =  jni_env->GetMethodID(javaClass,"onWarning","(I)V");
  assert(jOnWarnMid);
  jni_env->CallVoidMethod(m_jobAgoraJavaRecording, jOnWarnMid, warn);
  jni_env->DeleteLocalRef(javaClass);
	((JavaVM*)g_jvm)->DetachCurrentThread();
}

void AgoraJniProxySdk::onError(int error, agora::linuxsdk::STAT_CODE_TYPE stat_code) {
 	LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk onError");
  CHECK_PTR_RETURN(m_jobAgoraJavaRecording);
	JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
  if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK) {
		LOG_DIR(m_logdir.c_str(), INFO,"GetEnv failed");
		return;
	}
  assert(jni_env);
	jclass javaClass = jni_env->FindClass("AgoraJavaRecording");
  assert(javaClass);
	//int tid = GetCurrentThreadId();
	pid_t tid = getpid();
	LOG_DIR(m_logdir.c_str(), INFO,"onError this thread id is:%u",tid);
#if 0
  jmethodID jLeaveMid =  jni_env->GetStaticMethodID(javaClass,"onError","(II)V");
  assert(jcLeave);
  jni_env->CallStaticVoidMethod(javaClass, jLeaveMid, error, jint((int)(stat_code)));
#else
  jmethodID jLeaveMid =  jni_env->GetMethodID(javaClass,"onError","(II)V");
  if(!jLeaveMid) {
    LOG_DIR(m_logdir.c_str(), INFO,"get method onError failed!");
    return;
  }
  assert(jcLeave);
  jni_env->CallVoidMethod(m_jobAgoraJavaRecording, jLeaveMid, error, jint((int)(stat_code)));
#endif 
  leaveChannel();

  jni_env->DeleteLocalRef(javaClass);
	((JavaVM*)g_jvm)->DetachCurrentThread();
}

//TODO
#if 0
  convert jobject jni_recordingConfig to c++ recordingConfig
#endif
#if 1
JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
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
  (JNIEnv *, jobject, jlong nativeObjectRef) {
  cout<<"java call leaveChannel"<<endl;
  jniproxy::AgoraJniProxySdk* ajp = reinterpret_cast<jniproxy::AgoraJniProxySdk*>(nativeObjectRef);

  g_bSignalStop = true;
  return JNI_TRUE;
}

/*
 * Class:     AgoraJavaRecording
 * Method:    setVideoMixingLayout
 * Signature: (JLheaders/EnumIndex/VideoMixingLayout;)I
 */
JNIEXPORT jint JNICALL Java_AgoraJavaRecording_setVideoMixingLayout
  (JNIEnv * env, jobject job, jlong nativeObjectRef, jobject jVideoMixLayout)
{
  cout<<"Java_AgoraJavaRecording_setVideoMixingLayout ############"<<endl;
  jniproxy::AgoraJniProxySdk* ajp = reinterpret_cast<jniproxy::AgoraJniProxySdk*>(nativeObjectRef);
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
  cout<<"setVideoMixingLayout -1"<<endl;
  jint canvasWidth = env->GetIntField(jVideoMixLayout,jCanvasWidthID);
  jint canvasHeight = env->GetIntField(jVideoMixLayout,jCanvasWidthID);
  
  jstring jstrBackgroundColor = (jstring)env->GetObjectField(jVideoMixLayout, jBackgroundColorID);
  const char* c_backgroundColor = env->GetStringUTFChars(jstrBackgroundColor, NULL);
  
  jint regionCount = env->GetIntField(jVideoMixLayout,jRegionCountID);
  
  //not used yet?
  //jstring jstrAppData = (jstring)env->GetObjectField(jVideoMixLayout,jAppDataID);
  //const char* c_jstrAppData = env->GetStringUTFChars(jstrAppData, NULL);
  //jint appDataLength = env->GetIntField(jVideoMixLayout,jAppDataLengthID);

  cout<<"setVideoMixingLayout -2,canvasWidth:"<<canvasWidth<<",canvasHeight:"<<canvasHeight<<",c_backgroundColor:"<<c_backgroundColor
              <<",regionCount:"<<regionCount<<endl;

  agora::linuxsdk::VideoMixingLayout layout;
  //regions begin
  jobjectArray jobRegions =  (jobjectArray)env->GetObjectField(jVideoMixLayout, jRegionsID);
  if(!jobRegions) {
    cout<<"cannot find Regions in jVideoMixLayout!"<<endl;
    return JNI_FALSE;
  }
  jint arrLen = env->GetArrayLength(jobRegions);
  cout<<"arrLen:"<<int(arrLen)<<endl;
  if(arrLen != regionCount){
    cout<<"regionCount is not equal with arrLen"<<endl;
    return JNI_FALSE;//return ??
  }
  layout.canvasWidth = int(canvasWidth);
  layout.canvasHeight = int(canvasHeight);
  layout.backgroundColor = c_backgroundColor;
  layout.regionCount = int(regionCount);

  //layout.appData = c_jstrAppData;
  //layout.appDataLength = int(appDataLength);
  if(0<arrLen)
  {
    cout<<"set layout now!"<<endl;
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
      jint uidValue = env->GetIntField(region,jfidUid);
      //C++ uid is uint32
      cout<<"user id:"<<static_cast<uint32_t>(uidValue)<<endl;
      regionList[i].uid = static_cast<uint32_t>(uidValue);
      //x
      jfieldID jxID = env->GetFieldID(jcRegion, "x", DOUBLE_SIGNATURE);
      if(!jxID){
        cout<<"connot get region x,uid:"<<uint32_t(uidValue)<<endl;
        continue;
      }
      jdouble jx = env->GetIntField(region,jxID);
      cout<<"x:"<<double(jx)<<endl;
      regionList[i].x = static_cast<double>(jx);

      //y
      jfieldID jyID = env->GetFieldID(jcRegion, "y", DOUBLE_SIGNATURE);
      if(!jyID){
        cout<<"connot get region y,uid:"<<int(uidValue)<<endl;
        continue;
      }
      jdouble jy = env->GetIntField(region,jyID);
      cout<<"y:"<<double(jy)<<endl;
      regionList[i].y = static_cast<double>(jy);

      //width
      jfieldID jwidthID = env->GetFieldID(jcRegion, "width", DOUBLE_SIGNATURE);
      if(!jwidthID){
        cout<<"connot get region width, uid:"<<int(uidValue)<<endl;
        continue;
      }
      jdouble jwidth = env->GetIntField(region,jwidthID);
      cout<<"width:"<<double(jwidth)<<endl;
      regionList[i].width = static_cast<double>(jwidth);

      //height
      jfieldID jheightID = env->GetFieldID(jcRegion, "height", DOUBLE_SIGNATURE);
      if(!jheightID){
        cout<<"connot get region height, uid:"<<int(uidValue)<<endl;
        continue;
      }
      jdouble jheight = env->GetIntField(region,jheightID);
      cout<<"height:"<<double(jheight)<<endl;
      regionList[i].height = static_cast<double>(jheight);
      cout<<"user id:"<<static_cast<uint32_t>(uidValue)<<",x:"<<double(jx)<<",y:"<<double(jy)<<",width:"<<double(jwidth)<<",height:"<<double(jheight)<<endl;
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

  //layout.regions = ;
  cout<<"setVideoMixingLayout -4"<<endl;
  ajp->setVideoMixingLayout(layout);
  cout<<"setVideoMixingLayout -end"<<endl;
  return jint(0);
}



void AgoraJniProxySdk::stopJavaProc() {
  LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk stopJavaProc");
  CHECK_PTR_RETURN(m_jobAgoraJavaRecording);
	JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
  if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK) {
		LOG_DIR(m_logdir.c_str(), INFO,"GetEnv failed");
		return;
	}
  assert(jni_env);
	pid_t tid = getpid();
	LOG_DIR(m_logdir.c_str(), INFO,"stopJavaProc this thread id is:",tid);
  jmethodID jStopCB =  jni_env->GetMethodID(m_jcAgoraJavaRecording,"stopCallBack","()V");
  assert(jStopCB);
  jni_env->CallVoidMethod(m_jobAgoraJavaRecording, jStopCB);

	((JavaVM*)g_jvm)->DetachCurrentThread();
}

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
  int width = 0;
  int height = 0;
  int fps = 0;
  int kbps = 0;
  uint32_t count = 0;
  string upstreamResolution("640, 360, 15, 400"); 


		pid_t tid = getpid();
	  cout<<"enter Java_AgoraJavaRecording_createChannel,pid:"<<tid;
		g_bSignalStop = false;
	  signal(SIGQUIT, signal_handler);
  	signal(SIGABRT, signal_handler);
  	signal(SIGINT, signal_handler);
  	signal(SIGPIPE, SIG_IGN);

    //const char* appId = NULL;
    appId = env->GetStringUTFChars(jni_appid,false);
    if(appId.empty())
      cout<<"get appId is NULL";
    //const char* channelKey = NULL;
    channelKey = env->GetStringUTFChars(jni_channelKey,false);
    if(channelKey.empty())
      cout<<"no channel key";
    //const char* channelName = NULL; 
		channelName = env->GetStringUTFChars(jni_channelName,false);
    if(channelName.empty())
      cout<<"channel name is empty!";
    uid = (int)jni_uid;
		if(uid < 0){
			cout<<"jni uid is smaller than 0, set 0!";
			uid = 0;
		}
   	jclass jRecordingConfig =env->GetObjectClass(jni_recordingConfig); 
		if(jRecordingConfig == NULL){
			cout<<"jni_recordingConfig is NULL";
			return JNI_FALSE;
		}
        //get parameters field ID
		jfieldID idleLimitSecFieldID = env->GetFieldID(jRecordingConfig, "idleLimitSec", INT_SIGNATURE);
		jfieldID appliteDirFieldID = env->GetFieldID(jRecordingConfig, "appliteDir", STRING_SIGNATURE);
		jfieldID channelProfileFieldID = env->GetFieldID(jRecordingConfig, "channelProfile", CHANNEL_PROFILE_SIGNATURE);
		jfieldID streamTypeFieldID = env->GetFieldID(jRecordingConfig, "streamType", REMOTE_VIDEO_STREAM_SIGNATURE);
		jfieldID decodeAudioFieldID = env->GetFieldID(jRecordingConfig, "decodeAudio", AUDIO_FORMAT_TYPE_SIGNATURE);
		jfieldID decodeVideoFieldID = env->GetFieldID(jRecordingConfig, "decodeVideo", VIDEO_FORMAT_TYPE_SIGNATURE);
    jfieldID isMixingEnabledFID = env->GetFieldID(jRecordingConfig, "isMixingEnabled",BOOL_SIGNATURE);
    if (idleLimitSecFieldID == NULL ||appliteDirFieldID == NULL || channelProfileFieldID == NULL 
						|| streamTypeFieldID == NULL ||decodeAudioFieldID ==NULL ||decodeVideoFieldID ==NULL || !isMixingEnabledFID) { 
            cout<<"get fieldID failed!";return JNI_FALSE;}
		//idle
		idleLimitSec = (int)env->GetIntField(jni_recordingConfig, idleLimitSecFieldID); 
		//appliteDir
		jstring appliteDir = (jstring)env->GetObjectField(jni_recordingConfig, appliteDirFieldID);
		const char * c_appliteDir = env->GetStringUTFChars(appliteDir ,NULL);
		applitePath = c_appliteDir;
		env->ReleaseStringUTFChars(appliteDir,c_appliteDir);
		//CHANNEL_PROFILE_TYPE
		jobject channelProfileObject = (env)->GetObjectField(jni_recordingConfig, channelProfileFieldID);
		//assert(channelProfileObject);
		jclass enumClass = env->GetObjectClass(channelProfileObject);
		if(enumClass == NULL) {  
			cout<<"enumClass is null";
		}
		jmethodID getValue = env->GetMethodID(enumClass, "getValue", "()I");
		if (getValue == NULL) {
			cout<<"method not found";
			return JNI_FALSE; /* method not found */
		}
		jint channelProfileValue = env->CallIntMethod(channelProfileObject, getValue);
    //streamType
		jobject streamTypeObject = (env)->GetObjectField(jni_recordingConfig, streamTypeFieldID);
		jclass streamTypeClass = env->GetObjectClass(streamTypeObject);
		assert(streamTypeObject);
		if(streamTypeObject == NULL){cout<<"streamTypeEnum is NULL"; return JNI_FALSE;}
		jmethodID getValue2 = env->GetMethodID(streamTypeClass, "getValue", "()I");
		jint streamTypeValue = env->CallIntMethod(streamTypeObject, getValue2);
		//decodeAudio
		jobject jobDecodeAudio = (env)->GetObjectField(jni_recordingConfig, decodeAudioFieldID);
		jclass jcdecodeAudio = env->GetObjectClass(jobDecodeAudio);
		if(jcdecodeAudio == NULL) {
			cout<<"jcdecodeAudio is null";
		}
		jmethodID jmidGetValue = env->GetMethodID(jcdecodeAudio, "getValue", "()I");
		if (jmidGetValue == NULL) {
			cout<<"jmidGetValue not found";
			return JNI_FALSE; /* method not found */
		}
		jint decodeAudioValue = env->CallIntMethod(jobDecodeAudio, jmidGetValue);
    //decodeVideo
		jobject jobDecodeVideo = (env)->GetObjectField(jni_recordingConfig, decodeVideoFieldID);
		jclass jcdecodeVideo = env->GetObjectClass(jobDecodeVideo);
		if(jcdecodeVideo == NULL) {
			cout<<"jcdecodeVideo is null";
		}
		jmidGetValue = env->GetMethodID(jcdecodeVideo, "getValue", "()I");
		if (jmidGetValue == NULL) {
			cout<<"jmidGetValue not found";
			return JNI_FALSE; /* method not found */
		}
		jint decodeVideoValue = env->CallIntMethod(jobDecodeVideo, jmidGetValue);
    jboolean isMixingEnabledValue = env->GetBooleanField(jni_recordingConfig, isMixingEnabledFID);
    isMixingEnabled = bool(isMixingEnabledValue);
    //paser parameters end

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
    
    
		config.appliteDir = const_cast<char*>(applitePath.c_str());	
		config.idleLimitSec = idleLimitSec;
    cout<<"appId:"<<appId<<",uid:"<<uid<<",channelKey:"<<channelKey<<",channelName:"<<channelName<<",applitePath:"
          <<applitePath<<",channelProfileValue:"<<channelProfileValue<<",decodeAudio:"
          <<decodeAudioValue<<",decodeVideoValue:"<<decodeVideoValue;
	  config.decodeAudio = static_cast<agora::linuxsdk::AUDIO_FORMAT_TYPE>(decodeAudioValue);
    config.decodeVideo = static_cast<agora::linuxsdk::VIDEO_FORMAT_TYPE>(decodeVideoValue);
		config.streamType = static_cast<agora::linuxsdk::REMOTE_VIDEO_STREAM_TYPE>(streamTypeValue);
    config.isMixingEnabled = isMixingEnabled;
    //testCode
    jniRecorder.setTmp(std::string("helloJni"));

    if(!jniRecorder.createChannel(appId, channelKey, channelName, uid, config))
    {
      cerr << "Failed to create agora channel: " << channelName ;
      return JNI_FALSE;
    }
    //tell java this para pointer
    jlong nativeObjectRef = jlong(&jniRecorder);
    //find java callback function and set this value
    jmethodID jmid = env->GetMethodID(thisJcInstance, "nativeObjectRef", "(J)V");
    if(!jmid) {
      cout << "cannot find nativeObjectRef method " <<endl;
      return JNI_FALSE;
    }
    env->CallIntMethod(thisObj, jmid, nativeObjectRef);

    cout<<"Recording directory is "<<jniRecorder.getRecorderProperties()->storageDir<<endl;
    //sava jni_env,the same thread?
 		while (!jniRecorder.stopped() && !g_bSignalStop) {
  		usleep(10000); //10ms
		}
  	if (g_bSignalStop) {
    	jniRecorder.leaveChannel();
      cout<<"jni layer stopped!";
      jniRecorder.stopJavaProc();
    	jniRecorder.release();
  	}
    cout<<"Java_AgoraJavaRecording_createChannel  end"<<endl;
    return JNI_TRUE;
 
}
#ifdef __cplusplus
}
#endif
