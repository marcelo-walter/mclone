/*
 *------------------------------------------------------------
 *	vectorField.c
 *	Thompson Peter Lied
 *
 *	This module contains routines to create the orientation 
 *	vectors and the vector field
 *------------------------------------------------------------
 */

#include "vectorField.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "texture.h"
#include "../distance/interfacedistance.h"
#include "../simulation/relax.h"
#include "../data/Object.h"
#include "../data/fileManager.h"
#include "../data/Matrix4.h"
#include "../util/genericUtil.h"
#include "../util/matrix.h"
#include "../util/printInfo.h"
#include "../util/distPoints.h"

#include "../viewer/graph.h" //Remover isso, nao deveria usar esse arquivo
#include "../viewer/ui.h"

/*
 *---------------------------------------------------------------
 *
 * GLOBAL VARIABLES
 *
 *---------------------------------------------------------------
 */
/* head and tail nodes of the global linked list */
VECTORARRAY *headV;
VECTORARRAY *zedV;
int vectorSize = sizeof(VECTORARRAY);

float coordsBegin[3];
float coordsEnd[3];

int nVectors = 0;
int PointPicking = -1;

const float Pi = 3.14159265358979323846264338328;
const double RadToDeg = (3.14159265358979323846264338328/180);

int WithInterpolationType;

/*
 *---------------------------------------------------------------
 *
 * GLOBAL VARIABLES
 *
 *---------------------------------------------------------------
 */
flag createVect;
flag removevect;

flag polygonPicking = FALSE;

float **MatrixLUX, **MatrixLUY, **MatrixLUZ, *VectorLUResultX, *VectorLUResultY, *VectorLUResultZ, LUAuxX, LUAuxY, LUAuxZ;
int NumColunaLinha, *VectorIndexX, *VectorIndexY, *VectorIndexZ;

/*
 * Project the vector on the face.
 */
void ProjetaVetorFace(int IndexFace, double xFound, double yFound, double zFound, Point3D *p)
{
	
	double a, Modulo;
	Point3D VetorNormal, VetorVetor, VetorFace, VetorNaOrigem, Proj;
	
	VetorNormal.x = faces[IndexFace].planeCoef.a;
	VetorNormal.y = faces[IndexFace].planeCoef.b;
	VetorNormal.z = faces[IndexFace].planeCoef.c;
	
	VetorVetor.x = (xFound - faces[IndexFace].center3D.x);
	VetorVetor.y = (yFound - faces[IndexFace].center3D.y);
	VetorVetor.z = (zFound - faces[IndexFace].center3D.z);
	
	a = (((VetorVetor.x * VetorNormal.x) + (VetorVetor.y * VetorNormal.y) + (VetorVetor.z * VetorNormal.z)) / (((VetorNormal.x * VetorNormal.x) + (VetorNormal.y * VetorNormal.y)) + (VetorNormal.z * VetorNormal.z)));
	
	VetorFace.x = (VetorVetor.x - (a * VetorNormal.x));
	VetorFace.y = (VetorVetor.y - (a * VetorNormal.y));
	VetorFace.z = (VetorVetor.z - (a * VetorNormal.z));
	
	Proj.x = (VetorFace.x + faces[IndexFace].center3D.x);
	Proj.y = (VetorFace.y + faces[IndexFace].center3D.y);
	Proj.z = (VetorFace.z + faces[IndexFace].center3D.z);
	
	VetorNaOrigem.x = (Proj.x - faces[IndexFace].center3D.x);
	VetorNaOrigem.y = (Proj.y - faces[IndexFace].center3D.y);
	VetorNaOrigem.z = (Proj.z - faces[IndexFace].center3D.z);
	
	Modulo = sqrt((VetorFace.x*VetorFace.x) + (VetorFace.y*VetorFace.y) + (VetorFace.z*VetorFace.z));
	//Modulo = sqrt((VetorNaOrigem.x*VetorNaOrigem.x) + (VetorNaOrigem.y*VetorNaOrigem.y) + (VetorNaOrigem.z*VetorNaOrigem.z));
	//Normalize
	//p->x = ((((VetorNaOrigem.x) / Modulo)/10 ) + faces[IndexFace].center3D.x);
   // p->y = ((((VetorNaOrigem.y) / Modulo)/10) + faces[IndexFace].center3D.y);
	//p->z = ((((VetorNaOrigem.z) / Modulo)/10) + faces[IndexFace].center3D.z);
	
	//p->x = ((((VetorFace.x) / Modulo)/80) + faces[IndexFace].center3D.x);
	//p->y = ((((VetorFace.y) / Modulo)/80) + faces[IndexFace].center3D.y);
   // p->z = ((((VetorFace.z) / Modulo)/80) + faces[IndexFace].center3D.z);
	
	p->x = ((((VetorFace.x) / Modulo)/10) + faces[IndexFace].center3D.x);
	p->y = ((((VetorFace.y) / Modulo)/10) + faces[IndexFace].center3D.y);
    p->z = ((((VetorFace.z) / Modulo)/10) + faces[IndexFace].center3D.z);
	
	
	
	//p->x = (VetorNaOrigem.x) / Modulo ;
	//p->y = (VetorNaOrigem.y) / Modulo;
	//p->z = (VetorNaOrigem.z) / Modulo ;
}


void ProjetaVerticeVetorFace(int IndexFace, int IndexVertice, double xFound, double yFound, double zFound, Point3D *p)
{
	double a, Modulo;
	Point3D VetorNormal, VetorVetor, VetorFace, VetorNaOrigem, Proj;
	
	VetorNormal.x = faces[IndexFace].planeCoef.a;
	VetorNormal.y = faces[IndexFace].planeCoef.b;
	VetorNormal.z = faces[IndexFace].planeCoef.c;
	
	VetorVetor.x = (xFound - vert[faces[IndexFace].v[IndexVertice]].pos.x);
	VetorVetor.y = (yFound - vert[faces[IndexFace].v[IndexVertice]].pos.y);
	VetorVetor.z = (zFound - vert[faces[IndexFace].v[IndexVertice]].pos.z);
	
	a = (((VetorVetor.x * VetorNormal.x) + (VetorVetor.y * VetorNormal.y) + (VetorVetor.z * VetorNormal.z)) / (((VetorNormal.x * VetorNormal.x) + (VetorNormal.y * VetorNormal.y)) + (VetorNormal.z * VetorNormal.z)));
	
	VetorFace.x = (VetorVetor.x - (a * VetorNormal.x));
	VetorFace.y = (VetorVetor.y - (a * VetorNormal.y));
	VetorFace.z = (VetorVetor.z - (a * VetorNormal.z));
	
	Proj.x = (VetorFace.x + vert[faces[IndexFace].v[IndexVertice]].pos.x);
	Proj.y = (VetorFace.y + vert[faces[IndexFace].v[IndexVertice]].pos.y);
	Proj.z = (VetorFace.z + vert[faces[IndexFace].v[IndexVertice]].pos.z);
	
	VetorNaOrigem.x = (Proj.x -  vert[faces[IndexFace].v[IndexVertice]].pos.x);
	VetorNaOrigem.y = (Proj.y -  vert[faces[IndexFace].v[IndexVertice]].pos.y);
	VetorNaOrigem.z = (Proj.z -  vert[faces[IndexFace].v[IndexVertice]].pos.z);
	
	Modulo = sqrt((VetorFace.x*VetorFace.x) + (VetorFace.y*VetorFace.y) + (VetorFace.z*VetorFace.z));

	
	p->x = ((((VetorFace.x) / Modulo)) +  vert[faces[IndexFace].v[IndexVertice]].pos.x);
	p->y = ((((VetorFace.y) / Modulo)) +  vert[faces[IndexFace].v[IndexVertice]].pos.y);
    p->z = ((((VetorFace.z) / Modulo)) +  vert[faces[IndexFace].v[IndexVertice]].pos.z);
	
	//printf("Vetores finais: (%f,%f,%f) , (%f,%f,%f), (%f,%f,%f)\n", faces[whichFace].projectedVerticeVector1.x, faces[whichFace].projectedVerticeVector1.y, faces[whichFace].projectedVerticeVector1.z, faces[whichFace].projectedVerticeVector2.x, faces[whichFace].projectedVerticeVector2.y, faces[whichFace].projectedVerticeVector2.z, faces[whichFace].projectedVerticeVector3.x, faces[whichFace].projectedVerticeVector3.y, faces[whichFace].projectedVerticeVector3.z);


}

