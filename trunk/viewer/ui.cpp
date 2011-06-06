/*
 *------------------------------------------------------------
 *	ui.c
 *	Marcelo Walter
 *	This module contains all GLUT stuff to interface
 *	the OpenGL to X
 *	
 *------------------------------------------------------------
 */

#include "ui.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "graph.h"
#include "trackball.h"
#include "drawing.h"
#include "utilGraph.h"

#include "../control/primitives.h"
#include "../control/vectorField.h"
#include "../simulation/simulation.h"
#include "../simulation/planar.h"
#include "../simulation/morph.h"
#include "../data/Object.h"
#include "../util/genericUtil.h"
#include "../util/printInfo.h"

#include "../util/vect.h"

#include "../temporary.h"

/*
 *------------------------------------------------------------
 *
 *------------------------------------------------------------
 */
long IndiceGeodesicPath, OffSetIndiceGeodesicPath; //Tentar mover pra dentro da funcao...

GLfloat coords[3];
int OldFaceIndex = 0;


/*
 *--------------------------------------------------
 *	Local Global variables
 *--------------------------------------------------
 */

int winWidth = 720, winHeight = 720;
int screenFileName = 1, Contador = 0;
GLubyte *frameBuf = NULL;
BITMAPFILEHEADER bmpFileHeader;
BITMAPINFOHEADER bmpInfoHeader;


/* stop animation or not */
flag stopAnimation=TRUE;

/* which mouse button was pressed */
flag rotateflag = FALSE;	/* left */
flag panflag = FALSE;		/* middle */
flag zoomflag = FALSE;		/* left AND middle */

/* mouse flags specific for the picking part */
flag leftPickFlag = FALSE;
flag middlePickFlag = FALSE;

/* gear ratios for the mouse */
float mouseRotSpeed, mouseScaleSpeed,  mouseTransSpeed;

/* constrain mouse movements to be only vertical or horizontal */
flag horizontalFlag = FALSE;
flag verticalFlag = FALSE;

/* primitives flags moviments */
flag yFlag = FALSE;
flag xFlag = FALSE;
flag zFlag = FALSE;

/*
 * A record of where the last mouse position is
 */
int oldMouseX, oldMouseY;

/* defining land marks mode. The key 'L' activates
 and desativates this mode */
flag defineLandMarks = FALSE;

int winId[5];

flag  ntscSize = FALSE;

float reverseEye = 1.0;

int IndexAnterior = 0;

/*
 *--------------------------------------------------
 *	Init graphics and GLUT functions
 *--------------------------------------------------
 */
void InitGraphics(int argc, char *argv[])
{
    glutInitWindowPosition( 0, 0 );
	
    /* if ( ntscSize ) glutInitWindowSize( 640, 480 ); */
    /* if ( ntscSize ) glutInitWindowSize( 320, 320 ); mpeg size */
    if ( ntscSize ) glutInitWindowSize( 400, 320 );	/* mpeg size */
    else glutInitWindowSize( 600, 600 );
    /*else glutInitWindowSize( 300, 300 );*/
	
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	
    initWin();
    InitWorldTransf();
	
    loadSessionFile( sessionFileName );
	
    activePrim=WORLD;
	
    /* init mouse scale and rotation speeds */
    mouseRotSpeed = 1/100.0;
    mouseScaleSpeed = 1/50.0;
    mouseTransSpeed = 1/5.0;
    /* printf("mouse speeds: rot = %f trans = %f scale = %f\n",
	 mouseRotSpeed, mouseTransSpeed, mouseScaleSpeed); */
	
    /* never ending GLUT loop */
    glutMainLoop();
}	

/*
 *-----------------------------------------------------
 *	Register the callbacks used by GLUT
 *-----------------------------------------------------
 */
void initCallBacks( void )
{
    glutDisplayFunc( winDisplay );
    glutKeyboardFunc( winKbd );
    glutReshapeFunc( winReshape );
    glutMouseFunc( winMouse );
    glutMotionFunc( winMotion );
    glutSpecialFunc( specialKbd );
    glutPassiveMotionFunc( winPassive );
}

