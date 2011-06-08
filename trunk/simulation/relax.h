/*
 *-------------------------------------------------
 *	relax.h
 *-------------------------------------------------
 */

#ifndef _RELAX_H_
#define _RELAX_H_

#include "../data/Types.h"

#define	WRP			2.4;
/* Added by Thompson Peter Lied in 15/07/2002 */
/*
 #define WFORCE		0.067;
 #define WX			1.0;
 #define WY			1.0;
 */
#define WD			0.066;
#define WA                      0.000;
#define	INITWFORCE		0.005;
#define INITNUMRELAX		70;
#define TOTAL_TIME		10;
#define TIME_UPDATE		10;
#define	NCELLS			500;
#define ITER_PER_UNIT_TIME 	4;

/* Added by Thompson Peter Lied in 15/07/2002 */
#define ORIENTATION             0.0;
/* end */


/*
 *--------------------------------------------------
 *	Local Defined Global variables
 *--------------------------------------------------
 */
extern double 	radiusRep;
extern double  radiusRepSquare;

/* area of one cell */
extern double  AreaOneCell;

/* Debug information. How many times cells changed
 faces */
extern int nChangeFaces;

/* Anisotropy effects */
extern double AniX, AniY, AniCommon;

extern double Ani;

/*---PROTOTYPES---*/

void 	compRadiusRepulsion( int firstTime );
/* Modified by Fabiane Queiroz in 25/11/2009 */
void 	relaxation( int steps, RELAXMODE rMode, Array* listFaces );

void 	mapOntoPolySpace( int whichFace, double x, double y, double z, Point2D *p );
void 	mapFromPolySpace( int whichFace, double x, double y, Point3D *p );
void 	compCellsPolygonCoordinates(Array* listFaces );
void 	keepCellOnPlane( CELL *c );
void 	keepCellOnSurface( CELL *c );
int 	cellInPoly( CELL *c, int currentPosition );
int 	cellInPolyPrevious( CELL *c );
void 	mapPointOntoPlane( Point3D *p, int whichFace );
int 	pointInPoly2D( int whichFace, double x, double y);
int 	cellInPolyAndEdge( CELL *c, int *direction, Point3D *intersec3D,
						  double *t, int prevEdge );
void 	initRelax( void );
void 	compForces( Array* listFaces );



void 	updatePositions( Array* listFaces );
void 	updatePositionsForFace( int whichFace, double *fMin, double *fMax, double *avgF );
void	compCellsPolygonCoordinatesBary( void );

void 	initialRelaxation( void );

/* Added by Thompson Peter Lied in 15/07/2002 */
void    compOrientation( void );
void    comp_aniso_effect( void );

//Not implemented, end used only one time
void killCell(CELL *c);
/* end */

#endif //_RELAX_H_

