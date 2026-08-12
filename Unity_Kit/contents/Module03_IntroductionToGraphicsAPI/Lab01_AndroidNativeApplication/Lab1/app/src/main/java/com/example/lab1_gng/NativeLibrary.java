package com.example.lab1_gng;
public class NativeLibrary {

    static
    {
        System.loadLibrary("lab1_gng");
    }

    public static native void init();


}