/*
 *----------------------------------------------------------
 *	winPassive
 *	Handles passive house movements
 *	x, y - mouse position
 *----------------------------------------------------------
 */
static void winPassive (int x, int y)
{
    /*Verificar*/
    faces[OldFaceIndex].hit = FALSE;
    if (createVect)
    {
	    polygonPicking = TRUE;
	    checkHit(x,y);
	    polygonPicking = FALSE;
	    
	    capturePoints( x, y, &coords[0]);
	    faces[PointPicking].hit = TRUE;
	    OldFaceIndex = PointPicking;
    }
	
    oldMouseX = x;
    oldMouseY = y;
}
/*
 *----------------------------------------------------------
 *	winMotion
 *	Here is the central part for the interaction
 *	The viewing of the whole is controlled through
 *	a trackball. The trackball is only applied when
 *	the activePrim variable is equal to WORLD (ie, no
 *	primitive is selected)
 *	
 *	If it is not the WORLD, then it means we are
 *	modeling a given primitive. There are 3 possible
 *	cases: TRANS, ROT and SCALE
 *	TRANS: left mouse controls 'x' and 'y' translations
 *	       left AND middle mouse controls 'z' translation
 *	       through HORIZONTAL mouse movements!!!!
 *
 *	SCALE: left mouse controls 'x' and 'y' scaling
 *	       left AND middle mouse controls 'z' scaling
 *	       through HORIZONTAL mouse movements!!!!
 *
 *	ROT: left mouse controls 'x' and 'y' rotations
 *	     left AND middle mouse controls 'z' rotation
 *	     through HORIZONTAL mouse movements!!!!
 *----------------------------------------------------------
 */

/* Modified by Fabiane Queiroz in 11/22/09 */

static void winMotion (int x, int y)
{
    float t[XYZ];
	
    if(!polygonPicking && !cellPicking && !createVect){ /* not in picking mode */
		if ( activePrim == WORLD){
			mouseMovingFlag = TRUE;
			handleTrackball(x, y);
		}
		
		else{	/* the primitive being handled is NOT the world camera */
			switch(modelMode){
				case TRANS:
					t[X] = t[Y] = t[Z] = 0.0;
					
					/* printf("before %f %f %f\n", t[X], t[Y], t[Z]); */
					if (rotateflag)	pan(x,y,t);
					
					else if (zoomflag)zoom(x,y,t);             
					
					multiplyValue(t, mouseTransSpeed); /* apply some scaling to the mouse */
					/* printf("after %f %f %f\n", t[X], t[Y], t[Z]); */
					vadd(t, Prim[activePrim].trans, Prim[activePrim].trans);
					break;
					
				case ROT:
					/*printf("Entered Rotation Mode!\n"); */
					if (rotateflag){
						if (horizontalFlag){					
							Prim[activePrim].rot[Z] -= (float) (oldMouseX - x)*mouseRotSpeed;
						}else if(verticalFlag){
							Prim[activePrim].rot[Y] -= (float) (oldMouseY - y)*mouseRotSpeed;
						}else{
							Prim[activePrim].rot[Z] -= (float) (oldMouseX - x)*mouseRotSpeed;
							Prim[activePrim].rot[Y] -= (float) (oldMouseY - y)*mouseRotSpeed;
							//Prim[activePrim].rot[X] -= (float) (oldMouseX - x)*mouseRotSpeed;
							//Prim[activePrim].rot[Y] -= (float) (oldMouseY - y)*mouseRotSpeed;
						}
					}
					else if (zoomflag)
						Prim[activePrim].rot[X] -= (float) (oldMouseX - x)*mouseRotSpeed;
				    //Prim[activePrim].rot[X] -= (float) (oldMouseX - x)*mouseRotSpeed;
					break;
					
				case SCALE:			 
					/*printf("Entered Scaling Mode!\n");*/ 
					if (rotateflag){
						//if (horizontalFlag){				
						//Prim[activePrim].scale[X] -= (float) (oldMouseX - x)*mouseRotSpeed;
						
						// }else if(verticalFlag){
						//Prim[activePrim].scale[Y] -= (float) (oldMouseY - y)*mouseRotSpeed;
						
						// }else{
						Prim[activePrim].scale[Z] -= (float) (oldMouseX - x)*mouseScaleSpeed;
						Prim[activePrim].scale[Y] -= (float) (oldMouseX - x)*mouseScaleSpeed;
						//Prim[activePrim].scale[X] -= (float) (oldMouseX - x)*mouseScaleSpeed;
						//Prim[activePrim].scale[Y] += (float) (oldMouseY - y)*mouseScaleSpeed;
						
						//}
					}
					else if (zoomflag){				
						Prim[activePrim].scale[X] -= (float) (oldMouseX - x)*mouseScaleSpeed;
						//Prim[activePrim].scale[Z] -= (float) (oldMouseX - x)*mouseScaleSpeed;
						
					}
					break;
				default:
					break;
			}
		}
		oldMouseX = x;
		oldMouseY = y;
	    
		glutPostRedisplay();
    }
}
/*
 *----------------------------------------------------------
 *	winMouse
 *	There are 3 possible "modes":
 *	Rotation mode -> left mouse button (rotateflag)
 *	Pan mode -> middle mouse button (panflag)
 *	Zoom mode -> left AND middle mouse buttons (zoomflag)
 *----------------------------------------------------------
 */
