/*------------------------------------------------------------
 *	graph.c
 *	Marcelo Walter
 *
 *	Modification history:
 *	oct/01/1995 - start
 *	aug/13/1996 - included trackball
 *------------------------------------------------------------
 */
#include <stdlib.h>
#include <string.h>
#include <math.h>

//#include <GLUT/glut.h>
#include <GL/glut.h>
#include "../common.h"

// #include "flags.h" // added by Cadu in 29/10
#include "drawing.h"
//#include "../util/nrutil.h"
#include "trackball.h"
#include "../util/transformations.h"

void drawVoronoi( void );	/* for the picking part */
void initMenus( void );
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
GLubyte checkImage[checkImageWidth][checkImageHeight][3];

/* Local prototypes */
void makeCheckImage(void);
//static void processCellHits(int hits, GLuint buffer[]);

/*
 *------------------------------------------------------------
 *		External global definitions
 *------------------------------------------------------------
 */
extern char sessionFileName[128];

/* computes eingenvalues and eigenvectors for matrix a
 * procedure in jacobi.c
 */
/* void jacobi(float **a, int n, float d[], float **v,int *nrot); */

/* computes the inverse of a matrix of degree 'n'*/
/* void inverseMatrix(float **a, int n, float **y); */

/* minimum and maximum from the set of vertices */
extern Point3D minSet, maxSet;

/* index into the growth data array. Defined in
 * util.c
 */
extern int growthTime;

/* actual growth data array */
extern float **growthData;

/* which primitive is currently active */
extern int activePrim;

/* which mouse button was pressed */
extern flag rotateflag;
extern flag panflag;
extern flag zoomflag;

/* mouse flags specific for the picking part */
extern flag leftPickFlag;
extern flag middlePickFlag;

/* constrain mouse movements to be only vertical or horizontal */
extern flag horizontalFlag;
extern flag verticalFlag;

/* animate or not the model */
extern flag animFlag;

/* grow or not the model. The idea is that sometimes
 I want to only animate the model (such as the horse
 walking) without actually growing it at the same
 time
 */
extern flag growthFlag;

/* displays a "scene": background and floor */
extern flag sceneFlag;

flag mouseMovingFlag = FALSE;

/*
 * A record of where the last mouse position is
 */
extern int oldMouseX, oldMouseY;

extern float reverseEye;

extern flag createVect;
extern flag drawVector;


/* Declared in vectorField.c */
extern int PointPicking;


/*
 *--------------------------------------------------
 *	Local Global variables
 *--------------------------------------------------
 */ 
/* modeling mode. One of Rotation, Translation or
 Scaling */
MMODE modelMode=ROT;

/* viewing mode. One of Perspective, Side, Top or
 Front */
VMODE viewMode=PERSPECTIVE;

/* rendering mode. One of Fill, Wire Frame, Halo,
 backface or frontlines */
RMODE renderMode=FILL;

/* mouse button pressed (LEFT, MIDDLE, RIGHT */
static int mouseButton;

/* window limits of the ortographic projection.
 3 windows (top, front, side) and
 (xmin, xmax, ymin, ymax, zmin, zmax) for
 each window */
float orth[3][6];

/* width and height of the viewport */
GLint WIDTH, HEIGHT;

float colPrim[N_OF_PRIM+1][XYZ];

/* if drawing cylinders using gluNewQuadric */
/*GLUquadric *Cylinder;*/

/* window ids. If using more than one window */
extern int winId[5];

/* animate or not the model */
extern flag animFlag;

/*
 *--------------------------------------------------
 *		various flags
 *--------------------------------------------------
 */
/* splits the window in 4 or not */
flag windowSplitFlag = FALSE;

/* which vertex belongs to which primitive was computed */
flag morphedFlag = FALSE;

/* polygon picking mode active or not */
flag polygonPicking = FALSE;

/* cell picking mode active or not */
flag cellPicking = FALSE;

/* point picking mode active or not */
/*Tira*///Nunca foi usado; flag pointPicking = FALSE;	/* Added by Thompson at 04/12/2003 */

/* uses Open GL lights or not */
flag lightFlag = FALSE;

/* draws the object or not */
flag objectFlag = TRUE;

/* paint the polygons according to the primitives they
 belong */
flag colorPrimFlag = TRUE;

/* draw cells or not */
flag drawCells = FALSE;

/* display the primitives or not */
flag showPrimitives=FALSE;

/* display the landmarks or not */
flag showLandMarks=TRUE;

/* display the axes or not */
flag showAxes=TRUE;

/* display texture or not */
flag textureFlag = FALSE;

