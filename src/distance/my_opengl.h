/***************************************************************************
 my_opengl.h  -  description
 -------------------
 begin                : Sat Aug 16 2003
 copyright            : (C) 2003 by Dimas Martinez Morera
 email                : dimasmm@impa.br
 ***************************************************************************/
#ifndef MY_OPENGL_H
#define MY_OPENGL_H

#include <GL/gl.h>

void convert_hsv_to_rgb(double hue, double s, double v, double* r, double* g, double* b );

void glColorHSV3f(double, double, double);
void glColorHSV4f(double, double, double, double);

#endif