/*Modified by Fabiane Queiroz at 29/01/2009*/
static void winMouse( int button, int state, int x, int y )
{ 
	
	if( !polygonPicking && !cellPicking )
	{
		leftPickFlag = FALSE;
		middlePickFlag = FALSE;
		
		if (button == GLUT_MIDDLE_BUTTON) 
		{
			if (state == GLUT_DOWN)
			{
				/*Start*/
				
				/*comented by fabiane Queiroz
				 For many triangles, it has a "bus error" in McloneViewer*/
				//faces[PointPicking].hit = FALSE;
				
				
				createVect = TRUE;
				
				polygonPicking = TRUE;
				
				checkHit(x,y);
				
				polygonPicking = FALSE;
				createVect = FALSE;
				
				
				if (IndexAnterior < PointPicking)
				{
					IndiceGeodesicPath = ((	IndexAnterior * NumberFaces) - ((IndexAnterior * (IndexAnterior + 1))/2));
					OffSetIndiceGeodesicPath = ((PointPicking - IndexAnterior)-1);
					
				}
				else {
					IndiceGeodesicPath = ((PointPicking * NumberFaces) - ((PointPicking * (PointPicking+1))/2));
					OffSetIndiceGeodesicPath = ((IndexAnterior - PointPicking)-1);
					
				}
				
				/*fprintf( stderr, "\nIndex = %d [%d][%d] = %f (%d, %d)\n", PointPicking, IndexAnterior, PointPicking, ArrayGeodesicPath[(IndiceGeodesicPath 							+ OffSetIndiceGeodesicPath)].Distance, ArrayGeodesicPath[(IndiceGeodesicPath + OffSetIndiceGeodesicPath)].Path1, 							ArrayGeodesicPath			[(IndiceGeodesicPath + OffSetIndiceGeodesicPath)].Path2);*/
				IndexAnterior = PointPicking;
				faces[PointPicking].hit = TRUE;
				/*End*/
				oldMouseX = x;
				oldMouseY = y;
				if (rotateflag == TRUE)
				{
					zoomflag = TRUE;
					rotateflag = FALSE;
				}
				else panflag = TRUE;
			}
			else if (state == GLUT_UP)
			{
				if (zoomflag == TRUE)
				{
					zoomflag = FALSE;
					rotateflag = TRUE;
				}
				else panflag = FALSE;
			}
		}
		else if (button == GLUT_LEFT_BUTTON)
			
		{
			if (state == GLUT_DOWN)
			{
				if( createVect == TRUE )
				{	
					storePointVector( x, y );
					glutPostRedisplay();
				}
				
				if(removevect){
					printf("removendo vetor: %d \n", PointPicking);
					removeVect(PointPicking);
				}
				
				oldMouseX = x;
				oldMouseY = y;
				if (panflag == TRUE)
				{
					zoomflag = TRUE;
					panflag = FALSE;
				}
				else rotateflag = TRUE;
			}
			
			else if (state == GLUT_UP)
			{				
				if (zoomflag == TRUE)
				{
					zoomflag = FALSE;
					panflag = TRUE;
				}
				else rotateflag = FALSE;
			}	
		}
		
	}
	/* picking is in effect */
	else
	{		
		if (state == GLUT_DOWN)
		{
			switch( button ) 
			{
				case GLUT_LEFT_BUTTON:
					leftPickFlag = TRUE;
					middlePickFlag = FALSE;
					/* Added by Thompson at 11/12/2003 */
					//fprintf(stderr, "\n\tThank you!\n");
					checkHit(x,y);
					//printf("Left Button -> X = %d | Y = %d \n", x, y);
					break;	
				case GLUT_MIDDLE_BUTTON:
					leftPickFlag = FALSE;
					middlePickFlag = TRUE;
					/* Added by Thompson at 11/12/2003 */
					//fprintf(stderr, "\n\tThank you!\n");
					checkHit(x,y);
					//printf("Middle Button -> X = %d | Y = %d \n", x, y);
					break;
				default:
					break;
			}
		}
	}
}	

