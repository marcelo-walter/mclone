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

#include "distPoints.h"

#include <string.h>
#include <math.h>

#include "genericUtil.h"
#include "../data/Point3D.h"
#include "../data/Object.h"

/*
 *---------------------------------------------------------
 *	partialSumArea
 *---------------------------------------------------------
 */
void partialSumArea( double totalArea )
{
	int i;
	double sum = 0.0;
	
	/* 
	 * make each individual polygon area relative to the
	 * total area
	 */
	for(i=0; i < NumberFaces; i++){
		faces[i].relArea = faces[i].absArea / totalArea;
	}
	/*
	 * sort the polygons according to their relative area
	 * This actually changes the ordering of the faces array
	 */
	/*qsort( (void *) (faces),  numberFaces, sizeof( Face ), 
	 (int (*)(const void *,  const void *))faceSortCmp);*/
	
	/* compute the partial sum area in the ordered array */
	for( i = 0; i < NumberFaces; i++ ){
		sum += faces[i].relArea;
		faces[i].partArea = sum;
		/*fprintf(stderr,"Polygon %d sum area %lg\n",i,faces[i].partArea); */
	}
}

/*
 *--------------------------------------------------------------------------
 *	triangleAreas
 *
 *	Compute absolut and relative areas of each face and the sub-triangles
 *	that form each polygon (assumed convex).
 *	There are number of vertices minus 2 sub-triangles, each defined by
 *	the first point in the polygon and two other adjacent points
 *
 *	Returns the total area of the object
 *
 *	Adapted from Graphics Gems:
 *	/imager/local/generic/src/gemsI/Gems/TriPoints.c
 *-------------------------------------------------------------------------
 */
double triangleAreas( void )
{
	int i, j;
	float areaSum;
	Point3D v1, v2, v3;
	int vcount;
	double totalArea = 0.0;
	
	for( i = 0; i < NumberFaces; i++ ){
		vcount = faces[i].nverts;
		areaSum = 0.0;
		
		for( j = 0; j < vcount-2; j++ ){
			V3Sub(&(vert[faces[i].v[j+1]].pos), &(vert[faces[i].v[0]].pos), &v1);
			V3Sub(&(vert[faces[i].v[j+2]].pos), &(vert[faces[i].v[0]].pos), &v2);
			V3Cross( &v1,  &v2,  &v3);
			
			/* Warning: The cross product computes the area multiplied
			 * by 2. To get the actual area we have to divide the
			 * result of V3Length by 2
			 */
			faces[i].areas[j] = V3Length( &v3 ) / 2.0;
			areaSum += faces[i].areas[j];
		}
		/*
		 * Store the previous area as the current area, since we
		 * are computing a new area
		 */
		faces[i].prevAbsArea = faces[i].absArea;
		/* store the new area of this face */
		faces[i].absArea = areaSum;
		/* make the sub-areas relative (0,1) */
		for(j=0; j < vcount-2; j++)
			faces[i].areas[j] /= areaSum;
		
		/* keep trackof the total area of object */
		//printf("Area: %f ", totalArea);
		totalArea += faces[i].absArea;
	}
	//printf("\nArea: %f", totalArea);
	return totalArea;
}

/*
 *------------------------------------------------------------------
 *	square2polygon
 *
 *	This code comes from Graphics Gems I
 *
 *	Map a point from the square [0,1] x [0,1] into a convex polygon.
 *  	Uniform random points in the square will generate uniform random 
 *	points in the polygon
 *
 *	The procedure triangleAreas() must be called once to compute 
 *	'areas', and then this procedure can be called repeatedly.
 *
 *	Entry:
 *		whichFace - which face receives the point
 *		s,t	  - position in the square [0,1] x [0,1]
 *	Return:
 *		p - position in polygon
 *
 *----------------------------------------------------------------
 */
Point3D *square2polygon(int whichFace, float s, float t, Point3D *p )
{ 
	int i;
	float areaSum = 0.0;
	float a,b,c;
	int vcount;
	
	/* number of vertices in the polygon */
	vcount = faces[whichFace].nverts;
	
	/*
	 * Use 's' to pick one sub-triangle, weighted by relative
	 * area of triangles
	 */
	for(i = 0; i < vcount - 2; i++){
		areaSum += faces[whichFace].areas[i];
		if (areaSum >= s) break;
	}
	
	/* map 's' into the interval [0,1] */
	s = (s - areaSum + faces[whichFace].areas[i]) / faces[whichFace].areas[i];
	
	/* map (s,t) to a point in that sub-triangle */
	t = sqrt(t);
	
	a = 1 - t;
	b = (1 - s) * t;
	c = s * t;
	
	p->x = a * vert[faces[whichFace].v[0]].pos.x +
    b * vert[faces[whichFace].v[i+1]].pos.x +
    c * vert[faces[whichFace].v[i+2]].pos.x;
	
	p->y = a * vert[faces[whichFace].v[0]].pos.y +
    b * vert[faces[whichFace].v[i+1]].pos.y +
    c * vert[faces[whichFace].v[i+2]].pos.y;
	
	p->z = a * vert[faces[whichFace].v[0]].pos.z +
    b * vert[faces[whichFace].v[i+1]].pos.z +
    c * vert[faces[whichFace].v[i+2]].pos.z;
    
	return(p);
}

