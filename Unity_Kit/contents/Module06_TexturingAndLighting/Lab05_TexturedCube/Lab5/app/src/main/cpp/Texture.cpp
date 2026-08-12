#include "Texture.h"

#include <GLES2/gl2ext.h>
#include <GLES2/gl2.h>
#include <cstdio>
#include <cstdlib>

GLuint loadSimpleTexture()
{
    GLuint textureId;

    GLubyte pixels[9 * 4] =
            {
                    18, 140, 171, 255,
                    143, 143, 143, 255,
                    255, 255, 255, 255,
                    255, 255, 0,   255,
                    0,   255, 255, 255,
                    255, 0,   255, 255,
                    255, 0,   0,   255,
                    0,   255, 0,   255,
                    0,   0,   255, 255
            };

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &textureId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 3, 3, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return textureId;
}