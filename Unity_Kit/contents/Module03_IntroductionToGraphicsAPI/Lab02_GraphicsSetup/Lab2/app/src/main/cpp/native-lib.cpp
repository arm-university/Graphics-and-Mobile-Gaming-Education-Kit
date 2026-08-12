#include <jni.h>
#include <android/log.h>
#include <unistd.h>

#define LOG_TAG "libNative"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)



extern "C"
{
JNIEXPORT void JNICALL
Java_com_example_graphicssetup_NativeLibrary_init(JNIEnv *env, jclass obj,    jint width, jint height){};

JNIEXPORT void JNICALL
Java_com_example_graphicssetup_NativeLibrary_step(JNIEnv *env, jclass type){
    /* Sleeping to avoid thrashing the Android log. */
    sleep(5);
    LOGI("New Frame Ready to be Drawn!!!!");
}
};
