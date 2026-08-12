package com.example.lab1_gng;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.TextView;
import android.app.Activity;
import android.util.Log;
import com.example.lab1_gng.databinding.ActivityMainBinding;

public class FirstNative extends Activity {
    private static String LOGTAG = "FirstNative";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.d(LOGTAG, "On Create Method Calling Native Library");
        NativeLibrary.init();
    }

    @Override protected void onPause()
    {
        super.onPause();
    }

    @Override protected void onResume()
    {
        super.onResume();
    }





}