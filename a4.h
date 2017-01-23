#ifndef _A3_H
#define _A3_H

#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
// ///*
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

// #include <OpenGL/gl.h>
// #include <OpenGL/glu.h>
// #include <GLUT/glut.h>


struct Sphere
{
    float center[3];
    float colour[3];
    float radius;
};

void loadTexture(char * filename);
int drawCube();

#endif