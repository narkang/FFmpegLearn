#ifndef MYMUSIC_WLFFMPEG_H
#define MYMUSIC_WLFFMPEG_H

#include "CallJava.h"
#include "pthread.h"
#include "Audio.h"
#include "Playstatus.h"

extern "C"
{
#include "libavformat/avformat.h"
};


class FFmpeg {

public:
    CallJava *callJava = NULL;
    const char* url = NULL;
    pthread_t decodeThread;
    AVFormatContext *pFormatCtx = NULL;
    Audio *audio = NULL;
    Playstatus *playstatus = NULL;

public:
    FFmpeg(Playstatus *playstatus, CallJava *callJava, const char *url);
    ~FFmpeg();

    void parpared();
    void decodeFFmpegThread();
    void start();

};


#endif //MYMUSIC_WLFFMPEG_H