/*Add by Fabiane Queiroz at 08/2010*/
/*
 *--------------------------------------------------------------
 * 	For each vector, this procedure computes the vector's 3D
 * 	position in the new model based on its begin and end barycentric
 *	coordinates
 *--------------------------------------------------------------
 */
void morphVectorField( void )
{
	//VECTORARRAY *vect;
	//vect = malloc(sizeof(VECTORARRAY));
	//double a,b,c,mod;
	
	VECTORARRAY *NodoAux;

	NodoAux = headV->next;
	while (NodoAux != zedV)
	{
		fromVectorBarycentricCoord(NodoAux);
		NodoAux = NodoAux->next;
	}
	
	CertificaAreaCelulas();
	
	/*for(IndexFace = 0; IndexFace<NumberFaces; IndexFace++){
		vect->vEnd = vectors[IndexFace].vEnd;
		printf("vectors[IndexFace].vEnd: %f\n", vectors[IndexFace].vEnd.x);
		//faces[IndexFace].EndOfVector3D;
		//NodoAux->vEnd = 
		vect->vBegin = faces[IndexFace].center3D;
		///NodoAux->
		fromVectorBarycentricCoord(vect);
	}*/

}

/*
 *	Create the istruction of RBF interpolation algoritm. The parameter Type is used to: 0 = Straight Path; 1 = Geodesic Path
 */
void MakeLUDecomposition3D(int Type)
{
	//NumColunaLinha = nVectors+3; //For 2D
	NumColunaLinha = nVectors+4; //For 3D
	printf("Num Vetores [%i]\n", nVectors);
	
	free_vectorFloatCreate(VectorLUResultX, 1, NumColunaLinha);
	free_vectorFloatCreate(VectorLUResultY, 1, NumColunaLinha);
	free_vectorFloatCreate(VectorLUResultZ, 1, NumColunaLinha);
	
	MatrixLUX = matrixCreate(1, NumColunaLinha, 1, NumColunaLinha);
	MatrixLUY = matrixCreate(1, NumColunaLinha, 1, NumColunaLinha);
	MatrixLUZ = matrixCreate(1, NumColunaLinha, 1, NumColunaLinha);
	
	VectorIndexX = vectorIntCreate(1, NumColunaLinha);
	VectorIndexY = vectorIntCreate(1, NumColunaLinha);
	VectorIndexZ = vectorIntCreate(1, NumColunaLinha);
	
	VectorLUResultX = vectorFloatCreate(1, NumColunaLinha);
	VectorLUResultY = vectorFloatCreate(1, NumColunaLinha);
	VectorLUResultZ = vectorFloatCreate(1, NumColunaLinha);
	
	if (Type)
	{
		//Using Straight Path Lenght
		setMatrix(MatrixLUX, NumColunaLinha, -1); //ATENCAO: Usado -1 apenas para compilar, REVER. TODO
		setMatrix(MatrixLUY, NumColunaLinha, -1); //ATENCAO: Usado -1 apenas para compilar, REVER. TODO
		setMatrix(MatrixLUZ, NumColunaLinha, -1); //ATENCAO: Usado -1 apenas para compilar, REVER. TODO
	}
	else {
		//Using Geodesic Path Lenght
		setMatrixWithGeodesicPath(MatrixLUX, NumColunaLinha, -1); //ATENCAO: Usado -1 apenas para compilar, REVER. TODO
		setMatrixWithGeodesicPath(MatrixLUY, NumColunaLinha, -1); //ATENCAO: Usado -1 apenas para compilar, REVER. TODO
		setMatrixWithGeodesicPath(MatrixLUZ, NumColunaLinha, -1); //ATENCAO: Usado -1 apenas para compilar, REVER. TODO
	}
	
	setVector(VectorLUResultX, NumColunaLinha, 'X');
	setVector(VectorLUResultY, NumColunaLinha, 'Y');
	setVector(VectorLUResultZ, NumColunaLinha, 'Z');
	/*
	 printf("Primeiro\nX:\n");
	 ImprimeMatrix(MatrixLUX, NumColunaLinha);
	 printf("Y:\n");
	 ImprimeMatrix(MatrixLUY, NumColunaLinha);
	 printf("Z:\n");
	 ImprimeMatrix(MatrixLUZ, NumColunaLinha);
	 
	 printf("\nX:\n");
	 ImprimeVector(VectorLUResultX, NumColunaLinha);
	 printf("Y:\n");
	 ImprimeVector(VectorLUResultY, NumColunaLinha);
	 printf("Z:\n");
	 ImprimeVector(VectorLUResultZ, NumColunaLinha);
	 printf("\n");*/
	
	ludcmpLU(MatrixLUX, NumColunaLinha, VectorIndexX, &LUAuxX);
	ludcmpLU(MatrixLUY, NumColunaLinha, VectorIndexY, &LUAuxY);
	ludcmpLU(MatrixLUZ, NumColunaLinha, VectorIndexZ, &LUAuxZ);
	/*
	 printf("Segundo\n");
	 ImprimeMatrix(MatrixLUX, NumColunaLinha);
	 ImprimeVector(VectorLUResultX, NumColunaLinha);
	 printf("\n");*/
	lubksbLU(MatrixLUX, NumColunaLinha, VectorIndexX, VectorLUResultX);
	lubksbLU(MatrixLUY, NumColunaLinha, VectorIndexY, VectorLUResultY);
	lubksbLU(MatrixLUZ, NumColunaLinha, VectorIndexZ, VectorLUResultZ);
	/*
	 printf("Terceiro\n");
	 ImprimeMatrix(MatrixLUX, NumColunaLinha);
	 ImprimeVector(VectorLUResultX, NumColunaLinha);
	 */
	free_vectorIntCreate(VectorIndexX, 1, NumColunaLinha);
	free_vectorIntCreate(VectorIndexY, 1, NumColunaLinha);
	free_vectorIntCreate(VectorIndexZ, 1, NumColunaLinha);
	
	free_matrixLU(MatrixLUX, 1, NumColunaLinha, 1, NumColunaLinha);
	free_matrixLU(MatrixLUY, 1, NumColunaLinha, 1, NumColunaLinha);
	free_matrixLU(MatrixLUZ, 1, NumColunaLinha, 1, NumColunaLinha);
}

float InterpolationRBFX(float x, float y, float z)
{
	float rbfSomatorioResult = 0.0;
	int indiceAux = 1, p0 = NumColunaLinha-3, p1 = NumColunaLinha-2, p2 = NumColunaLinha-1, p3 = NumColunaLinha;
	float ValorEntradaFuncaoRadial;

	VECTORARRAY *NodoAux;
	
	NodoAux = headV->next;
	while (NodoAux != zedV)
	{
		ValorEntradaFuncaoRadial = sqrt( (pow((x - NodoAux->vBegin.x),2)) + (pow((y - NodoAux->vBegin.y),2)) + (pow((z - NodoAux->vBegin.z),2)) );
		
		//Using Multiquadratic
		//rbfSomatorioResult += (VectorLUResultX[indiceAux] * (pow((pow(ValorEntradaFuncaoRadial,2)+pow(0.5,2)),0.5)));
		
		//Using Gaussian RBF Function
		//rbfSomatorioResult += ( VectorLUResultX[indiceAux] * ( exp( -( pow(ValorEntradaFuncaoRadial,2) ) * pow(2.0,2) ) ) );
		rbfSomatorioResult +=  VectorLUResultX[indiceAux] *  exp( - (4.0 * (pow(ValorEntradaFuncaoRadial,2 )) ) );
		
		//Using Thin-Plane RBF (3D)
	//	rbfSomatorioResult += (VectorLUResultX[indiceAux] * (pow(ValorEntradaFuncaoRadial, 3)));
		
		//Using Raio only (3D)
		//rbfSomatorioResult += (VectorLUResultX[indiceAux] * (ValorEntradaFuncaoRadial));
		
		//Using Thin-Plane RBF (2D)
		//rbfSomatorioResult += (VectorLUResultX[indiceAux] * ( (pow((fabs(ValorEntradaFuncaoRadial)),2)) * (log10(fabs(ValorEntradaFuncaoRadial))) ));
		
		//Using Biharmonic
		//rbfSomatorioResult += (VectorLUResultX[indiceAux] * ValorEntradaFuncaoRadial);
		
		// Using Triharmonic
		//rbfSomatorioResult += (VectorLUResultX[indiceAux] * pow(ValorEntradaFuncaoRadial,3));
		
		indiceAux++;
		NodoAux = NodoAux->next;
	}
	rbfSomatorioResult += ((VectorLUResultX[p0])+(VectorLUResultX[p1]*x)+(VectorLUResultX[p2]*y)+(VectorLUResultX[p3]*z));
	return rbfSomatorioResult;
}

