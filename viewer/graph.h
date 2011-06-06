/*
 *  graph.h
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/10/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */

#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <GL/glut.h>
#include "../data/Types.h"

/*
 *------------------------------------------------------------
 *		Local definitions
 *------------------------------------------------------------
 */

/* for the perspective projection */
#define FRONT_CLIP_PLANE	0.1
#define BACK_CLIP_PLANE		1000.0

/* Texture definitions */
#define	checkImageWidth 64
#define	checkImageHeight 64



/*
 *--------------------------------------------------
 *	Local Global variables
 *--------------------------------------------------
 */
extern GLubyte checkImage[checkImageWidth][checkImageHeight][3];

/* modeling mode. One of Rotation, Translation or
 Scaling */
extern MMODE modelMode;

/* viewing mode. One of Perspective, Side, Top or
 Front */
extern VMODE viewMode;

/* rendering mode. One of Fill, Wire Frame, Halo,
 backface or frontlines */
extern RMODE renderMode;

/* mouse button pressed (LEFT, MIDDLE, RIGHT */
static int mouseButton;

/* window limits of the ortographic projection.
 3 windows (top, front, side) and
 (xmin, xmax, ymin, ymax, zmin, zmax) for
 each window */
extern float orth[3][6];

/* width and height of the viewport */
extern GLint WIDTH, HEIGHT;

extern float colPrim[N_OF_PRIM+1][XYZ];

/*
 *--------------------------------------------------
 *		various flags
 *--------------------------------------------------
 */
/* splits the window in 4 or not */
extern flag windowSplitFlag;

/* which vertex belongs to which primitive was computed */
extern flag morphedFlag;

/* polygon picking mode active or not */
//extern flag polygonPicking; //Duplicate: declared in vectorField

/* cell picking mode active or not */
extern flag cellPicking;

/* point picking mode active or not */
/*Tira*///Nunca foi usado; flag pointPicking = FALSE;	/* Added by Thompson at 04/12/2003 */

/* uses Open GL lights or not */
extern flag lightFlag;

/* draws the object or not */
extern flag objectFlag;

/* paint the polygons according to the primitives they
 belong */
extern flag colorPrimFlag;

/* draw cells or not */
extern flag drawCells;

/* display the primitives or not */
extern flag showPrimitives;

/* display the landmarks or not */
extern flag showLandMarks;

/* display the axes or not */
extern flag showAxes;

/* display texture or not */
extern flag textureFlag;

/* draws a grid or not */
extern flag gridFlag;

extern flag mouseMovingFlag;

/* mouse speed for panning */
extern float panSpeed;


/*
 *------------------------------------------------------------
 *		Local prototypes
 *------------------------------------------------------------
 */
void makeCheckImage(void);
void initWin();
void initTexture( void );
void initGLStuff( void );
void buildDisplayLists( void );
void InitWorldTransf(void);
void initOrthProjections( void );
void initLightingInfo( void );
void mouse2worldspace(int sx, int sy, float *xx, float *yy);
void handleTrackball(int nmx, int nmy);
void zoom(int nmx, int nmy, float t[]);
void pan(int nmx, int nmy, float t[]);
static void processCellHits(int hits, GLuint buffer[]);
static void processPolygonHits(int hits, GLuint buffer[]); /*Redone in 22/07/2004 by Vin?cius*/
void checkHit(int x, int y);
void primTransf(int i);
void myPrimTransf(int whichPrim);
void myOpenGLReshape( int w, int h );
void myOpenGLDisplay( void );
void auxDisplay(VMODE vmode, double eyex, double eyey, double eyez, double upx,
		double upy, double upz, int x, int y, int w, int h);
void initColorArray(void);


#endif //_GRAPH_H_
