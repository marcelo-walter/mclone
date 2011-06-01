/*
 *----------------------------------------------------------
 *	matrix.c
 *	From Numerical Recipes in C - 2nd edition
 *	Routines needed to inverse a matrix
 *	p. 43
 *---------------------------------------------------------- 
 */

#include "matrix.h"

#include <math.h> 
#include "matrixUtil.h"
#include "../distance/interfacedistance.h"


#define TINY 1.0e-20;

extern VECTORGEODESICPATH *ArrayGeodesicPath;

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


/* Old decompLU.c functions */


void ImprimeMatrix(float **matrix, int tamanho)
{
	int IndLinha, IndColuna;

	for (IndLinha=1; IndLinha<=tamanho; IndLinha++)
	{
		for (IndColuna=1; IndColuna<=tamanho; IndColuna++)
		{
			printf("%+3.3f\t", matrix[IndLinha][IndColuna]);
		}
		printf("\n");
	}
	printf("\n");
}

void ImprimeVector(float *vetor, int tamanho)
{
	int IndLinha;
	for (IndLinha=1; IndLinha<=tamanho; IndLinha++)
	{
		printf("%+2.5f\t", vetor[IndLinha]);
	}
	printf("\n");
}

float *vectorFloatCreate(long IndiceVetor_Inferior, long IndiceVetor_Superior)
{
	float *v;

	v = (float *) malloc((size_t) ((IndiceVetor_Superior-IndiceVetor_Inferior+1+NR_END)*sizeof(float)));
	if (!v)
	{
		printf("allocation failure in vectorFloatCreate()\n");
	}
	return v-IndiceVetor_Inferior+NR_END;
}


int *vectorIntCreate(long IndiceVetor_Inferior, long IndiceVetor_Superior)
{
	int *v;

	v = (int *) malloc((size_t) ((IndiceVetor_Superior-IndiceVetor_Inferior+1+NR_END)*sizeof(int)));
	if (!v)
	{
		printf("allocation failure in vectorFloatCreate()\n");
	}
	return v-IndiceVetor_Inferior+NR_END;
}

void setVector(float *vector, int tamanho, char eixo)
{
	int IndAux, vectorIndex = 1;
	AuxNodo1 = headV->next;

	if (eixo == 'X')
	{
		while (AuxNodo1 != zedV)
		{
			vector[vectorIndex] = (AuxNodo1->vEnd.x);

			vectorIndex++;
			AuxNodo1 = AuxNodo1->next;
		}
	}
	else {
		if (eixo == 'Y')
		{
			while (AuxNodo1 != zedV)
			{
				vector[vectorIndex] = (AuxNodo1->vEnd.y);

				vectorIndex++;
				AuxNodo1 = AuxNodo1->next;
			}
		}
		else {
			while (AuxNodo1 != zedV)
			{
				vector[vectorIndex] = (AuxNodo1->vEnd.z);

				vectorIndex++;
				AuxNodo1 = AuxNodo1->next;
			}
		}
	}

	for (IndAux=vectorIndex; IndAux<=tamanho; IndAux++)
	{
		vector[IndAux] = 0.0;
	}
}

void free_vectorFloatCreate(float *v, long IndiceVetor_Inferior, long IndiceVetor_Superior)
{
	free((FREE_ARG) (v+IndiceVetor_Inferior-NR_END));
}

void free_vectorIntCreate(int *v, long IndiceVetor_Inferior, long IndiceVetor_Superior)
{
	free((FREE_ARG) (v+IndiceVetor_Inferior-NR_END));
}

