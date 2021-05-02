package com.opensles.ffmpegmusicplayer.player;

import android.text.TextUtils;

import com.opensles.ffmpegmusicplayer.listener.WlOnParparedListener;
import com.opensles.ffmpegmusicplayer.log.MyLog;


public class Player {

    private String source;//数据源
    private WlOnParparedListener mnOnParparedListener;


    public Player()
    {}

    /**
     * 设置数据源
     * @param source
     */
    public void setSource(String source)
    {
        this.source = source;
    }

    /**
     * 设置准备接口回调
     * @param mnOnParparedListener
     */
    public void setWlOnParparedListener(WlOnParparedListener mnOnParparedListener)
    {
        this.mnOnParparedListener = mnOnParparedListener;
    }

    public void parpared()
    {
        if(TextUtils.isEmpty(source))
        {
            MyLog.d("source not be empty");
            return;
        }
        new Thread(new Runnable() {
            @Override
            public void run() {
                n_parpared(source);
            }
        }).start();

    }

    public void start()
    {
        if(TextUtils.isEmpty(source))
        {
            MyLog.d("source is empty");
            return;
        }
        new Thread(new Runnable() {
            @Override
            public void run() {
                n_start();
            }
        }).start();
    }


    /**
     * c++回调java的方法
     */
    public void onCallParpared()
    {
        if(mnOnParparedListener != null)
        {
            mnOnParparedListener.onParpared();
        }
    }

    public native void n_parpared(String source);
    public native void n_start();


}