/* draws a grid or not */
flag gridFlag=FALSE;

/* mouse speed for panning */
float panSpeed = 1.0;


/* Added by Thompson at 11/12/2003 */
/*Tira*///GLfloat backClipPlane;

/*
 *----------------------------------------------------------
 *	makeCheckImage
 *	Generates the checkerboard texture
 *----------------------------------------------------------
 */
void makeCheckImage(void)
{
	int i, j, c;
    
	for (i = 0; i < checkImageWidth; i++) {
		for (j = 0; j < checkImageHeight; j++) {
			c = ((((i&0x8)==0)^((j&0x8))==0))*255;
			checkImage[i][j][0] = (GLubyte) c;
			checkImage[i][j][1] = (GLubyte) c;
			checkImage[i][j][2] = (GLubyte) c;
		}
	}
}


/*
 *----------------------------------------------------------
 *	initWin
 *	This function initializes the OpenGL window 
 *----------------------------------------------------------
 */
void initWin()
{
	char windowName[128];
	int len;
	
	/* make the window's name */
	strcpy(windowName, "Growth - ");
	len = strlen(sessionFileName);
	/* removes the .session from the string */
	strncat(windowName, sessionFileName, len - 8 );
	winId[0] = glutCreateWindow( windowName );
	
	/*  register Window callbacks */
	initCallBacks();
	
	/* init the lighting information */
	initLightingInfo();
	
	/* create and initialize the menus */
	initMenus();
	
	/* init options related to OpenGL */
	initGLStuff();
	
	/* init texture stuff */
	initTexture();
	
	/* init variables for ortographic projections */
	initOrthProjections();
}
/*
 *-----------------------------------------------------------
 *-----------------------------------------------------------
 */
void initTexture( void )
{
	int i, j;
	int w = checkImageWidth;
	int h = checkImageHeight;
	
	/* if not using the checkerboard texture we have to
     enable the following calls */
	
	/* GLubyte textureImage[256][128][3];
	 
	 GLubyte ***textureImage;
	 Image myimage;
	 myimage.read( "globe1.rgb" );
	 int w = myimage.width();
	 int h = myimage.height();
	 
	 printf("w = %d h = %d\n", w,h);
	 
	 textureImage = (GLubyte ***) malloc( sizeof(GLubyte **) * w);
	 
	 for(i=0; i < w; i++){
	 textureImage[i] =  (GLubyte *) malloc( sizeof(GLubyte *) * h);
	 }
	 for(i=0; i < w; i++){
	 for(j=0; j < h; j++){
	 textureImage[i][j] =  (GLubyte *) malloc( sizeof(GLubyte) * 3);
	 }
	 }*/
	
	/*for (i = 0; i < w; i++) {
	 for (j = 0; j < h; j++) {
	 textureImage[i][j][0] = myimage.pixel(i, j, Image::red);
	 textureImage[i][j][1] = myimage.pixel(i, j, Image::green);
	 textureImage[i][j][2] = myimage.pixel(i, j, Image::blue);
	 }
	 }*/
	
	makeCheckImage();
	
    
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	/* if not using checkerboard */
	/* glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, 
	 &textureImage[0][0][0]); */
	/* filtering scheme */
	/* gluBuild2DMipmaps(GL_TEXTURE_2D, 3, w, h, GL_RGB, GL_UNSIGNED_BYTE,
	 &textureImage[0][0][0]); */
	
	/* using the checkerboard texture */
	glTexImage2D(GL_TEXTURE_2D, 0, 3, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, 
				 &checkImage[0][0][0]);
	/* filtering scheme */
	/* gluBuild2DMipmaps(GL_TEXTURE_2D, 3, w, h, GL_RGB, GL_UNSIGNED_BYTE,
	 &checkImage[0][0][0]); */
	
	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	/* type of Texture Mapping: Decal or Blend */
	/* glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL); */
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
	
	/* tries to correct perspective distortion in the Texture
     Mapping process */
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	/* for environment mapping stuff. If we want the texture to
     be mapped as environment we enable the following
     calls. Don't forget to disable the previous ones */
	/* glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);*/ /* env map. */
	/* glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	 glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	 glEnable(GL_TEXTURE_GEN_S);
	 glEnable(GL_TEXTURE_GEN_T);*/
}

/*
 *-----------------------------------------------------
 *	Init variables and options related to OpenGL
 *	- dithering is OFF
 *	- background clear color
 *	- enables Z-buffer
 *	- builds various display lists
 *-----------------------------------------------------
 */