float **matrixCreate(long IndiceLinhaMatrix_Infeior, long IndiceLinhaMatrix_Superior, long IndiceColunaMatrix_Inferior, long IndiceColunaMatrix_Superior)	//allocate a float matrix with subscript range m[IndiceLinhaMatrix_Infeior..IndiceLinhaMatrix_Superior][IndiceColunaMatrix_Inferior..IndiceColunaMatrix_Superior]
{
	long i, j, NumLinha = IndiceLinhaMatrix_Superior-IndiceLinhaMatrix_Infeior+1, NumColuna = IndiceColunaMatrix_Superior-IndiceColunaMatrix_Inferior+1;
	float **m;

	/* allocate pointers to rows */
	m = (float **) malloc((size_t) ((NumLinha+NR_END)*sizeof(float*)));
	if (!m)
	{
		printf("allocation failure 1 in matrix()\n");
	}
	m += NR_END;
	m -= IndiceLinhaMatrix_Infeior;

	/* allocate rows and set pointers to them */
	m[IndiceLinhaMatrix_Infeior]=(float *) malloc((size_t) ((NumLinha*NumColuna+NR_END)*sizeof(float)));

	if (!m[IndiceLinhaMatrix_Infeior])
	{
		printf("allocation failure 2 in matrix()\n");
	}
	m[IndiceLinhaMatrix_Infeior] += NR_END;
	m[IndiceLinhaMatrix_Infeior] -= IndiceColunaMatrix_Inferior;

	for (i = IndiceLinhaMatrix_Infeior+1; i<=IndiceLinhaMatrix_Superior; i++)
	{
		m[i] = m[i-1] + NumColuna;
	}

	for (i=1; i<=IndiceLinhaMatrix_Superior; i++)
	{
		for (j=1; j<=IndiceColunaMatrix_Superior; j++)
		{
			m[i][j] = 0.0;
		}
	}

	/* return pointer to array of pointers to rows */
	return m;
}

void setMatrix(float **m, long numColunaLinha, int TypeRBFEuclidian)
{
	long i, j, x, Aux, jPlus, jPlusPlus;
	float ValorEntradaFuncaoRadial = 0.0;
	float CiX, CiY, CiZ, CjX, CjY, CjZ;

	/* Puts the 0s and 1s to be use on Decomposicao LU */
	//Aux = numColunaLinha-2;	//For 2D
	Aux = numColunaLinha-3; //For 3D
	for (i=1; i < Aux; i++)
	{
		m[i][Aux] = 1.0;
		m[Aux][i] = 1.0;
	}
	for (i=Aux; i <= numColunaLinha; i++)
	{
		for (j=Aux; j <= numColunaLinha; j++)
		{
			m[i][j] = 0.0;
		}
	}

	i = 1;
	j = Aux+1;
	jPlus = Aux+2;
	jPlusPlus = Aux+3;

	AuxNodo1 = headV->next;
	while (AuxNodo1 != zedV)
	{
		CiX = (AuxNodo1->vBegin.x);
		CiY = (AuxNodo1->vBegin.y);
		CiZ = (AuxNodo1->vBegin.z);

		m[i][j] = CiX;
		m[i][jPlus] = CiY;
		m[i][jPlusPlus] = CiZ; //For 3D

		m[j][i] = CiX;
		m[jPlus][i] = CiY;
		m[jPlusPlus][i] = CiZ; //For 3D

		AuxNodo2 = headV->next;
		x = 1;
		while (AuxNodo2 != zedV)
		{
			CjX = (AuxNodo2->vBegin.x);
			CjY = (AuxNodo2->vBegin.y);
			CjZ = (AuxNodo2->vBegin.z);

			if (AuxNodo1->faceIndex == AuxNodo2->faceIndex)
			{
				m[i][x] = 0.0;
			}
			else {
				ValorEntradaFuncaoRadial = sqrt( (pow((CiX - CjX),2)) + (pow((CiY - CjY),2)) + (pow((CiZ - CjZ),2)) );
				/*
				 printf("(%f - %f) (%f - %f) (%f - %f)\n", CiX, CjX, CiY, CjY, CiZ, CjZ);
				 printf("[%i -> %i] <%f> <%f> <%f>", AuxNodo1->faceIndex, AuxNodo2->faceIndex, (CiX - CjX), (CiY - CjY), (CiZ - CjZ));
				 printf(" = %f\n", ValorEntradaFuncaoRadial);*/

				switch (TypeRBFEuclidian)
				{
					case 0: {
						//Using Multiquadratic
						m[i][x] = pow((pow(ValorEntradaFuncaoRadial,2)+pow(0.5,2)),0.5);
					} break;
					case 1: {
						//Using Gaussian RBF Function
						m[i][x] = exp(-(pow(ValorEntradaFuncaoRadial,2))*pow(2.0,2));
					} break;
					case 2: {
						//Using Thin-Plane RBF (3D)
						m[i][x] = pow(ValorEntradaFuncaoRadial, 3);
					} break;
					case 3: {
						//Using Raio only (3D)
						m[i][x] = ValorEntradaFuncaoRadial;
					} break;
					case 4: {
						//Using Thin-Plane RBF (2D)
						m[i][x] = ((pow((fabs(ValorEntradaFuncaoRadial)),2)) * (log10(fabs(ValorEntradaFuncaoRadial))));
					} break;
					case 5: {
						//Using Thin-Plane RBF (3D)
						m[i][x] = (pow((ValorEntradaFuncaoRadial * 2), 3));
					} break;
					case 6: {
						//Using Raio only (3D)
						m[i][x] = (ValorEntradaFuncaoRadial * 2);
					} break;
				}
			}
			x++;
			AuxNodo2 = AuxNodo2->next;
		}

		i++;
		AuxNodo1 = AuxNodo1->next;
	}
}

