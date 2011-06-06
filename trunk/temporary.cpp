/*
 * temporary.cpp
 *
 *  Created on: 02/06/2011
 *      Author: rlbinsfeld
 */

#include "temporary.h"

#include <stdlib.h>

#include "data/Types.h"

#include "data/Matrix4.h"
#include "data/Object.h"
#include "simulation/relax.h"
#include "simulation/planar.h"

/*
 *--------------------------------------------------
 *	Local Defined Global variables
 *--------------------------------------------------
 */
char sessionFileName[128]; //From main.h

/* name of the output CM file */
char outputCMfileName[128]; //From main.h

char expFileName[128]; //From main.h

/* Flags if there is or not a primitive file specified */
flag primFilePresent = FALSE; //From main.cpp

int *nnoprimVertex = NULL; // line added by Cadu in 31/10 //From main.cpp

/* From main.cpp
 *-----------------------------------------------------------------
 * Compute 2 vectors, v1 and v2 which define, together with the
 * normal vector a local orientation system for this polygon
 *-----------------------------------------------------------------
 */
void compMappingMatrices( int whichFace )
{
	Point3D normal, v1, v2;
	Matrix4 tmp;

	/* compute plane coefficients for each face */
	PlaneEquation( whichFace, &(faces[whichFace].planeCoef) );

	faces[whichFace].v1.x = vert[faces[whichFace].v[1]].pos.x -
	vert[faces[whichFace].v[0]].pos.x;
	faces[whichFace].v1.y = vert[faces[whichFace].v[1]].pos.y -
	vert[faces[whichFace].v[0]].pos.y;
	faces[whichFace].v1.z = vert[faces[whichFace].v[1]].pos.z -
	vert[faces[whichFace].v[0]].pos.z;

	V3Normalize(&(faces[whichFace].v1));

	normal.x = faces[whichFace].planeCoef.a;
	normal.y = faces[whichFace].planeCoef.b;
	normal.z = faces[whichFace].planeCoef.c;
	V3Cross( &normal, &(faces[whichFace].v1), &(faces[whichFace].v2) );

	V3Normalize(&(faces[whichFace].v2));

	/* now that we have the 3 vectors, we can store them as
	 * the matrix transformation p2o (polygon space to original
	 * space) and o2p (original space to polygon space).
	 * o2p is obtained by inverting p2o. We need also a translation
	 * to the first point and that's why the last column
	 * is equal to the first point which defines the origin
	 * of the polygon's coordinate system
	 */
	faces[whichFace].p2o.element[0][0] = faces[whichFace].v1.x;
	faces[whichFace].p2o.element[0][1] = faces[whichFace].v2.x;
	faces[whichFace].p2o.element[0][2] = normal.x;
	faces[whichFace].p2o.element[0][3] = vert[faces[whichFace].v[0]].pos.x;

	faces[whichFace].p2o.element[1][0] = faces[whichFace].v1.y;
	faces[whichFace].p2o.element[1][1] = faces[whichFace].v2.y;
	faces[whichFace].p2o.element[1][2] = normal.y;
	faces[whichFace].p2o.element[1][3] = vert[faces[whichFace].v[0]].pos.y;

	faces[whichFace].p2o.element[2][0] = faces[whichFace].v1.z;
	faces[whichFace].p2o.element[2][1] = faces[whichFace].v2.z;
	faces[whichFace].p2o.element[2][2] = normal.z;
	faces[whichFace].p2o.element[2][3] = vert[faces[whichFace].v[0]].pos.z;

	faces[whichFace].p2o.element[3][0] = 0.0;
	faces[whichFace].p2o.element[3][1] = 0.0;
	faces[whichFace].p2o.element[3][2] = 0.0;
	faces[whichFace].p2o.element[3][3] = 1.0;

	/* get the invert matrix */
	//double det;
	MxInvert( &(faces[whichFace].p2o), &(faces[whichFace].o2p));
	// printf("det: %f", det);
	/* I want to make numbers in the matrix which are smaller than
     zero are mapped to zero */
	/*zeroEntriesonMatrix();*/

	/* we want to make sure that the inversion
     routine is working. The 'tmp' matrix
     has to be == Identity matrix, otherwise
     the two matrices are not the inverse of
     each other
     */
	MatMul(  &(faces[whichFace].p2o), &(faces[whichFace].o2p), &tmp );
	checkIdentity( &tmp );
}


/* From main.cpp
 *------------------------------------------------
 *------------------------------------------------
 */
void computeGeometricCenter( int whichFace, int howManyVert )
{
	int j;
	double x, y, z;

	x = y = z = 0.0;
	for(j = 0; j < howManyVert; j++){
		x += vert[faces[whichFace].v[j]].pos.x;
		y += vert[faces[whichFace].v[j]].pos.y;
		z += vert[faces[whichFace].v[j]].pos.z;
	}
	faces[whichFace].center3D.x = x / howManyVert;
	faces[whichFace].center3D.y = y / howManyVert;
	faces[whichFace].center3D.z = z / howManyVert;

	/* compute the face center in the 2D space of the polygon */
	mapOntoPolySpace( whichFace,
					 faces[whichFace].center3D.x,
					 faces[whichFace].center3D.y,
					 faces[whichFace].center3D.z,
					 &(faces[whichFace].center2D) );
}
