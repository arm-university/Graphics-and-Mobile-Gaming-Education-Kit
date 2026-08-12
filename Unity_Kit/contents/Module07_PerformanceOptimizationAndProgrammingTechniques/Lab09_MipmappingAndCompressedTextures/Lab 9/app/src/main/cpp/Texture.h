#ifndef TEXTURE_H
#define TEXTURE_H

#include <GLES2/gl2.h>

void loadTexture(const char * texture, unsigned int level, unsigned int width, unsigned int height);
void loadCompressedTexture(const char * texture, unsigned int level);

#endif