void setMatrixWithGeodesicPath(float **m, long numColunaLinha, int TypeRBFGeodesic)
{
	long i, j, x, Aux, jPlus, jPlusPlus, IndiceGeodesicPath, OffSetIndiceGeodesicPath;
	float ValorEntradaFuncaoRadial = 0.0;
	float CiX, CiY, CiZ, CjX, CjY, CjZ;

	/* Puts the 0s and 1s to be use on Decomposicao LU */
	//Aux = numColunaLinha-2;	//For 2D
	Aux = numColunaLinha-3; //For 3D
	for (i=1; i < Aux; i++)
	{
		m[i][Aux] = 1.0;
		m[Aux][i] = 1.0;
	}
	for (i=Aux; i <= numColunaLinha; i++)
	{
		for (j=Aux; j <= numColunaLinha; j++)
		{
			m[i][j] = 0.0;
		}
	}

	i = 1;
	j = Aux+1;
	jPlus = Aux+2;
	jPlusPlus = Aux+3;

	AuxNodo1 = headV->next;
	while (AuxNodo1 != zedV)
	{
		CiX = (AuxNodo1->vBegin.x);
		CiY = (AuxNodo1->vBegin.y);
		CiZ = (AuxNodo1->vBegin.z);

		m[i][j] = CiX;
		m[i][jPlus] = CiY;
		m[i][jPlusPlus] = CiZ; //For 3D

		m[j][i] = CiX;
		m[jPlus][i] = CiY;
		m[jPlusPlus][i] = CiZ; //For 3D

		AuxNodo2 = headV->next;
		x = 1;
		while (AuxNodo2 != zedV)
		{
			CjX = (AuxNodo2->vBegin.x);
			CjY = (AuxNodo2->vBegin.y);
			CjZ = (AuxNodo2->vBegin.z);

			if (AuxNodo1->faceIndex == AuxNodo2->faceIndex)
			{
				m[i][x] = 0.0;
			}
			else {
				if (AuxNodo1->faceIndex < AuxNodo2->faceIndex)
				{
					IndiceGeodesicPath = ((AuxNodo1->faceIndex * NumberFaces) - ((AuxNodo1->faceIndex * (AuxNodo1->faceIndex + 1))/2));
					OffSetIndiceGeodesicPath = ((AuxNodo2->faceIndex - AuxNodo1->faceIndex)-1);
				}
				else {
					IndiceGeodesicPath = ((AuxNodo2->faceIndex * NumberFaces) - ((AuxNodo2->faceIndex * (AuxNodo2->faceIndex+1))/2));
					OffSetIndiceGeodesicPath = ((AuxNodo1->faceIndex - AuxNodo2->faceIndex)-1);
				}
				ValorEntradaFuncaoRadial = sqrt( (pow((CiX - CjX),2)) + (pow((CiY - CjY),2)) + (pow((CiZ - CjZ),2)) );
				ValorEntradaFuncaoRadial += ((ArrayGeodesicPath[(IndiceGeodesicPath + OffSetIndiceGeodesicPath)].Distance) * 2);

				//fprintf( stderr, "\n[%d][%d] = %f (%d, %d)\n", AuxNodo1->faceIndex, AuxNodo2->faceIndex, ArrayGeodesicPath[(IndiceGeodesicPath + OffSetIndiceGeodesicPath)].Distance, ArrayGeodesicPath[(IndiceGeodesicPath + OffSetIndiceGeodesicPath)].Path1, ArrayGeodesicPath[(IndiceGeodesicPath + OffSetIndiceGeodesicPath)].Path2);

				switch (TypeRBFGeodesic)
				{
					case 0: {
						//Using Multiquadratic
						m[i][x] = pow((pow(ValorEntradaFuncaoRadial,2)+pow(0.5,2)),0.5);
					} break;
					case 1: {
						//Using Gaussian RBF Function
						m[i][x] = exp(-(pow(ValorEntradaFuncaoRadial,2))*pow(2.0,2));
					} break;
					case 2: {
						//Using Thin-Plane RBF (3D)
						m[i][x] = pow(ValorEntradaFuncaoRadial, 3);
					} break;
					case 3: {
						//Using Raio only (3D)
						m[i][x] = ValorEntradaFuncaoRadial;
					} break;
					case 4: {
						//Using Thin-Plane RBF (2D)
						m[i][x] = ((pow((fabs(ValorEntradaFuncaoRadial)),2)) * (log10(fabs(ValorEntradaFuncaoRadial))));
					} break;
				}
			}
			x++;
			AuxNodo2 = AuxNodo2->next;
		}

		i++;
		AuxNodo1 = AuxNodo1->next;
	}
}

