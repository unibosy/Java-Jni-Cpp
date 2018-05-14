#pragma once

#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include <cinttypes>
#include <cstdint>
#include <cstdlib>
#include <stdarg.h>
#include <cassert>
#include <syslog.h>
#include "base/atomic.h"
#include "base/mutexer.h"
#include "IAgoraLinuxSdkCommon.h"

namespace agora {
namespace base {

enum agora_log_module{
    AGORA_LOG_MODULE_MEDIA_FILE = agora::linuxsdk::AGORA_LOG_MODULE_MEDIA_FILE,
    AGORA_LOG_MODULE_RECORDING_ENGINE = agora::linuxsdk::AGORA_LOG_MODULE_RECORDING_ENGINE,
    AGORA_LOG_MODULE_RTMP_RENDER = agora::linuxsdk::AGORA_LOG_MODULE_RTMP_RENDER,
    AGORA_LOG_MODULE_IPC = agora::linuxsdk::AGORA_LOG_MODULE_IPC,
    AGORA_LOG_MODULE_CORE_SERVICE_HANDLER = agora::linuxsdk::AGORA_LOG_MODULE_CORE_SERVICE_HANDLER,
    AGORA_LOG_MODULE_COMMON = agora::linuxsdk::AGORA_LOG_MODULE_COMMON,
    AGORA_LOG_MODULE_ANY = agora::linuxsdk::AGORA_LOG_MODULE_ANY 
};

enum log_levels{
    DEBUG_LOG = agora::linuxsdk::AGORA_LOG_LEVEL_DEBUG,
    INFO_LOG = agora::linuxsdk::AGORA_LOG_LEVEL_INFO,
    NOTICE_LOG = agora::linuxsdk::AGORA_LOG_LEVEL_NOTICE, 
    WARN_LOG = agora::linuxsdk::AGORA_LOG_LEVEL_WARN,
    ERROR_LOG = agora::linuxsdk::AGORA_LOG_LEVEL_ERROR, 
    FATAL_LOG = agora::linuxsdk::AGORA_LOG_LEVEL_FATAL 
};

enum log_facility {
    CRON_LOG_FCLT = LOG_CRON,
    DAEMON_LOG_FCLT = LOG_DAEMON,
    FTP_LOG_FCLT = LOG_FTP,
    NEWS_LOG_FCLT = LOG_NEWS,
    AUTH_LOG_FCLT = LOG_AUTH,		/* DEPRECATED */
    SYSLOG_LOG_FCLT = LOG_SYSLOG,
    USER_LOG_FCLT = LOG_USER,
    UUCP_LOG_FCLT = LOG_UUCP,

    LOCAL0_LOG_FCLT = LOG_LOCAL0,
    LOCAL1_LOG_FCLT = LOG_LOCAL1,
    LOCAL2_LOG_FCLT = LOG_LOCAL2,
    LOCAL3_LOG_FCLT = LOG_LOCAL3,
    LOCAL4_LOG_FCLT = LOG_LOCAL4,
    LOCAL5_LOG_FCLT = LOG_LOCAL5,
    LOCAL6_LOG_FCLT = LOG_LOCAL6,
    LOCAL7_LOG_FCLT = LOG_LOCAL7,
    
};

#define LOG_FACILITY_MASK  LOG_FACMASK 

class log_config {
public:
    class config_lock 
    {
    public:
        config_lock() { log_config::lock();}
        ~config_lock() { log_config::unlock();}
    };

    static inline void set_log_level(log_levels level) {
        if(level == enabled_level)
            return;

        config_lock lck;
        enabled_level = level;
    }

    static inline agora::base::log_levels get_log_level() {
        config_lock lck;
        return enabled_level;
    }

    static inline uint32_t get_enabled_modules() {
        config_lock lck;
        return enabled_modules;
    }

    static inline void enable_all_modules() {
        config_lock lck;
        enabled_modules = AGORA_LOG_MODULE_ANY;
    }

    static inline void disable_all_modules() {
        config_lock lck;
        enabled_modules = 0;
    }
    
    static inline void enable_module(uint32_t module) {
        config_lock lck;
        enabled_modules |= module;
    }

    static inline void disable_module(uint32_t module) {
        config_lock lck;
        enabled_modules &= ~module;
    }

    static inline void set_enabled_modules(uint32_t modules) {
        config_lock lck;
        enabled_modules = modules;
    }
    
    static inline bool log_enabled(log_levels level, agora_log_module module) {
        config_lock lck;
        if((level > enabled_level) || ((enabled_modules & module) == 0))
            return false;
        return true;
    }

    static bool set_drop_cannel(uint32_t cancel) {
        if (cancel > DROP_COUNT) {
            drop_cancel = DROP_COUNT;
            return false;
        }

        drop_cancel = cancel;
        return true;
    }

    /**
     *  get log_config current facility 
     *
     *  @return uint32: get log_config current facility .
     */
    static uint32_t getFacility() { return log_config::facility; }

    /**
     * change log_config Facility per your specific purpose like agora::base::LOCAL5_LOG_FCLT
     * Default:USER_LOG_FCLT. 
     * eg,
     * agora::base::log_config::setFacility(agora::base::USER_LOG_FCLT);
     *
     *  @param fac facility setting
     */
    static void setFacility(uint32_t fac) { log_config::facility = fac & LOG_FACILITY_MASK; }

