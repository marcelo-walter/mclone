/*
 *----------------------------------------------------------
 *	matrix.c
 *	From Numerical Recipes in C - 2nd edition
 *	Routines needed to inverse a matrix
 *	p. 43
 *---------------------------------------------------------- 
 */

#include <math.h> 
#include "nrutil.h"

#define TINY 1.0e-20;
void lubksb(float **a, int n, int *indx, float b[]);
void ludcmp(float **a, int n, int *indx, float *d);

/*
 * Inverts a matrix using an LU decomposition. The matrix 'y'
 * contains the inverse of the original matrix 'a',  which
 * is destroyed
 */
void inverseMatrix(float **a, int n, float **y)
{
	int i, j, *indx;
	float d, *col;
	
	indx = ivector(1, n);
	col = vector(1, n);
	ludcmp(a, n, indx, &d);
	for(j = 1; j <= n; j++){
		for(i = 1; i <= n; i++) col[i] = 0.0;
		col[j] = 1.0;
		lubksb(a, n, indx, col);
		for(i = 1; i <= n; i++) y[i][j] = col[i];
	}
	
	free_ivector(indx, 1, n);
	free_vector(col, 1, n);
}
/*
 * Solves the set of 'n' linear equations A X = B. Here a[1..n][1..n]
 * is input,  not as the matrix A but rather as its LU decomposition, 
 * determined by the routine 'ludcmp'. 'indx[1..n]' is input as the
 * permutation vector returned by 'ludcmp'. 'b[1..n]' is input as
 * the right-hand side vector B,  and returns with the solution vector
 * X. 'a', 'n', and 'indx' are not modified by this routine and can
 * be left in place for successive calls with different right-hand
 * sides 'b'. This routine takes into account the possibility that 'b'
 * will begin with many zero elements,  so it is efficient for use
 * in matrix inversion
 */
void lubksb(float **a, int n, int *indx, float b[]) 
{ 
	int i,ii=0,ip,j; 
	float sum; 
	
	for (i=1;i<=n;i++) { 
		ip=indx[i]; 
		sum=b[ip]; 
		b[ip]=b[i]; 
		if (ii) 
			for (j=ii;j<=i-1;j++) sum -= a[i][j]*b[j]; 
		else if (sum) ii=i; 
		b[i]=sum; 
	} 
	for (i=n;i>=1;i--) { 
		sum=b[i]; 
		for (j=i+1;j<=n;j++) sum -= a[i][j]*b[j]; 
		b[i]=sum/a[i][i]; 
	} 
} 

/*
 * Given an n x n matrix a[1..n][1..n] this routine replaces
 * it by the LU decomposition of a rowise permutation of itself.
 * 'a' and 'n' are input. 'a' is output,  arranged as in equation
 * (2.3.14) above; 'indx[1..n] is an output vector which records
 * the row permutation effected by the partial pivoting; 'd' is
 * output as +- 1 depending on whether the number of row interchanges
 * was even or odd,  respectively. This routine is used in combination
 * with 'lubksb' to solve linear equations or invert a matrix
 */
void ludcmp(float **a, int n, int *indx, float *d)
{ 
	int i,imax,j,k; 
	float big,dum,sum,temp; 
	float *vv;
	
	vv=vector(1,n); 
	*d=1.0; 
	for (i=1;i<=n;i++) { 
		big=0.0; 
		for (j=1;j<=n;j++) 
			if ((temp=fabs(a[i][j])) > big) big=temp; 
		if (big == 0.0) nrerror("Singular matrix in routine LUDCMP"); 
		vv[i]=1.0/big; 
	} 
	for (j=1;j<=n;j++) { 
		for (i=1;i<j;i++) { 
			sum=a[i][j]; 
			for (k=1;k<i;k++) sum -= a[i][k]*a[k][j]; 
			a[i][j]=sum; 
		} 
		big=0.0; 
		for (i=j;i<=n;i++) { 
			sum=a[i][j]; 
			for (k=1;k<j;k++) 
				sum -= a[i][k]*a[k][j]; 
			a[i][j]=sum; 
			if ( (dum=vv[i]*fabs(sum)) >= big) { 
				big=dum; 
				imax=i; 
			} 
		} 
		if (j != imax) { 
			for (k=1;k<=n;k++) { 
				dum=a[imax][k]; 
				a[imax][k]=a[j][k]; 
				a[j][k]=dum; 
			} 
			*d = -(*d); 
			vv[imax]=vv[j]; 
		} 
		indx[j]=imax; 
		if (a[j][j] == 0.0) a[j][j]=TINY; 
		if (j != n) { 
			dum=1.0/(a[j][j]); 
			for (i=j+1;i<=n;i++) a[i][j] *= dum; 
		} 
	} 
	free_vector(vv,1,n); 
} 

#undef TINY 
