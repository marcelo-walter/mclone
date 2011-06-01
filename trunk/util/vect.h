/*
 * vect.h
 */

#ifndef _VECTOR_H_
#define _VECTOR_H_

float *vnew();
float *vclone(float *v);
void vcopy(const float *v1, float *v2);
void vprint(float *v);
void vset(float *v, float x, float y, float z);
void vzero(float *v);
void vnormal(float *v);
float vlength(const float *v);
void vscale(float *v, float div);
void vmult(float *, float *, float *);
void vadd(const float *, const float *, float *);
void vsub(const float *, const float *, float *);
void vhalf(float *, float *, float *);
float vdot(const float *, const float *);
void vcross(const float *, const float *, float *);
void vdirection(float *, float *);
void vreflect(float *, float *, float *);
void vmultmatrix(float [4][4], float [4][4], float [4][4]);


#endif //_VECTOR_H_