void initGLStuff( void )
{
	/* ensure proper Z-buffering */
	glEnable( GL_DEPTH_TEST );
	glDepthFunc(GL_LEQUAL);
	
	glShadeModel(GL_SMOOTH); 
	//glShadeModel(GL_FLAT); 
	
	/*Default background colour */
	/*glClearColor( 0.7, 0.7, 0.7, 0 );*/
	glClearColor( 0.9, 0.9, 0.9, 0 ); 
	/*glClearColor( 1, 1, 1, 0 );*/
	
	/* enables normalization of normal vectors */
	/* glEnable(GL_NORMALIZE); */
	
	/* leave TRUE color. Dithering is the default option */
	/* glDisable(GL_DITHER); */
	
	/* remove back faces to speed things up
     Only used when rendering filled */
	glCullFace(GL_BACK);
    
	/* builds all display lists */
	buildDisplayLists();
	
	/* this creates a new quadric which is
     a cylinder */
	/*Cylinder = gluNewQuadric();*/
}

/*
 *----------------------------------------------------------
 *	Builds all display lists
 *----------------------------------------------------------
 */
void buildDisplayLists( void )
{
	/* this builds a display list to draw circles.
	 * It's more efficient than having a separate
	 * routine to do it
	 */
	buildCircle();
	
	/* builds a display list to display the small axes */
	buildAxes();
	
	/* builds a display list to display the lines
     separating the 4 screens */
	buildSeparatingLines();
	
	/* builds a display list to display a grid */
	buildGrid();
	
}

/*
 *----------------------------------------------------------
 *
 *----------------------------------------------------------
 */
void InitWorldTransf(void)
{
	/* assigns the initial rotation */
	assignValue(Prim[WORLD].rot, 0.0);
	Prim[WORLD].rot[W] = 1.0;
	
	/* assigns the initial scaling */
	assignValue(Prim[WORLD].scale, 1.0);
	
	/* assigns the initial translation */
	assignValue(Prim[WORLD].trans, 0.0);
	
	/* sets the initial translation of the camera away from origin */
	if (minSet.z < 0) Prim[WORLD].trans[Z] = minSet.z*2.0;
	else Prim[WORLD].trans[Z] = minSet.z*-2.0;
}

/*
 *-----------------------------------------------------
 *	Init variables used in the orthographic
 *	projections
 *-----------------------------------------------------
 */
void initOrthProjections( void )
{
	Point3D *min, *max;
	double mi, ma;
	
	/*min = V3Multiply( &minSet, 2.0);
	 max = V3Multiply( &maxSet, 2.0);*/
	
	if ( minSet.z < minSet.y ) mi = minSet.z;
	else  mi = minSet.y;
	if ( maxSet.z > maxSet.y ) ma = maxSet.z;
	else  ma = maxSet.y;
	
	mi *= 2.0;
	ma *= 2.0;
	
	orth[SIDE][0] = mi;
	orth[SIDE][1] = ma;
	orth[SIDE][2] = mi;
	orth[SIDE][3] = ma;
	orth[SIDE][4] = 0.0;
	orth[SIDE][5] = 300.0;
	
	orth[TOP][0] = mi;
	orth[TOP][1] = ma;
	orth[TOP][2] = mi;
	orth[TOP][3] = ma;
	orth[TOP][4] = 0.0;
	orth[TOP][5] = 300.0;
	
	orth[FRONT][0] = mi;
	orth[FRONT][1] = ma;
	orth[FRONT][2] = mi;
	orth[FRONT][3] = ma;
	orth[FRONT][4] = 0.0;
	orth[FRONT][5] = 300.0;
	
	/*orth[SIDE][0] = min->z;
	 orth[SIDE][1] = max->z;
	 orth[SIDE][2] = min->y;
	 orth[SIDE][3] = max->y;
	 orth[SIDE][4] = 0.0;
	 orth[SIDE][5] = 300.0;
	 
	 orth[TOP][0] = min->z;
	 orth[TOP][1] = max->z;
	 orth[TOP][2] = min->y;
	 orth[TOP][3] = max->y;
	 orth[TOP][4] = 0.0;
	 orth[TOP][5] = 300.0;
	 
	 orth[FRONT][0] = min->z;
	 orth[FRONT][1] = max->z;
	 orth[FRONT][2] = min->y;
	 orth[FRONT][3] = max->y;
	 orth[FRONT][4] = 0.0;
	 orth[FRONT][5] = 300.0;*/
	
}


/*
 *-----------------------------------------------------
 *	Initialize all information related to lighting
 *-----------------------------------------------------
 */