/*
 *----------------------------------------------------------
 *	specialKbd
 *	This function handles special keyboard strokes
 *	key:	Key pressed by user
 *	x:	X-position of mouse
 *	y:	Y-position of mouse
 *----------------------------------------------------------
 */
static void specialKbd( int key, int x, int y)
{
	
    switch ( key ){
		case GLUT_KEY_LEFT:
			activePrim = WORLD;
			break;
		case GLUT_KEY_RIGHT:
			activePrim++;
			while (Prim[activePrim].type != LANDMARK) activePrim++;
			if (activePrim > NumberPrimitives) activePrim = NumberPrimitives;
			break;
		case GLUT_KEY_UP:
			activePrim++;
			if (activePrim > NumberPrimitives) activePrim = NumberPrimitives;
			break;
		case GLUT_KEY_DOWN:
			activePrim--;
			if (activePrim < 0) activePrim=WORLD;
			break;
		case GLUT_KEY_HOME:
			break;
		case GLUT_KEY_END:
			break;
		case GLUT_KEY_PAGE_UP:
			mouseRotSpeed *= 1.1;
			mouseScaleSpeed *= 1.1;
			mouseTransSpeed *= 1.1;
			printf("New mouse rot speed = %f\n", mouseRotSpeed);
			printf("New mouse scale speed = %f\n", mouseScaleSpeed);
			break;
		case GLUT_KEY_PAGE_DOWN:
			mouseRotSpeed *= 0.9;
			mouseScaleSpeed *= 0.9;
			mouseTransSpeed *= 0.9;
			printf("New mouse rot speed = %f\n", mouseRotSpeed);
			printf("New mouse scale speed = %f\n", mouseScaleSpeed);
			break;
		case GLUT_KEY_F1:
			horizontalFlag = !horizontalFlag;
			if (horizontalFlag) verticalFlag = FALSE;
			printf("Horizontal Flag = %d\n",horizontalFlag);
			break;
		case GLUT_KEY_F2:
			verticalFlag = !verticalFlag;
			if (verticalFlag) horizontalFlag = FALSE;
			printf("Vertical Flag = %d\n",verticalFlag);
			break;
		case GLUT_KEY_F3:
			panSpeed *= 2;
			break;
		case GLUT_KEY_F7:
			viewMode = FRONT;
			break;
		case GLUT_KEY_F8:
			viewMode = SIDE;
			break;
		case GLUT_KEY_F9:
			viewMode = TOP;
			break;
		case GLUT_KEY_F10:
			reverseEye *= -1.0;
			break;
		case GLUT_KEY_F11:
			orth[viewMode][0] *= 1.5;
			orth[viewMode][1] *= 1.5;
			orth[viewMode][2] *= 1.5;
			orth[viewMode][3] *= 1.5;
			orth[viewMode][4] *= 1.5;
			orth[viewMode][5] *= 1.5;
			break;
		case GLUT_KEY_F12:
			orth[viewMode][0] *= 0.5;
			orth[viewMode][1] *= 0.5;
			orth[viewMode][2] *= 0.5;
			orth[viewMode][3] *= 0.5;
			orth[viewMode][4] *= 0.5;
			orth[viewMode][5] *= 0.5;
			break;
		default:
			break;
    }
    glutPostRedisplay();
}		

