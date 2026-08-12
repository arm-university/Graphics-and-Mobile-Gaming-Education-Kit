#include "Texture.h"

#include <android/log.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <cstdio>
#include <cstdlib>

#define LOG_TAG "libNative"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#define CHANNELS_PER_PIXEL 3

void loadTexture(const char * texture, unsigned int level, unsigned int width, unsigned int height)
{
    GLubyte * theTexture = (GLubyte*) malloc(width * height * CHANNELS_PER_PIXEL);

    FILE * theFile = fopen(texture, "rb");

    if(theFile == NULL)
    {
        LOGE("Failed to load raw texture");
        return;
    }

    fread(theTexture, width * height * CHANNELS_PER_PIXEL, 1, theFile);
    fclose(theFile);

    glTexImage2D(GL_TEXTURE_2D, level, GL_RGB, width, height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, theTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    free(theTexture);
}

void loadCompressedTexture(const char * texture, unsigned int level)
{
    GLubyte header[16];
    FILE * theFile = fopen(texture, "rb");

    if(theFile == NULL)
    {
        LOGE("Failed to load compressed texture");
        return;
    }

    fread(header, 16, 1, theFile);

    GLushort paddedWidth = (header[8] << 8) | header[9];
    GLushort paddedHeight = (header[10] << 8) | header[11];
    GLushort width = (header[12] << 8) | header[13];
    GLushort height = (header[14] << 8) | header[15];

    int dataSize = (paddedWidth * paddedHeight) >> 1;
    GLubyte * data = (GLubyte*) malloc(dataSize);

    fread(data, dataSize, 1, theFile);
    fclose(theFile);

    glCompressedTexImage2D(GL_TEXTURE_2D, level, GL_ETC1_RGB8_OES,
                           width, height, 0, dataSize, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    free(data);
}