void initLightingInfo( void )
{
	float  mat_amb[] = { 0.2, 0.2, 0.2, 1.0 };
	float  mat_dif[] = { 0.8, 0.1, 0.1, 1.0 };
	float  mat_spc[] = { 0.0, 0.0, 0.0, 1.0 }; /*0.9*/
	float  mat_shi[] = { 128.0 };
	float  lgt_amb[] = { 0.5, 0.5, 0.5, 1.0 };
	float  lgt_dif[] = { 0.9, 0.9, 0.9, 1.0 };
	/* 0.0 at the end means it's a directional light */
	float  lgt_pos[] = { 0.0, 1.0, 1.0, 0.0 };
	
	glMaterialfv( GL_FRONT, GL_AMBIENT, mat_amb );
	glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_dif );
	glMaterialfv( GL_FRONT, GL_SHININESS, mat_shi );
	glMaterialfv( GL_FRONT, GL_SPECULAR, mat_spc );
	
	glLightfv( GL_LIGHT0, GL_AMBIENT, lgt_amb );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, lgt_dif );
	/* somehow the default light position is fine... */
	glLightfv( GL_LIGHT0, GL_POSITION, lgt_pos );
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);	
}

/*
 *--------------------------------------------------------------
 *	Map mouse click (sx, sy) to a more convenient (-1.0, 1.0)
 *	range, based on window size
 *--------------------------------------------------------------
 */
void mouse2worldspace(int sx, int sy, float *xx, float *yy)
{
	
	(*xx) = (2.0 * sx) / (float) WIDTH - 1.0;
	(*yy) = (2.0 * (HEIGHT - sy)) / (float) HEIGHT - 1.0;
	
}

/*
 *--------------------------------------------------------------
 *	Auxiliary routine for winMotion below
 *	INPUT: - current mouse positions
 *--------------------------------------------------------------
 */
void handleTrackball(int nmx, int nmy)
{
	
	if ( zoomflag ){
		float t[XYZ];
		
		zoom(nmx, nmy, t);
		vadd( t, Prim[WORLD].trans, Prim[WORLD].trans);
	}
	else if ( panflag ){
		float t[XYZ];
	    
		pan( nmx, nmy, t );
		vadd( t, Prim[WORLD].trans, Prim[WORLD].trans);
	}
	else if ( rotateflag ){
		float r[XYZW];
		float p1x, p1y, p2x, p2y;
		
		assignValue( r, 0.0); r[W] = 1.0;
		
		mouse2worldspace(oldMouseX, oldMouseY, &p1x, &p1y);
		mouse2worldspace(nmx, nmy, &p2x, &p2y);
		trackball(r, p1x, p1y, p2x, p2y);
		
		add_quats(r,  Prim[WORLD].rot, Prim[WORLD].rot);
	}
}
/*
 *---------------------------------------------------------
 *	Zoom is activated by pressing the left AND
 *	MIDDLE mouse buttons together
 *	nmx -> current x mouse position
 *	nmy -> current y mouse position
 *---------------------------------------------------------
 */
void zoom(int nmx, int nmy, float t[])
{
	
	GLint viewport[4];
	
	glGetIntegerv (GL_VIEWPORT, viewport);
	//t[X] = 0.0;
	t[Y] = 0.0;
	//t[Z] = panSpeed * 40.0 * ((float)(nmx-oldMouseX)/(float)viewport[2] + (float)(oldMouseY-nmy)/(float)viewport[3]);
	t[X] = panSpeed * 40.0 * ((float)(nmx-oldMouseX)/(float)viewport[2] + (float)(oldMouseY-nmy)/(float)viewport[3]);
	t[Z] = 0.0;
}

/*
 *---------------------------------------------------------
 *	Pan the object around in the screen
 *	nmx -> current x mouse position
 *	nmy -> current y mouse position
 *	Pan mode is activated with the middle mouse
 *	button
 *---------------------------------------------------------
 */
void pan(int nmx, int nmy, float t[])
{
	
	GLint viewport[4];
	
	glGetIntegerv (GL_VIEWPORT, viewport);
	/* the number 30 is a scale factor to account for
	 * the object's size. It has to be adjusted depending
	 * on the object's and window size
	 * 30 is just a good number
	 */
	if (horizontalFlag){
		//t[X] = panSpeed * 30.0 * (float)(nmx-oldMouseX)/(float)viewport[2];
		t[Z] = panSpeed * 30.0 * (float)(nmx-oldMouseX)/(float)viewport[2];
		t[Y] = 0.0;
	}
	else if (verticalFlag){
		//t[X] = 0.0;
		t[Z] = 0.0;
		t[Y] = panSpeed * 30.0 * (float)(oldMouseY-nmy)/(float)viewport[3];
	}
	else{
		// t[X] = panSpeed * 30.0 * (float)(nmx-oldMouseX)/(float)viewport[2];
		t[Z] = panSpeed * 30.0 * (float)(nmx-oldMouseX)/(float)viewport[2];
		t[Y] = panSpeed * 30.0 * (float)(oldMouseY-nmy)/(float)viewport[3];
		
	}
	//t[Z] = 0.0;
	t[X] = 0.0;
}	
/*
 *-------------------------------------------------------------
 *	process cells hits
 *-------------------------------------------------------------
 */
