/*
 *  nrutil.h
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/7/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */

/*
 nrutil.h
 Numerical Recipes in C - Second Edition
 
 pg. 941
 
 */

#ifndef _NR_UTILS_H_
#define _NR_UTILS_H_
#endif

static float sqrarg;
#define SQR(a) ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg*sqrarg)

static float swap;
#define SWAP(a,b) {swap=(a);(a)=(b);b=swap;}

void nrerror(char error_text[]);
float *vector(long nl, long nh);
int *ivector(long nl, long nh);
void free_vector(float *v, long nl, long nh);
void free_ivector(int *v, long nl, long nh);
void free_matrix(float **m, long nrl, long nrh, long ncl, long nch);
float **matrix(long nrl, long nrh, long ncl, long nch);
