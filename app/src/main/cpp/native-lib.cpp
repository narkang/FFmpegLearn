#include <jni.h>
#include <string>
#include "JNICallback.h"
#include "RubyPlayer.h"
#include <android/native_window_jni.h> // 是为了 渲染到屏幕支持的

extern "C" {
#include <libavutil/avutil.h>
}

JavaVM *javaVm = 0;
RubyPlayer *player = 0;
ANativeWindow * nativeWindow = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // 静态初始化 互斥锁


int JNI_OnLoad(JavaVM *javaVm, void *pVoid){
    ::javaVm = javaVm;
    return JNI_VERSION_1_6; // 坑，这里记得一定要返回，和异步线程指针函数一样（记得返回）
}

/**
 * 专门渲染的函数
 * @param src_data 解码后的视频 rgba 数据
 * @param width 宽信息
 * @param height 高信息
 * @param src_liinesize 行数size相关信息
 */
void renderFrame(uint8_t * src_data, int width, int height, int src_liinesize) {
    pthread_mutex_lock(&mutex);

    if (!nativeWindow) {
        pthread_mutex_unlock(&mutex);
        return;
    }

    // 设置窗口属性
    ANativeWindow_setBuffersGeometry(nativeWindow, width, height , WINDOW_FORMAT_RGBA_8888);

    ANativeWindow_Buffer windowBuffer;
    if (ANativeWindow_lock(nativeWindow, &windowBuffer, 0)) {
        ANativeWindow_release(nativeWindow);
        nativeWindow = 0;
        pthread_mutex_unlock(&mutex);
        return;
    }

    // 填数据到buffer，其实就是修改数据
    uint8_t * dst_data = static_cast<uint8_t *>(windowBuffer.bits);
    int lineSize = windowBuffer.stride * 4; // RGBA
    // 下面就是逐行Copy了
    for (int i = 0; i < windowBuffer.height; ++i) {
        // 一行Copy
        memcpy(dst_data + i * lineSize, src_data + i * src_liinesize, lineSize);
    }

    ANativeWindow_unlockAndPost(nativeWindow);
    pthread_mutex_unlock(&mutex);
}

/**
 * 准备工作
 */
extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpegdemo_RubyPlayer_prepareNative(JNIEnv *env, jobject thiz,
                                                     jstring data_source_) {
    //准备工作，首先用来解封装
    JNICallback *jniCallback = new JNICallback(javaVm, env, thiz);
    const char * data_source = env->GetStringUTFChars(data_source_, NULL);
    player = new RubyPlayer(data_source, jniCallback);
    player->setRenderCallback(renderFrame);
    player->prepare();

    // 释放操作
    env->ReleaseStringUTFChars(data_source_, data_source);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpegdemo_RubyPlayer_startNative(JNIEnv *env, jobject thiz) {
    if(player){
        player->start();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpegdemo_RubyPlayer_stopNative(JNIEnv *env, jobject thiz) {

}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpegdemo_RubyPlayer_releaseNative(JNIEnv *env, jobject thiz) {

}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_ffmpegdemo_RubyPlayer_getFFmpegVersion(JNIEnv *env, jobject thiz) {
    return env->NewStringUTF(av_version_info());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_ffmpegdemo_RubyPlayer_setSurfaceNative(JNIEnv *env, jobject thiz,
                                                        jobject surface) {
    pthread_mutex_lock(&mutex);

    if (nativeWindow) {
        ANativeWindow_release(nativeWindow);
        nativeWindow = 0;
    }

    // 创建新的窗口用于视频显示
    nativeWindow = ANativeWindow_fromSurface(env, surface);

    pthread_mutex_unlock(&mutex);
}

