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

#include "../data/Types.h"

/*
 *---------------------------------------------------------------
 *
 * GLOBAL VARIABLES
 *
 *---------------------------------------------------------------
 */
/* head and tail nodes of the global linked list */
extern VECTORARRAY *headV;
extern VECTORARRAY *zedV;
extern int vectorSize;

extern float coordsBegin[3];
extern float coordsEnd[3];

extern int nVectors;
extern int PointPicking;

const float Pi = 3.14159265358979323846264338328;
const double RadToDeg = (3.14159265358979323846264338328/180);

extern int WithInterpolationType;

#define	VECTORID "VECTORID"

/*
 *---------------------------------------------------------------
 *
 * GLOBAL VARIABLES
 *
 *---------------------------------------------------------------
 */
extern flag createVect;
extern flag removevect;

extern flag polygonPicking;
extern flag vectorPresent;
extern char vectorFileName[128];

extern float **MatrixLUX, **MatrixLUY, **MatrixLUZ, *VectorLUResultX, *VectorLUResultY, *VectorLUResultZ, LUAuxX, LUAuxY, LUAuxZ;
extern int NumColunaLinha, *VectorIndexX, *VectorIndexY, *VectorIndexZ;

extern char vectorcoordFileName[128]; /*Save the vector final coordenates in a file to calculate de erro later*/



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
