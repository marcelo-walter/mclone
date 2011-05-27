/*
 *  intersect.c
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/7/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */

/*
 *-----------------------------------------------------
 *	intersect.c
 *	Routines for computing intersection between
 *	lines
 *-----------------------------------------------------
 */

#include "../common.h"
#include "intersect.h"
extern void mapOntoPolySpace(int whichFace, double x, double y, double z, Point2D *p);

extern int  eccwForFace( int whichEdge, int whichFace );
/*
 *------------------------------------------------------------------
 * Adapted from Graphics Gems III - p. 199
 * This routine computes the intersection between 2 pair of
 * lines defined by:
 * line 1 from p1 to p2
 * line 2 from p3 to p4
 * The intersection point (if any) is returned through 'i'
 * Return codes:
 *
 * DONT_INTERSECT
 * DO_INTERSECT
 * COLLINEAR
 *------------------------------------------------------------------
 */

int
linesIntersect(Point2D p1, Point2D p2, Point2D p3, Point2D p4, Point2D *i, double *t) 

{
	double Ax, Bx, Cx, Ay, By, Cy;
	double d, e, denom;
	double x1lo, x1hi, y1lo, y1hi;
	double alpha;
	
	Ax = p2.x-p1.x;
	Bx = p3.x-p4.x;
	
	/* X bound box test */
	if(Ax<0){			       
		x1lo=p2.x; x1hi=p1.x;
	}else{
		x1hi=p2.x; x1lo=p1.x;
	}
	if(Bx>0) {
		if(x1hi < p4.x || p3.x < x1lo){
#ifdef VERBOSE
			fprintf( stderr, "Failed x bounding box test in lines intersect!\n");
#endif
			return DONT_INTERSECT;
		}
	} else {
		if(x1hi < p3.x || p4.x < x1lo){
#ifdef VERBOSE
			fprintf( stderr, "Failed x bounding box test in lines intersect!\n");
#endif
			return DONT_INTERSECT;
		}
	}
	
	Ay = p2.y-p1.y;
	By = p3.y-p4.y;
	/* Y bound box test */
	if(Ay<0) {						
		y1lo=p2.y; y1hi=p1.y;
	} else {
		y1hi=p2.y; y1lo=p1.y;
	}
	if(By>0) {
		if(y1hi < p4.y || p3.y < y1lo){
#ifdef VERBOSE
			fprintf( stderr, "Failed y bounding box test in lines intersect!\n");
#endif
			return DONT_INTERSECT;
		}
	} else {
		if(y1hi < p3.y || p4.y < y1lo){
#ifdef VERBOSE
			fprintf( stderr, "Failed y bounding box test in lines intersect!\n");
#endif
			return DONT_INTERSECT;
		}
	}
	
	Cx = p1.x-p3.x;
	Cy = p1.y-p3.y;
	d = By*Cx - Bx*Cy;		/* alpha numerator */
	denom = Ay*Bx - Ax*By;	/* both denominator */
	
	if( denom > 0) {		/* alpha tests */
		if(d<0 || d>denom){
#ifdef VERBOSE
			fprintf( stderr, "Failed alpha test in lines intersect!\n");
#endif
			return DONT_INTERSECT;
		}
	} else {
		if(d>0 || d<denom){
#ifdef VERBOSE
			fprintf( stderr, "Failed alpha test in lines intersect!\n");
#endif
			return DONT_INTERSECT;
		}
	}
	
	e = Ax*Cy - Ay*Cx;		/* beta numerator */
	if(denom > 0) {		/* beta tests */
		if(e<0 || e>denom){
#ifdef VERBOSE
			fprintf( stderr, "Failed beta test in lines intersect!\n");
#endif
			return DONT_INTERSECT;
		}
	} else {
		if(e>0 || e<denom){
#ifdef VERBOSE
			fprintf( stderr, "Failed beta test in lines intersect!\n");
#endif
			return DONT_INTERSECT;
		}
	}
	
	if( denom == 0 ){
#ifdef VERBOSE
		fprintf( stderr, "Failed collinear test in lines intersect!\n");
#endif
		return COLLINEAR;
	}
	
	/* compute intersection coordinates */
	alpha = d / denom;
	
	i->x = p1.x + Ax * alpha;
	i->y = p1.y + Ay * alpha;
	
	*t = alpha;
	
	/*fprintf( stderr, "p1(%3.2f %3.2f) p2(%3.2f %3.2f)\n",
	 p1.x, p1.y, p2.x, p2.y );
	 fprintf( stderr, "p3(%3.2f %3.2f) p4(%3.2f %3.2f)\n",
	 p3.x, p3.y, p4.x, p4.y );
	 fprintf( stderr, "intersection(%3.2f %3.2f)\n", i->x, i->y);*/
	
	return DO_INTERSECT;
}
/*
 *------------------------------------------------------------------
 * Adapted from Graphics Gems III - p. 199
 * This routine computes the intersection between 2 pair of
 * lines defined by:
 * line 1 from p1 to p2
 * line 2 from p3 to p4
 * The intersection point (if any) is returned through 'i'
 * Return codes:
 *
 * DONT_INTERSECT
 * DO_INTERSECT
 * COLLINEAR
 *
 * THERE IS NO BOUNDING BOX TEST IN THIS!!
 *------------------------------------------------------------------
 */