static void processCellHits(int hits, GLuint buffer[])
{
	
	unsigned int i, j, closestZ, zmin, zmax;
	int names, *ptr, closestCell, closestFace, faceIndex;
	
	fprintf( stderr, "Got into check cells hits with %d hits\n", hits);
	
	/* set the z values */
	//closestZ = (unsigned int) 1.0*2e32;
	closestZ = 429496729;
	//zmin = (unsigned int) 1.0*2e32;
	zmin = 429496729;
	/*fprintf( stderr, "closestZ = %u\n", closestZ );*/
	
	ptr = (int *) buffer;
	
	for(i = 0; i < hits; i++){
		names = *ptr;
		if ( names != 0 ){
			ptr++;
			zmin = *ptr;
			ptr++;
			zmax = *ptr;
			ptr++;
			faceIndex = *ptr; 
			fprintf( stderr, "Names = %d zmin = %u zmax = %u faceIndex = %d\n",
					names, zmin, zmax, faceIndex ); 
			if ( names == 2 ){
				if ( zmin < closestZ ){
					closestZ = zmin;
					closestFace = faceIndex;
					ptr++;
					closestCell = *ptr;
					ptr++;
					fprintf( stderr, "Closest Cell = %d Closest Face = %d \n",
							closestCell, closestFace ); 
				}
				else{
					ptr++;
					/* added by Thompson in 22/05/2002 */
					closestCell = *ptr;  
					ptr++;
					fprintf( stderr, "Closest Cell = %d Closest Face = %d \n",
							closestCell, faceIndex );
				}
			}
			else{
				for(j=0; j < names; j++){
					//ptr++;
					fprintf(stderr, "Closest Cell -> %d ",*ptr); 
					ptr++;
				}
				fprintf( stderr, "\n" );
			}
		}
	}
	/* added by Thompson in 22/05/2002 */
	closestFace = faceIndex;
	
	fprintf( stderr, "Closest Cell %d on face %d\n",
			closestCell, closestFace ); 
	
	/* left mouse "picks"; middle mouse "unpicks" */
	if (leftPickFlag == TRUE){
		fprintf( stderr, "Cell type     %d on ",
				faces[closestFace].vorFacesList[closestCell].site->ctype );
		fprintf( stderr, "%lg %lg %lg\n",
				faces[closestFace].vorFacesList[closestCell].site->x,
				faces[closestFace].vorFacesList[closestCell].site->y,
				faces[closestFace].vorFacesList[closestCell].site->z ); 
		switchCellType(faces[closestFace].vorFacesList[closestCell].site, C);
		fprintf( stderr, "New cell type %d on ",
				faces[closestFace].vorFacesList[closestCell].site->ctype );
		fprintf( stderr, "%lg %lg %lg\n\n",
				faces[closestFace].vorFacesList[closestCell].site->x,
				faces[closestFace].vorFacesList[closestCell].site->y,
				faces[closestFace].vorFacesList[closestCell].site->z ); 
	}		
	if (middlePickFlag == TRUE){
		fprintf( stderr, "Cell type     %d on ",
				faces[closestFace].vorFacesList[closestCell].site->ctype );
		fprintf( stderr, "%lg %lg %lg\n",
				faces[closestFace].vorFacesList[closestCell].site->x,
				faces[closestFace].vorFacesList[closestCell].site->y,
				faces[closestFace].vorFacesList[closestCell].site->z ); 
		switchCellType(faces[closestFace].vorFacesList[closestCell].site, D);
		fprintf( stderr, "New cell type %d on ",
				faces[closestFace].vorFacesList[closestCell].site->ctype );
		fprintf( stderr, "%lg %lg %lg\n\n",
				faces[closestFace].vorFacesList[closestCell].site->x,
				faces[closestFace].vorFacesList[closestCell].site->y,
				faces[closestFace].vorFacesList[closestCell].site->z ); 
	}
	
	glutPostRedisplay();
}


/*
 *-------------------------------------------------------------
 *	process polygon hits
 *-------------------------------------------------------------
 */
