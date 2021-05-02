package com.opensles.ffmpegmusicplayer;

import android.os.Bundle;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;

import com.opensles.ffmpegmusicplayer.listener.WlOnParparedListener;
import com.opensles.ffmpegmusicplayer.log.MyLog;
import com.opensles.ffmpegmusicplayer.player.Player;

public class MainActivity extends AppCompatActivity {
    private Player wlPlayer;
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        wlPlayer = new Player();
        wlPlayer.setWlOnParparedListener(new WlOnParparedListener() {
            @Override
            public void onParpared() {
                MyLog.d("准备好了，可以开始播放声音了");
                wlPlayer.start();
            }
        });
    }

    public void begin(View view) {
        wlPlayer.setSource("http://mpge.5nd.com/2015/2015-11-26/69708/1.mp3");
        wlPlayer.parpared();
    }
}