float InterpolationRBFY(float x, float y, float z)
{
	float rbfSomatorioResult = 0.0;
	int indiceAux = 1, p0 = NumColunaLinha-3, p1 = NumColunaLinha-2, p2 = NumColunaLinha-1, p3 = NumColunaLinha;
	float ValorEntradaFuncaoRadial;
	
	VECTORARRAY *NodoAux;

	NodoAux = headV->next;
	while (NodoAux != zedV)
	{
		ValorEntradaFuncaoRadial = sqrt( (pow((x - NodoAux->vBegin.x),2)) + (pow((y - NodoAux->vBegin.y),2)) + (pow((z - NodoAux->vBegin.z),2)) );
		
		//Using Multiquadratic
		//rbfSomatorioResult += (VectorLUResultY[indiceAux] * (pow((pow(ValorEntradaFuncaoRadial,2)+pow(0.5,2)),0.5)));
		
		//Using Gaussian RBF Function
		//rbfSomatorioResult += ( VectorLUResultY[indiceAux] * ( exp( -( pow(ValorEntradaFuncaoRadial,2) ) * pow(2.0,2) ) ) );
		rbfSomatorioResult +=  VectorLUResultY[indiceAux] *  exp( - (4.0 * (pow(ValorEntradaFuncaoRadial,2 )) ) );
		
		//Using Thin-Plane RBF (3D)
		//rbfSomatorioResult += (VectorLUResultY[indiceAux] * (pow(ValorEntradaFuncaoRadial, 3)));
		
		//Using Raio only (3D)
		//rbfSomatorioResult += (VectorLUResultY[indiceAux] * (ValorEntradaFuncaoRadial));
		
		//Using Thin-Plane RBF (2D)
		//rbfSomatorioResult += (VectorLUResultY[indiceAux] * ( (pow((fabs(ValorEntradaFuncaoRadial)),2)) * (log10(fabs(ValorEntradaFuncaoRadial))) ));
		
		//Using Biharmonic
		//rbfSomatorioResult += (VectorLUResultY[indiceAux] * ValorEntradaFuncaoRadial);

		// Using Triharmonic
		//rbfSomatorioResult += (VectorLUResultY[indiceAux] * pow(ValorEntradaFuncaoRadial,3));
		
		indiceAux++;
		NodoAux = NodoAux->next;
	}
	rbfSomatorioResult += ((VectorLUResultY[p0])+(VectorLUResultY[p1]*x)+(VectorLUResultY[p2]*y)+(VectorLUResultY[p3]*z));
	return rbfSomatorioResult;
}

float InterpolationRBFZ(float x, float y, float z)
{
	float rbfSomatorioResult = 0.0;
	int indiceAux = 1, p0 = NumColunaLinha-3, p1 = NumColunaLinha-2, p2 = NumColunaLinha-1, p3 = NumColunaLinha;
	float ValorEntradaFuncaoRadial;
	
	VECTORARRAY *NodoAux;

	NodoAux = headV->next;
	while (NodoAux != zedV)
	{
		ValorEntradaFuncaoRadial = sqrt( (pow((x - NodoAux->vBegin.x),2)) + (pow((y - NodoAux->vBegin.y),2)) + (pow((z - NodoAux->vBegin.z),2)) );
		
		//Using Multiquadratic
		//rbfSomatorioResult += ( VectorLUResultZ[indiceAux] * ( pow( ( pow(ValorEntradaFuncaoRadial,2)+pow(0.5,2)),0.5)) );
		
		//Using Gaussian RBF Function
		//rbfSomatorioResult += ( VectorLUResultZ[indiceAux] * ( exp( -( pow(ValorEntradaFuncaoRadial,2) ) * pow(2.0,2) ) ) );
		//rbfSomatorioResult += ( VectorLUResultZ[indiceAux] * ( exp( - ( pow(ValorEntradaFuncaoRadial,2) ) * pow(2.0,2) ) ) );
		rbfSomatorioResult +=  VectorLUResultZ[indiceAux] *  exp( - (4.0 * (pow(ValorEntradaFuncaoRadial,2 )) ) );
		
		
		//Using Thin-Plane RBF (3D)
		//rbfSomatorioResult += (VectorLUResultZ[indiceAux] * (pow(ValorEntradaFuncaoRadial, 3)));
		
		//Using Raio only (3D)
		//rbfSomatorioResult += (VectorLUResultZ[indiceAux] * (ValorEntradaFuncaoRadial));
		
		//Using Thin-Plane RBF (2D)
		//rbfSomatorioResult += (VectorLUResultZ[indiceAux] * ( (pow((fabs(ValorEntradaFuncaoRadial)),2)) * (log10(fabs(ValorEntradaFuncaoRadial))) ));
		
		//Using Biharmonic
		//rbfSomatorioResult += (VectorLUResultZ[indiceAux] * ValorEntradaFuncaoRadial);
		
		// Using Triharmonic
		//rbfSomatorioResult += (VectorLUResultZ[indiceAux] * pow(ValorEntradaFuncaoRadial,3));

		indiceAux++;
		NodoAux = NodoAux->next;
	}
	
	rbfSomatorioResult += ((VectorLUResultZ[p0])+(VectorLUResultZ[p1]*x)+(VectorLUResultZ[p2]*y)+(VectorLUResultZ[p3]*z));
	return rbfSomatorioResult;
}

