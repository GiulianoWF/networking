//
// Created by giuliano on 06/08/2021.
//

#ifndef BACKEND_LOGS_H
#define BACKEND_LOGS_H
#include <android/log.h>

#define TAG "MY_TAG"

#define LOGG(tag, message) __android_log_print(ANDROID_LOG_ERROR, tag, "%s", message.c_str());

#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,    TAG, "%s", __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,     TAG, "%s", __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,     TAG, "%s", __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,    TAG, "%s", __VA_ARGS__)

#endif //BACKEND_LOGS_H