void free_matrixLU(float **m, long IndiceLinhaMatrix_Infeior, long IndiceLinhaMatrix_Superior, long IndiceColunaMatrix_Inferior, long IndiceColunaMatrix_Superior)	//free a float matrix allocated by matrix()
{
	free((FREE_ARG) (m[IndiceLinhaMatrix_Infeior]+IndiceColunaMatrix_Inferior-NR_END));
	free((FREE_ARG) (m+IndiceLinhaMatrix_Infeior-NR_END));
}

void ludcmpLU(float **a, int n, int *indx, float *d)
/*	Given a matrix a[1..n][1..n], this routine replaces it by the LU decomposition of a rowwise
 permutation of itself. a and n are input. a is output, arranged as in equation (2.3.14) above;
 indx[1..n] is an output vectorFloatCreate that records the row permutation e.ected by the partial
 pivoting; d is output as ?1 depending on whether the number of row interchanges was even
 or odd, respectively. This routine is used in combination with lubksb to solve linear equations
 or invert a matrix.	*/
{
	int i,imax,j,k;
	float big,dum,sum,temp;
	float *vv;		//vv stores the implicit scaling of each row.

	vv=vectorFloatCreate(1,n);
	*d=1.0;			//No row interchanges yet.
	for (i=1;i<=n;i++)
	{				//Loop over rows to get the implicit scaling information.
		big=0.0;
		for (j=1;j<=n;j++)
		{
			if ((temp=fabs(a[i][j])) > big)
			{
				big=temp;
			}
		}
		if (big == 0.0)
		{
			printf("Singular matrix in routine ludcmp\n");	//No nonzero largest element.
		}
		vv[i]=1.0/big;	//Save the scaling.
	}
	for (j=1;j<=n;j++)	//This is the loop over columns of Crout?s method.
	{
		for (i=1;i<j;i++)	//This is equation (2.3.12) except for i = j.
		{
			sum=a[i][j];
			for (k=1;k<i;k++)
			{
				sum -= a[i][k]*a[k][j];
			}
			a[i][j]=sum;
		}
		big=0.0;	//Initialize for the search for largest pivot element.
		for (i=j;i<=n;i++)	//This is i = j of equation (2.3.12) and i = j+1. . .N
		{
			sum=a[i][j];	//of equation (2.3.13).
			for (k=1;k<j;k++)
			{
				sum -= a[i][k]*a[k][j];
			}
			a[i][j]=sum;
			if ((dum=vv[i]*fabs(sum)) >= big)	//Is the .gure of merit for the pivot better than the best so far?
			{
				big=dum;
				imax=i;
			}
		}
		if (j != imax)	//Do we need to interchange rows?
		{
			for (k=1;k<=n;k++)	//Yes, do so...
			{
				dum=a[imax][k];
				a[imax][k]=a[j][k];
				a[j][k]=dum;
			}
			*d = -(*d);			//...and change the parity of d.
			vv[imax]=vv[j];		//Also interchange the scale factor.
		}
		indx[j]= imax;
		if (a[j][j] == 0.0)
		{
			a[j][j]=(float) TINY;
		}
		/*	If the pivot element is zero the matrix is singular (at least to the precision of the
		 algorithm). For some applications on singular matrices, it is desirable to substitute
		 TINY for zero.	*/
		if (j != n)	//Now, .nally, divide by the pivot element.
		{
			dum=1.0/(a[j][j]);
			for (i=j+1;i<=n;i++)
			{
				a[i][j] *= dum;
			}
		}
	}	//Go back for the next column in the reduction.
	free_vectorFloatCreate(vv,1,n);
}

