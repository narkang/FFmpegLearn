//
// Created by 24137 on 2020/2/19.
//

#include "JNICallback.h"
#include "AndroidLog.h"

JNICallback::JNICallback(JavaVM *javaVm, JNIEnv *env, jobject instance) {

    this->javaVm = javaVm;
    this->env = env;
    this->instance = env->NewGlobalRef(instance);

    jclass mRubyPlayerClass = env->GetObjectClass(this->instance);
    const char *sig = "()V";
    jmd_repared = env -> GetMethodID(mRubyPlayerClass, "onPrepared", sig);

    sig = "(I)V";
    jmd_error = env -> GetMethodID(mRubyPlayerClass, "onError", sig);

    sig = "(I)V";
    jmd_onProgress = env->GetMethodID(mRubyPlayerClass, "onProgress", sig);
}

/**
 * 析构函数：专门完成释放的工作
 */
JNICallback::~JNICallback() {
    javaVm = 0;
    env->DeleteGlobalRef(instance);
    instance = 0;
    env = 0;
}

void JNICallback::onPrepared(int thread_mode) {

    if(thread_mode == THREAD_MAIN){
        //主线程
        if(env)
            env->CallVoidMethod(this->instance, jmd_repared);
    } else{
        // 子线程
        // 用附加native线程到JVM的方式，来获得权限的env
        // 是子线程的env
        if(javaVm){
            JNIEnv * jniEnv = nullptr ;
            jint ret = javaVm->AttachCurrentThread(&jniEnv, 0);
            if(ret != JNI_OK){
                return;
            }
            jniEnv->CallVoidMethod(this->instance, jmd_repared);
            javaVm->DetachCurrentThread(); //解除附加
        }
    }

}

void JNICallback::onErrorAction(int thread_mode, int error_code) {

    if(thread_mode == THREAD_MAIN){
        //主线程
        if(env)
            env->CallVoidMethod(this->instance, jmd_error);
    } else{
        // 子线程
        // 用附加native线程到JVM的方式，来获得权限的env
        // 是子线程的env
        if(javaVm){
            JNIEnv * jniEnv = nullptr ;
            jint ret = javaVm->AttachCurrentThread(&jniEnv, 0);
            if(ret != JNI_OK){
                return;
            }
            jniEnv->CallVoidMethod(this->instance, jmd_error);
            javaVm->DetachCurrentThread(); //解除附加
        }
    }

}

void JNICallback::onProgress(int threadMode, int progress) {
    if (threadMode == THREAD_MAIN) {
        //主线程
        if(env)
            env->CallVoidMethod(instance, jmd_onProgress);
    } else {
        //子线程
        //当前子线程的 JNIEnv
        if(javaVm){
            JNIEnv * jniEnv = nullptr ;
            jint ret = javaVm->AttachCurrentThread(&jniEnv, 0);
            if(ret != JNI_OK){
                LOGE("进到里面了onProgress");
                return;
            }
            jniEnv->CallVoidMethod(this->instance, jmd_onProgress, progress);
            javaVm->DetachCurrentThread(); //解除附加
        }
    }
}