/*
 *----------------------------------------------------------
 * Given cell C and its barycentric coordinates
 * compute the 3D position of this cell
 *----------------------------------------------------------
 */
void fromBarycentricCoord( CELL *c )
{
	
	Point3D p1, p2, p3;
	
	V3Assign( &p1, vert[faces[c->whichFace].v[0]].pos );
	V3Assign( &p2, vert[faces[c->whichFace].v[1]].pos );
	V3Assign( &p3, vert[faces[c->whichFace].v[2]].pos );
	
	c->x = c->bary.u * p1.x + c->bary.v * p2.x + c->bary.w * p3.x;
	c->y = c->bary.u * p1.y + c->bary.v * p2.y + c->bary.w * p3.y;
	c->z = c->bary.u * p1.z + c->bary.v * p2.z + c->bary.w * p3.z;
	
}

/*Created by Fabiane Queiroz at 08/2010*/

/*
 *----------------------------------------------------------
 * Given vector v and its begin and end barycentric coordinates
 * compute the 3D position of this vector
 *----------------------------------------------------------
 */

void fromVectorBarycentricCoord( VECTORARRAY *v )
{
	
	Point3D p1, p2, p3;
	
	V3Assign( &p1, vert[faces[v->faceIndex].v[0]].pos );
	V3Assign( &p2, vert[faces[v->faceIndex].v[1]].pos );
	V3Assign( &p3, vert[faces[v->faceIndex].v[2]].pos );
	
	v->vBegin.x = v->baryBegin.u * p1.x + v->baryBegin.v * p2.x + v->baryBegin.w * p3.x;
	v->vBegin.y = v->baryBegin.u * p1.y + v->baryBegin.v * p2.y + v->baryBegin.w * p3.y;
	v->vBegin.z = v->baryBegin.u * p1.z + v->baryBegin.v * p2.z + v->baryBegin.w * p3.z;
	
	v->vEnd.x = v->baryEnd.u * p1.x + v->baryEnd.v * p2.x + v->baryEnd.w * p3.x;
	v->vEnd.y = v->baryEnd.u * p1.y + v->baryEnd.v * p2.y + v->baryEnd.w * p3.y;
	v->vEnd.z = v->baryEnd.u * p1.z + v->baryEnd.v * p2.z + v->baryEnd.w * p3.z;
	
}

/*
 *----------------------------------------------------------
 * Given cell C, computes the barycentric coordinates of
 * the cell relative to the face
 * CAVEAT: only works for triangular faces
 *----------------------------------------------------------
 */
void compBarycentricCoord( CELL *c )
{
	
	double totalArea;
	Point3D p, p1, p2, p3, t;
	Param3D b;
	
	p.x = c->x;
	p.y = c->y;
	p.z = c->z;
	
	V3Assign( &p1, vert[faces[c->whichFace].v[0]].pos );
	V3Assign( &p2, vert[faces[c->whichFace].v[1]].pos );
	V3Assign( &p3, vert[faces[c->whichFace].v[2]].pos );
	
	totalArea = areaTriangle( p1, p2, p3 );
	b.u = areaTriangle( p, p2, p3 ) / totalArea;
	b.v = areaTriangle( p1, p, p3 ) / totalArea;
	/*b.w = areaTriangle( p1, p2, p ) / totalArea;*/
	b.w = 1.0 - (b.u + b.v);
	
	if (  b.u < 0.0 ) b.u = 0.0;
	else if ( b.u > 1.0 ) b.u = 1.0;
	if (  b.v < 0.0 ) b.v = 0.0;
	else if ( b.v > 1.0 ) b.v = 1.0;
	if (  b.w < 0.0 ) b.w = 0.0;
	else if ( b.w > 1.0 ) b.w = 1.0;
	/*
	 * I want to check that every computed barycentric coordinates
	 * make sense
	 */
	t.x = b.u * p1.x + b.v * p2.x + b.w * p3.x;
	t.y = b.u * p1.y + b.v * p2.y + b.w * p3.y;
	t.z = b.u * p1.z + b.v * p2.z + b.w * p3.z;
	
	/* What is a reasonable difference here? */
	/*comented by Fabiane Queiroz*/
	/*if ( fabs(c->x - t.x) > 1e-3  ||
		fabs(c->y - t.y) > 1e-3 ||
		fabs(c->z - t.z) > 1e-3  ){
		
		fprintf( stderr,"p1(%lg %lg %lg)\n",
				p1.x, p1.y, p1.z );
		fprintf( stderr,"p2(%lg %lg %lg)\n",
				p2.x, p2.y, p2.z );
		fprintf( stderr,"p3(%lg %lg %lg)\n",
				p3.x, p3.y, p3.z );
		fprintf( stderr, "u = %lg v = %lg w = %lg u+v+w = %lg\n",
				b.u, b.v, b.w, b.u+b.v+b.w );
		fprintf( stderr, "cell (%lg %lg %lg)\n",
				c->x, c->y, c->z );
		fprintf( stderr, "barycentric (%lg %lg %lg)\n",
				t.x, t.y, t.z );
		fprintf( stderr, "difference (%lg %lg %lg)\n",
				c->x - t.x, c->y - t.y, c->z - t.z );
		
		errorMsg("Barycentric coordinates wrong! (distpoints.c)");
	}*/
	
	c->x = t.x; c->y = t.y; c->z = t.z;
	
	c->bary.u = b.u;
	c->bary.v = b.v;
	c->bary.w = b.w;
	
}

