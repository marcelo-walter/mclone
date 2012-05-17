/*
 *  matrix.h
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/14/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */

#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "control/vectorField.h"

#define NR_END 1
#define FREE_ARG char*


void lubksb(float **a, int n, int *indx, float b[]);
void ludcmp(float **a, int n, int *indx, float *d);
void inverseMatrix(float **a, int n, float **y);
void lubksb(float **a, int n, int *indx, float b[]);
void ludcmp(float **a, int n, int *indx, float *d);
/* Old decompLU.c functions */
void ImprimeMatrix(float **matrix, int tamanho);
void ImprimeVector(float *vetor, int tamanho);
float *vectorFloatCreate(long IndiceVetor_Inferior, long IndiceVetor_Superior);
int *vectorIntCreate(long IndiceVetor_Inferior, long IndiceVetor_Superior);
void setVector(float *vector, int tamanho, char eixo);
void free_vectorFloatCreate(float *v, long IndiceVetor_Inferior, long IndiceVetor_Superior);
void free_vectorIntCreate(int *v, long IndiceVetor_Inferior, long IndiceVetor_Superior);
float **matrixCreate(long IndiceLinhaMatrix_Infeior, long IndiceLinhaMatrix_Superior, long IndiceColunaMatrix_Inferior, long IndiceColunaMatrix_Superior);	//allocate a float matrix with subscript range m[IndiceLinhaMatrix_Infeior..IndiceLinhaMatrix_Superior][IndiceColunaMatrix_Inferior..IndiceColunaMatrix_Superior]
void setMatrix(float **m, long numColunaLinha, InterpolationMode typeRBFEuclidian);
void setMatrixWithGeodesicPath(float **m, long numColunaLinha, InterpolationMode typeRBFGeodesic);
void free_matrixLU(float **m, long IndiceLinhaMatrix_Infeior, long IndiceLinhaMatrix_Superior, long IndiceColunaMatrix_Inferior, long IndiceColunaMatrix_Superior); //free a float matrix allocated by matrix()
void ludcmpLU(float **a, int n, int *indx, float *d);
void lubksbLU(float **a, int n, int *indx, float b[]);

#endif //_MATRIX_H_