int
specialLinesIntersect(Point2D p1, Point2D p2, Point2D p3, Point2D p4, Point2D *i, double *t) 

{
	double Ax, Bx, Cx, Ay, By, Cy;
	double d, e, denom;
	double alpha;
	
	
	Ax = p2.x-p1.x;
	Bx = p3.x-p4.x;
	
	Ay = p2.y-p1.y;
	By = p3.y-p4.y;
	
	Cx = p1.x-p3.x;
	Cy = p1.y-p3.y;
	
	d = By*Cx - Bx*Cy;		/* alpha numerator */
	denom = Ay*Bx - Ax*By;	/* both denominator */
	
	if( Positive(denom) /*denom > 0*/) {		/* alpha tests */
		if(Negative(d)/*d<0*/ || d>denom){
#ifdef VERBOSE
			fprintf( stderr, "Failed alpha test in lines intersect!\n");
			fprintf( stderr, "d = %lg denom = %lg\n", d, denom );
			fprintf( stderr, "Ax = %lg Ay = %lg Bx = %lg By = %lg Cx = %lg Cy = %lg\n",
					Ax, Ay, Bx, By, Cx, Cy );
#endif
			return DONT_INTERSECT;
		}
	} else {
		if(Positive(d) /*d>0*/ || d<denom){
#ifdef VERBOSE
			fprintf( stderr, "Failed alpha test in lines intersect!\n");
			fprintf( stderr, "d = %lg denom = %lg\n", d, denom );
			fprintf( stderr, "Ax = %lg Ay = %lg Bx = %lg By = %lg Cx = %lg Cy = %lg\n",
					Ax, Ay, Bx, By, Cx, Cy );
#endif
			return DONT_INTERSECT;
		}
	}
	
	e = Ax*Cy - Ay*Cx;		/* beta numerator */
	if(Positive(denom) /*denom > 0*/) {		/* beta tests */
		if(Negative(e) /*e<0*/ || e>denom){
#ifdef VERBOSE
			fprintf( stderr, "Failed beta test in lines intersect!\n");
			fprintf( stderr, "e = %lg denom = %lg\n", e, denom );
			fprintf( stderr, "Ax = %lg Ay = %lg Bx = %lg By = %lg Cx = %lg Cy = %lg\n",
					Ax, Ay, Bx, By, Cx, Cy );
#endif
			return DONT_INTERSECT;
		}
	} else {
		if(Positive(e) /*e>0*/ || e<denom){
#ifdef VERBOSE
			fprintf( stderr, "Failed beta test in lines intersect!\n");
			fprintf( stderr, "e = %lg denom = %lg\n", e, denom );
			fprintf( stderr, "Ax = %lg Ay = %lg Bx = %lg By = %lg Cx = %lg Cy = %lg\n",
					Ax, Ay, Bx, By, Cx, Cy );
#endif
			return DONT_INTERSECT;
		}
	}
	
	if( denom == 0 ){
#ifdef VERBOSE
		fprintf( stderr, "Failed collinear test in lines intersect!\n");
#endif
		return COLLINEAR;
	}
	
	/* compute intersection coordinates */
	alpha = d / denom;
	
	if ( alpha < EPSILON ) alpha = 0.0;
	
	i->x = p1.x + Ax * alpha;
	i->y = p1.y + Ay * alpha;
	
	*t = alpha;
	
	return DO_INTERSECT;
	
	/*fprintf( stderr, "p1(%3.2f %3.2f) p2(%3.2f %3.2f)\n",
	 p1.x, p1.y, p2.x, p2.y );
	 fprintf( stderr, "p3(%3.2f %3.2f) p4(%3.2f %3.2f)\n",
	 p3.x, p3.y, p4.x, p4.y );
	 fprintf( stderr, "intersection(%f %f)\n", i->x, i->y);*/
	
}
/*
 *------------------------------------------------------------------
 * Adapted from Graphics Gems III - p. 199
 * This routine computes the intersection between 2 pair of
 * lines defined by:
 * line 1 from p1 to p2
 * line 2 from p3 to p4
 * The intersection point (if any) is returned through 'i'
 * Return codes:
 *
 * DONT_INTERSECT
 * DO_INTERSECT
 * COLLINEAR
 *
 * THERE IS NO BOUNDING BOX TEST IN THIS!!
 *------------------------------------------------------------------
 */

