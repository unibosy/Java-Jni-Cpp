#ifndef _IAGORA_SERVERENGINE_H_
#define _IAGORA_SERVERENGINE_H_
#include "IAgoraLinuxSdkCommon.h"
#include "IAgoraRecordingEngine.h"

namespace agora {
namespace server {

class IServerEngineEventHandler: public recording::IRecordingEngineEventHandler {
public:
    virtual ~IServerEngineEventHandler() {}
};

typedef struct ServerConfig : public agora::recording::RecordingConfig {
public:
    char *upstreamResolution;
    ServerConfig() : upstreamResolution(NULL) {}

} ServerConfig;

typedef struct ServerEngineProperties: public agora::recording::RecordingEngineProperties{
public:
    virtual ~ServerEngineProperties() {}
    ServerEngineProperties(): agora::recording::RecordingEngineProperties() {}

}ServerEngineProperties;

class IServerEngine: public agora::recording::IRecordingEngine {
public:

    /**
     *  create a new server engine instance
     *
     *  @param appId        The App ID issued to the application developers by Agora.io.
     *  @param eventHandler the callback interface
     *
     *  @return a server engine instance pointer
     */
    static IServerEngine* createAgoraServerEngine(const char * appId, IServerEngineEventHandler *eventHandler);
    virtual ~IServerEngine() {}

    /**
     *  This method lets the server engine join a channel, and start server
     *
     *  @param channelKey This parameter is optional if the user uses a static key, or App ID. In this case, pass NULL as the parameter value. More details refer to http://docs-origin.agora.io/en/user_guide/Component_and_Others/Dynamic_Key_User_Guide.html
     *  @param channelId  A string providing the unique channel id for the AgoraRTC session
     *  @param uid        The uid of server client
     *  @param config     The config of current server
     *
     *  @return 0: Method call succeeded. <0: Method call failed.
     */
    virtual int joinChannel(const char * channelKey, const char *channelId, uid_t uid, const ServerConfig &config) = 0;

    /**
     * Get server properties
     */
    virtual const ServerEngineProperties* getProperties() = 0;

    /**
     *  interface when send a audio frame
     *
     *
     *  @param uid          user ID
     *  @param frame        pointer to sending audio frame    
     *
     */
    virtual void audioFrameSent(const agora::linuxsdk::AudioFrame *pframe) const = 0;

    /**
     *  interface when send a video frame
     *
     *
     *  @param uid          user ID
     *  @param frame        pointer to sending video frame    
     *
     */
    virtual void videoFrameSent(const agora::linuxsdk::VideoFrame *pframe) const = 0;
};

}
}

#endif
