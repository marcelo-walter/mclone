/*
 *  intersect.h
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/7/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */

/*
 *-----------------------------------------------------
 *	intersect.h
 *-----------------------------------------------------
 */

#ifndef _INTERSECT_H_
#define _INTERSECT_H_


int  intersect( int whichFace, double v0x, double v0y,
			   double v1x, double v1y, Point2D *p );
int  oldlinesIntersect( double x1, double y1, double x2, double y2,
					   double x3, double y3, double x4, double y4,
					   double *x, double *y );
int  linesIntersect(Point2D p1, Point2D p2, Point2D p3, Point2D p4,
					Point2D *i, double *t );
int  specialLinesIntersect(Point2D p1, Point2D p2, Point2D p3, Point2D p4,
						   Point2D *i, double *t );
int  goodLinesIntersect(Point2D p1, Point2D p2, Point2D p3, Point2D p4,
						Point2D *i, double *t );
void edgeIntersect(Point2D p1, Point2D p2, Point2D p3, Point2D p4, Point2D *i);

int  intersect( int whichFace, double v0x, double v0y, double v1x, double v1y,
			   Point2D *p );
int  oldlinesIntersect(double x1, double y1, double x2, double y2,
					   double x3, double y3, double x4, double y4,
					   double *x, double *y);


#endif //_INTERSECT_H_
