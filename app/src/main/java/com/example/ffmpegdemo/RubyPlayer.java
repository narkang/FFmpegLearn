package com.example.ffmpegdemo;

import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.io.File;

/**
 * 上层核心播放
 */
public class RubyPlayer implements SurfaceHolder.Callback {

    static {
        System.loadLibrary("native-lib");
    }


    public RubyPlayer() {

    }

    // 媒体流的源 (文件路径 、 直播地址（要集成X264才行）)
    private String dataSource;

    public void setDataSource(String dataSource) {
        this.dataSource = dataSource;
    }

    /**
     * 准备工作（解封装-其实就是把.mp4给解出来）
     */
    public void prepare() {
        prepareNative(dataSource);
    }

    /**
     * 开始播放
     */
    public void start() {
        startNative();
    }

    /**
     * 停止播放
     */
    public void stop() {
        stopNative();
    }

    /**
     * 资源释放
     */
    public void release() {
        surfaceHolder.removeCallback(this);
        releaseNative();
    }

    /**
     * 获取总的播放时长
     * @return
     */
    public int getDuration(){
        return getDurationNative();
    }

    /**
     * 给JNI 方式调用的方法
     */
    public void onPrepared() {
        if (null != onPreparedListener) {
            this.onPreparedListener.onPrepared();
        }
    }

    /**
     * 给JNI 方式回调的方法（进行错误的回调）
     */
    public void onError(int errorCode) {
        if (null != onPreparedListener) {
            String errorText = null;

            switch (errorCode) {
                case Flags.FFMPEG_ALLOC_CODEC_CONTEXT_FAIL:
                    errorText = "无法根据解码器创建上下文";
                    break;
                case Flags.FFMPEG_CAN_NOT_FIND_STREAMS:
                    errorText = "找不到媒体流信息";
                    break;
                case Flags.FFMPEG_CAN_NOT_OPEN_URL:
                    errorText = "打不开媒体数据源";
                    break;
                case Flags.FFMPEG_CODEC_CONTEXT_PARAMETERS_FAIL:
                    errorText = "根据流信息 配置上下文参数失败";
                    break;
                case Flags.FFMPEG_FIND_DECODER_FAIL:
                    errorText = "找不到解码器";
                    break;
                case Flags.FFMPEG_NOMEDIA:
                    errorText = "没有音视频";
                    break;
                case Flags.FFMPEG_READ_PACKETS_FAIL:
                    errorText = "读取媒体数据包失败";
                    break;
                default:
                    errorText = "未知错误，自己去检测你的垃圾代码...";
                    break;
            }
            Log.e("TAG", "onError: " + errorCode);
            this.onPreparedListener.onError(errorText);
        }
    }

    // native层传递上来的 进度值
    public void onProgress(int progress) {
        if (null != onPreparedListener) {
            onPreparedListener.onProgress(progress);
        }
    }

    /**
     * MainActivity设置监听，就可以回调到MainActivity的方法，进行UI的操作了
     * @param onPreparedListener
     */
    private OnPreparedListener onPreparedListener;

    public void setOnPreparedListener(OnPreparedListener onPreparedListener) {
        this.onPreparedListener = onPreparedListener;
    }

    interface OnPreparedListener {
        void onPrepared();
        void onError(String errorText);
        void onProgress(int progress);
    }

    // ##############  Native 相关的
    public native void prepareNative(String dataSource);
    public native void startNative();
    public native void stopNative();
    public native void releaseNative();
    public native String getFFmpegVersion();
    public native int getDurationNative();
    private native void seekToNative(int playProgress);
    // ##############  SurfaceView 相关的

    private SurfaceHolder surfaceHolder;

    public void setSurfaceView(SurfaceView surfaceView) {
        if (null != this.surfaceHolder) {
            this.surfaceHolder.removeCallback(this);
        }
        this.surfaceHolder = surfaceView.getHolder();
        this.surfaceHolder.addCallback(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {

    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {
        Surface surface = surfaceHolder.getSurface();
        setSurfaceNative(surface);
    }

    private native void setSurfaceNative(Surface surface);

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {

    }
}
