package com.example.simplecube;

import android.app.Activity;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.util.Log;

import java.io.File;
import java.io.InputStream;
import java.io.RandomAccessFile;

public class SimpleCube extends Activity {
    private static String LOGTAG = "SimpleCube";
    private static String assetDirectory = null;
    private static android.content.Context applicationContext = null;

    protected TutorialView graphicsView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        applicationContext = getApplicationContext();
        assetDirectory = applicationContext.getFilesDir().getPath() + "/";

        extractAsset("normalMap256.raw");

        Log.i(LOGTAG, "Creating New Tutorial View");
        graphicsView = new TutorialView(this);
        setContentView(graphicsView);
    }

    private void extractAsset(String assetName) {
        File fileTest = new File(assetDirectory + assetName);

        if (fileTest.exists()) {
            Log.d(LOGTAG, assetName + " already exists no extraction needed");
        } else {
            Log.d(LOGTAG, assetName + " doesn't exist extraction needed");

            try {
                RandomAccessFile out = new RandomAccessFile(assetDirectory + assetName, "rw");
                AssetManager am = applicationContext.getResources().getAssets();
                InputStream inputStream = am.open(assetName);

                byte[] buffer = new byte[1024];
                int count = inputStream.read(buffer, 0, 1024);

                while (count > 0) {
                    out.write(buffer, 0, count);
                    count = inputStream.read(buffer, 0, 1024);
                }

                out.close();
                inputStream.close();
            } catch (Exception e) {
                Log.e(LOGTAG, "Failure in extractAssets(): " + e + " " + assetDirectory + assetName);
            }

            if (fileTest.exists()) {
                Log.d(LOGTAG, "File Extracted successfully");
            }
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        if (graphicsView != null) {
            graphicsView.onPause();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (graphicsView != null) {
            graphicsView.onResume();
        }
    }
}