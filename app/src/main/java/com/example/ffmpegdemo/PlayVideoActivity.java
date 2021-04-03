package com.example.ffmpegdemo;

import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.view.WindowManager;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;

import java.io.File;

public class PlayVideoActivity extends AppCompatActivity {

    private final static String PATH = Environment.getExternalStorageDirectory() + File.separator + "Demo2.mp4";

    private RubyPlayer rubyPlayer;

    private SurfaceView surfaceView;

    private SeekBar seekBar;//进度条-与播放总时长挂钩

    private boolean isTouch = false;

    private boolean isSeek ;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON,
                WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setContentView(R.layout.activity_play_vido);

        surfaceView = findViewById(R.id.surface_view);

        seekBar = findViewById(R.id.seekBar);

        rubyPlayer = new RubyPlayer();

        rubyPlayer.setSurfaceView(surfaceView);

        //调试ffmpeg是否成功
        Toast.makeText(this, "FFmpeg" + rubyPlayer.getFFmpegVersion(), Toast.LENGTH_SHORT).show();

        rubyPlayer.setDataSource(PATH);
        rubyPlayer.setOnPreparedListener(new RubyPlayer.OnPreparedListener() {
            @Override
            public void onPrepared() {
                final int duration = rubyPlayer.getDuration();
                //如果是直播，duration是0
                //不为0，可以显示seekbar
                if (duration != 0) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            seekBar.setVisibility(View.VISIBLE);
                        }
                    });
                }
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        new AlertDialog.Builder(PlayVideoActivity.this)
                                .setTitle("UI")
                                .setMessage("准备好了，开始播放 ...")
                                .setPositiveButton("老夫知道了", null)
                                .show();

                        TextView textView = findViewById(R.id.tv);
                        textView.setText("播放时长：" + duration + "s");
                    }
                });
                //准备好了，开始播放
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

            @Override
            public void onProgress(final int progress) {
                //progress: 当前的播放进度
//                Log.e("ruby", "progress: " + progress);
                //duration

                //非人为干预进度条，让进度条自然的正常播放
                if (!isTouch) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            int duration = rubyPlayer.getDuration();
//                            Log.e("ruby", "duration: " + duration);
                            if (duration != 0) {
                                if (isSeek) {
                                    isSeek = false;
                                    return;
                                }
                                seekBar.setProgress(progress * 100 / duration);
                            }
                        }
                    });
                }
            }
        });


//        findViewById(R.id.btn_start).setOnClickListener(new View.OnClickListener() {
//            @Override
//            public void onClick(View v) {
//                rubyPlayer.setDataSource(PATH);
//                rubyPlayer.prepare();
//            }
//        });
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
