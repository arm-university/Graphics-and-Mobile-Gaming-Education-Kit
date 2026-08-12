package com.example.simpletriangle;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

public class SimpleTriangle extends Activity {
    private static final String LOGTAG = "SimpleTriangle";
    private TutorialView graphicsView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i(LOGTAG, "Creating New Tutorial View");
        graphicsView = new TutorialView(this);
        setContentView(graphicsView);
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