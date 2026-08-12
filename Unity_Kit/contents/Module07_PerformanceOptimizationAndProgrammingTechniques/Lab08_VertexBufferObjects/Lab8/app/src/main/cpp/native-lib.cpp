#include <jni.h>
#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include "Matrix.h"

#define LOG_TAG "libNative"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static const char glVertexShader[] =
        "attribute vec4 vertexPosition;\n"
        "attribute vec3 vertexColour;\n"
        "varying vec3 fragColour;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 modelView;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = projection * modelView * vertexPosition;\n"
        "    fragColour = vertexColour;\n"
        "}\n";

static const char glFragmentShader[] =
        "precision mediump float;\n"
        "varying vec3 fragColour;\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = vec4(fragColour, 1.0);\n"
        "}\n";

GLuint simpleCubeProgram;
GLuint vertexLocation;
GLuint vertexColourLocation;
GLuint projectionLocation;
GLuint modelViewLocation;

float projectionMatrix[16];
float modelViewMatrix[16];
float angle = 0.0f;

static GLuint vboBufferIds[2];

static GLushort vertexBufferSize = 48 * 3 * sizeof(GLfloat);
static GLushort elementBufferSize = 36 * sizeof(GLushort);
static GLushort strideLength = 6 * sizeof(GLfloat);
static GLushort vertexColourOffset = 3 * sizeof(GLfloat);

static GLfloat cubeVertices[] = {
        -1.0f,  1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
        1.0f,  1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,

        -1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f,
        1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 0.0f,
        1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 0.0f,

        -1.0f,  1.0f, -1.0f,  0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, 0.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f,

        1.0f,  1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, -1.0f,  1.0f, 1.0f, 0.0f,
        1.0f, -1.0f,  1.0f,  1.0f, 1.0f, 0.0f,
        1.0f,  1.0f,  1.0f,  1.0f, 1.0f, 0.0f,

        -1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 1.0f,
        1.0f, -1.0f,  1.0f,  0.0f, 1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,  0.0f, 1.0f, 1.0f,

        -1.0f,  1.0f, -1.0f,  1.0f, 0.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
        1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 1.0f,
        1.0f,  1.0f, -1.0f,  1.0f, 0.0f, 1.0f
};

GLushort indices[] = {
        0, 2, 3,  0, 1, 3,
        4, 6, 7,  4, 5, 7,
        8, 9, 10, 11, 8, 10,
        12, 13, 14, 15, 12, 14,
        16, 17, 18, 16, 19, 18,
        20, 21, 22, 20, 23, 22
};

GLuint loadShader(GLenum shaderType, const char* shaderSource)
{
    GLuint shader = glCreateShader(shaderType);

    if (shader)
    {
        glShaderSource(shader, 1, &shaderSource, NULL);
        glCompileShader(shader);

        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

        if (!compiled)
        {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

            if (infoLen)
            {
                char* buf = (char*) malloc(infoLen);

                if (buf)
                {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGE("Could not compile shader %d:\n%s\n", shaderType, buf);
                    free(buf);
                }
            }

            glDeleteShader(shader);
            shader = 0;
        }
    }

    return shader;
}

GLuint createProgram(const char* vertexSource, const char* fragmentSource)
{
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
    if (!vertexShader)
    {
        return 0;
    }

    GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (!fragmentShader)
    {
        glDeleteShader(vertexShader);
        return 0;
    }

    GLuint program = glCreateProgram();

    if (program)
    {
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

        if (linkStatus != GL_TRUE)
        {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);

            if (bufLength)
            {
                char* buf = (char*) malloc(bufLength);

                if (buf)
                {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGE("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }

            glDeleteProgram(program);
            program = 0;
        }
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

bool setupGraphics(int width, int height)
{
    simpleCubeProgram = createProgram(glVertexShader, glFragmentShader);

    if (!simpleCubeProgram)
    {
        LOGE("Could not create program");
        return false;
    }

    vertexLocation = glGetAttribLocation(simpleCubeProgram, "vertexPosition");
    vertexColourLocation = glGetAttribLocation(simpleCubeProgram, "vertexColour");
    projectionLocation = glGetUniformLocation(simpleCubeProgram, "projection");
    modelViewLocation = glGetUniformLocation(simpleCubeProgram, "modelView");

    matrixPerspective(projectionMatrix, 45.0f, (float) width / (float) height, 0.1f, 100.0f);

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);

    glGenBuffers(2, vboBufferIds);

    glBindBuffer(GL_ARRAY_BUFFER, vboBufferIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboBufferIds[1]);

    glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, cubeVertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementBufferSize, indices, GL_STATIC_DRAW);

    return true;
}

void renderFrame()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    matrixIdentityFunction(modelViewMatrix);
    matrixRotateX(modelViewMatrix, angle);
    matrixRotateY(modelViewMatrix, angle);
    matrixTranslate(modelViewMatrix, 0.0f, 0.0f, -10.0f);

    glUseProgram(simpleCubeProgram);

    glBindBuffer(GL_ARRAY_BUFFER, vboBufferIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboBufferIds[1]);

    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, strideLength, 0);
    glEnableVertexAttribArray(vertexLocation);

    glVertexAttribPointer(vertexColourLocation, 3, GL_FLOAT, GL_FALSE, strideLength, (const void*) vertexColourOffset);
    glEnableVertexAttribArray(vertexColourLocation);

    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, projectionMatrix);
    glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, modelViewMatrix);

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

    angle += 1.0f;
    if (angle > 360.0f)
    {
        angle -= 360.0f;
    }
}

extern "C" {

JNIEXPORT void JNICALL
Java_com_example_simplecube_NativeLibrary_init(
        JNIEnv* env, jclass type, jint width, jint height)
{
    setupGraphics(width, height);
}

JNIEXPORT void JNICALL
Java_com_example_simplecube_NativeLibrary_step(
        JNIEnv* env, jclass type)
{
    renderFrame();
}

}