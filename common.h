/*
 *------------------------------------------------------------
 *		common.h
 *		Marcelo Walter
 *		start: march, 96
 *------------------------------------------------------------
 */
#ifdef GRAPHICS
#include <GL/gl.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include "data/Macros.h"
#include "data/Types.h"
#include "data/Globals.h"

#ifdef GRAPHICS
/*
 * various flags
 */
extern flag fillingFlag;	/* fill the polygons or not */
extern flag showPrimitives;	/* display the primitives or not */
extern flag showLandMarks;	/* display the landmarks or not */
extern flag drawCells;		/* draws the cells or not */
extern flag polygonPicking;	/* picking polygons active or not */
extern flag cellPicking;	/* picking cells active or not */
extern flag edgesFlag;		/* draw the polygon's edges or not */
extern flag objectFlag;		/* draws the object or not */
extern flag colorPrimFlag;	/* paint the polygons according to
 the primitive they belong */
extern flag lightFlag;
extern flag gridFlag;		/* draws a grid or not */
#endif

/*
 * Global declarations
 */
/* defined in readInpFile.c */ 
extern Vertex 	*vert;
extern Point3D 	*vertDisplay;
extern Point3D 	*origVert;
extern Point3D 	*vertn;
extern Point2D 	*vertt;
extern Face   	*faces;
extern VECTORARRAY   	*vectors;
extern Edge	*edges;
/*add by Fabiane*/
//extern VECTORARRAY *vectors;

extern MMODE 	modelMode;

/* declared in simulation.c */
extern Point3D 	*vertBody;

/* in myLinkCode.c */
void
getVoronoi(SCELL *headCell, SCELL *tailCell, 
           voronoiType **vorList, faceType **faceList, int *howManyVert);
/*
 * declarations in myvect.c
 */
double V3SquaredLength(Point3D *a);
double V3Length(Point3D *a);
Point3D *V3Sub(Point3D *a, Point3D *b, Point3D *c);
Point3D *V3Cross(Point3D *a, Point3D *b, Point3D *c);
Point3D *V3Zero(Point3D *v);
Point3D *V3Divide(Point3D *v,  double divisor);
Point3D *V3Multiply(Point3D *v,  double mult);
Point3D *V3AddPlus(Point3D *u,  Point3D *v);
Point3D *V3Init(Point3D *v, double value);
Point3D *V3Normalize(Point3D *a);
void V3Assign(Point3D *destiny, Point3D source);
double V3Dot(Point3D *a, Point3D *b);

void MatMul(Matrix4 *a, Matrix4 *b, Matrix4 *c);
void MatMulCum(Matrix4 *a, Matrix4 *b);
void V3PosMul(Point3D *p, Matrix4 *m);
void V3PreMul(Point3D *p, Matrix4 *m);
void loadIdentity(Matrix4 *m);
void MatrixCopy( Matrix4 *source, Matrix4 *destiny);
void MatRotateZ(double theta, Matrix4 *m);
int MxRotateAxis(Point3D p1, Point3D p2, double theta, Matrix4 *TM, Matrix4 *iTM);
double MxInvert( Matrix4 *a, Matrix4 *b);
double MxDeterminant ( Matrix4 *a, int order );
static double MxElementMinor(Matrix4 *a, register int i, register int j, int order);
void MxTranspose( Matrix4 *a, Matrix4 *r);
void MxZero ( Matrix4 *a );
void copyMatrix( Matrix4 *destiny, Matrix4 source );

/*
 * declarations in util.c
 */
void minMax(Point3D vert, Point3D *max, Point3D *min);
void compMomentInertia(Point3D *diagonal, Point3D *lower, Point3D vert, Point3D centroid);
void assignValue(float v[], float value);
void assignValueVector(float v[], float value[XYZ]);
float deg2rad(float angle);
void savePrimitivesFile(char *outFile, int nPrim);
int loadPrimitivesFile(char *inFile);
void beep ( void );
float rad2deg(float angle);
void errorMsg(char error_text[]);
void switchFlag(byte *flag);
int checkPrimitivesFile(void);
double distance(Point3D p, Point3D q);
void toOpenGL(Matrix4 m, float *glm);
void openParseGrowthData( int *r, int *c );
void multiplyValue( float v[], float value );
int Getline( char s[], int lim, FILE *fp );
float computeGlobalScale( int index, float max );
void includeUserAgeintoGrowthArray( int time, float scale );
void openParseAnimData( int *r, int *c );
void PlaneEquation( int faceIndex, Point4D *plane );
//void splineInterpGrowthData( int rowsGrowthData, int columnsGrowthData, int firstTime ); //removed function, by Bins
//void splineInterpAnimData( int rowsAnimData, int columnsAnimData, int firstTime ); //removed function, by Bins
void linearInterpGrowthData( int rowsGrowthData, int columnsGrowthData, int firstTime );
void linearInterpAnimData( int rowsAnimData, int columnsAnimData, int firstTime );
void saveObjFile( const char *fileName );
int **allocate2DArrayInt( int r, int c );