/*
 *----------------------------------------------------------
 *	winKbd
 *	This function handles keyboard strokes
 *	key:	Key pressed by user
 *	x:	X-position of mouse
 *	y:	Y-position of mouse
 *----------------------------------------------------------
 */

/*Modified by Fabiane Queiroz in 11/22/09*/
static void winKbd( unsigned char key, int x, int y )
{
	switch( key ){
		case 'a':
		case 'A':
			if ( parametOKFlag ) morphedFlag = TRUE;
			else{
				fprintf( stderr, "\tYou have first to parametrize the model!\n");
				beep();
				return;
			}
			morphOneStep(TRUE);
			break;
		case 'c':
		case 'C':
			cellPicking = !cellPicking;
			if ( cellPicking ) glutSetCursor( GLUT_CURSOR_INFO );
			else  glutSetCursor( GLUT_CURSOR_LEFT_ARROW );
			break;
		case 'l':
		case 'L':
			if ( parametOKFlag ) morphedFlag = TRUE;
			else{
				fprintf( stderr, "\tYou have first to parametrize the model!\n");
				beep();
				return;
			}
			morphOneStep(FALSE);
			break;
		case 'm':
		case 'M':
			initPatFormAndGrowth();
			/* patFormAndGrowthSimulation() */
			break;
		case 'o':
		case 'O':
			/* This outputs a file with all the vertices in 2D
			 which define a face and all its neighbors. The function
			 is defined in forces.c. '15' is the face number we want
			 the vertices information */
			planarizeFace( 15 );
			break;
		case 'p':
		case 'P':
			polygonPicking = !polygonPicking;
			if ( polygonPicking ) glutSetCursor( GLUT_CURSOR_INFO );
			else  glutSetCursor( GLUT_CURSOR_LEFT_ARROW );
			break;
		case 'q':			/*  Quit program */
		case 'Q':
		case '\x1b':
			printString( 0, "\n\tBye, Bye! \n");
			exit( 0 );
		case 'r':
		case 'R':			/* rotation mode */
			modelMode = ROT;
			break;
		case 's':
		case 'S':			/* scaling mode */
			modelMode = SCALE;
			break;
		case 't':
		case 'T':			/* translate mode */
			modelMode = TRANS;
			break;	
			// case 'x':
			// case 'X':
		case 'z':
		case 'Z':			/* reset values */			
			modelMode = TRANS;
			InitWorldTransf();
			break;
			/* Following lines add by Cadu in 19/09/01 (until the '***************') */
		case 'y':
		case 'Y':
			zoomflag = FALSE;
			rotateflag = TRUE;			
			break;
			//case 'z':
			// case 'Z':
		case 'x':
		case 'X':			
			zoomflag = TRUE;
			rotateflag = FALSE;
			break;
			/****************************/
		case '\040':		/* space */
			/* activePrim = WORLD; */
			viewMode = PERSPECTIVE;
			break;
		case 'v':
		case 'V': {
			if (WithInterpolationType < 3)
			{WithInterpolationType++;}
			else {WithInterpolationType = 0;}
			SetaTipoRBF();
			glutPostRedisplay();
		}
			break;
		default:
			break;
	}
	glutPostRedisplay();
	
}

