/*
 *------------------------------------------------------------
 *	util.c
 *	Marcelo Walter
 *
 *	This file contains useful general routines
 *------------------------------------------------------------
 */

#include "genericUtil.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "../data/Globals.h"

#include "../data/cellsList.h"
#include "../data/Matrix4.h"
#include "../simulation/simulation.h"
#include "../simulation/relax.h"
#include "transformations.h"


#ifdef GRAPHICS
#include "GL/glut.h"
#include "../viewer/graph.h"
#endif

/*
 *----------------------------------------------------------
 * 	Read an experiment file, ie, a file with the
 *	definition of position for all cells and extension
 *	'cm'
 *----------------------------------------------------------
 */
void readExpFile( const char *fileName )
{
	
	int result;
	
	result = loadCellsFile( fileName );
	if ( result ){
		NumberCells = result;
		cellsCreated = TRUE;
		
#ifdef GRAPHICS
		drawCells = TRUE;
#endif
		compRadiusRepulsion( TRUE );
		
		// post the first round of splitting events on the queue
		postSplitEventsOnQueue();
		// help message for the user
		printf("==========================\n");
		printf("\tExperiment file %s\n", fileName );
	}
	else{
		errorMsg("Could not load cells file!");
	}
}

/*
 *----------------------------------------------------------
 *----------------------------------------------------------
 */
int* myIntMalloc( int quantity )
{
	
	int *tmp;
	
	tmp = (int *) malloc( sizeof (int) * quantity );
	if ( tmp == NULL ){
		errorMsg( "Out of memory space in util.c (int)!");
		return NULL;
	}else
		return ( tmp );
}


/*
 *-----------------------------------------------------------------
 *	reads a line from a file specified by 'fp'
 *	code from C by K&R
 *-----------------------------------------------------------------
 */
int Getline(char s[], int lim, FILE *fp)
{
	int c,i;
	
	for(i=0; i < lim-1 && (c=getc(fp)) != EOF && c != '\n'; ++i)
		s[i] = c;
	if (c == '\n'){
		s[i] = c;
		++i;
	}
	s[i] = '\0';
	return(i);
}

/*
 *----------------------------------------------------------
 * converts a matrix into OpenGL format
 *----------------------------------------------------------
 */
void toOpenGL(Matrix4 m, float *glm)
{
	int i, j;
	
	/* transform internal matrix format into GL format */
	for(i=0; i < 4; i++){
		for(j=0; j < 4; j++){
			glm[4*i+j] = m.element[j][i];
		}
	}
}


/*
 *----------------------------------------------------------
 * If the received flag is TRUE,  make flag FALSE
 * and vice-versa
 *----------------------------------------------------------
 */
void switchFlag(byte *flag)
{
	if (*flag == TRUE) *flag = FALSE;
	else  *flag = TRUE;
}


/*
 *------------------------------------------------------------
 *------------------------------------------------------------
 */
void minMax(Point3D vert, Point3D *max, Point3D *min)
{
	if (vert.x > max->x ) max->x = vert.x;
	else if ( vert.x < min->x ) min->x = vert.x;
	if (vert.y > max->y ) max->y = vert.y;
	else if ( vert.y < min->y ) min->y = vert.y;
	if (vert.z > max->z ) max->z = vert.z;
	else if ( vert.z < min->z ) min->z = vert.z;
}
/*
 *------------------------------------------------------------
 *------------------------------------------------------------
 */
void assignValue(float v[], float value)
{
	v[X] = value;
	v[Y] = value;
	v[Z] = value;
}

/*
 *------------------------------------------------------------
 *------------------------------------------------------------
 */
void multiplyValue(float v[], float value)
{
	v[X] *= value;
	v[Y] *= value;
	v[Z] *= value;
}
/*
 *------------------------------------------------------------
 *------------------------------------------------------------
 */
void assignValueVector(float v[], float value[XYZ])
{
	v[X] = value[X];
	v[Y] = value[Y];
	v[Z] = value[Z];
}

/*
 *----------------------------------------------------------
 *       deg2rad
 *----------------------------------------------------------
 */
float deg2rad(float angle)
{
	return( angle * PI / 180.0 );
}

/*
 *----------------------------------------------------------
 *       rad2deg
 *----------------------------------------------------------
 */
float rad2deg(float angle)
{
	return( angle * 180.0 / PI );
}


/*
 *-------------------------------------------------------------
 * 	Allocates space for a 2D array of chars
 *
 *	INPUT: r - number of rows
 *	       c - number of columns
 *-------------------------------------------------------------
 */
char **allocate2DArrayChar( int r, int c )
{
	char **tmp;
	int i;
	
	tmp = (char **)malloc(r * sizeof(char *));
	for(i=0; i < r; i++){	
		tmp[i] = (char *)malloc(c * sizeof (char ));
	}
	if ( tmp == NULL )
		errorMsg( "No space for array of chars in allocate2DArrayChar!\n");
	
	return( tmp );
}
/*
 *-------------------------------------------------------------
 * 	Allocates space for a 2D array of chars
 *
 *	INPUT: r - number of rows
 *	       c - number of columns
 *-------------------------------------------------------------
 */
