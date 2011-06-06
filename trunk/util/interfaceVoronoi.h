/*
 *	interfaceVoronoi.h
 */

#ifndef _INTERFACE_VORONOI_H_
#define _INTERFACE_VORONOI_H_

#include "../data/Types.h"

/*
 *--------------------------------------------------------------------
 * 	Global Variables
 *--------------------------------------------------------------------
 */
extern flag voronoiFlag;
extern flag voronoiComputed;
extern flag voronoiColorFlag;

extern int NumberVoronoiVert; //From Globals.h

/*
 *--------------------------------------------------------------------
 * 	PROTOTYPES
 *--------------------------------------------------------------------
 */
void computeVoronoi( void );
CELL *findSite( CELL *c );
byte checkInsideOutside( int whichFace, double x, double y );
int  clipVoronoiPolygons( int whichFace, int prevNumberVertices );
void checkClippedVorPolygons( int whichFace );
void SutherlandHodgmanPolygonClip( Point2D inVrtArray[MAX_N_VOR_VERT],
								  Point2D *outVrtArray,
								  int inLength, int *outLength,
								  int whichEdge, int whichFace );
void newVorPolyClip( int whichFace, int prevNumberVorVert );
byte insideEdge( Point2D t, Point2D vStart, Point2D vEnd );
int  makeTmpListOfCells( int whichFace , SCELL *h, SCELL *t );
void includeDummyCells( int whichFace, SCELL *h, SCELL *t );
void voronoiZeroOneCells( int whichFace, SCELL* h, SCELL *t );
void voronoiTwoCells( int whichFace, SCELL* h, SCELL *t );
int  voronoiGeneral( int whichFace, SCELL *h, SCELL *t, int originalNumberCells);
int  checkIndexZero( int nCells, faceType *facesList );
void classifyVoronoiCells( int nCells, int whichFace, voronoiType *points,
						  faceType *facesList, int *nIn, int *nOut,
						  int *nBorder );
void includeBorderCells( int nCells, int whichFace, voronoiType *points,
						faceType *facesList, int *nOut, int *nBorder );
int  checkProperStatus( int nCells, faceType *facesList );
void transferTmpVoronoiToPolygon( int whichFace, voronoiType *points,
								 int *tmpVertArray, int nValVert );
void countNeighborsByType( int whichFace, int *nOfC, int *nOfD,
						  int *nOfE, int *nOfF );
void compVoronoiForFace( int whichFace );
void getSpaceForVorPts( int whichFace, int howMany );
void getSpaceForVorPol( int whichFace, int howMany );


#endif //_INTERFACE_VORONOI_H_
