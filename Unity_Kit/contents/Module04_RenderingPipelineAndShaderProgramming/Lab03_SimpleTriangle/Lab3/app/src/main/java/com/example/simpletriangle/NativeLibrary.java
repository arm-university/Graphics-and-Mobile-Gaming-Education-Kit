package com.example.simpletriangle;

public class NativeLibrary {
    static {
        System.loadLibrary("simpletriangle");
    }

    public static native void init(int width, int height);
    public static native void step();
}