static void processPolygonHits(int hits, GLuint buffer[]) /*Redone in 22/07/2004 by Vin?cius*/
{	
	unsigned int i, j, k, closest;
	int names, *ptr, closestIndex; /*closestIndex have the index of face that we hit*/
	
	ptr = (int *) buffer; /*Buffer have 4 parameters
	 1 - Num of names (I dont no what is that)
	 2 - Min Z of all vertex of face.
	 3 - Max Z of all vertex of face.
	 4 - Index of face on the stack.
	 */
	/*Set the first one how the bigest Z*/
	names = *ptr; 		/*Num of names*/
	ptr++;				/*Put on the Field Min Z*/
	ptr++;				/*Put on the Field Max Z*/
	closest = *ptr; 		/*Set actual how the bigest face*/
	ptr++;				/*Put on the Field Index Face*/
	closestIndex = *ptr;  /*Set the index of the face the bigest face*/
	for (j = 0; j < names; j++)
	{
		ptr++;
	}
	
	for (i = 1; i < hits; i++) {/* for each hit */
        names = *ptr;
		ptr++;
		ptr++;
		if (*ptr < closest)  /*Test if the actual face have the bigest z*/
		{
			closest = *ptr;
			ptr++;
			closestIndex = *ptr;
			for (j = 0; j < names; j++)
			{      /* for each name */
				ptr++;
			}
		}
		else {
			ptr++;;
			for (j = 0; j < names; j++)
			{      /* for each name */
				ptr++;
			}
		}
    }
	
  	/* Thompson 05/03/2004 */
	if( createVect )
	{
		PointPicking = closestIndex;
		//fprintf( stderr, "\n\t init face index = %d\n", PointPicking );
	}
	
	/* left mouse "picks"; middle mouse "unpicks" */
	if (leftPickFlag == TRUE && !createVect ){
		faces[closestIndex].hit = TRUE;
		for(k=0; k < faces[closestIndex].nverts; k++)
		{
			vert[faces[closestIndex].v[k]].hit = TRUE;
			fprintf( stderr, "vertex ( %f  %f  %f ) \n", vert[faces[closestIndex].v[k]].pos.x,
					vert[faces[closestIndex].v[k]].pos.y,
					vert[faces[closestIndex].v[k]].pos.z );
		}
	}		
	
	if (middlePickFlag == TRUE && !createVect ){
		faces[closestIndex].hit = FALSE;
		for(k=0; k < faces[closestIndex].nverts; k++)
			vert[faces[closestIndex].v[k]].hit = FALSE;
	}	
	glutPostRedisplay();
}


/*
 *-------------------------------------------------------------
 *	Check for Hits
 *-------------------------------------------------------------
 */
void checkHit(int x, int y)
{
	GLuint selectBuf[BUFSIZE];
	GLint hits;
	GLint viewport[4];
	
	glGetIntegerv(GL_VIEWPORT, viewport);	/* get viewport coordinates */
	
	glSelectBuffer(BUFSIZE, selectBuf);
	glRenderMode(GL_SELECT);
	
	glInitNames();
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	
	/* create 10 x 10 pixel picking region near cursor location */
	gluPickMatrix(x, (viewport[3] - y), 10.0, 10.0, viewport); 
	
	/* set the perspective window */
	glMatrixMode( GL_PROJECTION );
	gluPerspective(45.0, (GLfloat) WIDTH / (GLfloat) HEIGHT, FRONT_CLIP_PLANE, BACK_CLIP_PLANE );
	/* Added by Thompson at 11/12/2003 */
	/*Tira*///gluPerspective(45.0, (GLfloat) WIDTH / (GLfloat) HEIGHT, FRONT_CLIP_PLANE, backClipPlane );
	
	/* primTransf(WORLD); */
	myPrimTransf(WORLD);
	
	//if( polygonPicking && !createVect ) 
	if( polygonPicking )
	{
		drawObject();
	}
	else if( cellPicking )
	{
		drawVoronoi();
	}
	glPopMatrix();
	
	glFlush();
	
	glMatrixMode( GL_MODELVIEW );
	
	/*glRenderMode show us how much faces was hit*/
	hits = glRenderMode(GL_RENDER); /* GL_RENDER, GL_SELECT or GL_FEEDBACK */
	//printf("hits = %d\n", hits);
	if (hits > 0)
	{
		if( polygonPicking )
		{
			processPolygonHits(hits, selectBuf); /*With hit and the buffer we can make a test to see with one is the correct face*/
		}
		else if( cellPicking )
		{
			processCellHits(hits, selectBuf);
		}
	}
	else if (createVect)
	{
		PointPicking = -1;
	}
}