/* 
 * declarations in print.c
 */
void printInfoPrim( int nPrim );
void printVertexInfo( void );
void printMatrix(Matrix4 m);
void printArray( float **p, int rows, int columns );
void printMatrixInfo( char *s, Matrix4 m, Point3D v );
void printFacesInfo( void );
void printPoint3D( Point3D p);
void printPoint2D( Point2D p);
void printActivePrim( void );
void printEdges( void );
void printCellsInfo( void );
void printCellsTimeSplit( void );
void printString( int mode, char *string );

/*
 * declarations in distpoints.c
 */
double triangleAreas( void );
void partialSumArea( double totalArea );
int faceSortCmp( const Face *a,  const Face *b );
Point3D *square2polygon( int polygon, float s, float t, Point3D *p );
void createRandomCells( int quantity, int embryoShape );
int binarySearch( float value );
void compCanonicalCoord( CELL *c );
double areaTriangle( Point3D p1, Point3D p2, Point3D p3 );
void compBarycentricCoord( CELL *c );
void fromBarycentricCoord( CELL *c );
/*add by fabiane Queiroz at 08/2010*/
void compVectorBarycentricCoord( VECTORARRAY *v );
void fromVectorBarycentricCoord( VECTORARRAY *v );

/* morph.c */
void morph( int t );
void morphVertices( int t );
double weightFunction( double y, double h );
double applyGrowthInformation( int primIndex, /*int vertIndex,*/
							  Point3D vertexToBeTransformed,
							  Point3D *vReturn, int t );
void fromMaster2World( Point3D *vReturn );
void morphPrimitives( int t );
void followMasterPrimitive( int primIndex, Matrix4 *m, int t,
						   Point3D *vReturn );
void growRadiusHeight( int whichPrim, float *h, float *r, int t );
void morphOneStep( int direction );



/* ui.c */
void winDisplay(void);
static void winKbd( unsigned char, int, int );
static void winReshape( int, int );
static void winMotion (int x, int y);
static void winMouse( int button, int state, int x, int y );
static void specialKbd( int key, int x, int y);
static void myIdle(void);
static void winPassive (int x, int y);
void InitGraphics(int argc, char *argv[]);
static void myTimer(int value);
/* Added by Thompson at 10/12/2003 */
void capturePoints(int x, int y, float coords[]);

/* primitives.c */
void parametrize( void );
void createLandMark( void );
void duplicatePrimitive( void );
void createPrimitive( int type );
void parametrize( void );
void checkParametrization( void );
int  belongPrimitive( int whichPrim,
					 Point3D vertToBeTransformed,
					 Point3D *vReturn );
void compLandMarks( void );
void testLandMarks( void );
void checkLandMarks( void );
void compCylinderCoord( int whichPrim,
					   Point3D vertToBeTransformed,
					   Point3D *vReturn );
void assignPrim2Vert( void );
void assignPrim2Faces( void );
/*Modified by Fabiane Queiroz in 01/12/2009*/
void assignPrim2AllCells(Array* listFaces);

void checkCellsParametrization( void );
void assignPrim2Cell( CELL *c );
/*Modified by Fabiane Queiroz in 01/12/2009*/
void compParamCoordAllCells(Array* listFaces);

void compParamCoordOneCell( CELL *c );
void compUV( Point3D p, double *u, double *v );
void compParamCoordAllVert( void );
void compParamCoord3DPoint( int vertID );

/* heapTri.C */
void HeapUpTri(HEAPTRI *h, int k);
void HeapInsertTri(HEAPTRI *h, HEAPTRI_ELEMENT *a);
void HeapGrowTri(HEAPTRI *h, int grow);
HEAPTRI *HeapInitTri(int size);
void HeapPrintTri(HEAPTRI *h);
void HeapOrderPrint1Tri(HEAPTRI *h);
HEAPTRI_ELEMENT *HeapRemoveTri(HEAPTRI *h);
void HeapDownTri(HEAPTRI *h, int k);
void EmptyHeapTri(HEAPTRI *h);
HEAPTRI *ClearHeapTri(HEAPTRI *h);
HEAPTRI_ELEMENT *allocateHeapTriElem( void );

