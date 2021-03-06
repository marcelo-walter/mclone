/*
 *  morph.h
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/8/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */

/*
 *------------------------------------------------------------
 *	morph.h
 *	Marcelo Walter
 *
 *	This module contains the morphing part of
 *	the shape transformation
 *------------------------------------------------------------
 */

#ifndef _MORPH_H_
#define _MORPH_H_

#include "data/Types.h"

/*
 *------------------------------------------------------------
 *		Local definitions
 *------------------------------------------------------------
 */

/* used with the weigthing scheme. How much off the
 cylinder I am considering for weigthing purposes */
#define	HEIGHT_INCR_FACTOR	0.2
#define MASTER_PRIM		1


/*
 *----------------------------------------------------------
 * PROTOTYPES
 *----------------------------------------------------------
 */
void morph( int t );
void morphVertices( int t );
double weightFunction(double y, double h);
double applyGrowthInformation( int primIndex, Point3D vertexToBeTransformed, Point3D *vReturn, int t );
void fromMaster2World( Point3D *vReturn );
void followMasterPrimitive( int primIndex, Matrix4 *m, int tim, Point3D *vReturn);
void morphPrimitives( int t );
void growRadiusHeight( int whichPrim, float *h, float *r, int t);
void morphOneStep( int direction );
int getGrowthData( int day );

void computeGeometricCenter( int whichFace, int howManyVert ); //from main.h
void compMappingMatrices( int whichFace );//from main.h

#endif //_MORPH_H_