void CertificaAreaCelulasWithAngle(void)
{
	//printf("Função: CertificaAreaCelulasWithAngle \n");
	int Lenght = 2;
	
	int IndexFace;
	
	Point2D vectorRBFOn2D, faceReference2D;
	Point3D VectorReferenceTMP;
	
	double CoordX, CoordY;
	
	double AngleRBF;
	
	/*
	 Point2D center2D;
	 FILE *arq, *arq2;
	 */
	
	for(IndexFace=0; IndexFace < NumberFaces; IndexFace++)
	{
		faces[IndexFace].EndOfVector3D.x = InterpolationRBFX(faces[IndexFace].center3D.x, faces[IndexFace].center3D.y, faces[IndexFace].center3D.z);
		faces[IndexFace].EndOfVector3D.y = InterpolationRBFY(faces[IndexFace].center3D.x, faces[IndexFace].center3D.y, faces[IndexFace].center3D.z);
		faces[IndexFace].EndOfVector3D.z = InterpolationRBFZ(faces[IndexFace].center3D.x, faces[IndexFace].center3D.y, faces[IndexFace].center3D.z);
		
		/* To make the reference vector be the same as "eixo" X on 2d*/
		VectorReferenceTMP.x = faces[IndexFace].EixoReferencia.x + faces[IndexFace].center3D.x;
		VectorReferenceTMP.y = faces[IndexFace].EixoReferencia.y + faces[IndexFace].center3D.y;
		VectorReferenceTMP.z = faces[IndexFace].EixoReferencia.z + faces[IndexFace].center3D.z;
		
		/*Map from 3D to 2D*/
		mapFaceTo2D(IndexFace, VectorReferenceTMP.x, VectorReferenceTMP.y, VectorReferenceTMP.z, &faceReference2D);
		mapFaceTo2D(IndexFace, faces[IndexFace].EndOfVector3D.x, faces[IndexFace].EndOfVector3D.y, faces[IndexFace].EndOfVector3D.z, &vectorRBFOn2D);
		
		AngleRBF = ((180/Pi)*(acos(((vectorRBFOn2D.x*faceReference2D.x)+(vectorRBFOn2D.y*faceReference2D.y))
								   /((sqrt((pow(vectorRBFOn2D.x,2))+(pow(vectorRBFOn2D.y,2))))*(sqrt((pow(faceReference2D.x,2))+(pow(faceReference2D.y,2))))))));
		
		if (vectorRBFOn2D.y < 0) //Teste para saber se o vetor esta no terceiro ou no quarto quadrante, para tanto o (2?Y - 1?Y) deve ser negativo.
		{AngleRBF = (360.0 - AngleRBF);}
		
		CoordX = ( (cos( RadToDeg * AngleRBF) ) * Lenght);
		CoordY = ( (sin( RadToDeg * AngleRBF) ) * Lenght);
		
		map2DToFace(IndexFace, CoordX, CoordY, &(faces[IndexFace].EndOfVector3D));
		
		ProjetaVetorFace(IndexFace, faces[IndexFace].EndOfVector3D.x, faces[IndexFace].EndOfVector3D.y, faces[IndexFace].EndOfVector3D.z, &(faces[IndexFace].EndOfVector3D));
	}
	/*
	 //Desenha as faces como no onca, o primeiro em 3D e o segundo ja mapeado.
	 IndexFace = faceIndex;
	 
	 VectorReferenceTMP.x = faces[IndexFace].EixoReferencia.x + faces[IndexFace].center3D.x;
	 VectorReferenceTMP.y = faces[IndexFace].EixoReferencia.y + faces[IndexFace].center3D.y;
	 VectorReferenceTMP.z = faces[IndexFace].EixoReferencia.z + faces[IndexFace].center3D.z;
	 
	 mapFaceTo2D(IndexFace, faces[IndexFace].center3D.x, faces[IndexFace].center3D.y, faces[IndexFace].center3D.z, &center2D);
	 mapFaceTo2D(IndexFace, VectorReferenceTMP.x, VectorReferenceTMP.y, VectorReferenceTMP.z, &faceReference2D);
	 mapFaceTo2D(IndexFace, faces[IndexFace].EndOfVector3D.x, faces[IndexFace].EndOfVector3D.y, faces[IndexFace].EndOfVector3D.z, &vectorRBFOn2D);
	 
	 arq = fopen("/home/pgs/mello/Desktop/3dV.dat","w");
	 arq2 = fopen("/home/pgs/mello/Desktop/2dV.dat","w");
	 
	 fprintf(arq, "%f %f %f\n", VectorReferenceTMP.x,VectorReferenceTMP.y, VectorReferenceTMP.z);
	 fprintf(arq, "%f %f %f\n", faces[IndexFace].center3D.x,faces[IndexFace].center3D.y, faces[IndexFace].center3D.z);
	 fprintf(arq, "%f %f %f\n", faces[IndexFace].EndOfVector3D.x, faces[IndexFace].EndOfVector3D.y, faces[IndexFace].EndOfVector3D.z);
	 
	 fprintf(arq2, "%f %f 0.0\n", vectorRBFOn2D.x, vectorRBFOn2D.y);
	 fprintf(arq2, "%f %f 0.0\n", center2D.x, center2D.y);
	 fprintf(arq2, "%f %f 0.0\n", faceReference2D.x, faceReference2D.y);
	 
	 fclose(arq);
	 fclose(arq2);
	 
	 arq = fopen("/home/pgs/mello/Desktop/3d.dat","w");
	 arq2 = fopen("/home/pgs/mello/Desktop/2d.dat","w");
	 
	 mapFaceTo2D(IndexFace, vert[faces[IndexFace].v[0]].pos.x, vert[faces[IndexFace].v[0]].pos.y, vert[faces[IndexFace].v[0]].pos.z, &vectorRBFOn2D);
	 fprintf(arq, "%f %f %f\n", vert[faces[IndexFace].v[0]].pos.x, vert[faces[IndexFace].v[0]].pos.y, vert[faces[IndexFace].v[0]].pos.z);
	 fprintf(arq2, "%f %f 0.0\n", vectorRBFOn2D.x, vectorRBFOn2D.y);
	 
	 mapFaceTo2D(IndexFace, vert[faces[IndexFace].v[1]].pos.x, vert[faces[IndexFace].v[1]].pos.y, vert[faces[IndexFace].v[1]].pos.z, &vectorRBFOn2D);
	 fprintf(arq, "%f %f %f\n", vert[faces[IndexFace].v[1]].pos.x, vert[faces[IndexFace].v[1]].pos.y, vert[faces[IndexFace].v[1]].pos.z);
	 fprintf(arq2, "%f %f 0.0\n", vectorRBFOn2D.x, vectorRBFOn2D.y);
	 
	 mapFaceTo2D(IndexFace, vert[faces[IndexFace].v[2]].pos.x, vert[faces[IndexFace].v[2]].pos.y, vert[faces[IndexFace].v[2]].pos.z, &vectorRBFOn2D);
	 fprintf(arq, "%f %f %f\n", vert[faces[IndexFace].v[2]].pos.x, vert[faces[IndexFace].v[2]].pos.y, vert[faces[IndexFace].v[2]].pos.z);
	 fprintf(arq2, "%f %f 0.0\n", vectorRBFOn2D.x, vectorRBFOn2D.y);
	 
	 fclose(arq);
	 fclose(arq2);
	 */
}

/*
 Calcula a direcao do vetor na face, e coloca no mesmo plano.
 */
/*Modified by Fabiane Queiroz at 31/01/2010*/
void CertificaAreaCelulas(void)
{
	int IndexFace;
	
	//Point2D q;
	Point3D p, q;
	
	for(IndexFace=0; IndexFace < NumberFaces; IndexFace++)
	{
		faces[IndexFace].EndOfVector3D.x = InterpolationRBFX(faces[IndexFace].center3D.x, faces[IndexFace].center3D.y, faces[IndexFace].center3D.z);
		faces[IndexFace].EndOfVector3D.y = InterpolationRBFY(faces[IndexFace].center3D.x, faces[IndexFace].center3D.y, faces[IndexFace].center3D.z);
		faces[IndexFace].EndOfVector3D.z = InterpolationRBFZ(faces[IndexFace].center3D.x, faces[IndexFace].center3D.y, faces[IndexFace].center3D.z);
		
		ProjetaVetorFace(IndexFace, faces[IndexFace].EndOfVector3D.x, faces[IndexFace].EndOfVector3D.y, faces[IndexFace].EndOfVector3D.z, &(faces	[IndexFace].EndOfVector3D));
		
		vectors[IndexFace].vEnd.x = faces[IndexFace].EndOfVector3D.x;
		vectors[IndexFace].vEnd.y = faces[IndexFace].EndOfVector3D.y;
		vectors[IndexFace].vEnd.z = faces[IndexFace].EndOfVector3D.z;
		
		vectors[IndexFace].vBegin = faces[IndexFace].center3D;
		vectors[IndexFace].faceIndex = IndexFace;
		//printf( "Face: %d %f %f %f\n", IndexFace, faces[IndexFace].EndOfVector3D.x, faces[IndexFace].EndOfVector3D.y, faces[IndexFace].EndOfVector3D.z);
		
		/*add by Fabiane Queiroz*/
		/*vectors[IndexFace].vBegin.x = faces[IndexFace].center3D.x;
		vectors[IndexFace].vBegin.y = faces[IndexFace].center3D.y;
		vectors[IndexFace].vBegin.z = faces[IndexFace].center3D.z;
		
		vectors[IndexFace].vEnd.x = faces[IndexFace].EndOfVector3D.x;
		vectors[IndexFace].vEnd.y = faces[IndexFace].EndOfVector3D.y;
		vectors[IndexFace].vEnd.z = faces[IndexFace].EndOfVector3D.z;
		
		vectors[IndexFace].faceIndex = IndexFace;*/
		
		/*calculate the vectors field 2D projection */
		/*Add by Fabiane Queiroz*/
		
		/* get vector's current position in the 2D space */
		p.x = faces[IndexFace].center3D.x;
		p.y = faces[IndexFace].center3D.y;
		p.z = faces[IndexFace].center3D.z;
		
		q.x = faces[IndexFace].EndOfVector3D.x;
		q.y = faces[IndexFace].EndOfVector3D.y;
		q.z = faces[IndexFace].EndOfVector3D.z;
		
		
		mapOntoPolySpace(IndexFace, p.x, p.y, p.z, &(faces[IndexFace].centerProjecVector));
		mapOntoPolySpace(IndexFace, q.x, q.y, q.z, &(faces[IndexFace].endOfProjectVector));
		
		 //printf("AAAAAAAA: %f %f e %f %f\n ", faces[IndexFace].centerProjecVector.x , faces[IndexFace].endOfProjectVector.x,  faces[IndexFace].centerProjecVector.y , faces[IndexFace].endOfProjectVector.y);
		
	}
	
	/*Add by Fabiane Queiroz at 22/09/2010*/
	
	createVerticesVectors();
	for(IndexFace=0; IndexFace < NumberFaces; IndexFace++)
	{
		ProjetaVerticeVetorFace(IndexFace, 0, vert[faces[IndexFace].v[0]].EndOfVector3D.x ,  vert[faces[IndexFace].v[0]].EndOfVector3D.y, vert[faces[IndexFace].v[0]].EndOfVector3D.z, &(faces[IndexFace].projectedVerticeVector1));
		ProjetaVerticeVetorFace(IndexFace, 1, vert[faces[IndexFace].v[1]].EndOfVector3D.x ,  vert[faces[IndexFace].v[1]].EndOfVector3D.y, vert[faces[IndexFace].v[1]].EndOfVector3D.z, &(faces[IndexFace].projectedVerticeVector2));
		ProjetaVerticeVetorFace(IndexFace, 2, vert[faces[IndexFace].v[2]].EndOfVector3D.x ,  vert[faces[IndexFace].v[2]].EndOfVector3D.y, vert[faces[IndexFace].v[2]].EndOfVector3D.z, &(faces[IndexFace].projectedVerticeVector3));
	}
}