byte **allocate2DArrayByte( int r, int c )
{
	byte **tmp;
	int i;
	
	tmp = (byte **)malloc(r * sizeof(byte *));
	for(i=0; i < r; i++){	
		tmp[i] = (byte *)malloc(c * sizeof (byte));
	}
	if ( tmp == NULL )
		errorMsg( "No space for array of chars in allocate2DArrayByte!\n");
	
	return( tmp );
}

/*
 *-------------------------------------------------------------
 * 	Allocates space for a 2D array of ints
 *
 *	INPUT: r - number of rows
 *	       c - number of columns
 *-------------------------------------------------------------
 */
int **allocate2DArrayInt( int r, int c )
{
	int **tmp;
	int i;
	
	tmp = (int **)malloc(r * sizeof(int *));
	for(i=0; i < r; i++){	
		tmp[i] = (int *)malloc(c * sizeof (int));
	}
	if ( tmp == NULL )
		errorMsg( "No space for array of ints in allocate2DArrayInt!\n");
	
	return( tmp );
}

/*
 *-------------------------------------------------------------
 * 	Allocates space for a 2D array of floats
 *
 *	INPUT: r - number of rows
 *	       c - number of columns
 *-------------------------------------------------------------
 */
float **allocate2DArrayFloat( int r, int c )
{
	float **tmp;
	int i;
	
	tmp = (float **)malloc(r * sizeof(float *));
	for(i=0; i < r; i++){
		tmp[i] = (float *)malloc(c * sizeof(float));
	}
	if ( tmp == NULL )
		errorMsg( "No space for array of floats in allocate2DArray!\n");
	
	return( tmp );
}

/*
 *-----------------------------------------------------------------
 *	
 *-----------------------------------------------------------------
 */
float computeGlobalScale( int index, float max )
{
	float globalScale, l1;
	int i;
	Point3D v1, v2;
	Matrix4 transfMatrix;
	
	/*printf("Maximum from growth set = %f Index = %d\n",
	 max, index); */
	
	for(i=1; i < NumberPrimitives; i++){
		if (Prim[i].type == LANDMARK){
			if (Prim[i].gIndex == index){
				V3Assign( &v1, Prim[i].q1);
				V3Assign( &v2, Prim[i].q2);
				buildTransfMatrix(&transfMatrix, i);
				/* bring the points into World coordinates */
				V3PreMul(&v1, &transfMatrix);
				V3PreMul(&v2, &transfMatrix);
				l1 = distance(v1, v2);
				/* 1.1 is arbitrary!! */
				globalScale = max * 1.1 / l1;
				/*printf("Global Scale = %f\n", globalScale);*/
				return globalScale;
			}
		}
	}
	return -1;
}

/*
 *-----------------------------------------------------------------
 *	Prints an error message on stderr and
 *	exits the system
 *-----------------------------------------------------------------
 */
void errorMsg( char error_text[] )
{
	fprintf(stderr, "My Run-time error...\n");
	fprintf(stderr, "%s\n",error_text);
	fprintf(stderr, "...now exiting to system...\n");
	exit(0);
}

/*
 *------------------------------------------------------------
 *	Euclidian distance between 2 points in 3D
 *------------------------------------------------------------
 */
double distance( Point3D p, Point3D q )
{
    return( sqrt( (p.x - q.x) * (p.x - q.x) +
				 (p.y - q.y) * (p.y - q.y) +
				 (p.z - q.z) * (p.z - q.z) ) );
}

/*
 *------------------------------------------------------------
 *------------------------------------------------------------
 */
void beep (void)
{
	fprintf(stderr,"\a"); fflush(stderr);
}


/*
 * From splineInter.c used in Growth.c and Anim.c
 *-------------------------------------------------------------------------
 *	Given the arrays xa[1..n] and ya[1..n] which tabulate a
 *	function (with the xai's in order) and given a value of x,
 *	this routine returns a linear interpolated value y
 *-------------------------------------------------------------------------
 */
void linint(float xa[], float ya[], int n, float x, float *y)
{

	int i;
	float t;

	/* find the range for x */
	i = 1;
	while ( x > xa[i] ) i++;

	if ( i > n ){
		printf("i = %d n = %d\n", i, n );
		printf("x = %f xa = %f\n", x, xa[i]);
		/*comented by Fabiane Queiroz*/
		//nrerror("Bad index in routine LININT");
	}

	if ( i == 1){ /* it's the first element */
		*y = ya[i];
		/* printf("y = %f\n",  *y );
		 printf("y[%d] = %f\n", i, ya[i]);
		 printf("x = %f xa[%d] = %f\n", x, i, xa[i]); */
	}
	else{
		t = ( x - xa[i-1]) / ( xa[i] - xa[i-1] );
		*y = ya[i-1] * ( 1.0 - t) + ya[i] * t;
		/* printf("\n");
		 printf("t = %f y = %f\n", t, *y );
		 printf("y[%d] = %f y[%d] = %f\n", i-1, ya[i-1], i, ya[i]);
		 printf("x = %f xa[%d] = %f xa[%d] = %f\n", x, i-1, xa[i-1], i, xa[i]); */
	}
}
