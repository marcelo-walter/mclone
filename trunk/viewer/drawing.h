/*
 *---------------------------------------------------------
 *
 *      drawing.h
 *      Marcelo Walter
 *---------------------------------------------------------
 */

#ifndef _DRAWING_H_
#define _DRAWING_H_

#include <GL/glut.h>

#include "../data/Types.h"

#define MY_CIRCLE_LIST 	1	/* used with the display list */
#define MY_AXES_LIST	2	/* creates a display list for drawing
the axes */
#define MY_LINES_LIST	3	/* display list for the 2 lines
separating the 4 windows */
#define	MY_GRID_LIST	4	/* display list for the grid */
#define	MY_SCENE_LIST	5	/* scene: background, floor */

/*
 *-----------------------------------------------
 *	Local definitions
 *-----------------------------------------------
 */
#define	MY_FONT	GLUT_BITMAP_HELVETICA_18

extern flag sceneFlag;
extern flag shadowFlag;
extern flag modeFlag;
extern flag vectorsFlag;	/* show or not the vectors
 that define a local coordinate
 system for the polygon */
extern flag normalsFlag;	/* Vin?cius 17/09/2004 */
extern flag voronoiBorders;

extern flag bwOutput;
extern flag degreeVector;	/* Thompson 07/11/2003 */

extern flag drawVector;
extern flag drawVectorField;
extern flag drawVerticesVectorField;

extern flag fillingFlag;	/* draw or not filled polygons */ //From main.h
extern flag edgesFlag;		/* draw or not edges */ //From main.h

/* color definitions */
extern float backColor[XYZ];
extern float foreColor[XYZ];
extern float colorBlue[XYZ];
extern float colorRed[XYZ];
/*float colorRed[XYZ] = {0.0, 0.0, 0.0};*/
/* float colorWhite[XYZ] = {0.0, 0.0, 0.0};*/
extern float colorWhite[XYZ]; //Confere
extern float colorGrey[XYZ];
extern float colorGreen[XYZ];
extern float colorYellow[XYZ];
extern float colorBrown[XYZ];
/*float colorBrown[XYZ] = {0.5, 0.5, 0.5};*/
extern float colorMagenta[XYZ];
extern float colorDarkYellow[XYZ];
extern float colorBuff[XYZ];
extern float colorMediumBrown[XYZ];



/*
 *-----------------------------------------------
 *	Local prototypes
 *-----------------------------------------------
 */
void setVertexDisplayShadow(int ix, double y);
void drawAll(MMODE mmode, VMODE vmode, RMODE rmode, int t);
void drawVoronoi( void );
void drawVoronoiBorders( void );
void drawObject( void );
void drawDegreeVector( void );
void drawNormals( void );
void drawVectors( void );
void drawAxes( Point3D v1, Point3D v2, Point3D v3, GLfloat zero[XYZ]);
void setNormal(int ix);
void setTexture(int ix, float scale);
void setColor(float c[], int vertIndex, int faceIndex);
void drawEdgesObject(void);
void drawVerticesObject(void);
void drawPoints( void );
void drawArc(float radius, float theta, float alpha, int nseg);
void buildAxes(void);
void buildCircle(void);
void drawShadows( double y );
void drawScene( void );
void buildSeparatingLines( void );
void buildGrid( void );
void drawMode(MMODE mmode, VMODE vmode, int pick, int t);
void drawSmall2DAxes( VMODE vmode );
void draw2DText( char *str, float xpos, float ypos, void *font);
void drawCircle(int npies, float radius);
void drawWireCylinder1(float r, float h, int nseg);
void drawWireCylinder(float r, float h, int nseg);
void drawPrimitives(int nPrim, int active, int t);
void drawLandMarks(int which, int active, int t, float *color);
void drawOnePrimitive(int whichPrim, int active, int t, float *color);
void drawLine(Point3D p1, Point3D p2);
void defLandMarks( void );
void drawCross( int i, float *color );
void changeCellColor( CELL *c, float r, float g, float b );
void drawVerticesVectorsField(void);
void drawVectorsField(void);
void drawCentroids(void);
void drawOrientationVectors();
void drawVect( float coords1[], float coords2[] );
void drawArrow( void );
double teste(int ix);


#endif //_DRAWING_H_
