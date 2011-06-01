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

#include "../common.h"

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
 * Grow or not the model. The idea is that sometimes
 * I want to only animate the model (such as the horse
 * walking) without actually growing it at the same
 * time
 */
extern flag growthFlag;
extern flag keepAspectFlag;

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

#endif //_MORPH_H_
