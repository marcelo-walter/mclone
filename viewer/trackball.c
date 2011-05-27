/*
 *-------------------------------------------------------------------
 *	This module contains original code from SGI as given in their
 *	4DGifts directory.
 *	This code implements a trackball. All the vector
 *	auxiliary routines are here together in the same
 *	file (originally they were in 2 different files)
 *	I've changed some stuff to work with OpenGL and
 *	GLUT. Basically the build_rotmatrix now uses
 *	my Matrix4 typedef for the matrix format
 *
 *	Marcelo Walter - August 1996
 *-------------------------------------------------------------------
 */

/*
 * (c) Copyright 1993, 1994, Silicon Graphics, Inc.
 * ALL RIGHTS RESERVED
 * Permission to use, copy, modify, and distribute this software for
 * any purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright notice
 * and this permission notice appear in supporting documentation, and that
 * the name of Silicon Graphics, Inc. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
 * KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
 * LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
 * THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
 * POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * US Government Users Restricted Rights
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the Rights in Technical Data and Computer Software
 * clause at DFARS 252.227-7013 and/or in similar or successor
 * clauses in the FAR or the DOD or NASA FAR Supplement.
 * Unpublished-- rights reserved under the copyright laws of the
 * United States.  Contractor/manufacturer is Silicon Graphics,
 * Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
 *
 * OpenGL(TM) is a trademark of Silicon Graphics, Inc.
 */
/*
 * Trackball code:
 *
 * Implementation of a virtual trackball.
 * Implemented by Gavin Bell, lots of ideas from Thant Tessman and
 *   the August '88 issue of Siggraph's "Computer Graphics," pp. 121-129.
 *
 * Vector manip code:
 *
 * Original code from:
 * David M. Ciemiewicz, Mark Grossman, Henry Moreton, and Paul Haeberli
 *
 * Much mucking with by:
 * Gavin Bell
 */

#include <math.h>
#include "../common.h"
#include "trackball.h"

/*
 * This size should really be based on the distance from the center of
 * rotation to the point on the object underneath the mouse.  That
 * point would then track the mouse as closely as possible.  This is a
 * simple example, though, so that is left as an Exercise for the
 * Programmer.
 */
#define TRACKBALLSIZE  (1.0)

/*
 * Local function prototypes (not defined in trackball.h)
 */
static float tb_project_to_sphere(float, float, float);
static void normalize_quat(float [4]);

/*
 * Ok, simulate a track-ball.  Project the points onto the virtual
 * trackball, then figure out the axis of rotation, which is the cross
 * product of P1 P2 and O P1 (O is the center of the ball, 0,0,0)
 * Note:  This is a deformed trackball-- is a trackball in the center,
 * but is deformed into a hyperbolic sheet of rotation away from the
 * center.  This particular function was chosen after trying out
 * several variations.
 *
 * It is assumed that the arguments to this routine are in the range
 * (-1.0 ... 1.0)
 *----------------------------------------------------------------------
 * Pass the x and y coordinates of the last and current positions of
 * the mouse, scaled so they are from (-1.0 ... 1.0).
 *
 * The resulting rotation is returned as a quaternion rotation in the
 * first paramater.
 */

void
trackball(float q[4], float p1x, float p1y, float p2x, float p2y)
{
    float a[3]; /* Axis of rotation */
    float phi;  /* how much to rotate about axis */
    float p1[3], p2[3], d[3];
    float t;
	
    if (p1x == p2x && p1y == p2y) {
        /* Zero rotation */
        vzero(q);
        q[3] = 1.0;
        return;
    }
	
    /*
     * First, figure out z-coordinates for projection of P1 and P2 to
     * deformed sphere
     */
    vset(p1,p1x,p1y,tb_project_to_sphere(TRACKBALLSIZE,p1x,p1y));
    vset(p2,p2x,p2y,tb_project_to_sphere(TRACKBALLSIZE,p2x,p2y));
	
    /*
     *  Now, we want the cross product of P1 and P2
     */
    vcross(p2,p1,a);
	
    /*
     *  Figure out how much to rotate around that axis.
     */
    vsub(p1,p2,d);
    t = vlength(d) / (2.0*TRACKBALLSIZE);
	
    /*
     * Avoid problems with out-of-control values...
     */
    if (t > 1.0) t = 1.0;
    if (t < -1.0) t = -1.0;
    phi = 2.0 * asin(t);
	
    axis_to_quat(a,phi,q);
}

