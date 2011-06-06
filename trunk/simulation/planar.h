/*
 *  planar.h
 *  Mclone3D
 *
 *  Separated from forces.h by Ricardo Binsfeld on 30/05/11.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */

/*
 *-------------------------------------------------
 *	planar.h
 *-------------------------------------------------
 */

#ifndef _PLANAR_H_
#define _PLANAR_H_

#include "../common.h"

/* This functions are used in both files (planar.c and forces.c), but are declared in forces.c
enum{CW, CCW};
void 	rotatePointAroundEdge( Point3D *r, int whichEdge, int whichFace );
void 	findByteAndBit( int faceIndex, int *byte, int *bit );
SCELL 	*makeCopyCellSimpler( double px, double py, CELL *c, int process );
void 	formHeapNeighFaces( int whichFace );
*/

void 	planarizeFace( int whichFace );
void 	writeTmpListCells( SCELL *h, SCELL *t );
void 	planarizePrimFaces( int whichFace, SCELL *h, SCELL *t );
void 	planarizeOneFace( int whichFace, SCELL *h, SCELL *t );
void 	planarizeSecFaces( int whichFace, SCELL *h, SCELL *t );
void 	planarizeFacesOnHeap( int whichFace , SCELL *h, SCELL *t );
void 	planarizeSecToTmpList( int whichFace, int sourceFace, Matrix4 m, SCELL *h, SCELL *t );

//From genericUtil.c
void 	PlaneEquation(int faceIndex, Point4D *plane);

#endif //_PLANAR_H_

