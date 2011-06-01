/*
 *------------------------------------------------------------
 *	vectorField.h
 *	Thompson Peter Lied
 *
 *	This module contains routines to create the orientation 
 *	vectors and the vector field
 *------------------------------------------------------------
 */

#ifndef _VECTOR_FIELD_H_
#define _VECTOR_FIELD_H_

#include "../common.h"

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
extern int vectorSize;

float coordsBegin[3];
float coordsEnd[3];

extern int nVectors;
extern int PointPicking;

VECTORARRAY *NodoAux;
const float Pi = 3.14159265358979323846264338328;
const double RadToDeg = (3.14159265358979323846264338328/180);

int WithInterpolationType;

#define	VECTORID "VECTORID"

/*
 *---------------------------------------------------------------
 *
 * EXTERNAL GLOBAL VARIABLES
 *
 *---------------------------------------------------------------
 */
flag createVect;
flag removevect;

flag showAxes;

flag polygonPicking;
flag vectorPresent;
char vectorFileName[128];

float **MatrixLUX, **MatrixLUY, **MatrixLUZ, *VectorLUResultX, *VectorLUResultY, *VectorLUResultZ, LUAuxX, LUAuxY, LUAuxZ;
int NumColunaLinha, *VectorIndexX, *VectorIndexY, *VectorIndexZ;

char vectorcoordFileName[128]; /*Save the vector final coordenates in a file to calculate de erro later*/



/*
 * LOCAL PROTOTYPES
 */

void 	morphVectorField( void );
void createVerticesVectors(void);
void ProjetaVetorFace(int IndexFace, double xFound, double yFound, double zFound, Point3D *p);
void ProjetaVerticeVetorFace(int IndexFace, int IndexVertice, double xFound, double yFound, double zFound, Point3D *p);
void MakeLUDecomposition3D(int Type);
float InterpolationRBFX(float x, float y, float z);
float InterpolationRBFY(float x, float y, float z);
float InterpolationRBFZ(float x, float y, float z);
void CertificaAreaCelulasWithAngle(void);
void CertificaAreaCelulas(void);
void createVerticesVectors(void);
float InterpolationRBFXWithGeodesicPath(float x, float y, float z, int IndexOfFace);
float InterpolationRBFYWithGeodesicPath(float x, float y, float z, int IndexOfFace);
float InterpolationRBFZWithGeodesicPath(float x, float y, float z, int IndexOfFace);
void CertificaAreaCelulasWithGeodesicPath(void);
void CertificaAreaCelulasWithGeodesicPathWithAngle(void);
void create_global_list( void );
VECTORARRAY *vectorAlloc(void);
void insert(VECTORARRAY *v, VECTORARRAY *h, VECTORARRAY *t);
void removeVector(VECTORARRAY *v, VECTORARRAY *h);
void mapFaceTo2D(int faceIndex, double x, double y, double z, Point2D *p);
void map2DToFace(int faceIndex, double x, double y, Point3D *p);
void storePointVector( int x, int y );
void SetaTipoRBF(void);
void createVector( float coordsBegin[], float coordsEnd[], int faceIndex );
void removeVect (int faceIndex);
void FoundError(void);
void printListOfVectors( void );
void readVectorFile( char *inpFileName );
void saveVectors( char *sessionFileName );
void saveVectorField( char *FileName );
// From main.c by Bins
void loadVectorField( char *VFfileName );

#endif //_VECTOR_FIELD_H_