/*created by Fabiane Queiroz*/

void createVerticesVectors(void)
{
	int indexVertice, indexFace;
	byte facesNumber;
	Point3D vect;
	Point3D sum;
	sum.x = 0; sum.y = 0; sum.z = 0;
	Point3D p, q;
	
	for(indexVertice = 0; indexVertice < NumberVertices; indexVertice ++)
	{
		facesNumber = vert[indexVertice].nNeighFaces;
		
		for(indexFace = 0; indexFace < facesNumber; indexFace ++)
		{
			
			vect.x = faces[vert[indexVertice].neighFaces[indexFace]].EndOfVector3D.x - faces[vert[indexVertice].neighFaces[indexFace]].center3D.x;
			vect.y = faces[vert[indexVertice].neighFaces[indexFace]].EndOfVector3D.y - faces[vert[indexVertice].neighFaces[indexFace]].center3D.y;
			vect.z = faces[vert[indexVertice].neighFaces[indexFace]].EndOfVector3D.z - faces[vert[indexVertice].neighFaces[indexFace]].center3D.z;
			
			sum.x =  vect.x + sum.x;
			sum.y =  vect.y + sum.y;
			sum.z =  vect.z + sum.z;

		}
		
		vect.x = sum.x/(facesNumber);
		vect.y = sum.y/(facesNumber);
		vect.z = sum.z/(facesNumber);
		
		vert[indexVertice].EndOfVector3D.x = vert[indexVertice].pos.x + vect.x;
		vert[indexVertice].EndOfVector3D.y = vert[indexVertice].pos.y + vect.y;
		vert[indexVertice].EndOfVector3D.z = vert[indexVertice].pos.z + vect.z;
		 
		sum.x = 0; sum.y = 0; sum.z = 0;
		
		/* get vector's current position in the 2D space */
		p.x = vert[indexVertice].pos.x;
		p.y = vert[indexVertice].pos.y;
		p.z = vert[indexVertice].pos.z;
		
		q.x = vert[indexVertice].EndOfVector3D.x;
		q.y = vert[indexVertice].EndOfVector3D.y;
		q.z = vert[indexVertice].EndOfVector3D.z;
		
		
		mapOntoPolySpace(indexVertice, p.x, p.y, p.z, &(vert[indexVertice].beginOfProjectvector));
		mapOntoPolySpace(indexVertice, q.x, q.y, q.z, &(vert[indexVertice].endOfProjectVector));
		
		//printf("OUPAAAAAAAA: %f %f e %f %f \n ", vert[indexVertice].endOfProjectVector.x , vert[indexVertice].beginOfProjectvector.x, vert[indexVertice].endOfProjectVector.y , vert[indexVertice].beginOfProjectvector.y);
		
	}
	
}



float InterpolationRBFXWithGeodesicPath(float x, float y, float z, int IndexOfFace)
{
	float rbfSomatorioResult = 0.0;
	int indiceAux = 1, p0 = NumColunaLinha-3, p1 = NumColunaLinha-2, p2 = NumColunaLinha-1, p3 = NumColunaLinha;
	float ValorEntradaFuncaoRadial = 0.0;
	
	long IndiceGeodesicPath, OffSetIndiceGeodesicPath;
	
	VECTORARRAY *NodoAux;

	NodoAux = headV->next;
	while (NodoAux != zedV)
	{
		if (NodoAux->faceIndex != IndexOfFace)
		{
			if (NodoAux->faceIndex < IndexOfFace)
			{
				IndiceGeodesicPath = ((	NodoAux->faceIndex * NumberFaces) - ((NodoAux->faceIndex * (NodoAux->faceIndex + 1))/2));
				OffSetIndiceGeodesicPath = ((IndexOfFace - 	NodoAux->faceIndex)-1);
			}
			else {
				IndiceGeodesicPath = ((IndexOfFace * NumberFaces) - ((IndexOfFace * (IndexOfFace+1))/2));
				OffSetIndiceGeodesicPath = ((NodoAux->faceIndex - IndexOfFace)-1);
			}
			ValorEntradaFuncaoRadial = sqrt( (pow((x - NodoAux->vBegin.x),2)) + (pow((y - NodoAux->vBegin.y),2)) + (pow((z - NodoAux->vBegin.z),2)) );
			ValorEntradaFuncaoRadial += ((ArrayGeodesicPath[(IndiceGeodesicPath + OffSetIndiceGeodesicPath)].Distance)*2);
		}
		else {
			ValorEntradaFuncaoRadial = 0.0;
		}
		
		//Using Multiquadratic
		//rbfSomatorioResult += (VectorLUResultX[indiceAux] * (pow((pow(ValorEntradaFuncaoRadial,2)+pow(0.5,2)),0.5)));
		
		//Using Gaussian RBF Function
		//rbfSomatorioResult += (VectorLUResultX[indiceAux] * (exp(-(pow(ValorEntradaFuncaoRadial,2))*pow(2.0,2))));
		
		//Using Thin-Plane RBF (3D)
		//rbfSomatorioResult += (VectorLUResultX[indiceAux] * (pow(ValorEntradaFuncaoRadial, 3)));
		
		//Using Raio only (3D)
		rbfSomatorioResult += (VectorLUResultX[indiceAux] * (ValorEntradaFuncaoRadial));
		
		//Using Thin-Plane RBF (2D)
		//rbfSomatorioResult += (VectorLUResultX[indiceAux] * ( (pow((fabs(ValorEntradaFuncaoRadial)),2)) * (log10(fabs(ValorEntradaFuncaoRadial))) ));
		
		indiceAux++;
		NodoAux = NodoAux->next;
	}
	rbfSomatorioResult += ((VectorLUResultX[p0])+(VectorLUResultX[p1]*x)+(VectorLUResultX[p2]*y)+(VectorLUResultX[p3]*z));
	return rbfSomatorioResult;
}

