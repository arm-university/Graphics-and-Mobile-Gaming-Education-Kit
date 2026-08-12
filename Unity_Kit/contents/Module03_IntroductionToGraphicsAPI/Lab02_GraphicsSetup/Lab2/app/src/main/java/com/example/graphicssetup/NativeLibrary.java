package com.example.graphicssetup;
public class NativeLibrary {

    static
    {
        System.loadLibrary("graphicssetup");
    }


    public static native void init(int width, int height);
    public static native void step();


}
