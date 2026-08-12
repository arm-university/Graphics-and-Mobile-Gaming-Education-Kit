#ifndef MATRIX_H
#define MATRIX_H

#include <math.h>

void matrixIdentityFunction(float* matrix);
void matrixTranslate(float* matrix, float x, float y, float z);
void matrixMultiply(float* destination, float* operand1, float* operand2);
void matrixFrustum(float* matrix, float left, float right, float bottom, float top, float zNear, float zFar);
void matrixPerspective(float* matrix, float fieldOfView, float aspectRatio, float zNear, float zFar);
void matrixRotateX(float* matrix, float angle);
void matrixRotateY(float* matrix, float angle);
void matrixRotateZ(float* matrix, float angle);
void matrixScale(float* matrix, float x, float y, float z);
float matrixDegreesToRadians(float degrees);

#endif