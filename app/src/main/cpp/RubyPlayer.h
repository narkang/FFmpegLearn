//
// Created by 24137 on 2020/2/19.
//

#ifndef FFMPEGDEMO_RUBYPLAYER_H
#define FFMPEGDEMO_RUBYPLAYER_H

#include <cstring>
#include "AudioChannel.h"
#include "VideoChannel.h"
#include "JNICallback.h"
#include "macro.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavutil/time.h>
}

class RubyPlayer {
    friend void *task_stop(void *args) ;
public:
    RubyPlayer();

    // RubyPlayer(const char *data_source);

    RubyPlayer(const char *data_source, JNICallback *pCallback);

    ~RubyPlayer(); // 这个类没有子类，所以没有添加虚函数

    void prepare();

    void prepare_();

    void start();

    void start_();

    void stop();

    void setRenderCallback(RenderCallback renderCallback);

    int getDuration() const;

    void setDuration(int duration);

private:
    char *data_source = 0;

    pthread_t pid_prepare ;
    pthread_t pid_start;
    pthread_t pid_stop;

    AVFormatContext *formatContext = 0;
    AudioChannel *audioChannel = 0;
    VideoChannel *videoChannel = 0;
    JNICallback *pCallback;
    bool isPlaying;

    RenderCallback  renderCallback;

    int duration;  // 总时长;
    pthread_mutex_t seekMutex;
};


#endif //FFMPEGDEMO_RUBYPLAYER_H
