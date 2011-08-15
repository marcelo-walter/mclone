/***************************************************************************
 my_opengl.cpp  -  description
 -------------------
 begin                : Sat Aug 16 2003
 copyright            : (C) 2003 by Dimas Martinez Morera
 email                : dimasmm@impa.br
 ***************************************************************************/
/* Conversion from Hue, Saturation, Value to Red, Green, and Blue */
/* From "Computer Graphics", Donald Hearn & M. Pauline Baker, pg.304 */
/* rgb values of 0.0 - 1.0      */

#include <math.h>
#include <iostream>
#include "my_opengl.h"

void convert_hsv_to_rgb (double hue, double s, double v,
						 double *r, double *g, double *b)
{
    double p1, p2, p3, i, f;
    double xh;
    double nr, ng, nb;
    /* hue (0.0 to 360.0, is circular, 0=360)
	 s and v are from 0.0 - 1.0) */
	
    if (hue == 360.0)
		hue = 0.0;		/* (THIS LOOKS BACKWARDS)       */
	
    xh = hue / 60.;		/* convert hue to be in 0,6       */
    i = floor (xh);		/* i = greatest integer <= h    */
    f = xh - i;			/* f = fractional part of h     */
    p1 = v * (1.0 - s);
    p2 = v * (1.0 - (s * f));
    p3 = v * (1.0 - (s * (1.0 - f)));
	
    switch ((int) i) {
		case 0:
			nr = v;
			ng = p3;
			nb = p1;
			break;
		case 1:
			nr = p2;
			ng = v;
			nb = p1;
			break;
		case 2:
			nr = p1;
			ng = v;
			nb = p3;
			break;
		case 3:
			nr = p1;
			ng = p2;
			nb = v;
			break;
		case 4:
			nr = p3;
			ng = p1;
			nb = v;
			break;
		case 5:
			nr = v;
			ng = p1;
			nb = p2;
			break;
    }
	
    *r = nr;
    *g = ng;
    *b = nb;
}

void glColorHSV3f (double h, double s, double v)
{
    double r, g, b;
    convert_hsv_to_rgb (h, s, v, &r, &g, &b);
    glColor3f (r, g, b);
}

void glColorHSV4f (double h, double s, double v, double alfa)
{
    double r, g, b;
    convert_hsv_to_rgb (h, s, v, &r, &g, &b);
    glColor4f (r, g, b, alfa);
}