/*
 *----------------------------------------------------------
 *	Apply transformations to a particular primitive 'i'
 * 	This uses the OpenGL calls
 *----------------------------------------------------------
 */
void primTransf(int i)
{
	glTranslatef( Prim[i].trans[X],  Prim[i].trans[Y],  Prim[i].trans[Z] );
    
	glRotatef(  Prim[i].rot[Z], 0.0, 0.0, 1.0 );
	glRotatef(  Prim[i].rot[Y], 0.0, 1.0, 0.0 );
	glRotatef(  Prim[i].rot[X], 1.0, 0.0, 0.0 );
    
	if (i != WORLD)
		glScalef(  Prim[i].scale[X],  Prim[i].scale[Y],  Prim[i].scale[Z] );
}

/*
 *----------------------------------------------------------
 *	Apply transformations to a particular primitive
 *	including the WORLD (or camera) since I am treating
 *	the camera as anotehr primitive
 *
 *	Here I am building the transformation matrix myself
 *	and loading it into the GL_MODELVIEW matrix stack
 *----------------------------------------------------------
 */
void myPrimTransf(int whichPrim)
{
	float myMatrix[16];
	Matrix4 transfMatrix;
	
	/* if the primitive is the WORLD, it defines
     how we are positioning the camera. In this case
     I am using a trackball to position the camera.
     The call "build_rotmatrix" builds a rotation
     matrix based on the quaternion computed from
     the mouse interaction
     */
	if (whichPrim == WORLD){
		glTranslatef( Prim[whichPrim].trans[X],
					 Prim[whichPrim].trans[Y],
					 Prim[whichPrim].trans[Z]);
		build_rotmatrix(&transfMatrix, Prim[whichPrim].rot);
	}
	else{
		/* Basically I don't want the original
		 scaling (given by the user) to interfere with
		 subsequent scalings (derived from the growth
		 information). If the object was parametrized
		 then we want to use the growth data to scale
		 the primitives and therefore we use the
		 matrix transformations without the original
		 scale. If the object was not parameterized
		 then we use the original transformations
		 including the original scaling */
		if ( morphedFlag || computingPatternFlag )
			buildTransfMatrixSpecial(&transfMatrix, whichPrim);
		else
			buildTransfMatrix(&transfMatrix, whichPrim);
		
	}	
	/* transform internal matrix format into OpenGL format */
	toOpenGL( transfMatrix, &myMatrix[0]);
	
	/* multiply top of OpenGL stack matrix by my matrix */
	glMultMatrixf(myMatrix);
}

/*
 *-----------------------------------------------------------
 *	This is the pure OpenGL reshape routine
 *-----------------------------------------------------------
 */
void myOpenGLReshape( int w, int h )
{
	WIDTH = w;
	HEIGHT = h;
	
	/* set the perspective window */
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective(45.0, (GLfloat) w / (GLfloat) h,
				   FRONT_CLIP_PLANE, BACK_CLIP_PLANE );
	
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glViewport( 0, 0, w, h);
}
/*
 *-----------------------------------------------------------
 *	This is the pure OpenGL display routine
 *-----------------------------------------------------------
 */
