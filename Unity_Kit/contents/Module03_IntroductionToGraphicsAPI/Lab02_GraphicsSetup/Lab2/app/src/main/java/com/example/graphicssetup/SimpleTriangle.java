package com.example.graphicssetup;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.TextView;
import android.app.Activity;
import android.util.Log;
import com.example.graphicssetup.databinding.ActivityMainBinding;

public class SimpleTriangle extends Activity {
    protected TutorialView graphicsView;
    private static String LOGTAG = "SimpleTriangle";

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
        graphicsView.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        graphicsView.onResume();
    }
}