float InterpolationRBFYWithGeodesicPath(float x, float y, float z, int IndexOfFace)
{
	float rbfSomatorioResult = 0.0;
	int indiceAux = 1, p0 = NumColunaLinha-3, p1 = NumColunaLinha-2, p2 = NumColunaLinha-1, p3 = NumColunaLinha;
	float ValorEntradaFuncaoRadial;
	
	long IndiceGeodesicPath, OffSetIndiceGeodesicPath;
	
	VECTORARRAY *NodoAux;

	NodoAux = headV->next;
	while (NodoAux != zedV)
	{
		if (NodoAux->faceIndex != IndexOfFace)
		{
			if (NodoAux->faceIndex < IndexOfFace)
			{
				IndiceGeodesicPath = ((	NodoAux->faceIndex * NumberFaces) - ((NodoAux->faceIndex * (NodoAux->faceIndex + 1))/2));
				OffSetIndiceGeodesicPath = ((IndexOfFace - 	NodoAux->faceIndex)-1);
			}
			else {
				IndiceGeodesicPath = ((IndexOfFace * NumberFaces) - ((IndexOfFace * (IndexOfFace+1))/2));
				OffSetIndiceGeodesicPath = ((NodoAux->faceIndex - IndexOfFace)-1);
			}
			ValorEntradaFuncaoRadial = sqrt( (pow((x - NodoAux->vBegin.x),2)) + (pow((y - NodoAux->vBegin.y),2)) + (pow((z - NodoAux->vBegin.z),2)) );
			ValorEntradaFuncaoRadial += ((ArrayGeodesicPath[(IndiceGeodesicPath + OffSetIndiceGeodesicPath)].Distance)*2);
		}
		else {
			ValorEntradaFuncaoRadial = 0.0;
		}
		
		//Using Multiquadratic
		//rbfSomatorioResult += (VectorLUResultY[indiceAux] * (pow((pow(ValorEntradaFuncaoRadial,2)+pow(0.5,2)),0.5)));
		
		//Using Gaussian RBF Function
		//rbfSomatorioResult += (VectorLUResultY[indiceAux] * (exp(-(pow(ValorEntradaFuncaoRadial,2))*pow(2.0,2))));
		
		//Using Thin-Plane RBF (3D)
		//rbfSomatorioResult += (VectorLUResultY[indiceAux] * (pow(ValorEntradaFuncaoRadial, 3)));
		
		//Using Raio only (3D)
		rbfSomatorioResult += (VectorLUResultY[indiceAux] * (ValorEntradaFuncaoRadial));
		
		//Using Thin-Plane RBF (2D)
		//rbfSomatorioResult += (VectorLUResultY[indiceAux] * ( (pow((fabs(ValorEntradaFuncaoRadial)),2)) * (log10(fabs(ValorEntradaFuncaoRadial))) ));
		
		indiceAux++;
		NodoAux = NodoAux->next;
	}
	
	rbfSomatorioResult += ((VectorLUResultY[p0])+(VectorLUResultY[p1]*x)+(VectorLUResultY[p2]*y)+(VectorLUResultY[p3]*z));
	return rbfSomatorioResult;
}

float InterpolationRBFZWithGeodesicPath(float x, float y, float z, int IndexOfFace)
{
	float rbfSomatorioResult = 0.0;
	int indiceAux = 1, p0 = NumColunaLinha-3, p1 = NumColunaLinha-2, p2 = NumColunaLinha-1, p3 = NumColunaLinha;
	float ValorEntradaFuncaoRadial;
	
	long IndiceGeodesicPath, OffSetIndiceGeodesicPath;
	
	VECTORARRAY *NodoAux;

	NodoAux = headV->next;
	while (NodoAux != zedV)
	{
		if (NodoAux->faceIndex != IndexOfFace)
		{
			if (NodoAux->faceIndex < IndexOfFace)
			{
				IndiceGeodesicPath = ((NodoAux->faceIndex * NumberFaces) - ((NodoAux->faceIndex * (NodoAux->faceIndex + 1))/2));
				OffSetIndiceGeodesicPath = ((IndexOfFace - 	NodoAux->faceIndex)-1);
			}
			else {
				IndiceGeodesicPath = ((IndexOfFace * NumberFaces) - ((IndexOfFace * (IndexOfFace+1))/2));
				OffSetIndiceGeodesicPath = ((NodoAux->faceIndex - IndexOfFace)-1);
			}
			ValorEntradaFuncaoRadial = sqrt( (pow((x - NodoAux->vBegin.x),2)) + (pow((y - NodoAux->vBegin.y),2)) + (pow((z - NodoAux->vBegin.z),2)) );
			ValorEntradaFuncaoRadial += ((ArrayGeodesicPath[(IndiceGeodesicPath + OffSetIndiceGeodesicPath)].Distance)*2);
		}
		else {
			ValorEntradaFuncaoRadial = 0.0;
		}
		
		//Using Multiquadratic
		//rbfSomatorioResult += (VectorLUResultZ[indiceAux] * (pow((pow(ValorEntradaFuncaoRadial,2)+pow(0.5,2)),0.5)));
		
		//Using Gaussian RBF Function
		//rbfSomatorioResult += (VectorLUResultZ[indiceAux] * (exp(-(pow(ValorEntradaFuncaoRadial,2))*pow(2.0,2))));
		
		//Using Thin-Plane RBF (3D)
		//rbfSomatorioResult += (VectorLUResultZ[indiceAux] * (pow(ValorEntradaFuncaoRadial, 3)));
		
		//Using Raio only (3D)
		rbfSomatorioResult += (VectorLUResultZ[indiceAux] * (ValorEntradaFuncaoRadial));
		
		//Using Thin-Plane RBF (2D)
		//rbfSomatorioResult += (VectorLUResultZ[indiceAux] * ( (pow((fabs(ValorEntradaFuncaoRadial)),2)) * (log10(fabs(ValorEntradaFuncaoRadial))) ));
		
		indiceAux++;
		NodoAux = NodoAux->next;
	}
	rbfSomatorioResult += ((VectorLUResultZ[p0])+(VectorLUResultZ[p1]*x)+(VectorLUResultZ[p2]*y)+(VectorLUResultZ[p3]*z));
	return rbfSomatorioResult;
}

/*
 *   Calculate the Vector Field using the Lenght of Geodesic Path.
 */
void CertificaAreaCelulasWithGeodesicPath(void)
{
	//printf("Função: CertificaAreaCelulasWithGeodesicPath \n");
	int IndexFace;
	
	for(IndexFace=0; IndexFace < NumberFaces; IndexFace++)
	{
		faces[IndexFace].EndOfVector3D.x = InterpolationRBFXWithGeodesicPath(faces[IndexFace].center3D.x, faces[IndexFace].center3D.y, faces[IndexFace].center3D.z, IndexFace);
		faces[IndexFace].EndOfVector3D.y = InterpolationRBFYWithGeodesicPath(faces[IndexFace].center3D.x, faces[IndexFace].center3D.y, faces[IndexFace].center3D.z, IndexFace);
		faces[IndexFace].EndOfVector3D.z = InterpolationRBFZWithGeodesicPath(faces[IndexFace].center3D.x, faces[IndexFace].center3D.y, faces[IndexFace].center3D.z, IndexFace);
		
		ProjetaVetorFace(IndexFace, faces[IndexFace].EndOfVector3D.x, faces[IndexFace].EndOfVector3D.y, faces[IndexFace].EndOfVector3D.z, &(faces[IndexFace].EndOfVector3D));
	}
}

void CertificaAreaCelulasWithGeodesicPathWithAngle(void)
{
	//printf("Função: CertificaAreaCelulasWithGeodesicPathWhithAngle \n");
	int Lenght = 2;
	
	int IndexFace;
	
	Point2D vectorRBFOn2D, faceReference2D;
	Point3D VectorReferenceTMP;
	
	double CoordX, CoordY;
	
	double AngleRBF;
	
	/*
	 Point2D center2D;
	 FILE *arq, *arq2;
	 */
	
	for(IndexFace=0; IndexFace < NumberFaces; IndexFace++)
	{
		faces[IndexFace].EndOfVector3D.x = InterpolationRBFXWithGeodesicPath(faces[IndexFace].center3D.x, faces[IndexFace].center3D.y, faces[IndexFace].center3D.z, IndexFace);
		faces[IndexFace].EndOfVector3D.y = InterpolationRBFYWithGeodesicPath(faces[IndexFace].center3D.x, faces[IndexFace].center3D.y, faces[IndexFace].center3D.z, IndexFace);
		faces[IndexFace].EndOfVector3D.z = InterpolationRBFZWithGeodesicPath(faces[IndexFace].center3D.x, faces[IndexFace].center3D.y, faces[IndexFace].center3D.z, IndexFace);
		
		/* To make the reference vector be the same as "eixo" X on 2d*/
		VectorReferenceTMP.x = faces[IndexFace].EixoReferencia.x + faces[IndexFace].center3D.x;
		VectorReferenceTMP.y = faces[IndexFace].EixoReferencia.y + faces[IndexFace].center3D.y;
		VectorReferenceTMP.z = faces[IndexFace].EixoReferencia.z + faces[IndexFace].center3D.z;
		
		/*Map from 3D to 2D*/
		mapFaceTo2D(IndexFace, VectorReferenceTMP.x, VectorReferenceTMP.y, VectorReferenceTMP.z, &faceReference2D);
		mapFaceTo2D(IndexFace, faces[IndexFace].EndOfVector3D.x, faces[IndexFace].EndOfVector3D.y, faces[IndexFace].EndOfVector3D.z, &vectorRBFOn2D);
		
		AngleRBF = ((180/Pi)*(acos(((vectorRBFOn2D.x*faceReference2D.x)+(vectorRBFOn2D.y*faceReference2D.y))
								   /((sqrt((pow(vectorRBFOn2D.x,2))+(pow(vectorRBFOn2D.y,2))))*(sqrt((pow(faceReference2D.x,2))+(pow(faceReference2D.y,2))))))));
		
		if (vectorRBFOn2D.y < 0) //Teste para saber se o vetor esta no terceiro ou no quarto quadrante, para tanto o (2?Y - 1?Y) deve ser negativo.
		{AngleRBF = (360.0 - AngleRBF);}
		
		CoordX = ( (cos( RadToDeg * AngleRBF) ) * Lenght);
		CoordY = ( (sin( RadToDeg * AngleRBF) ) * Lenght);
		
		map2DToFace(IndexFace, CoordX, CoordY, &(faces[IndexFace].EndOfVector3D));
		
		ProjetaVetorFace(IndexFace, faces[IndexFace].EndOfVector3D.x, faces[IndexFace].EndOfVector3D.y, faces[IndexFace].EndOfVector3D.z, &(faces[IndexFace].EndOfVector3D));
	}
}