void myOpenGLDisplay( void )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	/*Add by Fabiane Queiroz at 04/19/2010*/
	glShadeModel(GL_SMOOTH);
	
	/* wants light or not */
	if ( lightFlag ) glEnable( GL_LIGHTING );
	else glDisable( GL_LIGHTING );
	
	glPushMatrix();
	
	/* primTransf(WORLD); */
	myPrimTransf(WORLD);
	
	if(windowSplitFlag){ /* 4 windows */
		/* draws lines separating windows */
		glViewport( 0, 0, WIDTH, HEIGHT);
		glCallList(MY_LINES_LIST);
		
		/* front */
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		auxDisplay(FRONT, 0.0, 0.0, 2.0, 0.0, 1.0, 0.0,
				   0, 0,  WIDTH/2, HEIGHT/2);
		
		/* top */
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		auxDisplay(TOP, 0.0, 2.0, 0.0, 1.0, 0.0, 0.0,
				   WIDTH/2 , 0, WIDTH - WIDTH/2, HEIGHT/2);
		
		/* side */
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		auxDisplay(SIDE, -2.0, 0.0, 0.0, 0.0, 1.0, 0.0,
				   0,  HEIGHT/2, WIDTH/2, HEIGHT-HEIGHT/2);
		
		/* draw the perspective window */
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		gluPerspective(45.0, (GLfloat) WIDTH / (GLfloat) HEIGHT,
					   FRONT_CLIP_PLANE, BACK_CLIP_PLANE );
		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();
		/* set the perspective view */
		glViewport( WIDTH/2 , HEIGHT/2, WIDTH - WIDTH/2, HEIGHT - HEIGHT/2);
		drawAll(modelMode, VNONE, renderMode, growthTime);
		glPopMatrix();
	}
	/*  only one window is displayed
	 *  The type of projection is decided by the variable
	 *  viewMode
	 */
	else{
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		/* check viewing mode */
		switch(viewMode){
			case TOP:
				auxDisplay(TOP, 0.0, reverseEye*2.0, 0.0,
						   1.0, 0.0, 0.0, 0, 0, WIDTH, HEIGHT);
				break;
			case SIDE:
				auxDisplay(SIDE, reverseEye*-2.0, 0.0, 0.0,
						   0.0, 1.0, 0.0, 0, 0, WIDTH, HEIGHT); 
				break;	
			case FRONT:
				auxDisplay(FRONT, 0.0, 0.0, reverseEye*2.0,
						   0.0, 1.0, 0.0, 0, 0, WIDTH, HEIGHT);
				break;
			case PERSPECTIVE:
				gluPerspective(45.0, (GLfloat) WIDTH / (GLfloat) HEIGHT,
							   FRONT_CLIP_PLANE, BACK_CLIP_PLANE );
				glMatrixMode( GL_MODELVIEW );
				glPushMatrix();
				glViewport( 0, 0, WIDTH, HEIGHT);
				drawAll(modelMode, VNONE, renderMode, growthTime);
				glPopMatrix();
				break;
			default:
				break;
		}
		
	}
	glPopMatrix();
	
	if ( mouseMovingFlag ){
		mouseMovingFlag = FALSE;
		glutPostRedisplay();
	}
	//glFlush();
} 

/*
 *-----------------------------------------------------------
 *	This is an auxiliary routine for myDisplay above
 *-----------------------------------------------------------
 */
void auxDisplay(VMODE vmode, double eyex, double eyey, double eyez,
				double upx, double upy, double upz,
				int x, int y, int w, int h)
{
    
	if ( w <= h ){
		glOrtho(orth[vmode][0], orth[vmode][1],
				orth[vmode][2] * (GLfloat) h / (GLfloat) w,
				orth[vmode][3] * (GLfloat) h / (GLfloat) w,
				orth[vmode][4], orth[vmode][5]);
	}else{
		glOrtho(orth[vmode][0] * (GLfloat) w / (GLfloat) h,
				orth[vmode][1] * (GLfloat) w / (GLfloat) h,
				orth[vmode][2], orth[vmode][3],
				orth[vmode][4], orth[vmode][5]);
	}
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	gluLookAt(eyex, eyey, eyez, 0.0, 0.0, 0.0, upx, upy, upz);
	glViewport(x, y, w, h);
	drawAll(modelMode, vmode, renderMode, growthTime);
	glPopMatrix();
}

/*
 *-------------------------------------------------
 * test to check if the transformation matrices
 * are correct
 *-------------------------------------------------
 */
void test(void)
{
	Matrix4 m1, im1, result;
	Point3D p1, p2;
	
	p1.x = 1.0;
	p1.y = 1.0;
	p1.z = 1.0;
	
	p2.x = 0.0;
	p2.y = 0.0;
	p2.z = 0.0;
	
	loadIdentity(&m1);
	loadIdentity(&im1);
	loadIdentity(&result);
	
	printMatrix(m1);
	
	/* buildTransfMatrix(&m1, 1);
	 m1.element[0][0] = 2.0;
	 m1.element[1][1] = 3.0;
	 m1.element[2][2] = 4.5; */
	MxRotateAxis(p1, p2, 45.0 , &m1, &im1);
	printMatrix(m1);
	printMatrix(im1);
	
	/* MxInvert( &m1, &im1); */
	MatMul(&m1, &im1, &result);
	printMatrix(result);
	
	/* buildInvTransfMatrix(&m1, 1);
	 printMatrix(m2); */
	
}
/*
 *----------------------------------------------------------
 *	This procedure assigns a different color for
 *	each primitive. I can then color the vertices according
 *	to the primitive they belong to
 *----------------------------------------------------------
 */
void initColorArray(void)
{
	int i;
	float step;
	
	step = 1.0 /  NumberPrimitives;
	
	for(i=1; i < NumberPrimitives;i++){
		colPrim[i][X] = 0.5;
		colPrim[i][Y] = i*step;
		colPrim[i][Z] = 0.0;
	}
}
