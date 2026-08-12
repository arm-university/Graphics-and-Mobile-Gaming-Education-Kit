package com.example.simplecube;

import android.app.Activity;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.util.Log;

import java.io.File;
import java.io.InputStream;
import java.io.RandomAccessFile;

public class SimpleCube extends Activity {
    private static final String LOGTAG = "SimpleCube";
    private static String assetDirectory = null;
    private static android.content.Context applicationContext = null;

    private TutorialView graphicsView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        applicationContext = getApplicationContext();
        assetDirectory = applicationContext.getFilesDir().getPath() + "/";

        extractAsset("level0.raw");
        extractAsset("level1.raw");
        extractAsset("level2.raw");
        extractAsset("level3.raw");
        extractAsset("level4.raw");
        extractAsset("level5.raw");
        extractAsset("level6.raw");
        extractAsset("level7.raw");
        extractAsset("level8.raw");
        extractAsset("level9.raw");

        // Only keep these if you actually have the PKM files too.
        // extractAsset("level0.pkm");
        // extractAsset("level1.pkm");
        // extractAsset("level2.pkm");
        // extractAsset("level3.pkm");
        // extractAsset("level4.pkm");
        // extractAsset("level5.pkm");
        // extractAsset("level6.pkm");
        // extractAsset("level7.pkm");
        // extractAsset("level8.pkm");
        // extractAsset("level9.pkm");

        graphicsView = new TutorialView(this);
        setContentView(graphicsView);
    }

    private void extractAsset(String assetName) {
        File fileTest = new File(assetDirectory + assetName);

        if (fileTest.exists()) {
            Log.d(LOGTAG, assetName + " already exists");
            return;
        }

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
            Log.d(LOGTAG, "Extracted " + assetName);
        } catch (Exception e) {
            Log.e(LOGTAG, "Failed to extract " + assetName + ": " + e);
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