    static inline void lock(){ logger_mutex.lock(); }
    static inline void unlock(){ logger_mutex.unlock(); }
    static inline bool trylock(){ return logger_mutex.trylock(); }

private:
    static Mutexer logger_mutex;
    static log_levels enabled_level;
    static uint32_t enabled_modules;
    static uint32_t drop_cancel;
    const static uint32_t DROP_COUNT = 1000;
    static uint32_t facility;
};



void open_log();
inline void close_log() {
    ::closelog();
}
void log_dir(const char* logdir, log_levels level, agora_log_module module, const char* format, ...);
void log(log_levels level, agora_log_module module, const char* format, ...);


}
}

#define LOG(level, module, fmt, ...) log(agora::base::level ## _LOG,  module,\
        "(%d) %s:%d: " fmt, getpid(), __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_DIR(logdir, level, module, fmt, ...) log_dir(logdir, agora::base::level ## _LOG,  module,\
        "(%d) %s:%d: " fmt, getpid(), __FILE__, __LINE__, ##__VA_ARGS__)

#define LOG_DIR_IF(logdir, level, module, cond, fmt, ...) \
    if (cond) { \
        log_dir(logdir, agora::base::level ## _LOG,   module, \
                "(%d) %s:%d: " fmt, getpid(), __FILE__, __LINE__, ##__VA_ARGS__); \
    }

#define LOG_IF(level, module, cond, ...) \
    if (cond) { \
        LOG(level,  module, __VA_ARGS__); \
    }

#define LOG_EVERY_N(level, module, N, ...) \
{  \
    static unsigned int count = 0; \
    if (++count % N == 0) \
    LOG(level, module, __VA_ARGS__); \
}


#define MF_LOG(level, fmt, ...) LOG(level, agora::base::AGORA_LOG_MODULE_MEDIA_FILE, fmt, ##__VA_ARGS__)
#define MF_LOG_DIR(logdir, level, fmt, ...) LOG_DIR(logdir, level, agora::base::AGORA_LOG_MODULE_MEDIA_FILE, fmt, ##__VA_ARGS__)

#define RE_LOG(level, fmt, ...) LOG(level, agora::base::AGORA_LOG_MODULE_RECORDING_ENGINE, fmt, ##__VA_ARGS__)
#define RE_LOG_DIR(logdir, level, fmt, ...) LOG_DIR(logdir, level, agora::base::AGORA_LOG_MODULE_RECORDING_ENGINE, fmt, ##__VA_ARGS__)

#define RR_LOG(level, fmt, ...) LOG(level, agora::base::AGORA_LOG_MODULE_RTMP_RENDER, fmt, ##__VA_ARGS__)
#define RR_LOG_DIR(logdir, level, fmt, ...) LOG_DIR(logdir, level, agora::base::AGORA_LOG_MODULE_RTMP_RENDER, fmt, ##__VA_ARGS__)

#define IPC_LOG(level, fmt, ...) LOG(level, agora::base::AGORA_LOG_MODULE_IPC, fmt, ##__VA_ARGS__)
#define IPC_LOG_DIR(logdir, level, fmt, ...) LOG_DIR(logdir, level, agora::base::AGORA_LOG_MODULE_IPC, fmt, ##__VA_ARGS__)

#define CSH_LOG(level, fmt, ...) LOG(level, agora::base::AGORA_LOG_MODULE_CORE_SERVICE_HANDLER, fmt, ##__VA_ARGS__)
#define CSH_LOG_DIR(logdir, level, fmt, ...) LOG_DIR(logdir, level, agora::base::AGORA_LOG_MODULE_CORE_SERVICE_HANDLER, fmt, ##__VA_ARGS__)

#define CM_LOG(level, fmt, ...) LOG(level, agora::base::AGORA_LOG_MODULE_COMMON, fmt, ##__VA_ARGS__)
#define CM_LOG_DIR(logdir, level, fmt, ...) LOG_DIR(logdir, level, agora::base::AGORA_LOG_MODULE_COMMON, fmt, ##__VA_ARGS__)



//#define DEBUG_MODE

#ifdef DEBUG_MODE

#define SPLIT_ARR 50
#define DEBUG_LOG_RAW_DATA(logdir, level, module, STR, msg, size, ...) do { \
    uint32_t i;    \
    std::string output(#STR);    \
    output.reserve(size*3);    \
    output.append(" "); \
    for (uint32_t i = 0; i < size/SPLIT_ARR+1; i++) {    \
        uint32_t end = size > (i+1) * SPLIT_ARR?  SPLIT_ARR:(size - (i * SPLIT_ARR)); \
        for(uint32_t j = 0;j < end;j++)    \
        {    \
            uint32_t index = i * SPLIT_ARR + j;    \
            output.append(std::to_string(msg[index])+"_"); \
        }    \
        output.append("@"); \
    }    \
    output.append("\n"); \
    log_dir(logdir, agora::base::level ## _LOG, ## module,\
            "(%d) %s:%d: %s" , getpid(), __FILE__, __LINE__,output.c_str(), ##__VA_ARGS__); \
}while(0)


#define DEBUG_RAW_DATA(STR, msg, size) do {  \
    unsigned int i;  \
    printf("%s", #STR); \
    for (i = 0; i < size; i++) {  \
        if(i % 50 == 0)  \
            printf("\n");\
        else \
            printf("%x", msg[i]); \
    }    \
     \
}while(0)
#else
#define DEBUG_RAW_DATA(STR, msg, size)
#define DEBUG_LOG_RAW_DATA(logdir, level, module, STR, msg, size, ...)
#endif