/*
 *-------------------------------------------------------------------
 *	winReshape
 *	This function resets the viewport to cover the entire window
 *	w:	Current width of window
 *	h:	Current height of window
 *-------------------------------------------------------------------
 */

static void winReshape( int w, int h )
{
	myOpenGLReshape( w, h );
	
	winWidth = w;
	winHeight = h;
	if(frameBuf) free(frameBuf);
	frameBuf = (GLubyte*) malloc(sizeof(GLubyte)*winWidth*winHeight*3);
	
	bmpFileHeader.bfType = 'MB';
	bmpFileHeader.bfSize = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER ) + (winWidth * winHeight * 3);
	bmpFileHeader.bfReserved1 = bmpFileHeader.bfReserved2 = 0;
	bmpFileHeader.bfOffBits = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER ) - 2;
	
 	bmpInfoHeader.biSize = sizeof( BITMAPINFOHEADER );
	bmpInfoHeader.biWidth = winWidth;
	bmpInfoHeader.biHeight = winHeight;
	bmpInfoHeader.biPlanes = 1;
	bmpInfoHeader.biBitCount = 24;
	bmpInfoHeader.biCompression = 0; // BI_RGB
	bmpInfoHeader.biSizeImage = 0;
	bmpInfoHeader.biXPelsPerMeter = 0;
	bmpInfoHeader.biYPelsPerMeter = 0;
	bmpInfoHeader.biClrUsed = 0;
	bmpInfoHeader.biClrImportant = 0;
}

/*
 *----------------------------------------------------------
 *	ConvertIntString
 *	This function create a sequencial name to save bitmaps with
 *  the actual scene.
 *----------------------------------------------------------
 */
void ConvertIntString(char* str, int value)
{
	int i=0;
	int temp = 0;
	char first;
    while(value > 0)
    {
		str[i] = (char)(value % 10) + 48;
		value = value / 10;
		i++;
    }
	str[i] = '\0';
	first = str[0];
	i--;
	while (i > 0)
	{
		str[temp] = str[i];
		temp++;
		i--;
	} 
	str[temp] = first;
	
	if (str[2] == '\0')
	{
		str[3] = '\0';
		str[2] = str[1];
		str[1] = str[0];
		str[0] = '0';
	}
	else {
		if (str[1] == '\0')
		{
			str[3] = '\0';
			str[2] = str[0];
			str[1] = '0';
			str[0] = '0'; 
		}
	}
}

/*
 *----------------------------------------------------------
 *	WriteBMP
 *	This function capture a bitmap from Buffer and save with .bmp
 *  Gera um arquivo BMP 24BPP a partir de uma imagem 8BPP na memoria
 *----------------------------------------------------------
 */