/*
 *-----------------------------------------------------------
 * Creates the list. Initialize head and
 * zed (dummy node). Head and zed are global
 * variables
 *---------------------------------------------------------------
 */
void create_global_list( void )
{
	headV = vectorAlloc();
	zedV = vectorAlloc();
	
	headV->next = zedV;
	zedV->next = (VECTORARRAY *) NULL;
}

/*
 *-----------------------------------------------------------
 * Allocate space for a new node and returns
 * a pointer to the newly allocated node.
 * If malloc cannot find space the routine
 * exits.
 *-----------------------------------------------------------
 */
VECTORARRAY *vectorAlloc(void)
{
	VECTORARRAY *n;
	
	if((n = (VECTORARRAY *) malloc( vectorSize )) == NULL)
	{
		errorMsg("Error with malloc in function vectorAlloc()!");
	}
	n->next = (VECTORARRAY *) NULL;
	return n;
}


/*
 *-----------------------------------------------------------
 *  Inserts a node in the list
 *  Receives a pointer to the node to be
 *  inserted, a pointer to the head
 *  node of the list and to the
 *  tail node
 *-----------------------------------------------------------
 */
void insert(VECTORARRAY *v, VECTORARRAY *h, VECTORARRAY *t)
{
	VECTORARRAY *s;
	
	s = h;
	while(s->next != t)
	{
		s = s->next;
	}
	s->next = v;
	v->next = t;
}

void removeVector(VECTORARRAY *v, VECTORARRAY *h)
{
	printf("Entrei em removeVector!\n");
	VECTORARRAY *s;
	
	s = h;
	while(s->next != v) s = s->next;
	
	s->next = v->next;
	free(v); v = NULL;
	
	
}


/*
 * Map from Universe co-ordinated system to 2D face co-ordinated system
 */
void mapFaceTo2D(int faceIndex, double x, double y, double z, Point2D *p)
{
	Point3D pointIn;
	
	pointIn.x = x;
	pointIn.y = y;
	pointIn.z = z;
	
	V3PreMul( &pointIn, &(faces[faceIndex].MatrixMapTo2D));
	
	p->x = pointIn.x;
	p->y = pointIn.y;
}

/*
 * Map from 2D co-ordinated system to universe face co-ordinated system
 */
void map2DToFace(int faceIndex, double x, double y, Point3D *p)
{
	Point3D q;
	
	q.x = x;
	q.y = y;
	q.z = 0.0;
	
	V3PreMul( &q, &(faces[faceIndex].MatrixMapTo3D));
	
	p->x = q.x;
	p->y = q.y;
	p->z = q.z;
}

/* Modified by Fabiane Queiroz at 24/03/2010 */

void storePointVector( int x, int y )
{
    int faceIndex;
	
    polygonPicking = TRUE;
    checkHit(x,y);
    polygonPicking = FALSE;
	
    capturePoints( x, y, &coordsEnd[0]);
    faceIndex = PointPicking;
	
    coordsBegin[0] = faces[faceIndex].center3D.x;
    coordsBegin[1] = faces[faceIndex].center3D.y;
    coordsBegin[2] = faces[faceIndex].center3D.z;
	
    if( PointPicking == -1 )
    {
	    printString( 0, "Oooops! => Try another init point on the polygonal meshes!");
    }
	
    createVector( coordsBegin, coordsEnd, faceIndex );
	
    
    
    
    /* reestart variables */
    PointPicking = -1;
    createVect = FALSE;
    showAxes = TRUE; //Why??? TODO by Bins
}

void SetaTipoRBF(void)
{
	
	switch (WithInterpolationType) {
		case 0: {
			printf("%d -> 3D with straight path. ", WithInterpolationType);
			MakeLUDecomposition3D(1);
			CertificaAreaCelulas();
		} break;
		case 1: {
			printf("%d -> 3D with straight path using angle. ", WithInterpolationType);
			MakeLUDecomposition3D(1);
			CertificaAreaCelulasWithAngle();
		} break;
		case 2: {
			printf("%d -> 3D with geodesic path. ", WithInterpolationType);
			MakeLUDecomposition3D(0);
			CertificaAreaCelulasWithGeodesicPath();
		} break;
		case 3: {
			printf("%d -> 3D with geodesic path usign angle. ", WithInterpolationType);
			MakeLUDecomposition3D(0);
			CertificaAreaCelulasWithGeodesicPathWithAngle();
		} break;
	}
}

void createVector( float coordsBegin[], float coordsEnd[], int faceIndex )
{
	VECTORARRAY *v;
	
	v = vectorAlloc();
	
	fprintf(stderr, "\tVector with index = %d\n", faceIndex );
	
	v->vBegin.x = coordsBegin[0];
	v->vBegin.y = coordsBegin[1];
	v->vBegin.z = coordsBegin[2];
	fprintf(stderr, "\tVector begin at:  %f  %f  %f \n", v->vBegin.x, v->vBegin.y, v->vBegin.z );
	
	v->vEnd.x = coordsEnd[0];
	v->vEnd.y = coordsEnd[1];
	v->vEnd.z = coordsEnd[2];
	fprintf(stderr, "\tVector ends at:  %f  %f  %f\n\n", v->vEnd.x, v->vEnd.y, v->vEnd.z );
	
	v->faceIndex = faceIndex;
	
	insert( v, headV, zedV );
	
	compVectorBarycentricCoord(v);
	
	nVectors++;
	
	SetaTipoRBF();
}

void removeVect (int faceIndex){
	VECTORARRAY *v;
	//v = NodoAux;
	
	printf("Entrei em removeVect!!\n");
	v = headV->next;
	while (v != zedV)
	{
		
		if(v->faceIndex == faceIndex)
		{
			removeVector(v, headV);
		}
		v = v->next;
	}
	
	CertificaAreaCelulas();
	printf("Saí des removeVect!!\n");
}


void FoundError(void)
{
	int IndexFace;
	double ErroMax = 0, ErroMin = 999999999, ErroDiference;
	int Histograma[99], PositionHistograma, Contador;
	double Fator, Inicio, Fim;
	
	FILE *vectorFile, *vectorFileCor;
	
	for(IndexFace=0; IndexFace < NumberFaces; IndexFace++)
	{
		faces[IndexFace].Erro = sqrt( (pow((faces[IndexFace].EndOfVector1.x - faces[IndexFace].EndOfVector0.x),2)) +
									 (pow((faces[IndexFace].EndOfVector1.y - faces[IndexFace].EndOfVector0.y),2)) +
									 (pow((faces[IndexFace].EndOfVector1.z - faces[IndexFace].EndOfVector0.z),2)) );
		
		if (faces[IndexFace].Erro > ErroMax)
		{
			ErroMax = faces[IndexFace].Erro;
		}
		
		if (faces[IndexFace].Erro < ErroMin)
		{
			ErroMin = faces[IndexFace].Erro;
		}
	}
	
	ErroDiference = (ErroMax - ErroMin);
	
	/* Preapare the Error to setr the colors */
	for(IndexFace=0; IndexFace < NumberFaces; IndexFace++)
	{
		if (faces[IndexFace].Erro != 0)
		{
			faces[IndexFace].Erro = ((faces[IndexFace].Erro - ErroMin) / ErroDiference);
		}
	}
	
	vectorFile = fopen(vectorcoordFileName, "w");
	
	for(IndexFace=0; IndexFace < NumberFaces; IndexFace++)
	{
		fprintf( vectorFile, "%1.9f\n", faces[IndexFace].Erro);
	}
	
	fprintf( vectorFile, "#%1.9f %1.9f - %d\n", ErroMin, ErroMax, Contador);
	
	fclose(vectorFile);	
}



