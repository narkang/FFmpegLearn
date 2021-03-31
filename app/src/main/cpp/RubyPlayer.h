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
public:
    RubyPlayer();

    // RubyPlayer(const char *data_source);

    RubyPlayer(const char *data_source, JNICallback *pCallback);

    ~RubyPlayer(); // 这个类没有子类，所以没有添加虚函数

    void prepare();

    void prepare_();

    void start();

    void start_();

    void setRenderCallback(RenderCallback renderCallback);

private:
    char *data_source = 0;

    pthread_t pid_prepare ;

    AVFormatContext *formatContext = 0;

    AudioChannel *audioChannel = 0;
    VideoChannel *videoChannel = 0;
    JNICallback *pCallback;
    pthread_t pid_start;
    bool isPlaying;

    RenderCallback  renderCallback;
};


#endif //FFMPEGDEMO_RUBYPLAYER_H
