package com.example.ffmpegdemo;

import android.os.Bundle;
import android.os.Environment;
import android.view.SurfaceView;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import java.io.File;

public class PlayVideoActivity extends AppCompatActivity {

    private final static String PATH = Environment.getExternalStorageDirectory() + File.separator + "Demo.mp4";

    private RubyPlayer rubyPlayer;

    private SurfaceView surfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_play_rtmp);

        surfaceView = findViewById(R.id.surface_view);

        rubyPlayer = new RubyPlayer();

        rubyPlayer.setSurfaceView(surfaceView);

        //调试ffmpeg是否成功
        Toast.makeText(this, "FFmpeg" + rubyPlayer.getFFmpegVersion(), Toast.LENGTH_SHORT).show();

        rubyPlayer.setDataSource(PATH);
        rubyPlayer.setOnPreparedListener(new RubyPlayer.OnPreparedListener() {
            @Override
            public void onPrepared() {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        new AlertDialog.Builder(PlayVideoActivity.this)
                                .setTitle("UI")
                                .setMessage("准备好了，开始播放 ...")
                                .setPositiveButton("老夫知道了", null)
                                .show();
                    }
                });
                rubyPlayer.start();
            }

            @Override
            public void onError(final String errorText) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        new AlertDialog.Builder(PlayVideoActivity.this)
                                .setTitle("Error")
                                .setMessage("已经发生错误，请查阅:" + errorText)
                                .setPositiveButton("我来个去，什么情况", null)
                                .show();
                    }
                });
            }
        });

    }

    @Override
    protected void onResume() {
        super.onResume();
        rubyPlayer.prepare();
    }

    @Override
    protected void onStop() {
        super.onStop();
        rubyPlayer.stop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        rubyPlayer.release();
    }
}
