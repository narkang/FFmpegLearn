
#ifndef MYMUSIC_WLCALLJAVA_H
#define MYMUSIC_WLCALLJAVA_H

#include "jni.h"
#include <linux/stddef.h>
#include "AndroidLog.h"

#define MAIN_THREAD 0
#define CHILD_THREAD 1


class CallJava {

public:
    _JavaVM *javaVM = NULL;
    JNIEnv *jniEnv = NULL;
    jobject jobj;

    jmethodID jmid_parpared;

public:
    CallJava(_JavaVM *javaVM, JNIEnv *env, jobject *obj);
    ~CallJava();

    void onCallParpared(int type);

};


#endif //MYMUSIC_WLCALLJAVA_H
