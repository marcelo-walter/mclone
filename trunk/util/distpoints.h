/*
 *  distpoints.c
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/8/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */


/*
 *---------------------------------------------------------
 *
 *	distpoints.c
 *	Marcelo Walter
 *	Routines to create a number of random points on
 *  	the surface of the object
 *---------------------------------------------------------
 */

#ifndef _DISTPOINTS_H_
#define _DISTPOINTS_H_


#include "../common.h"


void partialSumArea( double totalArea );
double triangleAreas( void );
Point3D *square2polygon(int whichFace, float s, float t, Point3D *p );
void fromBarycentricCoord( CELL *c );
void fromVectorBarycentricCoord( VECTORARRAY *v );
void compBarycentricCoord( CELL *c );
void compVectorBarycentricCoord( VECTORARRAY *v );
double areaTriangle( Point3D p1, Point3D p2, Point3D p3 );
int binarySearch(float value);


#endif //_DISTPOINTS_H_
