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
        "attribute vec3 vertexNormal;\n"
        "varying vec3 fragColour;\n"
        "uniform mat4 projection;\n"
        "uniform mat4 modelView;\n"
        "void main()\n"
        "{\n"
        "    gl_Position = projection * modelView * vertexPosition;\n"
        "    vec3 transformedVertexNormal = normalize((modelView * vec4(vertexNormal, 0.0)).xyz);\n"
        "    vec3 inverseLightDirection = normalize(vec3(0.0, 1.0, 1.0));\n"
        "    fragColour = vec3(0.0);\n"
        "    vec3 diffuseLightIntensity = vec3(1.0, 1.0, 1.0);\n"
        "    vec3 vertexDiffuseReflectionConstant = vertexColour;\n"
        "    float normalDotLight = max(0.0, dot(transformedVertexNormal, inverseLightDirection));\n"
        "    fragColour += normalDotLight * vertexDiffuseReflectionConstant * diffuseLightIntensity;\n"
        "    vec3 ambientLightIntensity = vec3(0.1, 0.1, 0.1);\n"
        "    vec3 vertexAmbientReflectionConstant = vertexColour;\n"
        "    fragColour += vertexAmbientReflectionConstant * ambientLightIntensity;\n"
        "    vec3 inverseEyeDirection = normalize(vec3(0.0, 0.0, 1.0));\n"
        "    vec3 specularLightIntensity = vec3(1.0, 1.0, 1.0);\n"
        "    vec3 vertexSpecularReflectionConstant = vec3(1.0, 1.0, 1.0);\n"
        "    float shininess = 2.0;\n"
        "    vec3 lightReflectionDirection = reflect(vec3(0.0) - inverseLightDirection, transformedVertexNormal);\n"
        "    float normalDotReflection = max(0.0, dot(inverseEyeDirection, lightReflectionDirection));\n"
        "    fragColour += pow(normalDotReflection, shininess) * vertexSpecularReflectionConstant * specularLightIntensity;\n"
        "}\n";

static const char glFragmentShader[] =
        "precision mediump float;\n"
        "varying vec3 fragColour;\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = vec4(fragColour, 1.0);\n"
        "}\n";

GLuint lightingProgram;
GLuint vertexLocation;
GLuint vertexColourLocation;
GLuint vertexNormalLocation;
GLuint projectionLocation;
GLuint modelViewLocation;

float projectionMatrix[16];
float modelViewMatrix[16];
float angle = 0.0f;

GLfloat vertices[] = {
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        0.0f,  0.0f, -2.0f,

        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        0.0f,  0.0f,  2.0f,

        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        -2.0f,  0.0f,  0.0f,

        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        2.0f,  0.0f,  0.0f,

        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        0.0f, -2.0f,  0.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        0.0f,  2.0f,  0.0f
};

GLfloat colour[] = {
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,

        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,

        0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 1.0f,

        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f
};

GLfloat normals[] = {
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        0.0f,  0.0f, -1.0f,

        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        0.0f,  0.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        -1.0f,  0.0f,  0.0f,

        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  0.0f,  0.0f,

        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        0.0f, -1.0f,  0.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        0.0f,  1.0f,  0.0f
};

GLushort indices[] = {
        0,  2,  4,  0,  4,  1,  1,  4,  3,  2,  3,  4,
        5,  7,  9,  5,  9,  6,  6,  9,  8,  7,  8,  9,
        10, 12, 14, 10, 14, 11, 11, 14, 13, 12, 13, 14,
        15, 17, 19, 15, 19, 16, 16, 19, 18, 17, 18, 19,
        20, 22, 24, 20, 24, 21, 21, 24, 23, 22, 23, 24,
        25, 27, 29, 25, 29, 26, 26, 29, 28, 27, 28, 29
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
    lightingProgram = createProgram(glVertexShader, glFragmentShader);

    if (!lightingProgram)
    {
        LOGE("Could not create program");
        return false;
    }

    vertexLocation = glGetAttribLocation(lightingProgram, "vertexPosition");
    vertexColourLocation = glGetAttribLocation(lightingProgram, "vertexColour");
    vertexNormalLocation = glGetAttribLocation(lightingProgram, "vertexNormal");
    projectionLocation = glGetUniformLocation(lightingProgram, "projection");
    modelViewLocation = glGetUniformLocation(lightingProgram, "modelView");

    matrixPerspective(projectionMatrix, 45.0f, (float) width / (float) height, 0.1f, 100.0f);

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);

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

    glUseProgram(lightingProgram);

    glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(vertexLocation);

    glVertexAttribPointer(vertexColourLocation, 3, GL_FLOAT, GL_FALSE, 0, colour);
    glEnableVertexAttribArray(vertexColourLocation);

    glVertexAttribPointer(vertexNormalLocation, 3, GL_FLOAT, GL_FALSE, 0, normals);
    glEnableVertexAttribArray(vertexNormalLocation);

    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, projectionMatrix);
    glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, modelViewMatrix);

    glDrawElements(GL_TRIANGLES, 72, GL_UNSIGNED_SHORT, indices);

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