/*
 *-----------------------------------------------------------
 *  Prints the list. Receives a pointer to the
 *  head and tail nodes of the list
 *-----------------------------------------------------------
 */
void printListOfVectors( void )
{
	VECTORARRAY *v;
	int i = 1;
	
	if( nVectors == 0)
	{
		printString( 0, "\tThere is no vectors orientation to print!");
	}
	else
	{
		fprintf( stderr, "\nPrinting List of %d Vectors Orientation:\n\n", nVectors );
		v = headV->next;
		while (v != zedV)
		{		
			fprintf( stderr, "  V%d (Face = %d) = (%3.4f  %3.4f  %3.4f) -> ", i, v->faceIndex, v->vBegin.x, v->vBegin.y, v->vBegin.z );
			fprintf( stderr, "(%3.4f  %3.4f  %3.4f)\n", v->vEnd.x, v->vEnd.y, v->vEnd.z );
			v = v->next;
			i++;
		}
	}
}


/*
 *----------------------------------------------------------
 * Read Vectors File
 *----------------------------------------------------------
 */
void readVectorFile( char *inpFileName )
{
	printf("readvectorfield\n");
	char line[255], input[255];
	int vecID, faceIndex;
	
	FILE *fp;
	
	fprintf( stderr, "\n=========================================================\n");
	fprintf( stderr, "Loading Field Vector Structure from file %s \n", inpFileName);
	fprintf( stderr, "=========================================================\n");
	
	if( (fp = fopen(inpFileName, "r")) == NULL )
		errorMsg("Could not open vector file to read!");
	
	while (fscanf(fp, "%s", input) != EOF)
	{
		if (!strcmp(input, COMMENT))
		{
			Getline(line, 256, fp);
			//fprintf( stderr, "%s", line );  // Commented by Thompson Lied - 23/10/2003
		}
		else if( !strcmp(input, VECTORID) )
		{
			fscanf(fp,"%d %d", &vecID, &faceIndex );
			fprintf( stderr, "\t-> vecID = %d feceIndex = %d\n", vecID, faceIndex );
			
			fscanf( fp, "%f  %f  %f", &coordsBegin[0], &coordsBegin[1], &coordsBegin[2] );
			//fprintf( stderr, "\t> %f  %f  %f\n", coordsBegin[0], coordsBegin[1], coordsBegin[2] );
			fscanf( fp, "%f  %f  %f", &coordsEnd[0], &coordsEnd[1], &coordsEnd[2] );
			//fprintf( stderr, "\t> %f  %f  %f \n", coordsEnd[0], coordsEnd[1], coordsEnd[2] );
			
			createVector( coordsBegin, coordsEnd, faceIndex );
			
			/*calculate the vectors field 2D projection */
			/*Add by Fabiane Queiroz*/
			
			/* get vecto's current position in the 2D space */
			//p.x = coordsBegin[0];
			//p.y = coordsBegin[1];
			//p.z = coordsBegin[2];
			
			//q.x = coordsEnd[0];
			//q.y = coordsEnd[1];
			//q.z = coordsEnd[2];
			
			
			//mapOntoPolySpace(faceIndex, p.x, p.y, p.z, &(faces[faceIndex].centerProjecVector));
			//mapOntoPolySpace(faceIndex, q.x, q.y, q.z, &(faces[faceIndex].endOfProjectVector));
			
		}
		else
		{
			fprintf( stderr, "Obj file name = %s\n", input );
			errorMsg("Got token not readable in input vector file...");
		}
	}
	fprintf( stderr, "\t nVectors = %d\n", nVectors);
}

/*
 *----------------------------------------------------------
 * Save Vectors File
 *----------------------------------------------------------
 */
void saveVectors( char *sessionFileName )
{
	int length, i;
	char vectorsFile[128];
	
	VECTORARRAY *v;
	
	FILE *fpVect;
	
	strcpy(vectorsFile, sessionFileName);
	
	length = strlen( vectorsFile );
	
	strncpy( vectorsFile, sessionFileName, length-8 );
	vectorsFile[length-8]='\0';
	
	strcat( vectorsFile, ".cvf");
	
	if(( fpVect = fopen( vectorsFile, "w+" )) == NULL )
	{
		printf( "Could not open Control Vectors File ( %s ) to write! \n", vectorsFile );
		exit(1);
	}
	
	fprintf( stderr, "Saving the vectors orientation into file %s... ", vectorsFile );
	
	fprintf( fpVect, "# \n" );
	fprintf( fpVect, "# Onca Viewer Control Vectors Field\n" );
	fprintf( fpVect, "# \n" );
	
	v = headV->next;
	i = 0;
	while (v != zedV)
	{	
		fprintf( fpVect, "# vector %d \n", i );		
		fprintf( fpVect, "VECTORID %d %d\n", i, v->faceIndex );
		fprintf( fpVect, "%f  %f  %f \n", v->vBegin.x, v->vBegin.y, v->vBegin.z );
		fprintf( fpVect, "%f  %f  %f \n", v->vEnd.x, v->vEnd.y, v->vEnd.z );
		
		v = v->next;
		i++;
	}
	
	fclose( fpVect );
	
	fprintf( stderr, "...done.\n" );
}

/* Save the entire Vector Field (one vector per face)*/
void saveVectorField( char *FileName )
{
	//printf("Função: saveVectorField \n");
	int IndexFace, Lenght;
	char FileNameAux[128];
	FILE *FileVectorField;
	
	strcpy(FileNameAux, FileName);
	
	Lenght = strlen( FileNameAux );
	FileNameAux[Lenght-8]='\0';
	strcat( FileNameAux, ".vf");
	
	if (( FileVectorField = fopen( FileNameAux, "w+" )) == NULL )
	{
		printf( "Could not open Vectors Field File ( %s ) to write! \n", FileNameAux );
		exit(1);
	}
	
	fprintf( stderr, "Saving Vector Field into file %s... ", FileNameAux );
	
	fprintf( FileVectorField, "# \n" );
	fprintf( FileVectorField, "# Onca Viewer Vectors Field\n" );
	fprintf( FileVectorField, "# \n" );
	
	for(IndexFace = 0; IndexFace < NumberFaces; IndexFace++)
	{
		fprintf( FileVectorField, "%f %f %f\n", faces[IndexFace].EndOfVector3D.x, faces[IndexFace].EndOfVector3D.y, faces[IndexFace].EndOfVector3D.z);
	}
	
	fclose( FileVectorField );
	
	fprintf( stderr, "...done.\n" );
}


/*
 * Moved from main.c by Bins
 *
 *---------------------------------------------------------
 *	Load the entire Vector Field from the file (one vector per face)
 *---------------------------------------------------------
 */

void loadVectorField( char *VFfileName )
{
	char line[255], input[255];
	int IndexFaceVF;

	float X_, Y_, Z_;

	FILE *FileVectorField;

	if (( FileVectorField = fopen( VFfileName, "r" )) == NULL )
	{
		printf( "Could not open Vectors Field File ( %s ) to read! \n", VFfileName );
		exit(1);
	}

	fprintf( stderr, "Loading Vector Field from file %s ... ", VFfileName );

	Getline(line, 256, FileVectorField);
	Getline(line, 256, FileVectorField);
	Getline(line, 256, FileVectorField);

	for (IndexFaceVF = 0; IndexFaceVF < NumberFaces; IndexFaceVF++)
	{
		fscanf( FileVectorField, "%f %f %f", &X_, &Y_, &Z_);
		faces[IndexFaceVF].EndOfVector3D.x = X_;
		faces[IndexFaceVF].EndOfVector3D.y = Y_;
		faces[IndexFaceVF].EndOfVector3D.z = Z_;
	}

	fclose( FileVectorField );

	fprintf( stderr, "... Done.\n" );
}
