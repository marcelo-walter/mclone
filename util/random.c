/*
 *  random.c
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/7/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */


/*
 *--------------------------------------------------
 * Random distributions from Numerical recipes in C
 * 1989
 *
 * Typed in by Marcelo Walter
 *--------------------------------------------------
 */

/*
 *---------------------------------------------------
 *	Include
 *---------------------------------------------------
 */

#include "random.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/*
 *---------------------------------------------------
 *	FUNCTIONS
 *---------------------------------------------------
 */
/*
 *--------------------------------------------------
 * Returns a uniform deviate between 0.0 and 1.0
 * Set idum to any negative value to initialize
 * or reinitialize the sequence
 * pg. 210 of the book
 *--------------------------------------------------
 */
float ran1(int *idum)
{
	static long ix1,ix2,ix3;
	static float r[98];
	float temp;
	static int iff=0;
	int j;
	
	if(*idum < 0 || iff == 0){
		iff=1;
		ix1=(IC1-(*idum))%M1;
		ix1=(IA1*ix1+IC1)%M1;
		ix2=ix1%M2;
		ix1=(IA1*ix1+IC1)%M1;
		ix3=ix1%M3;
		for(j=1;j<=97;j++){
			ix1=(IA1*ix1+IC1)%M1;
			ix2=(IA2*ix2+IC2)%M2;
			r[j]=(ix1+ix2*RM2)*RM1;
		}
		*idum=1;
	}
	ix1=(IA1*ix1+IC1)%M1;
	ix2=(IA2*ix2+IC2)%M2;
	ix3=(IA3*ix3+IC3)%M3;
	j=1 + ((97*ix3)/M3);
	if(j>97||j<1){
		printf("Error in ran1.This should not happen!\n");
		exit(0);
	}
	temp=r[j];
	r[j]=(ix1+ix2*RM2)*RM1;
	return (temp);
}


/*
 *----------------------------------------------------------------
 * Returns as a floating point number an integer value that is
 * a random deviate drawn from a Poisson distribution of mean
 * "xm", using ran1(idum) as a source of uniform random deviates
 * Pg. 294 from the book
 *----------------------------------------------------------------
 */
float poidev(float xm, int *idum)
{
	/* oldm is a flag for whether "xm" has changed since last call */
	static float sq, alxm, g, oldm=(-1);
	float em,t,y;
	float gammln(float xx);
	
	if(xm < 12.0){
		if(xm != oldm){
			oldm = xm;
			g = exp(-xm);
		}
		em = -1;
		t=1.0;
		do{
			++em;
			t *= ran1(idum);
		} while (t>g);
	} else{
		if (xm != oldm){
			oldm = xm;
			sq = sqrt(2.0*xm);
			alxm = log(xm);
			g = xm*alxm - gammln(xm+1.0);
		}
		do{
			do{
				y = tan(M_PI*ran1(idum));
				em = sq * y + xm;
			} while( em < 0.0);
			em = floor(em);
			t = 0.9*(1.0+y*y)*exp(em*alxm-gammln(em+1.0)-g);
		} while( ran1(idum) > t);
	}
	return em;
}


/*
 *------------------------------------------------------------
 * Returns a normally distributed deviate with zero mean and
 * unit variance, using ran1(idum) as the source of uniform
 * deviates
 * pg. 217 from the book
 *------------------------------------------------------------
 */
float gasdev(int *idum)
{
	static int iset=0;
	static float gset;
	float fac,r,v1,v2;
	float ran1(int *idum);
	
	if(iset==0){
		do{
			v1=2.0*ran1(idum)-1.0;
			v2=2.0*ran1(idum)-1.0;
			r=v1*v1+v2*v2;
		}while (r>=1.0);
		fac=sqrt(-2.0*log(r)/r);
		
		gset=v1*fac;
		iset=1;
		return v2*fac;
	}
	else{
		iset=0;
		return gset;
	}
}


/*
 * Natural log of the gamma function
 * Pg 168 from the book
 *
 */
float gammln(float xx)
{
	
	double x, tmp, ser;
	static double cof[6]={76.18009173, -86.50532033,
		24.01409822, -1.231739516,
	0.120858003e-2, -0.536382e-5};
	int j;
	
	x = xx-1.0;
	tmp = x + 5.5;
	tmp -= (x+0.5)*log(tmp);
	ser = 1.0;
	for(j=0; j<=5; j++){
		x += 1.0;
		ser += cof[j]/x;
	}	
	return -tmp + log(2.50662827465*ser);
}


/*
 *----------------------------------------------------------------
 *	Gamma Distribution
 *	Returns a deviate distributed as a gamma distribution
 *	of integer order ia, i.e., a waiting time to the iath
 *	event in a Poisson process of unit mean, using ran1(idum)
 *	as the source of uniform deviates
 *	p. 292 of the book
 *----------------------------------------------------------------
 */
float gamdev(int ia, int *idum)
{
	int j;
	float am,e,s,v1,v2,x,y;
    
	
	if (ia < 1){
		printf("Error in routine GAMDEV!");
		exit ( 0 );
	}
	if (ia < 6) {
		x=1.0;
		for (j=1;j<=ia;j++) x *= ran1(idum);
		x = -log(x);
	} else {
		do {
			do {
				do {
					v1=2.0*ran1(idum)-1.0;
					v2=2.0*ran1(idum)-1.0;
				} while (v1*v1+v2*v2 > 1.0);
				y=v2/v1;
				am=ia-1;
				s=sqrt(2.0*am+1.0);
				x=s*y+am;
			} while (x <= 0.0);
			e=(1.0+y*y)*exp(am*log(x/am)-s*y);
		} while (ran1(idum) > e);
	}
	return x;
}


