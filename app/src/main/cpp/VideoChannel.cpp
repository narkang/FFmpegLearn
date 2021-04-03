//
// Created by 24137 on 2020/2/19.
//

#include "VideoChannel.h"

// 丢包的  函数指针 具体实现 frame
void dropAVFrame(queue<AVFrame *> & qq) {
    if (!qq.empty()) {
        AVFrame * frame = qq.front();
        BaseChannel::releaseAVFrame(&frame); // 释放掉
        qq.pop();
    }
}

// 丢包的  函数指针 具体实现 packet
void dropAVPacket(queue<AVPacket *> & qq) {
    if (!qq.empty()) {
        AVPacket* packet = qq.front();
        // BaseChannel::releaseAVPacket(&packet); // 关键帧没有了
        if (packet->flags != AV_PKT_FLAG_KEY) { // AV_PKT_FLAG_KEY == 关键帧
            BaseChannel::releaseAVPacket(&packet);
        }
        qq.pop();
    }
}

VideoChannel::VideoChannel(int stream_index, AVCodecContext *pContext, AVRational rational, int fpsValue, JNICallback *jniCallback)
    : BaseChannel(stream_index, pContext, rational, jniCallback) {
    this->fpsValue = fpsValue;
    this->frames.setSyncCallback(dropAVFrame);
    this->packages.setSyncCallback(dropAVPacket);
}

VideoChannel::~VideoChannel() {

}

void * task_video_decode(void * pVoid) {
    VideoChannel * videoChannel = static_cast<VideoChannel *>(pVoid);
    videoChannel->video_decode();
    return 0;
}

void * task_video_player(void * pVoid) {
    VideoChannel * videoChannel = static_cast<VideoChannel *>(pVoid);
    videoChannel->video_player();
    return 0;
}

/**
 * 真正的解码，并且，播放
 * 1.解码（解码只有的是原始数据）
 * 2.播放
 */
void VideoChannel::start() {
    isPlaying = 1;

    // 存放未解码的队列开始工作了
    packages.setFlag(1);

    // 存放解码后的队列开始工作了
    frames.setFlag(1);

    // 1.解码的线程
    pthread_create(&pid_video_decode, 0, task_video_decode, this);

    // 2.播放的线程
    pthread_create(&pid_video_player, 0, task_video_player, this);
}

void VideoChannel::stop() {
    isPlaying = 0;
    pCallback = 0;
    packages.setFlag(0);
    frames.setFlag(0);
    pthread_join(pid_video_decode, 0);
    pthread_join(pid_video_player, 0);
}

/**
 * 运行在异步线程（视频真正解码函数）
 */
void VideoChannel::video_decode() {
    AVPacket * packet = 0;
    while (isPlaying) {
        // 生产快  消费慢
        // 消费速度比生成速度慢（生成100，只消费10个，这样队列会爆）
        // 内存泄漏点2，解决方案：控制队列大小
        if (isPlaying && frames.queueSize() > 100) {
            // 休眠 等待队列中的数据被消费
            av_usleep(10 * 1000);
            continue;
        }


        int ret = packages.pop(packet);

        // 如果停止播放，跳出循环, 出了循环，就要释放
        if (!isPlaying) {
            break;
        }

        if (!ret) {
            continue;
        }

        // 走到这里，就证明，取到了待解码的视频数据包 avcodec_send_packet 和 avcodec_receive_frame成双出对  解码
        //avcodec_send_frame 和 avcodec_receive_packet成双出对  编码

        ret = avcodec_send_packet(pContext, packet);
        if (ret) {
            // 失败了（给”解码器上下文“发送Packet失败）
            break;
        }

        // 走到这里，就证明，packet，用完毕了，成功了（给“解码器上下文”发送Packet成功），那么就可以释放packet了
        releaseAVPacket(&packet);

        AVFrame * frame = av_frame_alloc(); // AVFrame 拿到解码后的原始数据包
        ret = avcodec_receive_frame(pContext, frame);
        if (ret == AVERROR(EAGAIN)) {
            // 重来，重新取
            continue;
        } else if(ret != 0) {
            releaseAVFrame(&frame); // 内存释放点
            break;
        }

        // 终于取到了，解码后的视频数据（原始数据）
        frames.push(frame); // 加入队列
    }
    // 出了循环，就要释放
    releaseAVPacket(&packet);
}

