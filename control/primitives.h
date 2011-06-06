/*
 *  primitives.h
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/8/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */

/*
 *--------------------------------------------------
 *	primitives.h
 *--------------------------------------------------
 */

#ifndef _PRIMITIVES_H_
#define _PRIMITIVES_H_

#include "../data/Types.h"

/*
 *---------------------------------------------------------
 *	Local Variables
 *---------------------------------------------------------
 */
extern char primitivesFileName[128]; //From main.h

/* which primitive is currently active */
extern int activePrim;

/* parametrization was computed or not */
extern flag parametOKFlag;


/*
 *--------------------------------------------------
 *	Local Prototypes
 *--------------------------------------------------
 */
void createLandMark( void );
void createPrimitive(int type);
void duplicatePrimitive( void );
void parametrize( void );
void assignPrim2Cell( CELL *c );
void assignPrim2AllCells(Array* listFaces );
void assignPrim2Faces( void );
void assignPrim2Vert( void );
void checkLandMarks( void );
void testLandMarks( void );
void compLandMarks( void );
void checkParametrization(void);
void checkCellsParametrization( void );
int  belongPrimitive( int whichPrim, Point3D vertToBeTransformed, Point3D *vReturn );
void compCylinderCoord( int whichPrim, Point3D vertToBeTransformed, Point3D *vReturn );
void compParamCoordAllVert( void );
void compParamCoord3DPoint( int vertID );
void compParamCoordAllCells(Array* listFaces);
void compParamCoordOneCell( CELL *c );
void compUV( Point3D p, double *u, double *v );
//From genericUtil.c
void savePrimitivesFile( char *outFile, int nPrim );
int loadPrimitivesFile( char *inFile );
int checkPrimitivesFile(void);

#endif //_PRIMITIVES_H_
