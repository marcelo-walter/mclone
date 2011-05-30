/*
 *-------------------------------------------------------
 *	print.c
 *	Useful information "printed" to the screen
 *-------------------------------------------------------
 */

#include "../common.h"

extern int activePrim;

void printActivePrim( void );
void printArray(float **p, int rows, int columns);
void printInfoPrim(int nPrim);
void printPoint3D( Point3D p);
void printPoint2D( Point2D p);
void printFacesInfo(void);
void printVertexInfo(void);
void printMatrixInfo(char *s, Matrix4 m, Point3D v);
void printMatrix(Matrix4 m);
void printEdges( void );
void printCellsInfo( void );
void printCellsTimeSplit( void );
void printTmpListCells( int whichFace,  SCELL *h, SCELL *t );
void printString( int mode, char *string );
