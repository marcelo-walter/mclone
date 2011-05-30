/*
 *  matrixUtil.c
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/7/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */


/*
 matrixUtil.c
 Numerical Recipes in C - Second Edition
 
 Partial code for nrutil.c - pg. 942
 
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include "matrixUtil.h"

#define NR_END 1
#define FREE_ARG char*

/* Numerical Recipes standard error handler */
void nrerror(char error_text[])
{
	printf("Numerical Recipes run-time error...\n");
	printf("%s\n",error_text);
	printf("...now exiting to system...\n");
	exit(1);
}

/* allocate a float vector with subscript range v[nl..nh] */
float *vector(long nl, long nh)
{
	float *v;
	
	v=(float *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(float)));
	if (!v) nrerror("allocation failure in vector()");
	return v-nl+NR_END;
}

/* allocate an int vector with subscript range v[nl..nh] */
int *ivector(long nl, long nh)
{
	int *v;
	
	v=(int *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(int)));
	if (!v) nrerror("allocation failure in ivector()");
	return v-nl+NR_END;
}

/* free a float vector allocated with vector() */
void free_vector(float *v, long nl, long nh)
{
	free((FREE_ARG) (v+nl-NR_END));
}

/* free an int vector allocated with ivector() */
void free_ivector(int *v, long nl, long nh)
{
	free((FREE_ARG) (v+nl-NR_END));
}

/* free a float matrix allocated by matrix() */
void free_matrix(float **m, long nrl, long nrh, long ncl, long nch)
{
	free((FREE_ARG) (m[nrl]+ncl-NR_END));
	free((FREE_ARG) (m+nrl-NR_END));
}


/* allocate a float matrix with subscript range m[nrl..nrh][ncl..nch] */
float **matrix(long nrl, long nrh, long ncl, long nch)
{
	long i, nrow=nrh-nrl+1,ncol=nch-ncl+1;
	float **m;
	
	/* allocate pointers to rows */
	m=(float **) malloc((size_t) ((nrow+NR_END)*sizeof(float*)));
	if(!m) nrerror("allocation failure 1 in matrix()");
	m += NR_END;
	m -= nrl;
	
	/* allocate rows and set pointers to them */
	m[nrl]=(float *)malloc((size_t)((nrow*ncol+NR_END)*sizeof(float)));
	if(!m[nrl]) nrerror("allocation failure 2 in matrix()");
	m[nrl] += NR_END;
	m[nrl] -= ncl;
	
	for(i=nrl+1; i<=nrh; i++) m[i]=m[i-1]+ncol;
	
	/* return pointer to array of pointers to rows */
	return m;
}
