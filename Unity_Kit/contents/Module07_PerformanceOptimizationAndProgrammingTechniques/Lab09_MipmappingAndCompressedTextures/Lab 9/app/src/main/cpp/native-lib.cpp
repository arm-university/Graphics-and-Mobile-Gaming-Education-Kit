#include <jni.h>
#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <math.h>

#include "Matrix.h"
#include "Texture.h"

#define LOG_TAG "libNative"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

GLuint glProgram;
GLuint vertexLocation;
GLuint textureCordLocation;
GLuint projectionLocation;
GLuint modelViewLocation;
GLuint samplerLocation;

GLuint textureIds[2];

float projectionMatrix[16];
float modelViewMatrix[16];

float distance = 1.0f;
float velocity = 0.5f;
GLuint textureModeToggle = 0;

GLfloat squareVertices[] = {
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
};

GLfloat textureCords[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f
};

GLushort indices[] = {0,2,3, 0,3,1};

static const char vertexShader[] =
        "attribute vec4 vertexPosition;\n"
        "attribute vec2 vertexTextureCord;\n"
        "varying vec2 textureCord;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 modelView;\n"
        "void main() {\n"
        "gl_Position = projection * modelView * vertexPosition;\n"
        "textureCord = vertexTextureCord;\n"
        "}\n";

static const char fragmentShader[] =
        "precision mediump float;\n"
        "uniform sampler2D texture;\n"
        "varying vec2 textureCord;\n"
        "void main() {\n"
        "gl_FragColor = texture2D(texture, textureCord);\n"
        "}\n";

GLuint loadShader(GLenum type, const char* src)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader,1,&src,NULL);
    glCompileShader(shader);
    return shader;
}

GLuint createProgram()
{
    GLuint vs = loadShader(GL_VERTEX_SHADER, vertexShader);
    GLuint fs = loadShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLuint program = glCreateProgram();
    glAttachShader(program,vs);
    glAttachShader(program,fs);
    glLinkProgram(program);
    return program;
}

bool setupGraphics(int width, int height)
{
    glProgram = createProgram();

    vertexLocation = glGetAttribLocation(glProgram,"vertexPosition");
    textureCordLocation = glGetAttribLocation(glProgram,"vertexTextureCord");
    projectionLocation = glGetUniformLocation(glProgram,"projection");
    modelViewLocation = glGetUniformLocation(glProgram,"modelView");
    samplerLocation = glGetUniformLocation(glProgram,"texture");

    matrixPerspective(projectionMatrix,45,(float)width/height,0.1f,170);

    glViewport(0,0,width,height);

    glGenTextures(2, textureIds);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureIds[0]);

    loadTexture("/data/data/com.example.simplecube/files/level0.raw",0,512,512);
    loadTexture("/data/data/com.example.simplecube/files/level1.raw",1,256,256);
    loadTexture("/data/data/com.example.simplecube/files/level2.raw",2,128,128);
    loadTexture("/data/data/com.example.simplecube/files/level3.raw",3,64,64);
    loadTexture("/data/data/com.example.simplecube/files/level4.raw",4,32,32);
    loadTexture("/data/data/com.example.simplecube/files/level5.raw",5,16,16);
    loadTexture("/data/data/com.example.simplecube/files/level6.raw",6,8,8);
    loadTexture("/data/data/com.example.simplecube/files/level7.raw",7,4,4);
    loadTexture("/data/data/com.example.simplecube/files/level8.raw",8,2,2);
    loadTexture("/data/data/com.example.simplecube/files/level9.raw",9,1,1);

    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, textureIds[1]);

    //loadCompressedTexture("/data/data/com.example.simplecube/files/level0.pkm",0);
    //loadCompressedTexture("/data/data/com.example.simplecube/files/level1.pkm",1);
    //loadCompressedTexture("/data/data/com.example.simplecube/files/level2.pkm",2);
    //loadCompressedTexture("/data/data/com.example.simplecube/files/level3.pkm",3);
    //loadCompressedTexture("/data/data/com.example.simplecube/files/level4.pkm",4);
    //loadCompressedTexture("/data/data/com.example.simplecube/files/level5.pkm",5);
    //loadCompressedTexture("/data/data/com.example.simplecube/files/level6.pkm",6);
    //loadCompressedTexture("/data/data/com.example.simplecube/files/level7.pkm",7);
    //loadCompressedTexture("/data/data/com.example.simplecube/files/level8.pkm",8);
    //loadCompressedTexture("/data/data/com.example.simplecube/files/level9.pkm",9);

    return true;
}

void renderFrame()
{
    glClear(GL_COLOR_BUFFER_BIT);

    matrixIdentityFunction(modelViewMatrix);
    matrixTranslate(modelViewMatrix,0,0,-distance);

    glUseProgram(glProgram);

    glVertexAttribPointer(vertexLocation,3,GL_FLOAT,GL_FALSE,0,squareVertices);
    glEnableVertexAttribArray(vertexLocation);

    glVertexAttribPointer(textureCordLocation,2,GL_FLOAT,GL_FALSE,0,textureCords);
    glEnableVertexAttribArray(textureCordLocation);

    glUniformMatrix4fv(projectionLocation,1,GL_FALSE,projectionMatrix);
    glUniformMatrix4fv(modelViewLocation,1,GL_FALSE,modelViewMatrix);

    glUniform1i(samplerLocation, textureModeToggle);

    glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_SHORT,indices);

    distance += velocity;

    if(distance > 160 || distance < 1)
    {
        velocity *= -1;
       // textureModeToggle = !textureModeToggle;
    }
}

extern "C" {

JNIEXPORT void JNICALL
Java_com_example_simplecube_NativeLibrary_init(JNIEnv*, jclass, jint w, jint h)
{
    setupGraphics(w,h);
}

JNIEXPORT void JNICALL
Java_com_example_simplecube_NativeLibrary_step(JNIEnv*, jclass)
{
    renderFrame();
}
}