void lubksbLU(float **a, int n, int *indx, float b[])
/*	Solves the set of n linear equations A?X = B. Here a[1..n][1..n] is input, not as the matrix
 A but rather as its LU decomposition, determined by the routine ludcmp. indx[1..n] is input
 as the permutation vectorFloatCreate returned by ludcmp. b[1..n] is input as the right-hand side vectorFloatCreate
 B, and returns with the solution vectorFloatCreate X. a, n, and indx are not modi.ed by this routine
 and can be left in place for successive calls with di.erent right-hand sides b. This routine takes
 into account the possibility that b will begin with many zero elements, so it is e.cient for use
 in matrix inversion.	*/
{
	int i,ii=0,ip,j;
	float sum;

	for (i=1;i<=n;i++)
	{
		/*	When ii is set to a positive value, it will become the
		 index of the .rst nonvanishing element of b. Wenow
		 do the forward substitution, equation (2.3.6). The
		 oIndiceVetor_Inferiory new wrinkle is to unscramble the permutation
		 as we go.	*/
		ip= indx[i];
		sum=b[ip];
		b[ip]=b[i];
		if (ii)
		{
			for (j=ii;j<=i-1;j++)
			{
				sum -= a[i][j]*b[j];
			}
		}
		else {
			if (sum)
			{
				ii=i;
				/*	A nonzero element was encountered, so from now on we
				 will have to do the sums in the loop above. b[i]=sum;	*/
			}
		}
		b[i]=sum;
	}

	for (i=n;i>=1;i--)
	{				//Now we do the backsubstitution, equation (2.3.7).
		sum=b[i];
		for (j=i+1;j<=n;j++)
		{
			sum -= a[i][j]*b[j];
		}
		b[i]=sum/a[i][i];	//Store a component of the solution vectorFloatCreate X.
	}	//All done!
}

#undef TINY