int WriteBMP(char *strFileName, unsigned char *pData, int iWidth, int iHeight)
{
	FILE *fStream = NULL;
	long c;
	unsigned char ch;
	char teste[10], Aux[10] = "img";
	
	ConvertIntString(teste,screenFileName);
	screenFileName++;
	
	// Create file
	fStream = fopen( strcat(strcat(Aux,teste),strFileName), "wb" );
	if( fStream == NULL )
		return 0;
	
	// Write file header
	fwrite( &bmpFileHeader.bfType, sizeof(unsigned short), 1, fStream );
	fwrite( &bmpFileHeader.bfSize, sizeof(unsigned long), 1, fStream );
	fwrite( &bmpFileHeader.bfReserved1, sizeof(unsigned short), 1, fStream );
	fwrite( &bmpFileHeader.bfReserved2, sizeof(unsigned short), 1, fStream );
	fwrite( &bmpFileHeader.bfOffBits, sizeof(unsigned long), 1, fStream );
	
	// Write file info
	fwrite( &bmpInfoHeader.biSize, sizeof(unsigned long), 1, fStream );
	fwrite( &bmpInfoHeader.biWidth, sizeof(long), 1, fStream );
	fwrite( &bmpInfoHeader.biHeight, sizeof(long), 1, fStream );
	fwrite( &bmpInfoHeader.biPlanes, sizeof(unsigned short), 1, fStream );
	fwrite( &bmpInfoHeader.biBitCount, sizeof(unsigned short), 1, fStream );
	fwrite( &bmpInfoHeader.biCompression, sizeof(unsigned long), 1, fStream );
	fwrite( &bmpInfoHeader.biSizeImage, sizeof(unsigned long), 1, fStream );
	fwrite( &bmpInfoHeader.biXPelsPerMeter, sizeof(long), 1, fStream );
	fwrite( &bmpInfoHeader.biYPelsPerMeter, sizeof(long), 1, fStream );
	fwrite( &bmpInfoHeader.biClrUsed, sizeof(unsigned long), 1, fStream );
	fwrite( &bmpInfoHeader.biClrImportant, sizeof(unsigned long), 1, fStream );
	
	// Write BMP data
	for( c = 0; c < (iWidth * iHeight * 3); c+=3 )
	{
		ch = pData[c];
		pData[c] = pData[c+2]; 
		pData[c+2] = ch; 
	}
	
	fwrite(pData, sizeof( unsigned char ), (iWidth * iHeight * 3), fStream );
	//fwrite(imageData, 1, bitmapInfoHeader.biSizeImage, filePtr); 
	
	fclose( fStream );
	
	return 1;
}

/*
 *----------------------------------------------------------
 *	winDisplay
 *	This function handles window redisplay requests
 *----------------------------------------------------------
 */
void winDisplay(void)
{
	myOpenGLDisplay();
	glutSwapBuffers();
	
	/* Tira o coment?rio para poder gravar a imagem, vai ficar assim por motivos de desenpenho.
	 Contador++; //Mais um swap ocorreu.
	 if (Contador == 5) //A cada 5 swaps eu gravo um bmp.
	 {
	 glReadPixels(0, 0, winWidth, winHeight, GL_RGB, GL_UNSIGNED_BYTE, (GLubyte*) frameBuf); //Le do buffer
	 WriteBMP(".bmp", frameBuf, winWidth, winHeight); //Grava
	 Contador = 0; //Inicia o ciclo.
	 }
	 */
}

/*
 *-----------------------------------------------------------------
 * Save the GL window into an SGI RGB file
 * by calling "scrsave"
 *-----------------------------------------------------------------
 */
/*void outputViewWindowAsRGB (const char *file_name)
{
	char gumby [400];
	long x2, y2, xorig, yorig, xsize, ysize;
	
	glutSetWindow(winId[0]);
	
	xsize = glutGet( GLUT_WINDOW_WIDTH );
	ysize = glutGet( GLUT_WINDOW_HEIGHT );
	xorig = glutGet( GLUT_WINDOW_X );
	yorig = glutGet( GLUT_SCREEN_HEIGHT ) - glutGet( GLUT_WINDOW_Y) - ysize;
	
	x2 = xorig + xsize - 1;
	y2 = yorig + ysize - 1;
	sprintf (gumby, "scrsave %s %d %d %d %d", file_name, xorig, x2, yorig, y2);
	system (gumby);
}*/


/* Added by Thompson at 10/12/2003 */
void capturePoints(int x, int y, float coords[])
{
	GLint viewport[4];
	GLdouble mvmatrix[16];
	GLdouble projmatrix[16];
	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;
	
	glPushMatrix();
	myPrimTransf(WORLD);
	
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
	glPopMatrix();
	
	winX = (float)x;
	winY = (float)viewport[3] - (float)y;
	glReadPixels(x, (int) winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
	gluUnProject(winX, winY, winZ, mvmatrix, projmatrix, viewport, &posX, &posY, &posZ);
	
	coords[0] = posX;
	coords[1] = posY;
	coords[2] = posZ;
	
   	//fprintf( stderr, "\t Coordinates at cursor are (%4d, %4d)\n", x, y );
   	//fprintf( stderr, "\t World coords at z=%f are (%f, %f, %f)\n", winZ, posX, posY, posZ);
}

