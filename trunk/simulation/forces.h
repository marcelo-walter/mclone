/*
 *  forces.h
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/7/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */

/*
 *-------------------------------------------------
 *	forces.h
 *-------------------------------------------------
 */

#ifndef _FORCES_H_
#define _FORCES_H_

#include "../data/Types.h"

enum{CW, CCW};

/*
 *--------------------------------------------------
 *	Locally Defined Global variables
 *--------------------------------------------------
 */
extern int totalNumOfForcesComputation, nOfNeighBelowIdeal;

/*
 * The initing flag flags to the relaxation
 * routine that we DO NOT want the adhesion
 * between cells to play any role at this
 * stage. It is only used in this context.
 */
extern flag 	initingFlag;

//For using Vector Fiels defined in vertices
//OBS: Vector Field is defined in Faces by Default
extern flag verticesVectorFieldFlag;


/*
 *--------------------------------------------------
 *	PROTOTYPES
 *--------------------------------------------------
 */
double 	linearForceFunction( double distance );
void 	force( Point2D p, Point2D q,
			  CELL_TYPE ptype, CELL_TYPE qtype,
			  double *dix, double *diy );
void 	mapToPlane( Point3D *r, int currentSecFace, int *listOfSecFaces,
				   int *listOfSecEdges, CELL *n );
void 	rotateCellAroundEdge( Point3D *r, int whichEdge,
							 int whichFace, CELL *c );
void 	rotatePointAroundEdge( Point3D *r, int whichEdge, int whichFace );
void 	formListOfSecFaces( int whichVertex, int whichFace, int nSecFaces,
						   int *list, int type, int parity );
void 	formListOfSecEdges( int *listOfSecFaces, int nSecFaces , int *list );
void 	insertSecAdjFacesInHeap( int whichFace );
void 	findByteAndBit( int faceIndex, int *byte, int *bit );
void 	insertFaceInHeap(int faceIndex, int edgeIndex,
						 int vertIndex, double dist, Matrix4 t );
SCELL 	*makeCopyCellSimpler( double px, double py, CELL *c, int process );
int	includeCellsOnSameFace( int whichFace, SCELL *h, SCELL *t );
int	includeCellsFromPrimFaces( int whichFace, SCELL *h, SCELL *t );
int 	includeCellsFromOtherFaces( int whichFace, SCELL *h, SCELL *t );
int	formTmpListOfCells( int whichFace, SCELL *h, SCELL *t );
void 	compForcesForFace( int whichFace );
void 	processCellsInTmpList( SCELL *h, SCELL *t );
void 	processOneCell( SCELL *s, SCELL *h, SCELL *t, double *dx, double *dy );
void 	formHeapNeighFaces( int whichFace );
int	processHeapNeighFaces( int whichFace, SCELL *h, SCELL *t);
void 	markFaceAsVisited( int whichFace, unsigned char *facesAlreadyVisited );
void 	markPrimFacesAsVisited( int whichFace, unsigned char *facesAlreadyVisited );
void 	compUnfoldingMatrix( int currentSecFace, int inSecFaces,
							int *listOfSecFaces, int *listOfSecEdges,
							Matrix4 *t );
int 	insertCellsFromFaceInTmpList( int whichFace, int sourceFace, Matrix4 m,
									 SCELL *h, SCELL *t );

#endif //_FORCES_H_