/*
 * declarations in list.c
 */
void freeListOfCells( CELL *h, CELL *t );
CELL *cellAlloc( void );
void insertOnListOfCells(CELL *p, CELL *h, CELL *t);
void printListOfCells(CELL *h, CELL *t);
void removeFromListOfCells(CELL *p, CELL *h);
void changeFacesList(CELL *p, int newFace, int oldFace);
/* routines for simple cells */
void freeListOfCells_S( SCELL *h, SCELL *t );
SCELL *cellAlloc_S(void);
void insertOnListOfCells_S(SCELL *p, SCELL *h, SCELL *t);
void printListOfCells_S(SCELL *h, SCELL *t);
void removeFromListOfCells_S(SCELL *p, SCELL *h);

/*
 * declarations in cells.c
 */
CELL *createCell( Point3D p, int whichFace, CELL_TYPE t, byte g );
void initCellsInfo( void );
void handleSplit( CELL *c );
void switchCellType(CELL *p, CELL_TYPE newType);
CELL_TYPE getNewType( int type );

/*
 * declarations in vectorField.c 
 */
void create_global_list(void);
VECTORARRAY *vectorAlloc(void);
void insert(VECTORARRAY *v, VECTORARRAY *h, VECTORARRAY *t);
void removeVector(VECTORARRAY *v, VECTORARRAY *h);
void storePointVector( int x, int y );
void createVector( float coordsBegin[], float coordsEnd[], int faceIndex );
void printListOfVectors( void );
void readVectorFile( char *inpFileName );
void saveVectors( char *sessionFileName );
void SetaTipoRBF(void);

#ifdef GRAPHICS
/* 
 * declarations in graph.c
 */
void buildDisplayLists( void );

/*
 * initialization functions
 */
void initWin( void );
void initCallBacks( void );
void initLightingInfo( void );
void initOrthProjections( void );
void initGLStuff( void );
void globalCameraTransf(void);
void InitWorldTransf(void);
void primTransf(int i);
void checkHit(int x, int y);
static void processHits(int hits, GLuint buffer[]);
void myPrimTransf(int whichPrim);
void test(void);
void initColorArray(void);
void auxDisplay(VMODE vmode, double eyex, double eyey, double eyez,
				double upx, double upy, double upz,
				int x, int y, int w, int h);

/* trackball functions */
void mouse2worldspace(int sx, int sy, float *xx, float *yy);
void handleTrackball(int nmx, int nmy);
void zoom(int nmx, int nmy, float t[]);
void pan(int nmx, int nmy, float t[]);

void initTexture( void );
void myOpenGLDisplay( void );
void myOpenGLReshape(int w, int h);

void initLandMarks( void );

/* utilGraph.c */
void saveSessionFile( char *outFile );
int loadSessionFile( char *inFile );

/*
 * declarations in drawing.c
 */
void drawArc(float radius, float theta, float alpha, int nseg);
void buildCircle(void);
void drawMode(MMODE mmode, VMODE vmode, int pick, int t);
void drawCircle(int npies, float radius);
void drawWireCylinder(float r, float h, int nseg);
void drawPrimitives(int nPrim, int active, int t);
void drawPoints( void );
void drawEdgesObject(void);
void drawObject(void);
void drawAll(MMODE mmode, VMODE vmode, RMODE rmode, int t);
void buildAxes(void);
void setNormal(int ix);
void setColor(float c[], int vertIndex, int faceIndex);
void buildSeparatingLines( void );
double teste(int ix);
void setTexture(int ix, float scale);
void drawWireCylinder1(float r, float h, int nseg);
void draw2DText( char *str, float xpos, float ypos, void *font);
void buildGrid( void );
void drawSmall2DAxes(VMODE vmode);
void drawOnePrimitive(int whichPrim, int active, int t, float *color);
void drawCross( int i, float *color);
void drawEdgesObject1(void);
void defLandMarks( void );
void drawLandMarks(int n, int active, int t, float *color);
void drawLine(Point3D p1, Point3D p2);
void drawVectors( void );
void drawAxes( Point3D v1, Point3D v2, Point3D v3, GLfloat zero[XYZ]);
void drawDegreeVector( void );
void drawArrow( void );
void drawVectorsField( void );
/*added by Fabiane Queiroz*/
void drawVerticesVectorsField(void);
void drawSecFaces( int ix );
void drawTerFaces( int ix );
#endif