/*Add by Fabiane Queiroz at 08/2010*/
/*
 *----------------------------------------------------------
 * Given vector V, computes the barycentric coordinates of
 * the vector begin and vector end relative to the face
 * CAVEAT: only works for triangular faces
 *----------------------------------------------------------
 */
void compVectorBarycentricCoord( VECTORARRAY *v )
{
	
	double totalArea;
	Point3D pBegin,pEnd, p1, p2, p3, t;
	Param3D b1,b2;
	
	pBegin.x = v->vBegin.x;
	pBegin.y = v->vBegin.y;
	pBegin.z = v->vBegin.z;
	
	pEnd.x = v->vEnd.x;
	pEnd.y = v->vEnd.y;
	pEnd.z = v->vEnd.z;
	
	V3Assign( &p1, vert[faces[v->faceIndex].v[0]].pos );
	V3Assign( &p2, vert[faces[v->faceIndex].v[1]].pos );
	V3Assign( &p3, vert[faces[v->faceIndex].v[2]].pos );
	
	totalArea = areaTriangle( p1, p2, p3 );
	b1.u = areaTriangle( pBegin, p2, p3 ) / totalArea;
	b1.v = areaTriangle( p1, pBegin, p3 ) / totalArea;
	/*b.w = areaTriangle( p1, p2, p ) / totalArea;*/
	b1.w = 1.0 - (b1.u + b1.v);
	
	b2.u = areaTriangle( pEnd, p2, p3 ) / totalArea;
	b2.v = areaTriangle( p1, pEnd, p3 ) / totalArea;
	/*b.w = areaTriangle( p1, p2, p ) / totalArea;*/
	b2.w = 1.0 - (b2.u + b2.v);
	
	if (  b1.u < 0.0 ) b1.u = 0.0;
	else if ( b1.u > 1.0 ) b1.u = 1.0;
	if (  b1.v < 0.0 ) b1.v = 0.0;
	else if ( b1.v > 1.0 ) b1.v = 1.0;
	if (  b1.w < 0.0 ) b1.w = 0.0;
	else if ( b1.w > 1.0 ) b1.w = 1.0;
	
	if (  b2.u < 0.0 ) b2.u = 0.0;
	else if ( b2.u > 1.0 ) b2.u = 1.0;
	if (  b2.v < 0.0 ) b2.v = 0.0;
	else if ( b2.v > 1.0 ) b2.v = 1.0;
	if (  b2.w < 0.0 ) b2.w = 0.0;
	else if ( b2.w > 1.0 ) b2.w = 1.0;
	/*
	 * I want to check that every computed barycentric coordinates
	 * make sense
	 */
	t.x = b1.u * p1.x + b1.v * p2.x + b1.w * p3.x;
	t.y = b1.u * p1.y + b1.v * p2.y + b1.w * p3.y;
	t.z = b1.u * p1.z + b1.v * p2.z + b1.w * p3.z;
	
	/* What is a reasonable difference here? */
	/*if ( fabs(v->vBegin.x - t.x) > 1e-3  ||
		fabs(v->vBegin.y - t.y) > 1e-3 ||
		fabs(v->vBegin.z - t.z) > 1e-3  ){
		fprintf( stderr,"p1(%lg %lg %lg)\n",
				p1.x, p1.y, p1.z );
		fprintf( stderr,"p2(%lg %lg %lg)\n",
				p2.x, p2.y, p2.z );
		fprintf( stderr,"p3(%lg %lg %lg)\n",
				p3.x, p3.y, p3.z );
		fprintf( stderr, "u = %lg v = %lg w = %lg u+v+w = %lg\n",
				b1.u, b1.v, b1.w, b1.u+b1.v+b1.w );
		fprintf( stderr, "cell (%lg %lg %lg)\n",
				v->vBegin.x, v->vBegin.y, v->vBegin.z );
		fprintf( stderr, "barycentric (%lg %lg %lg)\n",
				t.x, t.y, t.z );
		fprintf( stderr, "difference (%lg %lg %lg)\n",
				v->vBegin.x - t.x, v->vBegin.y - t.y, v->vBegin.z - t.z );
		printf("aqui?????????????????\n");
		errorMsg("Barycentric coordinates wrong! (distpoints.c)");
	}*/
	
	v->vBegin.x = t.x; v->vBegin.y = t.y; v->vBegin.z = t.z;
	
	v->baryBegin.u = b1.u;
	v->baryBegin.v = b1.v;
	v->baryBegin.w = b1.w;
	
	/*
	 * I want to check that every computed barycentric coordinates
	 * make sense
	 */
	t.x = b2.u * p1.x + b2.v * p2.x + b2.w * p3.x;
	t.y = b2.u * p1.y + b2.v * p2.y + b2.w * p3.y;
	t.z = b2.u * p1.z + b2.v * p2.z + b2.w * p3.z;
	
	/* What is a reasonable difference here? */
	/*if ( fabs(v->vEnd.x - t.x) > 1e-3  ||
		fabs(v->vEnd.y - t.y) > 1e-3 ||
		fabs(v->vEnd.z - t.z) > 1e-3  ){
		fprintf( stderr,"p1(%lg %lg %lg)\n",
				p1.x, p1.y, p1.z );
		fprintf( stderr,"p2(%lg %lg %lg)\n",
				p2.x, p2.y, p2.z );
		fprintf( stderr,"p3(%lg %lg %lg)\n",
				p3.x, p3.y, p3.z );
		fprintf( stderr, "u = %lg v = %lg w = %lg u+v+w = %lg\n",
				b2.u, b1.v, b2.w, b2.u+b2.v+b2.w );
		fprintf( stderr, "cell (%lg %lg %lg)\n",
				v->vEnd.x, v->vEnd.y, v->vEnd.z );
		fprintf( stderr, "barycentric (%lg %lg %lg)\n",
				t.x, t.y, t.z );
		fprintf( stderr, "difference (%lg %lg %lg)\n",
				v->vEnd.x - t.x, v->vEnd.y - t.y, v->vEnd.z - t.z );
		printf("aqui?????????????????1111\n");
		errorMsg("Barycentric coordinates wrong! (distpoints.c)");
	}*/
	
	v->vEnd.x = t.x; v->vEnd.y = t.y; v->vEnd.z = t.z;
	
	v->baryEnd.u = b2.u;
	v->baryEnd.v = b2.v;
	v->baryEnd.w = b2.w;
}