/*
 *  Given an axis and angle, compute quaternion.
 *  This function computes a quaternion based on an axis (defined by
 *  the given vector) and an angle about which to rotate.  The angle is
 *  expressed in radians.  The result is put into the third argument.
 */
void axis_to_quat(float a[3], float phi, float q[4])
{
    vnormal(a);
    vcopy(a,q);
    vscale(q,sin(phi/2.0));
    q[3] = cos(phi/2.0);
}

/*
 * Project an x,y pair onto a sphere of radius r OR a hyperbolic sheet
 * if we are away from the center of the sphere.
 */
static float tb_project_to_sphere(float r, float x, float y)
{
    float d, t, z;
	
    d = sqrt(x*x + y*y);
    if (d < r * 0.70710678118654752440) {    /* Inside sphere */
        z = sqrt(r*r - d*d);
    } else {           /* On hyperbola */
        t = r / 1.41421356237309504880;
        z = t*t / d;
    }
    return z;
}

/*
 *      Given two rotations, e1 and e2, expressed as Euler paramaters,
 * figure out the equivalent single rotation and stuff it into dest.
 *
 * This routine also normalizes the result every COUNT times it is
 * called, to keep error from creeping in.
 */
#define COUNT 100
void add_eulers(float *e1, float *e2, float *dest)
{
	static int count=0;
	register int i;
	float t1[3], t2[3], t3[3];
	float tf[4];
	
	vcopy(e1, t1); vscale(t1, e2[3]);
	vcopy(e2, t2); vscale(t2, e1[3]);
	vcross(e2, e1, t3);
	vadd(t1, t2, tf);
	vadd(t3, tf, tf);
	tf[3] = e1[3] * e2[3] - vdot(e1, e2);
	
	for (i = 0 ; i < 4 ;i++)
	{
		dest[i] = tf[i];
	}
	
	if (++count > COUNT)
	{
		count = 0;
		normalize_euler(dest);
	}
}
/*
 * Euler paramaters always obey:  a^2 + b^2 + c^2 + d^2 = 1.0
 * We'll normalize based on this formula.  Also, normalize greatest
 * component, to avoid problems that occur when the component we're
 * normalizing gets close to zero (and the other components may add up
 * to more than 1.0 because of rounding error).
 */
void normalize_euler(float *e)
{       /* Normalize result */
	int which, i;
	float gr;
	
	which = 0;
	gr = e[which];
	for (i = 1 ; i < 4 ; i++)
	{
		if (fabs(e[i]) > fabs(gr))
		{
			gr = e[i];
			which = i;
		}
	}
	
	e[which] = 0.0;
	
	e[which] = sqrt(1.0 - (e[0]*e[0] + e[1]*e[1] +
						   e[2]*e[2] + e[3]*e[3]));
	
	/* Check to see if we need negative square root */
	if (gr < 0.0)
		e[which] = -e[which];
}

/*
 * Given two rotations, e1 and e2, expressed as quaternion rotations,
 * figure out the equivalent single rotation and stuff it into dest.
 *
 * This routine also normalizes the result every RENORMCOUNT times it is
 * called, to keep error from creeping in.
 *
 * NOTE: This routine is written so that q1 or q2 may be the same
 * as dest (or each other).
 *-----------------------------------------------------------------------
 * Given two quaternions, add them together to get a third quaternion.
 * Adding quaternions to get a compound rotation is analagous to adding
 * translations to get a compound translation.  When incrementally
 * adding rotations, the first argument here should be the new
 * rotation, the second and third the total rotation (which will be
 * over-written with the resulting new total rotation).
 */
#define RENORMCOUNT 97

void add_quats(float q1[4], float q2[4], float dest[4])
{
    static int count=0;
    float t1[4], t2[4], t3[4];
    float tf[4];
	
    vcopy(q1,t1);
    vscale(t1,q2[3]);
	
    vcopy(q2,t2);
    vscale(t2,q1[3]);
	
    vcross(q2,q1,t3);
    vadd(t1,t2,tf);
    vadd(t3,tf,tf);
    tf[3] = q1[3] * q2[3] - vdot(q1,q2);
	
    dest[0] = tf[0];
    dest[1] = tf[1];
    dest[2] = tf[2];
    dest[3] = tf[3];
	
    if (++count > RENORMCOUNT) {
        count = 0;
        normalize_quat(dest);
    }
}