int
goodLinesIntersect(Point2D p1, Point2D p2, Point2D p3, Point2D p4, Point2D *i, double *t) 

{
	double Ax, Bx, Cx, Ay, By, Cy;
	double d, e, f, denom;
	double alpha, beta;
	
	Ax = p2.x-p1.x;
	Bx = p3.x-p4.x;
	
	Ay = p2.y-p1.y;
	By = p3.y-p4.y;
	
	Cx = p1.x-p3.x;
	Cy = p1.y-p3.y;
	
	d = By*Cx - Bx*Cy;		/* alpha numerator */
	f = Ax*Cy - Ay*Cx;		/* beta numerator */
	denom = Ay*Bx - Ax*By;	/* both denominator */
	
	
	if( denom == 0 ){
#ifdef VERBOSE
		fprintf( stderr, "Failed collinear test in lines intersect!\n");
#endif
		return COLLINEAR;
	}
	
	/* compute intersection coordinates */
	alpha = d / denom;
	beta = f / denom;
	
	if ( (GreaterEqualZero(alpha)) && (alpha <= 1.0) ){
		if ( (GreaterEqualZero(beta)) && (beta <= 1.0) ){
			i->x = p1.x + Ax * alpha;
			i->y = p1.y + Ay * alpha;
			
			*t = alpha;
			return DO_INTERSECT;
		}
		else{
			return DONT_INTERSECT;
		}
	}
	else{
		return DONT_INTERSECT;
	}
	
}

/*
 *------------------------------------------------------------------
 * Adapted from Graphics Gems III - p. 199
 * This routine computes the intersection between a line and
 * an infinite edge. The main difference from the original algorithm
 * is that the 2 parametric values do not need to be inside the
 * (0,1) interval for the intersection to be valid
 * The edge is defined from p1 to p2 and the line is defined from
 * p3 to p4.
 *
 * The assumption is that the line and edge ALWAYS intersect
 *
 *------------------------------------------------------------------
 */

void
edgeIntersect(Point2D p1, Point2D p2, Point2D p3, Point2D p4, Point2D *i) 

{
	double Ax, Bx, Cx, Ay, By, Cy;
	double d, e, denom;
	double x1lo, x1hi, y1lo, y1hi;
	double alpha;
	
	Ax = p2.x-p1.x;
	Bx = p3.x-p4.x;
	
	Ay = p2.y-p1.y;
	By = p3.y-p4.y;
    
	Cx = p1.x-p3.x;
	Cy = p1.y-p3.y;
	
	d = By*Cx - Bx*Cy;		/* alpha numerator */
	denom = Ay*Bx - Ax*By;	/* both denominator */
	
	/* compute intersection coordinates */
	alpha = d / denom;
	
	i->x = p1.x + Ax * alpha;
	i->y = p1.y + Ay * alpha;
	
	/*if ( alpha < 0 ){
	 fprintf( stderr, "alpha = %3.2f p1.x = %3.2f p1.y = %3.2f\n",
	 alpha, p1.x, p1.y );
	 fprintf( stderr, "p2.x = %3.2f p2.y = %3.2f\n", p2.x, p2.y );
	 fprintf( stderr, "p3.x = %3.2f p3.y = %3.2f\n", p3.x, p3.y );
	 fprintf( stderr, "p4.x = %3.2f p4.y = %3.2f\n", p4.x, p4.y );
	 fprintf( stderr, "i.x = %3.2f i.y = %3.2f\n", i->x, i->y );
	 }*/
}

/*
 *---------------------------------------------------
 *
 *---------------------------------------------------
 */
