#include<highgui.h>
#include<cv.h>
#include<stdio.h>
#include<stdlib.h>

#ifndef EIGEN_H
#define EIGEN_H

void eigen(float q1, float q2, float q3, float q4, float * e1, float * e2, float * v);	   
float quad(float a, float b, float c);
float det2x2(float m1, float m2, float m3, float m4);

#endif

