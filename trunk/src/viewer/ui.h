/*
 *------------------------------------------------------------
 *	ui.c
 *	Marcelo Walter
 *	This module contains all GLUT stuff to interface
 *	the OpenGL to X
 *	
 *------------------------------------------------------------
 */

#ifndef _UI_H_
#define _UI_H_

#include <GL/glut.h>

#include "../data/Types.h"

/*
 *------------------------------------------------------------
 *		Structures
 *------------------------------------------------------------
 */
// Estruturas utilizadas para leitura de arquivos BMP
typedef struct { 
    unsigned short  bfType; 
    unsigned long   bfSize; 
    unsigned short  bfReserved1; 
    unsigned short  bfReserved2; 
    unsigned long   bfOffBits; 
} BITMAPFILEHEADER;

typedef struct {
    unsigned long  biSize; 
    long biWidth; 
    long biHeight; 
    unsigned short biPlanes; 
    unsigned short biBitCount; 
    unsigned long biCompression; 
    unsigned long biSizeImage; 
    long biXPelsPerMeter; 
    long biYPelsPerMeter; 
    unsigned long biClrUsed; 
    unsigned long biClrImportant; 
} BITMAPINFOHEADER;

extern int winWidth, winHeight;
extern int screenFileName, Contador;
extern GLubyte *frameBuf;
extern BITMAPFILEHEADER bmpFileHeader;
extern BITMAPINFOHEADER bmpInfoHeader;

/*
 *--------------------------------------------------
 *	Local Global variables
 *--------------------------------------------------
 */
/* stop animation or not */
extern flag stopAnimation;

/* which mouse button was pressed */
extern flag rotateflag;	/* left */
extern flag panflag;		/* middle */
extern flag zoomflag;		/* left AND middle */

/* mouse flags specific for the picking part */
extern flag leftPickFlag;
extern flag middlePickFlag;

/* gear ratios for the mouse */
extern float mouseRotSpeed, mouseScaleSpeed,  mouseTransSpeed;

/* constrain mouse movements to be only vertical or horizontal */
extern flag horizontalFlag;
extern flag verticalFlag;

/* primitives flags moviments */
extern flag yFlag;
extern flag xFlag;
extern flag zFlag;

/*
 * A record of where the last mouse position is
 */
extern int oldMouseX, oldMouseY;

/* defining land marks mode. The key 'L' activates
 and desativates this mode */
extern flag defineLandMarks;

extern int winId[5];

extern flag  ntscSize;

extern float reverseEye;

extern int IndexAnterior;

/*
 *--------------------------------------------------
 *	Prototypes
 *--------------------------------------------------
 */
void InitGraphics(int argc, char *argv[]);
void initCallBacks( void );
static void winPassive (int x, int y);
static void winMotion (int x, int y);
static void winMouse( int button, int state, int x, int y );
static void specialKbd( int key, int x, int y);
static void winKbd( unsigned char key, int x, int y );
static void winReshape( int w, int h );
void ConvertIntString(char* str, int value);
int WriteBMP(char *strFileName, unsigned char *pData, int iWidth, int iHeight);
void winDisplay(void);
void capturePoints(int x, int y, float coords[]);

#endif //_UI_H_