int
intersect( int whichFace, double v0x, double v0y, double v1x, double v1y,
		  Point2D *p )
{
	int i, firstEdge, currentEdge;
	double x0, y0, z0, x1, y1, z1;
	Point2D p1, p2;
	double x, y;
	
	firstEdge = faces[whichFace].edge;
	currentEdge = firstEdge;
	
	for( i = 0; i < faces[whichFace].nPrimFaces; i++){
		/* get the two vertices that define this edge */
		x0 = vert[edges[currentEdge].vstart].pos.x;
		y0 = vert[edges[currentEdge].vstart].pos.y;
		z0 = vert[edges[currentEdge].vstart].pos.z;
		x1 = vert[edges[currentEdge].vend].pos.x;
		y1 = vert[edges[currentEdge].vend].pos.y;
		z1 = vert[edges[currentEdge].vend].pos.z;
		
		mapOntoPolySpace(whichFace, x0, y0, z0, &p1);
		mapOntoPolySpace(whichFace, x1, y1, z1, &p2);
		
		if ( oldlinesIntersect(p1.x, p1.y, p2.x, p2.y, v0x, v0y, v1x, v1y, &x, &y) == DO_INTERSECT){
			p->x = x;
			p->y = y;
			/*fprintf( stderr, "v0(%3.2f %3.2f) v1(%3.2f %3.2f)\n",
			 v0x, v0y, v1x, v1y);
			 fprintf( stderr, "p1(%3.2f %3.2f) p2(%3.2f %3.2f)\n",
			 p1.x, p1.y, p2.x, p2.y);
			 fprintf( stderr, "intersection(%3.2f %3.2f)\n",
			 x, y);*/
			return currentEdge;
		}    
		else{
			/* get next edge */
			currentEdge = eccwForFace( currentEdge, whichFace );
			if ( currentEdge == -1 )
				errorMsg( "Could not find next edge in intersect routine (voronoi.c)!");
			/* check whether we went around not finding a intersection */
			if ( currentEdge == firstEdge )
				return -1;
		}
	}
}

/*
 *------------------------------------------------------------------
 * Adpated from Graphics Gems III - p. 199
 * This routine computes the intersection between 2 pair of
 * lines defined by:
 * line 1 from (x1,y1) to (x2,y2)
 * line 2 from (x3,y3) to (x4,y4)
 * The intersection point (if any) is returned through (x, y)
 * Return codes:
 *
 * DONT_INTERSECT
 * DO_INTERSECT
 * COLLINEAR
 *------------------------------------------------------------------
 */

int
oldlinesIntersect(double x1, double y1, double x2, double y2,
				  double x3, double y3, double x4, double y4,
				  double *x, double *y) 

{
	double Ax, Bx, Cx, Ay, By, Cy;
	double d, e, denom;
	double x1lo, x1hi, y1lo, y1hi;
	double alpha;
	
	Ax = x2-x1;
	Bx = x3-x4;
	
	/* X bound box test */
	if(Ax<0){			       
		x1lo=x2; x1hi=x1;
	}else{
		x1hi=x2; x1lo=x1;
	}
	if(Bx>0) {
		if(x1hi < x4 || x3 < x1lo) return DONT_INTERSECT;
	} else {
		if(x1hi < x3 || x4 < x1lo) return DONT_INTERSECT;
	}
	
	Ay = y2-y1;
	By = y3-y4;
	/* Y bound box test */
	if(Ay<0) {						
		y1lo=y2; y1hi=y1;
	} else {
		y1hi=y2; y1lo=y1;
	}
	if(By>0) {
		if(y1hi < y4 || y3 < y1lo) return DONT_INTERSECT;
	} else {
		if(y1hi < y3 || y4 < y1lo) return DONT_INTERSECT;
	}
	
	Cx = x1-x3;
	Cy = y1-y3;
	d = By*Cx - Bx*Cy;		/* alpha numerator */
	denom = Ay*Bx - Ax*By;	/* both denominator */
	
	if( denom > 0) {		/* alpha tests */
		if(d<0 || d>denom) return DONT_INTERSECT;
	} else {
		if(d>0 || d<denom) return DONT_INTERSECT;
	}
	
	e = Ax*Cy - Ay*Cx;		/* beta numerator */
	if(denom > 0) {		/* beta tests */
		if(e<0 || e>denom) return DONT_INTERSECT;
	} else {
		if(e>0 || e<denom) return DONT_INTERSECT;
	}
	
	if( denom == 0 ) return COLLINEAR;
	
	/* compute intersection coordinates */
	alpha = d / denom;
	
	*x = x1 + Ax * alpha;
	*y = y1 + Ay * alpha;
	
	return DO_INTERSECT;
}