/**
 * 运行在异步线程（视频真正播放函数）
 */
void VideoChannel::video_player() {
    // 原始图形宽和高，可以通过解码器拿到
    // 1.TODO 把元素的 yuv  --->  rgba
    SwsContext * sws_ctx = sws_getContext(pContext->width, pContext->height, pContext->pix_fmt, // 原始图形信息相关的
                                          pContext->width, pContext->height, AV_PIX_FMT_RGBA, // 目标 尽量 要和 元素的保持一致
                                          SWS_BILINEAR, NULL, NULL, NULL
    );

    // 2.TODO 给dst_data rgba 这种 申请内存
    uint8_t * dst_data[4];
    int dst_linesize[4];
    AVFrame * frame = 0;

    av_image_alloc(dst_data, dst_linesize,
                   pContext->width, pContext->height, AV_PIX_FMT_RGBA, 1);

    // 3.TODO 原始数据 格式转换的函数 （从队列中，拿到（原始）数据，一帧一帧的转换（rgba），一帧一帧的渲染到屏幕上）
    while(isPlaying) {
        int ret = frames.pop(frame);

        // 如果停止播放，跳出循环, 出了循环，就要释放
        if (!isPlaying) {
            break;
        }

        if (!ret) {
            continue;
        }

        // 格式的转换 (yuv --> rgba)   frame->data == yuv是原始数据      dst_data是rgba格式的数据
        sws_scale(sws_ctx, frame->data,
                  frame->linesize, 0, pContext->height, dst_data, dst_linesize);

        // 得到当前帧的额外延时时间 == repeat_pict
        double extra_delay = frame->repeat_pict;

        // 根据FPS得到延时时间
        double base_delay = 1.0 / this->fpsValue;

        // 得到最终 当前帧 时间延时时间
        double result_delay = extra_delay + base_delay;

        // 等一等 音频
        // av_usleep(result_delay * 1000000);

        // 知识控制来 视频慢下来来而已，还没有同步

        // 拿到视频
        this->videoTime = frame->best_effort_timestamp * av_q2d(this->time_base);

        // 拿到音频 播放时间基 audioChannel.audioTime
        double_t audioTime = this->audioChannel->audioTime; // 音频那边的值，是根据它来计算的

        // 计算 音频 和 视频的 差值
        double time_diff = videoTime - audioTime;
        if (time_diff > 0) {
            // 说明：视频快一些，音频慢一些
            // 那就要，比音频播放的时间，久一点才行（等待音频），让音频自己追上来

            // 灵活等待【慢下来，睡眠的方式】
            if (time_diff > 1) {
                av_usleep((result_delay * 2) * 1000000); // 久一点
            } else {
                // 0 ~ 1 说明相差不大
                av_usleep((time_diff + result_delay) * 1000000);
            }

        } else if (time_diff < 0) {
            // 说明：视频慢一写，音频快一些
            // 那就要，比音频播放的时间，快一点才行（追音频），追赶音频
            // 丢帧，把冗余的帧丢弃，就快起来了
            // packtes  frames
            // 同步丢包操作
            // releaseAVFrame(&frame); 这样是不可以的

            this->frames.syncAction();

            // 继续 循环
            continue;

        } else {
            // 百分百完美 音频 视频 同步了
        }

        // 渲染，显示在屏幕上了
        // 渲染的两种方式：
        // 渲染一帧图像（宽，高，数据）
        renderCallback(dst_data[0], pContext->width, pContext->height , dst_linesize[0]);
        releaseAVFrame(&frame); // 渲染完了，frame没用了，释放掉
    }
    releaseAVFrame(&frame);
    isPlaying = 0;

    // 一旦看到 "alloc" / "malloc" 关键字，满足释放条件下，最后一定要尝试 xx_free
    av_freep(&dst_data[0]);
    //如果看到 "xxxContext" 次上下文，通常情况下，也要释放掉
    sws_freeContext(sws_ctx);
}

void VideoChannel::setRenderCallback(RenderCallback renderCallback) {
    this->renderCallback = renderCallback;
}

void VideoChannel::setAudioChannel(AudioChannel *audioChannel) {
    this->audioChannel = audioChannel;
}
