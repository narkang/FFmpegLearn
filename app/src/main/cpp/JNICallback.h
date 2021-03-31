//
// Created by 24137 on 2020/2/19.
//

#ifndef FFMPEGDEMO_JNICALLBACK_H
#define FFMPEGDEMO_JNICALLBACK_H

#include <jni.h>
#include "macro.h"

class JNICallback {
public:
    JNICallback(JavaVM *javaVm, JNIEnv *env, jobject instance);
    ~JNICallback();

    void onPrepared(int thread_mode);
    void onErrorAction(int thread_mode, int error_code);

private:
    JavaVM *javaVm = 0;
    JNIEnv *env = 0;
    jobject instance;

    jmethodID jmd_repared;
    jmethodID jmd_error;
};


#endif //FFMPEGDEMO_JNICALLBACK_H
