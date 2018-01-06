#include "IAgoraLinuxSdkCommon.h"
#include "./jni/AgoraJavaRecording.h"
#include "AgoraJniProxy.h"
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
#define STRING_SIGNATURE "Ljava/lang/String;"
#define BYTEARRAY "[B"
#define CHANNEL_PROFILE_SIGNATURE "Lio/agora/recording/common/Common$CHANNEL_PROFILE_TYPE;"
#define REMOTE_VIDEO_STREAM_SIGNATURE "Lio/agora/recording/common/Common$REMOTE_VIDEO_STREAM_TYPE;"

#define VIDEOFRAME_H264_SIGNATURE "Lio/agora/recording/common/Common$VideoH264Frame;"
#define VIDEOFRAME_YUV_SIGNATURE "Lio/agora/recording/common/Common$VideoYuvFrame;"
#define VIDEOFRAME_JPG_SIGNATURE "Lio/agora/recording/common/Common$VideoJpgFrame;" 

#define VIDEO_FRAME_TYPE_SIGNATURE "Lio/agora/recording/common/Common$VIDEO_FRAME_TYPE;"

#define AUDIO_FORMAT_TYPE_SIGNATURE "Lio/agora/recording/common/Common$AUDIO_FORMAT_TYPE;"
#define VIDEO_FORMAT_TYPE_SIGNATURE "Lio/agora/recording/common/Common$VIDEO_FORMAT_TYPE;"


#define VIDEOMIXLAYOUT_SIGNATURE "[Lio/agora/recording/common/Common$VideoMixingLayout$Region;"

AgoraJniProxySdk::AgoraJniProxySdk():AgoraSdk(){
  LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk constructor");
  m_jobAgoraJavaRecording = NULL;
  m_jcAgoraJavaRecording = NULL;
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
  jni_env->SetLongField(job, fid, jlong(frame_ms_));
  //aacBuf_
  fid = jni_env->GetFieldID(jc, "aacBuf_", BYTEARRAY);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get frame_ms_ value of audioAacFrame");
    return false;
  }
  const unsigned char* aacBuf_ = f->aacBuf_;
  long aacBufSize_ = f->aacBufSize_;

  jbArr = jni_env->NewByteArray(aacBufSize_);
  jbyte *jby = jni_env->GetByteArrayElements(jbArr,0); 
  memcpy(jby, aacBuf_, aacBufSize_);
  jni_env->SetByteArrayRegion(jbArr, 0, aacBufSize_, jby);
  jni_env->SetObjectField(job, fid, jbArr);
  //aacBufSize_
  fid = jni_env->GetFieldID(jc, "aacBufSize_", LONG_SIGNATURE);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO, "cannot get aacBufSize_ value of audioPcmFrame");
    return false;
  }
  jni_env->SetLongField(job, fid, jlong(aacBufSize_));

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
  jni_env->SetLongField(job, fid, jlong(frame_ms_));
#if 1
  //channels_
  fid = jni_env->GetFieldID(jc, "channels_", LONG_SIGNATURE);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get channels_ value of audioPcmFrame");
    return false;
  }
  long channels_ = f->channels_;
  jni_env->SetLongField(job, fid, jlong(channels_));
  //sample_bits_
  fid = jni_env->GetFieldID(jc, "sample_bits_", LONG_SIGNATURE);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get sample_bits_ value of audioPcmFrame");
    return false;
  }
  long sample_bits_ = f->sample_bits_;
  jni_env->SetLongField(job, fid, jlong(sample_bits_));
  //sample_rates_
  fid = jni_env->GetFieldID(jc, "sample_rates_", LONG_SIGNATURE);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get sample_rates_value of audioPcmFrame");
    return false;
  }
  long sample_rates_ = f->sample_rates_;
  jni_env->SetLongField(job, fid, jlong(sample_rates_));
  //pcmBuf_
  fid = jni_env->GetFieldID(jc, "pcmBuf_", BYTEARRAY);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get frame_ms_ value of audioPcmFrame");
    return false;
  }
  const unsigned char* pcmBuf_ = f->pcmBuf_;
  long pcmBufSize_ = f->pcmBufSize_;

  jbArr = jni_env->NewByteArray(pcmBufSize_);
  jbyte *jby = jni_env->GetByteArrayElements(jbArr,0); 
  memcpy(jby, pcmBuf_, pcmBufSize_);
  jni_env->SetByteArrayRegion(jbArr, 0, pcmBufSize_, jby);
  jni_env->SetObjectField(job, fid, jbArr);
  //pcmBufSize_
  fid = jni_env->GetFieldID(jc, "pcmBufSize_", LONG_SIGNATURE);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get pcmBufSize_ value of audioPcmFrame");
    return false;
  }
  jni_env->SetLongField(job, fid, jlong(pcmBufSize_));

#endif 
  //fill AudioPcmFrame field
  return true;
#endif
}

