


#include "util/nrutil.h"
#include <stdio.h>

/*
 *-------------------------------------------------------------------------
 *	Given arrays x[1..n] and y[1..n] containing a tabulated
 *	function yi = f(xi), with x1 < x2 < ... < xN and given
 *	values yp1 and ypn for the first derivative of the
 *	interpolating function at points 1 and n, respectively,
 *	this routine returns an array y2[1..n] that contains
 *	the 2nd derivatives of the interpolating function at the
 *	tabulated points xi. If yp1 and/or ypn are equal to
 *	1e30 or larger, the routine is signaled to set the
 *	corresponding boundary condition for a natural spline
 *	with zero 2nd derivative on that boundary
 *-------------------------------------------------------------------------
 */
void spline(float x[], float y[], int n, float yp1, float ypn, float y2[])
{
    int i,k;
    float p,qn,sig,un,*u;
	
    u=vector(1,n-1);
    if (yp1 > 0.99e30)
		y2[1]=u[1]=0.0;
    else {
		y2[1] = -0.5;
		u[1]=(3.0/(x[2]-x[1]))*((y[2]-y[1])/(x[2]-x[1])-yp1);
    }
    for (i=2;i<=n-1;i++) {
		sig=(x[i]-x[i-1])/(x[i+1]-x[i-1]);
		p=sig*y2[i-1]+2.0;
		y2[i]=(sig-1.0)/p;
		u[i]=(y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
		u[i]=(6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
    }
    if (ypn > 0.99e30)
		qn=un=0.0;
    else {
		qn=0.5;
		un=(3.0/(x[n]-x[n-1]))*(ypn-(y[n]-y[n-1])/(x[n]-x[n-1]));
    }
    y2[n]=(un-qn*u[n-1])/(qn*y2[n-1]+1.0);
    for (k=n-1;k>=1;k--)
		y2[k]=y2[k]*y2[k+1]+u[k];
    free_vector(u,1,n-1);
}

/*
 *-------------------------------------------------------------------------
 *	Given the arrays xa[1..n] and ya[1..n] which tabulate a
 *	function (with the xai's in order) and given the array
 *	y2a[1..n] which is the output from the function spline
 *	above, and given a value of x, this routine returns a
 *	cubic-spline interpolated value y
 *-------------------------------------------------------------------------
 */
void splint(float xa[], float ya[], float y2a[], int n, float x,float *y)
{
    int klo,khi,k;
    float h,b,a;
	
    klo=1;
    khi=n;
    while (khi-klo > 1) {
		k=(khi+klo) >> 1;
		if (xa[k] > x) khi=k;
		else klo=k;
    }
    h=xa[khi]-xa[klo];
    if (h == 0.0) nrerror("Bad XA input to routine SPLINT");
    a=(xa[khi]-x)/h;
    b=(x-xa[klo])/h;
    *y=a*ya[klo]+b*ya[khi]+((a*a*a-a)*y2a[klo]+(b*b*b-b)*y2a[khi])*(h*h)/6.0;
}
/*
 *-------------------------------------------------------------------------
 *	Given the arrays xa[1..n] and ya[1..n] which tabulate a
 *	function (with the xai's in order) and given a value of x,
 *	this routine returns a linear interpolated value y
 *-------------------------------------------------------------------------
 */
void linint(float xa[], float ya[], int n, float x, float *y)
{
	
	int i;
	float t;
	
	/* find the range for x */
	i = 1;
	while ( x > xa[i] ) i++;
	
	if ( i > n ){
		printf("i = %d n = %d\n", i, n );
		printf("x = %f xa = %f\n", x, xa[i]);
		/*comented by Fabiane Queiroz*/
		//nrerror("Bad index in routine LININT");
	}
	
	if ( i == 1){ /* it's the first element */
		*y = ya[i];
		/* printf("y = %f\n",  *y );
		 printf("y[%d] = %f\n", i, ya[i]);
		 printf("x = %f xa[%d] = %f\n", x, i, xa[i]); */
	}
	else{
		t = ( x - xa[i-1]) / ( xa[i] - xa[i-1] );
		*y = ya[i-1] * ( 1.0 - t) + ya[i] * t;
		/* printf("\n");
		 printf("t = %f y = %f\n", t, *y );
		 printf("y[%d] = %f y[%d] = %f\n", i-1, ya[i-1], i, ya[i]);
		 printf("x = %f xa[%d] = %f xa[%d] = %f\n", x, i-1, xa[i-1], i, xa[i]); */
	}
}
