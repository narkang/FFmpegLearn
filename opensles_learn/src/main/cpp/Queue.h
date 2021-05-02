#ifndef MYMUSIC_WLQUEUE_H
#define MYMUSIC_WLQUEUE_H

#include "queue"
#include "pthread.h"
#include "AndroidLog.h"
#include "Playstatus.h"

extern "C"
{
#include "libavcodec/avcodec.h"
};


class Queue {

public:
    std::queue<AVPacket *> queuePacket;
    pthread_mutex_t mutexPacket;
    pthread_cond_t condPacket;
    Playstatus *playstatus = NULL;

public:

    Queue(Playstatus *playstatus);
    ~Queue();

    int putAvpacket(AVPacket *packet);
    int getAvpacket(AVPacket *packet);

    int getQueueSize();




};


#endif //MYMUSIC_WLQUEUE_H