bool AgoraJniProxySdk::fillJAudioFrameByFields(JNIEnv* jni_env, const agora::linuxsdk::AudioFrame*& frame, jclass& jcAudioFrame, jobject& jobAudioFrame) const {
  //1.find main class
  if (frame->type == agora::linuxsdk::AUDIO_FRAME_RAW_PCM) {
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
  return true;
}

bool AgoraJniProxySdk::fillAudioAacFrame(JNIEnv* jni_env, const agora::linuxsdk::AudioFrame*& frame, \
            jclass& jcAudioFrame, jobject& jobAudioFrame) const  {
  CHECK_PTR_RETURN_BOOL(m_jobAgoraJavaRecording);
  if(frame->type != agora::linuxsdk::AUDIO_FRAME_AAC) return false;
  jmethodID jmid = NULL;
  jclass jc = NULL;
  jfieldID fid = NULL;
  jmethodID initMid = NULL;
  jobject job = NULL;

  //get pcm frame by find class
  jc = jni_env->FindClass("Lio/agora/recording/common/Common$AudioAacFrame;");
  if(!jc) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get AudioAccFrame jclass");
    return false;
  }
  //new AudioAacFrame
  //TODO  place these to one function
  initMid = jni_env->GetMethodID(jc,"<init>","(Lio/agora/recording/common/Common;J)V");
  if(!initMid) {
    LOG_DIR(m_logdir.c_str(), INFO,"get AudioAacFrame init methid failed!");
    return false;
  }
  job = jni_env->NewObject(jc, initMid);
  if(!job){
    LOG_DIR(m_logdir.c_str(), INFO,"new AudioAacFrame failed! no memory?");
    return false;
  }
  fid = jni_env->GetFieldID(jcAudioFrame, "aac", "Lio/agora/recording/common/Common$AudioAacFrame;");
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), INFO,"cannot get aac AUDIO_FRAME_TYPE field");
    return false;
  }
  //fill all fields of AudioAacFrame jobject
  if(!fillAacAllFields(jni_env, job, jc, frame)){
    LOG_DIR(m_logdir.c_str(), ERROR,"fillAacAllFields failed!");
    return false;
  }
  //Fill in the jobAdudioFrame
  jni_env->SetObjectField(jobAudioFrame, fid, job);
  return true;
}
bool AgoraJniProxySdk::fillAudioPcmFrame(JNIEnv* jni_env, const agora::linuxsdk::AudioFrame*& frame, \
            jclass& jcAudioFrame, jobject& jobAudioFrame) const  {
  CHECK_PTR_RETURN_BOOL(m_jobAgoraJavaRecording);
  if(frame->type != agora::linuxsdk::AUDIO_FRAME_RAW_PCM) return false;
  jmethodID jmid = NULL;
  jclass jc = NULL;
  jfieldID fid = NULL;
  jmethodID initMid = NULL;
  jobject job = NULL;

  //get pcm frame by find class
  jc = jni_env->FindClass("Lio/agora/recording/common/Common$AudioPcmFrame;");
  if(jc == NULL) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get AudioPcmFrame jclass");
    return false;
  }
  //new AudioPcmFrame
  //TODO  place these to one function
  initMid = jni_env->GetMethodID(jc,"<init>","(Lio/agora/recording/common/Common;JJJ)V");
  if(initMid == NULL) {
    LOG_DIR(m_logdir.c_str(), ERROR,"get AudioPcmFrame init methid failed!");
    return false;
  }
  job = jni_env->NewObject(jc, initMid);
  if(job == NULL){
    LOG_DIR(m_logdir.c_str(), ERROR,"new AudioPcmFrame failed! no memory?");
    return false;
  }
  fid = jni_env->GetFieldID(jcAudioFrame, "pcm", "Lio/agora/recording/common/Common$AudioPcmFrame;");
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get pcm AUDIO_FRAME_TYPE field");
    return false;
  }
  //fill all fields of AudioPcmFrame jobject
  if(!fillPcmAllFields(jni_env, job, jc, frame)){
    LOG_DIR(m_logdir.c_str(), INFO,"fillPcmAllFields failed!");
    return false;
  }
  //Fill in the jobAdudioFrame
  jni_env->SetObjectField(jobAudioFrame, fid, job);
  return true;
}
bool AgoraJniProxySdk::fillJVideoOfYUV(JNIEnv* jni_env, const agora::linuxsdk::VideoFrame*& frame, jclass& jcVideoFrame, jobject& jobVideoFrame) const {
  CHECK_PTR_RETURN_BOOL(m_jobAgoraJavaRecording);
  if(frame->type != agora::linuxsdk::VIDEO_FRAME_RAW_YUV) return false;
  if(!jni_env || !frame) return false;
  jclass jc = NULL;
  jmethodID initMid = NULL;
  jobject job = NULL;
  jfieldID fid = NULL;
  int fieldId = 0; //TODO
  jbyteArray jbArr = NULL;

  //1.get subclass
  jc = jni_env->FindClass("Lio/agora/recording/common/Common$VideoYuvFrame;");
  if(!jc) {
    LOG_DIR(m_logdir.c_str(), ERROR,"fillJVideoOfYUV cannot find subclass");
    return false;
  }
  //2.get subclass init method
  initMid = jni_env->GetMethodID(jc,"<init>","(Lio/agora/recording/common/Common;JIIIII)V");
  if(!initMid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get init methodid");
    return false;
  }
  //3.new VideoXXXXXFrame object
  job = jni_env->NewObject(jc, initMid);
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
  fid = jni_env->GetFieldID(jc, "frame_ms_", LONG_SIGNATURE);
  if(!fid) {
    //TODO add field ID
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get field,field ID::%u",fid);
    return false;
  }
  long frame_ms_ = f->frame_ms_;
  jni_env->SetLongField(job, fid, jlong(frame_ms_));
  //buf_
  fid = jni_env->GetFieldID(jc, "buf_", BYTEARRAY);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get field,field ID::%u",fid);
    return false;
  }
  const unsigned char* buf_ = f->buf_;
  long bufSize_ = f->bufSize_;

  jbArr = jni_env->NewByteArray(bufSize_);
  if(!jbArr) {
    LOG_DIR(m_logdir.c_str(), ERROR,"NewByteArray failed, no memory??");
    return false;
  }
  jbyte *jby = jni_env->GetByteArrayElements(jbArr,0); 
  memcpy(jby, buf_, bufSize_);
  jni_env->SetByteArrayRegion(jbArr, 0, bufSize_, jby);
  jni_env->SetObjectField(job, fid, jbArr);
  //bufSize_
  fid = jni_env->GetFieldID(jc, "bufSize_", LONG_SIGNATURE);
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get field,field ID::%u",fid);
    return false;
  }
  jni_env->SetLongField(job, fid, jlong(bufSize_));

  //5.get subclass field
  fid = jni_env->GetFieldID(jcVideoFrame, "yuv", VIDEOFRAME_YUV_SIGNATURE);
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get VIDEOFRAME_YUV_SIGNATURE");
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
  jc = jni_env->FindClass("Lio/agora/recording/common/Common$VideoJpgFrame;");
  if(!jc) {
    LOG_DIR(m_logdir.c_str(), ERROR,"fillJVideoOfJPG cannot find subclass");
    return false;
  }
  //2.get subclass init method
  initMid = jni_env->GetMethodID(jc,"<init>","(Lio/agora/recording/common/Common;)V");
  if(!initMid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get init methodid");
    return false;
  }
  //3.new VideoXXXXXFrame object
  job = jni_env->NewObject(jc, initMid);
  if(!job){
    LOG_DIR(m_logdir.c_str(), ERROR,"new subclass  failed! no memory?");
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
    LOG_DIR(m_logdir.c_str(), ERROR,"fillJVideoOfJPG cannot get field,field ID:");
    return false;
  }
  long frame_ms_ = f->frame_ms_;
  jni_env->SetLongField(job, fid, jlong(frame_ms_));
  //buf_
  fid = jni_env->GetFieldID(jc, "buf_", BYTEARRAY);
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get field,field ID::%u",fid);
    return false;
  }
  const unsigned char* buf_ = f->buf_;
  long bufSize_ = f->bufSize_;

  jbArr = jni_env->NewByteArray(bufSize_);
  if(!jbArr) {
    LOG_DIR(m_logdir.c_str(), ERROR, "NewByteArray failed, no memory??");
    return false;
  }
  jbyte *jby = jni_env->GetByteArrayElements(jbArr,0); 
  memcpy(jby, buf_, bufSize_);
  jni_env->SetByteArrayRegion(jbArr, 0, bufSize_, jby);
  jni_env->SetObjectField(job, fid, jbArr);
  //bufSize_
  fid = jni_env->GetFieldID(jc, "bufSize_", LONG_SIGNATURE);
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get field,field ID::%u",fid);
    return false;
  }
  jni_env->SetLongField(job, fid, jlong(bufSize_));

  //5.get subclass field
  fid = jni_env->GetFieldID(jcVideoFrame, "jpg", VIDEOFRAME_JPG_SIGNATURE);
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get VIDEOFRAME_JPG_SIGNATURE");
    return false;
  }
  //6.fill jobVideFrame
  jni_env->SetObjectField(jobVideoFrame, fid, job);
  return  true;
}
bool AgoraJniProxySdk::fillJVideoOfH264(JNIEnv* jni_env, const agora::linuxsdk::VideoFrame*& frame, jclass& jcVideoFrame, jobject& jobVideoFrame) const{
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
  jc = jni_env->FindClass("Lio/agora/recording/common/Common$VideoH264Frame;");
  if(!jc) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot find subclass, type:%d",static_cast<int>(frame->type));
    return false;
  }
  //2.get subclass init method
  initMid = jni_env->GetMethodID(jc,"<init>","(Lio/agora/recording/common/Common;)V");
  if(!initMid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get init methodid");
    return false;
  }
  //3.new VideoH264Frame object
  job = jni_env->NewObject(jc, initMid);
  if(!job){
    LOG_DIR(m_logdir.c_str(), ERROR,"new subclass  failed! no memory?");
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
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get field,field ID::%d",fid);
    return false;
  }
  long frame_ms_ = f->frame_ms_;
  jni_env->SetLongField(job, fid, jlong(frame_ms_));
  //frame_num_
  fid = jni_env->GetFieldID(jc, "frame_num_", LONG_SIGNATURE);
  if(!fid) {
    //TODO add field ID
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get field,field ID:%d",fid);
    return false;
  }
  long frame_num_ = f->frame_num_;
  jni_env->SetLongField(job, fid, jlong(frame_num_));

  //buf_
  fid = jni_env->GetFieldID(jc, "buf_", BYTEARRAY);
  if(fid == NULL) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get field,field ID:%d",fieldId);
    return false;
  }
  const unsigned char* buf_ = f->buf_;
  long bufSize_ = f->bufSize_;

  jbArr = jni_env->NewByteArray(bufSize_);
  if(!jbArr) {
    LOG_DIR(m_logdir.c_str(), ERROR,"NewByteArray failed, no memory??");
    return false;
  }
  jbyte *jby = jni_env->GetByteArrayElements(jbArr,0); 
  memcpy(jby, buf_, bufSize_);
  jni_env->SetByteArrayRegion(jbArr, 0, bufSize_, jby);
  jni_env->SetObjectField(job, fid, jbArr);
  LOG_DIR(m_logdir.c_str(), ERROR,"set buf_ value  ok");
  //bufSize_
  fid = jni_env->GetFieldID(jc, "bufSize_", LONG_SIGNATURE);
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get field,field ID:%s",fieldId);
    return false;
  }
  jni_env->SetLongField(job, fid, jlong(bufSize_));

  //5.get subclass field
  fid = jni_env->GetFieldID(jcVideoFrame, "h264", VIDEOFRAME_H264_SIGNATURE);
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get VIDEOFRAME_H264_SIGNATURE");
    return false;
  }
  //6.fill jobVideFrame
  jni_env->SetObjectField(jobVideoFrame, fid, job);
  return  true;
}
bool AgoraJniProxySdk::fillJVideoFrameByFields(JNIEnv* jni_env, const agora::linuxsdk::VideoFrame*& frame, jclass& jcVideoFrame, jobject& jobVideoFrame) const {
  CHECK_PTR_RETURN_BOOL(m_jobAgoraJavaRecording);
  bool ret = false;
  jfieldID jfid = NULL;
  jclass jc = NULL;
  jmethodID initMid = NULL;
  jfieldID fid = NULL;

  if(!jni_env || !frame || !jcVideoFrame || !jobVideoFrame){
    LOG_DIR(m_logdir.c_str(), ERROR,"AgoraJniProxySdk::fillJVideoFrameByFields para error!");
    return ret;
  }
  jobject job = NULL;
  #if 1
  if (frame->type == agora::linuxsdk::VIDEO_FRAME_RAW_YUV) {
    //3.1
    if(!fillJVideoOfYUV(jni_env, frame, jcVideoFrame, jobVideoFrame)){
      LOG_DIR(m_logdir.c_str(), INFO,"fill subclass falied!");
      if(job)
        jni_env->DeleteLocalRef(job);
      return false;
    }
  }else if(frame->type == agora::linuxsdk::VIDEO_FRAME_JPG){
    //3.2
    if(!fillJVideoOfJPG(jni_env, frame, jcVideoFrame, jobVideoFrame)) {
      LOG_DIR(m_logdir.c_str(), INFO,"fill subclass falied!");
      if(job)
        jni_env->DeleteLocalRef(job);
      return false;
    }
  }else{
    //3.3
    if(!fillJVideoOfH264(jni_env, frame, jcVideoFrame, jobVideoFrame))
    {
      LOG_DIR(m_logdir.c_str(), INFO,"fillJVideoOfH264 failed!");
      return false;
    }
  }
  #endif 
  //fill VIDEO_FRAME_TYPE
  //set type of VIDEO_FRAME_TYPE by FindClass
  jc = jni_env->FindClass("Lio/agora/recording/common/Common$VIDEO_FRAME_TYPE;");
  if(!jc) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get VIDEO_FRAME_TYPE class");
    return false;
  }
  //4.1.1
  initMid = jni_env->GetMethodID(jc,"<init>","(Lio/agora/recording/common/Common;)V");
  if(!initMid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"get init methid failed!");
    return false;
  }
  //4.1.2
  job = jni_env->NewObject(jc, initMid);
  //4.1.3 get field of this class
  fid = jni_env->GetFieldID(jc, "type", "I");
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get value of VIDEO_FRAME_TYPE class");
    return false;
  }
  //4.1.4 fill this field
  int iVideoFrameType = static_cast<int>(frame->type);
  jni_env->SetIntField(job, fid, jint(iVideoFrameType));
  //4.2
  //set this object into jobAudioFrame!
  //step 1:get this object field
  fid = jni_env->GetFieldID(jcVideoFrame, "type", VIDEO_FRAME_TYPE_SIGNATURE);
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get VIDEO_FORMAT_TYPE field");
    return false;
  }
  //4.3
  jni_env->SetObjectField(jobVideoFrame, fid, job);
  return true;
}
void AgoraJniProxySdk::videoFrameReceived(unsigned int uid, const agora::linuxsdk::VideoFrame *frame) const {
  CHECK_PTR_RETURN(m_jobAgoraJavaRecording);
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
		LOG_DIR(m_logdir.c_str(), ERROR,"GetEnv failed");
		return;
	}
	assert(jni_env);
	pid_t tid = getpid();
  //1.find VideoFrame class
  jcVideoFrame = jni_env->FindClass("io/agora/recording/common/Common$VideoFrame");
  if(!jcVideoFrame) {
    LOG_DIR(m_logdir.c_str(), ERROR,"videoFrameReceived cannot find class VideoFrame!");
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  //2.get init methodid to new VideoFrame object
  initMid = jni_env->GetMethodID(jcVideoFrame,"<init>","(Lio/agora/recording/common/Common;)V");
  if(!initMid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get VideoFrmae init method ");
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  //new VideoFrame object
  jobVideoFrame = jni_env->NewObject(jcVideoFrame, initMid);
  if(!jobVideoFrame) {
    LOG_DIR(m_logdir.c_str(), ERROR,"new video frame object failed!");
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  //3.get subclass or enumm details and fill these' fields
  if(!fillJVideoFrameByFields(jni_env, frame, jcVideoFrame, jobVideoFrame))
  {
    LOG_DIR(m_logdir.c_str(), ERROR,"jni fillJVideoFrameByFields failed!" );
    //TODO delete ref
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  //4.find class where callback function in
  //5. find callback function
  jmid = jni_env->GetMethodID(m_jcAgoraJavaRecording, "videoFrameReceived", "(JLio/agora/recording/common/Common$VideoFrame;)V");
  if(!jmid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get callback function");
    //TODO memory leak..
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  //so far so well
  //6. call back java method, para is jobVideoFrame
  jni_env->CallVoidMethod(m_jobAgoraJavaRecording, jmid,jlong(long(uid)), jobVideoFrame);
  
  ((JavaVM*)g_jvm)->DetachCurrentThread();
  return;
}
//TODO  use the same parameter
void AgoraJniProxySdk::audioFrameReceived(unsigned int uid, const agora::linuxsdk::AudioFrame *frame) const {
  static int count = 0;
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
		LOG_DIR(m_logdir.c_str(), ERROR,"GetEnv failed");
		return;
	}
	assert(jni_env);
	pid_t tid = getpid();
  jclass jcAudioFrame = NULL;

  jcAudioFrame = (jni_env)->FindClass("io/agora/recording/common/Common$AudioFrame");
  if(!jcAudioFrame){
    LOG_DIR(m_logdir.c_str(), ERROR,"not find audioFrame");
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return ;
  }
  iAudioFrameType = static_cast<int>(frame->type);
  //2.get main class init methodid
  initMid = jni_env->GetMethodID(jcAudioFrame,"<init>","(Lio/agora/recording/common/Common;)V");
  if(!initMid){
    LOG_DIR(m_logdir.c_str(), ERROR,"not find AudioFrameOfPcm initMid!");
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  //3.new main class object
  jobject jobAudioFrame = jni_env->NewObject(jcAudioFrame, initMid);
  if(!jobAudioFrame){
    LOG_DIR(m_logdir.c_str(), ERROR,"new audio frame object failed!");
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  if(!fillJAudioFrameByFields(jni_env, frame, jcAudioFrame,jobAudioFrame)) {
    LOG_DIR(m_logdir.c_str(), ERROR,"fillJAudioFrameByFields failed!" );
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  
  //4.get main class field
  //4.1.find subclass in main class
  //set type of AUDIO_FRAME_TYPE by FindClass
  jc = jni_env->FindClass("Lio/agora/recording/common/Common$AUDIO_FRAME_TYPE;");
  if(!jc) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get AUDIO_FRAME_TYPE class");
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  //4.1.1
  initMid = jni_env->GetMethodID(jc,"<init>","(Lio/agora/recording/common/Common;)V");
  if(!initMid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"get init methid failed!");
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  //4.1.2
  job = jni_env->NewObject(jc, initMid);
  //4.1.3 get field of this class
  fid = jni_env->GetFieldID(jc, "type", "I");
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get value of AUDIO_FRAME_TYPE class");
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  //4.1.4 fill this field
  jni_env->SetIntField(job, fid, jint(iAudioFrameType));
  //4.2
  //set this object into jobAudioFrame!
  //step 1:get this object field
  fid = jni_env->GetFieldID(jcAudioFrame, "type", "Lio/agora/recording/common/Common$AUDIO_FRAME_TYPE;");
  if(!fid) {
    LOG_DIR(m_logdir.c_str(), ERROR,"cannot get AUDIO_FRAME_TYPE field");
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  //4.3
  jni_env->SetObjectField(jobAudioFrame, fid, job);
 //6.find callback method in Java ---wrong, should use thisObj
  mid = jni_env->GetMethodID(m_jcAgoraJavaRecording, "audioFrameReceived", "(JLio/agora/recording/common/Common$AudioFrame;)V");
  if(!mid){
    LOG_DIR(m_logdir.c_str(), ERROR,"audioFrameReceived get method failed!");
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return ;
  }
  //7. callback java method
  jni_env->CallVoidMethod(m_jobAgoraJavaRecording, mid,jlong(long(uid)), jobAudioFrame);
  return;
}

void AgoraJniProxySdk::onUserJoined(agora::linuxsdk::uid_t uid, agora::linuxsdk::UserJoinInfos &infos) {
  LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk User:%u" ,uid , " joined, RecordingDir:%s" , (infos.storageDir? infos.storageDir:"NULL") );
  CHECK_PTR_RETURN(m_jobAgoraJavaRecording);
  std::string store_dir = std::string(infos.storageDir);
  m_logdir = store_dir;
	JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
	if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK) {
		LOG_DIR(m_logdir.c_str(), ERROR,"GetEnv failed");
		return;
	}
  assert(jni_env);
	//int tid = GetCurrentThreadId();
	pid_t tid = getpid();
  jmethodID jUserJoinedMid =  jni_env->GetMethodID(m_jcAgoraJavaRecording,"onUserJoined","(JLjava/lang/String;)V");
  assert(jUserJoinedMid);
  if(!jUserJoinedMid){
    LOG_DIR(m_logdir.c_str(), ERROR,"jUserJoinedMid is null");
    ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  jstring jstrRecordingDir = jni_env->NewStringUTF(store_dir.c_str());
  jni_env->CallVoidMethod(m_jobAgoraJavaRecording, jUserJoinedMid, jlong((long)(uid)),jstrRecordingDir);
	((JavaVM*)g_jvm)->DetachCurrentThread();
  return;
}
void AgoraJniProxySdk::onUserOffline(agora::linuxsdk::uid_t uid, agora::linuxsdk::USER_OFFLINE_REASON_TYPE reason) {
  LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk onUserOffline User:%u",uid, ",reason:%d",static_cast<int>(reason));
  CHECK_PTR_RETURN(m_jobAgoraJavaRecording);
	JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
	if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK) {
		LOG_DIR(m_logdir.c_str(), ERROR,"GetEnv failed");
		return;
	}
  assert(jni_env);
	//int tid = GetCurrentThreadId();
	//pid_t tid = getpid();
	//LOG_DIR(m_logdir.c_str(), INFO,"onUserOffline this thread id is:%u",tid);
  jmethodID jUserOfflineMid =  jni_env->GetMethodID(m_jcAgoraJavaRecording,"onUserOffline","(JI)V");
  if(!jUserOfflineMid){
    LOG_DIR(m_logdir.c_str(), ERROR,"cnnot find jUserOfflineMid" );
	  ((JavaVM*)g_jvm)->DetachCurrentThread();
    return;
  }
  jni_env->CallVoidMethod(m_jobAgoraJavaRecording, jUserOfflineMid, jlong((long)(uid)),jint(int(reason)));

	((JavaVM*)g_jvm)->DetachCurrentThread();
  return;
}
void AgoraJniProxySdk::onLeaveChannel(agora::linuxsdk::LEAVE_PATH_CODE code) {
	LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk onLeaveChannel");
  CHECK_PTR_RETURN(m_jobAgoraJavaRecording);
	JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
	if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK)
	{
		LOG_DIR(m_logdir.c_str(), ERROR,"GetEnv failed");
		return;
	}
  assert(jni_env);
	//pid_t tid = getpid();
	//LOG_DIR(m_logdir.c_str(), INFO,"onLeaveChannel this thread id is:%u",tid);
  jmethodID jLeaveMid =  jni_env->GetMethodID(m_jcAgoraJavaRecording,"onLeaveChannel","(I)V");
  jni_env->CallVoidMethod(m_jobAgoraJavaRecording, jLeaveMid, jint((int)(code)));
	((JavaVM*)g_jvm)->DetachCurrentThread();
  return;
}
void AgoraJniProxySdk::onWarning(int warn) {
  CHECK_PTR_RETURN(m_jobAgoraJavaRecording);
	JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
	if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK) {
		LOG_DIR(m_logdir.c_str(), ERROR,"GetEnv failed");
		return;
	}
  assert(jni_env);
	jclass javaClass = jni_env->FindClass("AgoraJavaRecording");
  assert(javaClass);
	//pid_t tid = getpid();
	//LOG_DIR(m_logdir.c_str(), INFO,"onWarning this thread id is:%u",tid);
  jmethodID jOnWarnMid =  jni_env->GetMethodID(javaClass,"onWarning","(I)V");
  assert(jOnWarnMid);
  jni_env->CallVoidMethod(m_jobAgoraJavaRecording, jOnWarnMid, warn);
  jni_env->DeleteLocalRef(javaClass);
	((JavaVM*)g_jvm)->DetachCurrentThread();
  return;
}

void AgoraJniProxySdk::onError(int error, agora::linuxsdk::STAT_CODE_TYPE stat_code) {
 	LOG_DIR(m_logdir.c_str(), INFO,"AgoraJniProxySdk onError");
  CHECK_PTR_RETURN(m_jobAgoraJavaRecording);
	JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
  if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK) {
		LOG_DIR(m_logdir.c_str(), ERROR,"GetEnv failed");
		return;
	}
  assert(jni_env);
	jclass javaClass = jni_env->FindClass("AgoraJavaRecording");
  assert(javaClass);
	//pid_t tid = getpid();
	//LOG_DIR(m_logdir.c_str(), INFO,"onError this thread id is:%u",tid);
  jmethodID jLeaveMid =  jni_env->GetMethodID(javaClass,"onError","(II)V");
  if(!jLeaveMid) {
    LOG_DIR(m_logdir.c_str(), INFO,"get method onError failed!");
    return;
  }
  jni_env->CallVoidMethod(m_jobAgoraJavaRecording, jLeaveMid, error, jint((int)(stat_code)));
  leaveChannel();

  jni_env->DeleteLocalRef(javaClass);
	((JavaVM*)g_jvm)->DetachCurrentThread();
  return;
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
	g_jvm = vm;
	assert(g_jvm);
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
JNIEXPORT jobject JNICALL Java_AgoraJavaRecording_getProperties(JNIEnv * jni_env, jobject, jlong nativeObjectRef){
  jniproxy::AgoraJniProxySdk* nativeHandle = reinterpret_cast<jniproxy::AgoraJniProxySdk*>(nativeObjectRef);
  if(!nativeHandle){
    return JNI_FALSE;
  }
  jclass jc = jni_env->FindClass("io/agora/recording/common/RecordingEngineProperties");
  if(!jc){
    cout<<"cannot get jclass RecordingEngineProperties!"<<endl;    
    return JNI_FALSE;
  }
  jmethodID initMid = jni_env->GetMethodID(jc,"<init>","(Lio/agora/recording/common/Common;)V");
  if(!initMid){
    cout<<"cannot get RecordingEngineProperties init!"<<endl;
    return JNI_FALSE;
  }
  jobject job = NULL;
  job = jni_env->NewObject(jc, initMid);
  if(!job){
    cout<<"new object failed!"<<endl;
    return JNI_FALSE;
  }
  jfieldID storageFid = jni_env->GetFieldID(jc, "storageDir", "Ljava/lang/String;");
  if(!storageFid){
    cout<<"storageDir fid not found!"<<endl;
    return JNI_FALSE;
  }
  char* storageDir = nativeHandle->getRecorderProperties()->storageDir;
  jstring jstrStorageDir = jni_env->NewStringUTF(storageDir);
  jni_env->SetObjectField(job, storageFid, jstrStorageDir);
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


void AgoraJniProxySdk::stopJavaProc() {
  LOG_DIR(m_logdir.c_str(), WARN,"AgoraJniProxySdk stopJavaProc");
  CHECK_PTR_RETURN(m_jobAgoraJavaRecording);
	JNIEnv* jni_env = NULL;
	((JavaVM*)g_jvm)->AttachCurrentThread((void**)&jni_env, NULL);
  if(((JavaVM*)g_jvm)->GetEnv((void**)&jni_env, JNI_VERSION_1_4) != JNI_OK) {
		LOG_DIR(m_logdir.c_str(), ERROR,"GetEnv failed");
		return;
	}
  assert(jni_env);
	//pid_t tid = getpid();
	//LOG_DIR(m_logdir.c_str(), INFO,"stopJavaProc this thread id is:",tid);
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
  int triggerMode = 0;
  /*
  int width = 0;
  int height = 0;
  int fps = 0;
  int kbps = 0;*/
  string upstreamResolution("640, 360, 15, 400"); 
  pid_t tid = getpid();
	cout<<"enter Java_AgoraJavaRecording_createChannel,pid:"<<tid<<endl;
  
  g_bSignalStop = false;

	signal(SIGQUIT, signal_handler);
  signal(SIGABRT, signal_handler);
  signal(SIGINT, signal_handler);
  signal(SIGPIPE, SIG_IGN);

  //const char* appId = NULL;
  appId = env->GetStringUTFChars(jni_appid,false);
  if(appId.empty()){
    cout<<"get appId is NULL"<<endl;
    return JNI_FALSE;
  }
  //const char* channelKey = NULL;
  channelKey = env->GetStringUTFChars(jni_channelKey,false);
  //const char* channelName = NULL; 
  channelName = env->GetStringUTFChars(jni_channelName,false);
  if(channelName.empty())
    cout<<"channel name is empty!"<<endl;
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
  //3.CHANNEL_PROFILE_TYPE
  jobject channelProfileObject = (env)->GetObjectField(jni_recordingConfig, channelProfileFieldID);
  //assert(channelProfileObject);
  jclass enumClass = env->GetObjectClass(channelProfileObject);
  if(!enumClass) {  
    cout<<"enumClass is null";
    return JNI_FALSE;
  }
  jmethodID getValue = env->GetMethodID(enumClass, "getValue", "()I");
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
  jmethodID getValue2 = env->GetMethodID(streamTypeClass, "getValue", "()I");
  jint streamTypeValue = env->CallIntMethod(streamTypeObject, getValue2);
  streamType = int(streamTypeValue);
  //5.decodeAudio
  jobject jobDecodeAudio = (env)->GetObjectField(jni_recordingConfig, decodeAudioFieldID);
  jclass jcdecodeAudio = env->GetObjectClass(jobDecodeAudio);
  if(!jcdecodeAudio) {
    cout<<"jcdecodeAudio is null";
  }
  jmethodID jmidGetValue = env->GetMethodID(jcdecodeAudio, "getValue", "()I");
  if (!jmidGetValue) {
    cout<<"jmidGetValue not found";
    return JNI_FALSE; /* method not found */
  }
  jint decodeAudioValue = env->CallIntMethod(jobDecodeAudio, jmidGetValue);
  getAudioFrame = int(decodeAudioValue);
  //6.decodeVideo
  jobject jobDecodeVideo = (env)->GetObjectField(jni_recordingConfig, decodeVideoFieldID);
  jclass jcdecodeVideo = env->GetObjectClass(jobDecodeVideo);
  if(!jcdecodeVideo) {
    cout<<"jcdecodeVideo is null";
  }
  jmidGetValue = env->GetMethodID(jcdecodeVideo, "getValue", "()I");
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

  //11.mixedVideoAudio
  jboolean jmixedVideoAudio = env->GetBooleanField(jni_recordingConfig, mixedVideoAudioFid);
   mixedVideoAudio = bool(jmixedVideoAudio);

  //12.recordFileRootDir
  jstring jrecordFileRootDir = (jstring)env->GetObjectField(jni_recordingConfig, recordFileRootDirFid);
  const char * c_recordFileRootDir = env->GetStringUTFChars(jrecordFileRootDir, NULL);
  recordFileRootDir = c_recordFileRootDir;
  env->ReleaseStringUTFChars(jrecordFileRootDir, c_recordFileRootDir);

  //14.cfgFilePath
  jstring jcfgFilePath = (jstring)env->GetObjectField(jni_recordingConfig, cfgFilePathFid);
  const char * c_cfgFilePath = env->GetStringUTFChars(jcfgFilePath, NULL);
  cfgFilePath = c_cfgFilePath;
  env->ReleaseStringUTFChars(jcfgFilePath, c_cfgFilePath);
  //15.secret
  jstring jsecret = (jstring)env->GetObjectField(jni_recordingConfig, secretFid);
  const char * c_secret = env->GetStringUTFChars(jsecret, NULL);
  secret = c_secret;
  env->ReleaseStringUTFChars(jsecret, c_secret);
      
  //16.decryptionMode
  jstring jdecryptionMode = (jstring)env->GetObjectField(jni_recordingConfig, decryptionModeFid);
  const char * c_decryptionMode = env->GetStringUTFChars(jdecryptionMode, NULL);
  decryptionMode = c_decryptionMode;
  env->ReleaseStringUTFChars(jdecryptionMode, c_decryptionMode);
      
  //17.lowUdpPort
  lowUdpPort = (int)env->GetIntField(jni_recordingConfig, lowUdpPortFid); 
  //18.highUdpPort
  highUdpPort = (int)env->GetIntField(jni_recordingConfig, highUdpPortFid); 
  //19.captureInterval
  captureInterval = (int)env->GetIntField(jni_recordingConfig, captureIntervalFid); 
  //20.triggerMode
  triggerMode = (int)env->GetIntField(jni_recordingConfig, triggerModeFid); 
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
  agora::base::log_config::setFacility(agora::base::LOCAL5_LOG_FCLT);
  if(!jniRecorder.createChannel(appId, channelKey, channelName, uid, config))
  {
    cerr << "Failed to create agora channel: " << channelName <<endl;
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
  while (!jniRecorder.stopped() && !g_bSignalStop) {

    usleep(10000); //10ms
  }
  if (g_bSignalStop) {
    jniRecorder.leaveChannel();
    jniRecorder.release();
    cout<<"jni layer stopped!";
  }
  jniRecorder.stopJavaProc();
  cout<<"Java_AgoraJavaRecording_createChannel  end"<<endl;
  return JNI_TRUE;
 
}
#ifdef __cplusplus
}
#endif
