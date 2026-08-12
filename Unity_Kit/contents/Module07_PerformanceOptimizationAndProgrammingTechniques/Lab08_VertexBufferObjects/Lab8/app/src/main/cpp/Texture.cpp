#include "Texture.h"

#include <android/log.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <cstdio>
#include <cstdlib>

#define LOG_TAG "libNative"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define TEXTURE_WIDTH 256
#define TEXTURE_HEIGHT 256
#define CHANNELS_PER_PIXEL 3

GLubyte* theTexture;

GLuint loadTexture()
{
    static GLuint textureId;

    theTexture = (GLubyte*) malloc(sizeof(GLubyte) * TEXTURE_WIDTH * TEXTURE_HEIGHT * CHANNELS_PER_PIXEL);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &textureId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    FILE* theFile = fopen("/data/data/com.example.simplecube/files/normalMap256.raw", "rb");
    LOGI("Normal map opened successfully");
    if (theFile == NULL)
    {
        LOGE("Failure to load the texture");
        free(theTexture);
        return 0;
    }

    fread(theTexture, TEXTURE_WIDTH * TEXTURE_HEIGHT * CHANNELS_PER_PIXEL, 1, theFile);
    LOGI("Normal map loaded");
    fclose(theFile);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, theTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    free(theTexture);

    return textureId;
}