/*
 *----------------------------------------------------------
 * Computes the area of a 3D triangle
 *----------------------------------------------------------
 */
double areaTriangle( Point3D p1, Point3D p2, Point3D p3 )
{
	
	Point3D v1, v2, v3;
	
	V3Sub( &p2, &p1, &v1 );
	V3Sub( &p3, &p1, &v2 );
	V3Cross( &v1, &v2, &v3 );
	return( V3Length ( &v3 ) / 2.0 );
}
/*
 *----------------------------------------------------------
 *	Returns the entry in the faces array for the polygon
 *	which has the partial summed area less than or equal
 *	to "value" received as input parameter
 *
 *	The faces array is sorted by the partial summed
 *	areas and therefore a binary search can be made
 *	The routine returns the index for the face which
 *	matches the given value (ie, less than or equal)
 *----------------------------------------------------------
 */
int binarySearch(float value)
{
	int key, r, v, u;
	
	key = -1;
	r = 0;
	v = NumberFaces - 1;
    
	while( r <= v && key == -1){
		u = ( r + v ) / 2;
		if ( u == 0 && value <= faces[u].partArea ) return(u);
		if ( (value <= faces[u].partArea) && (value > faces[u-1].partArea) )
			key = u;
		else if ( value > faces[u].partArea ) r = u + 1;
		else v = u - 1;
	}
	if (key == -1){
		fprintf(stderr,"value = %f faces[%d].partArea= %f v = %d r = %d\n",
				value, u, faces[u].partArea, v, r);
		errorMsg("Problem in binary search!");
		return(0);
	}
	else return(key);
}
/*
 *---------------------------------------------------------
 *	auxiliary routine for the qsort procedure
 *---------------------------------------------------------
 */
/*int faceSortCmp( const Face *a,  const Face *b )
 {
 int rc=0;
 if (a->area < b->area) rc=-1;
 else if (a->area > b->area) rc=1;
 return rc;
 }*/