/*
 * Quaternions always obey:  a^2 + b^2 + c^2 + d^2 = 1.0
 * If they don't add up to 1.0, dividing by their magnitued will
 * renormalize them.
 *
 * Note: See the following for more information on quaternions:
 *
 * - Shoemake, K., Animating rotation with quaternion curves, Computer
 *   Graphics 19, No 3 (Proc. SIGGRAPH'85), 245-254, 1985.
 * - Pletinckx, D., Quaternion calculus as a basic tool in computer
 *   graphics, The Visual Computer 5, 2-13, 1989.
 */
static void normalize_quat(float q[4])
{
    int i;
    float mag;
	
    mag = (q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
    for (i = 0; i < 4; i++) q[i] /= mag;
}

/*
 * Build a rotation matrix, given a quaternion rotation.
 * A useful function, builds a rotation matrix in Matrix based on
 * given quaternion
 */
void build_rotmatrix(Matrix4 *m, float q[4])
{
    m->element[0][0] = 1.0 - 2.0 * (q[1] * q[1] + q[2] * q[2]);
    m->element[1][0] = 2.0 * (q[0] * q[1] - q[2] * q[3]);
    m->element[2][0] = 2.0 * (q[2] * q[0] + q[1] * q[3]);
    m->element[3][0] = 0.0;
	
    m->element[0][1] = 2.0 * (q[0] * q[1] + q[2] * q[3]);
    m->element[1][1]= 1.0 - 2.0 * (q[2] * q[2] + q[0] * q[0]);
    m->element[2][1] = 2.0 * (q[1] * q[2] - q[0] * q[3]);
    m->element[3][1] = 0.0;
	
    m->element[0][2] = 2.0 * (q[2] * q[0] - q[1] * q[3]);
    m->element[1][2] = 2.0 * (q[1] * q[2] + q[0] * q[3]);
    m->element[2][2] = 1.0 - 2.0 * (q[1] * q[1] + q[0] * q[0]);
    m->element[3][2] = 0.0;
	
    m->element[0][3] = 0.0;
    m->element[1][3] = 0.0;
    m->element[2][3] = 0.0;
    m->element[3][3] = 1.0;
}

/*
 *-------------------------------------------------------
 *		vect.c
 *-------------------------------------------------------
 */
//extern void vzero(float*);
//extern void vset(float*);
/*void
vzero(float *v)
{
    v[0] = 0.0;
    v[1] = 0.0;
    v[2] = 0.0;
}*/

/*void
vset(float *v, float x, float y, float z)
{
    v[0] = x;
    v[1] = y;
    v[2] = z;
}*/

//extern void vsub(const float *, const float *, float *);
/*void
vsub(const float *src1, const float *src2, float *dst)
{
    dst[0] = src1[0] - src2[0];
    dst[1] = src1[1] - src2[1];
    dst[2] = src1[2] - src2[2];
}*/

//extern void vcopy(const float *, float *);
/*void
vcopy(const float *v1, float *v2)
{
    register int i;
    for (i = 0 ; i < 3 ; i++)
        v2[i] = v1[i];
}*/

//extern void vcross(const float *, const float *, float *);

/*void
vcross(const float *v1, const float *v2, float *cross)
{
    float temp[3];
	
    temp[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
    temp[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
    temp[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
    vcopy(temp, cross);
}*/

//extern float vlength(const float*);
/*float
vlength(const float *v)
{
    return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}*/

//extern void vscale(float *, float );
/*void
vscale(float *v, float div)
{
    v[0] *= div;
    v[1] *= div;
    v[2] *= div;
}*/

//extern void vnormal(float *);
/*void
vnormal(float *v)
{
    vscale(v,1.0/vlength(v));
}*/

//extern float vdot(const float *, const float *);

/*float
vdot(const float *v1, const float *v2)
{
    return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}*/

//extern void vadd(const float *, const float *, float *);

/*void
vadd(const float *src1, const float *src2, float *dst)
{
    dst[0] = src1[0] + src2[0];
    dst[1] = src1[1] + src2[1];
    dst[2] = src1[2] + src2[2];
}*/
