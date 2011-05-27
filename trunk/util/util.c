/*
 *------------------------------------------------------------
 *	util.c
 *	Marcelo Walter
 *
 *	This file contains useful general routines
 *------------------------------------------------------------
 */

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "../common.h"
#include "util.h"
#include "../simulation/relax.h"
#include "transformations.h"

#ifdef GRAPHICS
//#include <GLUT/glut.h>
#include "GL/glut.h"
//#include "sgi/myforms.h"
#endif

/*
 *------------------------------------------------------------
 *	External function definitions
 *------------------------------------------------------------
 */

/* declared in simulation.c */
extern void postSplitEventsOnQueue( void );

/*
 *------------------------------------------------------------
 *		External global definitions
 *------------------------------------------------------------
 */
extern char sessionFileName[128];
extern char primitivesFileName[128];
extern int activePrim;
extern char sessionFileName[128];
extern flag cellsCreated;
extern flag primFilePresent;

/* keep track of how many cells of each type we have */
extern int nCellsType[HOW_MANY_TYPES];
/*
 *------------------------------------------------------------
 *		Local global definitions
 *------------------------------------------------------------
 */
/*  indexes the growth array. 'growthTime' points to
 *  the current time we want the growth data, eg,
 *  3 months, 18 months and so on
 */
int growthTime;
int FINAL_GROWTH_TIME;

/* growth data array */
float **growthData;
float **originalGrowthData;

/* default growth data array */
char growthDataFileName[128] =
{"/home/pgs/mclone/onca/data/growth/fakeGrowth.data"};

/* animation data array */
float **animData;
float **originalAnimData;
int FINAL_ANIM_TIME;

/* default totalnumber of frames */
int numberOfInBet = 0;

/* default growth data array */
char animDataFileName[128] =
{"/home/pgs/mclone/onca/data/animation/fakeAnim.anim"};

/* how to interpret the values in the animation file,
 either as relative or absolute */
flag absRotValuesFlag=FALSE;

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
		
		/* post the first round of splitting events on the queue */
		postSplitEventsOnQueue();
		/* help message for the user */
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
int*
myIntMalloc( int quantity )
{
	
	int *tmp;
	
	tmp = (int *) malloc( sizeof (int) * quantity );
	if ( tmp == NULL )
		errorMsg( "Out of memory space in util.c (int)!");
	else return ( tmp );
}
/*
 *----------------------------------------------------------
 *	This routine will check if there exists a primitives
 *	file and if so will load it beforehand
 *	Return the number of primitives defined in the
 *	file. Returns TRUE if there is a
 *	primitive file (since I always have WORLD as the
 *	first primitive) and FALSE otherwise
 *----------------------------------------------------------
 */
int checkPrimitivesFile(void)
{
	int n;
	
	n = loadPrimitivesFile( primitivesFileName );
	if( n != -1){
		NumberPrimitives = n;
		primFilePresent = TRUE;	/* set the flag */
		return TRUE;
	}
	else{
		NumberPrimitives = 1;
		fprintf(stderr,"This object has no primitives file associated with it!\n");
		fprintf(stderr,"It just means that there are no primitives defined!\n");
		return FALSE;
	}
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
 *	Computes the matrix elements which define the moment
 *	of Inertia matrix.
 *	This is an auxiliary routine called by "createPrimitive"
 *------------------------------------------------------------
 */
void compMomentInertia(Point3D *diagonal, Point3D *lower, Point3D vert, Point3D centroid )
{
	Point3D square;
	Point3D p;
	
	/* compute coordinates relative to the centroid of volume */
	p.x = vert.x - centroid.x;
	p.y = vert.y - centroid.y;
	p.z = vert.z - centroid.z;
	
	square.x = p.x * p.x;
	square.y = p.y * p.y;
	square.z = p.z * p.z;
    
	diagonal->x += square.y + square.z;
	diagonal->y += square.z + square.x;
	diagonal->z += square.x + square.y;
	lower->x += p.x * p.y;
	lower->y += p.y * p.z;
	lower->z += p.z * p.x;
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
 *----------------------------------------------------------
 *	savePrimitivesFile
 *----------------------------------------------------------
 */
void savePrimitivesFile( char *outFile, int nPrim )
{
	FILE *fp;
	int i;
	
	/* change cursor to a watch */
#ifdef GRAPHICS
	glutSetCursor( GLUT_CURSOR_WAIT );
#endif
	
	if( (fp = fopen(outFile,"w")) == NULL)
		errorMsg("Could not open primitives file to write!");
	
	fprintf(fp, "%s\n", growthDataFileName);
	fprintf(fp, "%s\n", animDataFileName);
	fprintf(fp, "%d\n", nPrim);
	for(i=0; i < nPrim; i++){
		fprintf( fp, "#### %d\n", i );
		fprintf( fp, "%d\n", Prim[i].type);
		fprintf( fp, "%f %f %f\n",
				Prim[i].trans[X],
				Prim[i].trans[Y],
				Prim[i].trans[Z]);
		if ( i == WORLD){
			fprintf( fp, "%f %f %f %f\n",
					Prim[i].rot[X], Prim[i].rot[Y],
					Prim[i].rot[Z], Prim[i].rot[W] );
		}
		else{
			fprintf( fp, "%f %f %f\n",
					rad2deg( Prim[i].rot[X] ),
					rad2deg( Prim[i].rot[Y] ),
					rad2deg( Prim[i].rot[Z] ) );
		}
		fprintf( fp, "%f %f %f\n",
				Prim[i].scale[X],
				Prim[i].scale[Y],
				Prim[i].scale[Z] );
		/* write info specific to a CYLINDER or LANDMARK */
		if ( Prim[i].type == CYLINDER ){
			fprintf( fp, "%d %d\n", Prim[i].landMarkR, Prim[i].landMarkH );
			fprintf( fp, "%d\n", Prim[i].parentPrim );
		}
		else if ( Prim[i].type == LANDMARK){
			fprintf(fp, "%d\n", Prim[i].gIndex);
			fprintf(fp, "%d\n", Prim[i].primInd);
		}
		else errorMsg("Trying to write unknown primitive from primitives file!");
		/*fprintf(fp, "\n");*/
	}
	/* close the file */
	fclose(fp);
	/* change cursor back to normal */
#ifdef GRAPHICS
	glutSetCursor( GLUT_CURSOR_LEFT_ARROW );
	/* inform the user */
#endif
	printf("\tPrimitives saved on file %s\n", outFile);
	
}

/*
 *----------------------------------------------------------
 *	loadPrimitives
 *	If the file does not exists or could not be
 *	open this routine will return -1
 *----------------------------------------------------------
 */
int loadPrimitivesFile( char *inFile )
{
	FILE *fp;
	int i, j, nPrim;
	char line[255];
	
	if( (fp=fopen(inFile,"r")) == NULL) return(-1);
	
	/* read name of the file with the growth data */
	fscanf(fp, "%s\n", growthDataFileName);
	/* read name of the file with the animation data */
	fscanf(fp, "%s\n", animDataFileName);
	/* read number of primitives */
	fscanf(fp, "%d\n", &nPrim );
	
	for(i=0; i < nPrim; i++){
		Getline( line, 255, fp );
		/*printf( "Read Line %s\n", line );*/
		fscanf(fp, "%d\n", &(Prim[i].type));
		/*printf( "Read Prim %d\n", Prim[i].type );*/
		if ( Prim[i].type == CYLINDER || Prim[i].type == LANDMARK ){
			Prim[i].center.x = 0.0;
			Prim[i].center.y = 0.0;
			Prim[i].center.z = 0.0;
			Prim[i].h = 1.0;
			Prim[i].r = 1.0;
			fscanf(fp, "%f %f %f\n",
				   &(Prim[i].trans[X]),
				   &(Prim[i].trans[Y]),
				   &(Prim[i].trans[Z]));
			if ( i == WORLD){
				fscanf(fp, "%f %f %f %f\n",
					   &(Prim[i].rot[X]), &(Prim[i].rot[Y]),
					   &(Prim[i].rot[Z]), &(Prim[i].rot[W]) );
			}
			else{
				fscanf(fp, "%f %f %f\n",
					   &(Prim[i].rot[X]),
					   &(Prim[i].rot[Y]),
					   &(Prim[i].rot[Z]));
				/* convert rotation values into radians */
				Prim[i].rot[X] = deg2rad( Prim[i].rot[X] );
				Prim[i].rot[Y] = deg2rad( Prim[i].rot[Y] );
				Prim[i].rot[Z] = deg2rad( Prim[i].rot[Z] );
			}
			fscanf(fp, "%f %f %f\n",
				   &(Prim[i].scale[X]),
				   &(Prim[i].scale[Y]),
				   &(Prim[i].scale[Z]));
			/* read info specific to a CYLINDER or LANDMARK */
			if ( Prim[i].type == CYLINDER){
				fscanf(fp, "%d %d\n", &(Prim[i].landMarkR),&(Prim[i].landMarkH));
				fscanf(fp, "%d\n", &(Prim[i].parentPrim));
				Prim[i].gIndex = -1;
				for( j = 0; j < N_TEXT_PER_PRIM; j++ )
					Prim[i].textID[j] = -1;
				
			}
			else if ( Prim[i].type == LANDMARK){
				fscanf(fp, "%d\n", &(Prim[i].gIndex));
				fscanf(fp, "%d\n", &(Prim[i].primInd));
				Prim[i].parentPrim = -1;
				Prim[i].landMarkR = -1; Prim[i].landMarkH = -1;
				Prim[i].q1.x = -5.0;
				Prim[i].q1.y = 0.0;
				Prim[i].q1.z = 0.0;
				Prim[i].q2.x = 5.0;
				Prim[i].q2.y = 0.0;
				Prim[i].q2.z = 0.0;
			}
		}
		else{
			fprintf( stderr, "Primitive file %s\n", inFile );
			errorMsg("Trying to read unknown primitive from primitives file!");
		}
		fscanf(fp, "\n");
	}
	
	/* close the file */
	fclose(fp);
	
	/* show primitives */
	/*showPrimitives = TRUE;*/
	
	/* make sure that all primitives don't have animation
     information associated. Later on when processing
     the animation file, the primitives with animation
     information associated will have this flag switched
     to TRUE */
	for(i=1; i < nPrim; i++)
		Prim[i].animation = -1;
	
	/* help message for the user */
	//printf("==========================\n");
//	printf("Primitives loaded from file %s\n\n", inFile);
	
	/* returns the number of primitives */
	return( nPrim );
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

/*
 *-----------------------------------------------------------------
 *	This routine gets the given animation data
 *	and computes intermediate values using linear
 *	interpolation.
 *-----------------------------------------------------------------
 */
void linearInterpAnimData( int rowsAnimData, int columnsAnimData, int firstTime)
{
	static int rg, ra, ca;
	int numberOfFrames;
	int i, j, k, jj;
	float y, step;

	/* for the interpolation */
	static float **dataY;
	static float *dataX;

	numberOfFrames = FINAL_GROWTH_TIME + 1;

	if ( firstTime ){
		ra = rowsAnimData;
		ca = columnsAnimData;
		rg = ( numberOfFrames + numberOfInBet ) / ( 1 + numberOfInBet);

		/* allocates space for arrays */
		dataX = (float *)malloc(sizeof(float) * ca);
		dataY = allocate2DArrayFloat( ra, ca );

		/* copy original months information into dataX */
		dataX[0] = originalGrowthData[0][MONTH];
		dataX[ca-1] = originalGrowthData[rg-1][MONTH];
		if ( ca > rg ){
			printf("rows in growth data = %d\n", rg);
			printf("columns in animation data = %d\n", ca);
			errorMsg("There is more animation data than growth...\n");
		}
		for(i=1; i < ca-1; i++){
			dataX[i] = originalGrowthData[i][MONTH];
		}
		/*for(i=0; i < ca; i++) printf("dataX[%d] = %f\n", i, dataX[i]);*/

		/* copy the various data to interpolate */
		for(i=0; i < ra; i++){
			for(j=0; j < ca; j++){
				dataY[i][j] = originalAnimData[i][j];
				/*printf("dataY[%d][%d] = %f\n",i,j,dataY[i][j]);*/
			}
		}
	}

	/* get rid of the previous allocation for growth data array */
	free( animData ); animData = NULL;

	/* get space for new animation data array */
	animData = allocate2DArrayFloat( ra, numberOfFrames );

	/* compute interpolated values */
	for(i=0; i < ra; i++){
		for(j=0; j < numberOfFrames; j++){
			linint(dataX-1, dataY[i]-1, ca, growthData[j][MONTH], &y );
			animData[i][j] = y;
		}
	}

	FINAL_ANIM_TIME = numberOfFrames - 1;

	/*fprintf( stderr, "\nNew animation data:\n");
	 printArray(animData, ra, numberOfFrames);*/

	if (firstTime)
		fprintf( stderr, "\nComputed animation frames.\n");
	else
		fprintf(stderr, "\nComputed new animation frames.\n");
}


/*
 *-----------------------------------------------------------------
 *	This routine gets the given growth data
 *	and computes intermediate values using linear
 *	interpolation.
 *-----------------------------------------------------------------
 */
void linearInterpGrowthData( int rowsGrowthData,
							int columnsGrowthData,
							int firstTime)
{
	static int rg, cg;
	int numberOfFrames;
	int i, j, k, jj;
	float step;
	
	if ( firstTime ){
		rg = rowsGrowthData;
		cg = columnsGrowthData;
	}
	
	numberOfFrames = rg + ( (rg - 1) * numberOfInBet );
	
	/* get rid of the previous allocation for growth data array */
	free( growthData ); growthData = NULL;
	/* get space for new growth data array */
	growthData = allocate2DArrayFloat( numberOfFrames, cg );
	
	/* linear interpolation */
	for(jj=0; jj < cg; jj++){
		for(i=0, k=0; i < rg-1; i++){
			step = (originalGrowthData[i+1][jj] - originalGrowthData[i][jj]) /
			(float) (numberOfInBet + 1);
			for(j=0; j <= numberOfInBet; j++){
				growthData[k][jj] = originalGrowthData[i][jj] + j*step;
				k++;
			}
		}
	}
	
	/* fix for the last row */
	for(j=0; j < cg; j++){
		growthData[k][j] = originalGrowthData[rg-1][j];
	}
	
	/*fprintf( stderr, "New growth data:\n");
	 printArray(growthData, numberOfFrames, cg);*/
	
	if (firstTime)
		fprintf( stderr, "\nNumber of frames = %d\n", numberOfFrames);
	else
		fprintf(stderr, "\nNew number of frames = %d\n", numberOfFrames);
	
	FINAL_GROWTH_TIME = numberOfFrames - 1;
	growthTime = numberOfFrames;
}

/*
 *-----------------------------------------------------------------
 *	This routine goes over the animation data file and
 *	reads it into the array originalAnimData
 *-----------------------------------------------------------------
 */
void openParseAnimData( int *r, int *c )
{
	FILE *fpin;
	int rows, columns, j, primNumber;
	char line[255], input[255];
	int animTime;
	
	if( (fpin=fopen(animDataFileName,"r")) == NULL )
		errorMsg("Could not open input animation file to read!");
	
	printf("==========================\n");
	printf("Animation data loaded from file %s\n\n", animDataFileName);
	
	animTime = 0;
	
	while (fscanf (fpin, "%s", input) != EOF){
		if (!strcmp (input, COMMENT)){
			Getline(line, 255, fpin);
		}
		else if (!strcmp (input, SIZE)){
			fscanf(fpin, "%d %d %d\n",
				   &absRotValuesFlag, &rows, &columns);
			/*fprintf(stderr, "rows = %d columns = %d\n", rows, columns);*/
			
			/* This is not very smart but I'm in a hurry.
			 The rows and columns information given in the file
			 are for the actual number of rows and columns in
			 the animation file. Since the first 2 numbers
			 in the animation file are NOT related to the
			 actual angles used in the animation, I'm subtracting
			 2 from the given number of columns to reflect
			 the real size of the internal animation array */
			
			columns -= 2;
			
			/* get space for animation data */
			originalAnimData = allocate2DArrayFloat( rows, columns );
		}
		else if(!strcmp(input, DATA)){
			fscanf(fpin,"%d",&primNumber);
			/*printf("prim number = %d\n",primNumber);*/
			if (Prim[primNumber].type != CYLINDER)
				errorMsg("Trying to assign animation info to a Landmark!");
			else{
				Prim[primNumber].animation = animTime;
				fscanf(fpin,"%d",&(Prim[primNumber].axis));
				/*printf("Axis = %d\n",prim[primNumber].axis);*/
			}
			for(j=0; j < columns; j++){
				fscanf(fpin, "%f", &(originalAnimData[animTime][j]));
			}
			animTime++;
		}
		else{
			errorMsg("Got token not readable in input anim file...");
		}
	}
	
	fclose(fpin);
	
	FINAL_ANIM_TIME = columns - 1;
	
	printf("Final Animation Time = %d\n", FINAL_ANIM_TIME);
	/* print the growth file for debugging purposes */
	/*printArray(originalAnimData, rows, columns);*/
	
	*r = rows;
	*c = columns;
}
/*
 *-----------------------------------------------------------------
 *	This routine goes over the growth data file and
 *	reads it into the array originalGrowthData
 *-----------------------------------------------------------------
 */
void openParseGrowthData( int *r, int *c )
{
	FILE *fpin;
	char input[255];
	int rows, columns;
	int i,j,k;
	char line[255];
	float max, globalScale;
	int indexIntoGrowthArray;
	
	max = -1.0;
	growthTime = 0;
	
	if( (fpin=fopen(growthDataFileName,"r")) == NULL )
		errorMsg("Could not open input growth file to read!");
	
	printf("==========================\n");
	printf("Growth data loaded from file %s\n\n", growthDataFileName);
	
	while (fscanf (fpin, "%s", input) != EOF){
		if (!strcmp (input, COMMENT)){
			Getline(line, 255, fpin);
		}
		else if (!strcmp (input, SIZE)){
			fscanf(fpin, "%d %d\n", &rows, &columns);
			/* I am adding one to the number of rows since I want
			 to include a row with the user-defined age, from
			 the information gathered from the landmarks defined
			 by the user */
			rows++;
			/* get space for growth data */
			originalGrowthData = allocate2DArrayFloat( rows, columns );;
			/*fprintf(stderr, "rows = %d columns = %d\n", rows,columns);*/
		}
		else if(!strcmp(input, DATA)){
			for(i=0; i < columns; i++){
				if( fscanf(fpin, "%f", &originalGrowthData[growthTime][i]) == EOF )
					fprintf( stderr, "Uh-oh! File has improper format\n" );
				if ( max < originalGrowthData[growthTime][i] && i > 0){
					max = originalGrowthData[growthTime][i];
					indexIntoGrowthArray = i;
				}
			}
			growthTime++;
		}
		else{
			errorMsg("Got token not readable in input growth file...");
		}
	}
	fclose(fpin);
	
	globalScale = computeGlobalScale( indexIntoGrowthArray, max);
	
	/* HACK! I'll save the globalscale into the WORLD
     primitive so that I can zoom out the camera
     in the 'z' direction the same amount as the
     global scale */
	
	Prim[WORLD].cosine.z = globalScale;
	
	if ( globalScale == -1){
		printf("Could not compute the user-defined age!\n");
		printf("Remember that you won't be able to apply growth!\n");
		/* this is only for fake purposes */
		includeUserAgeintoGrowthArray(growthTime, 1.0);
	}
	else includeUserAgeintoGrowthArray(growthTime, globalScale);
	
	/* global variable final time */
	FINAL_GROWTH_TIME = growthTime;
	printf("Final growth time = %d\n", growthTime );
	printf("Global Scale = %f\n", globalScale );
	
	/* adjust for the testMorph routine */
	growthTime++;
	
	/* print the growth file for debugging purposes */
	/*printArray(originalGrowthData, rows, columns);*/
	
	*r = rows;
	*c = columns;
}

/*
 *-----------------------------------------------------------------
 *	
 *-----------------------------------------------------------------
 */
void includeUserAgeintoGrowthArray( int time, float scale )
{
	int i;
	float l1;
	Point3D v1, v2;
	Matrix4 transfMatrix;
	
	/* add the user-defined 'month' */
	/* 1.1 is arbitrary!! Means that the user-defined age is 10% more
	 * than the last defined age. For instance, if the last defined age
	 * is 60 months, the user-defined age will be 66 months
	 */
	originalGrowthData[time][0] = (int) originalGrowthData[time-1][0] * 1.1;
	
	for(i=0; i < NumberPrimitives; i++){
		if (Prim[i].type == LANDMARK){
			V3Assign( &v1, Prim[i].q1);
			V3Assign( &v2, Prim[i].q2);
			buildTransfMatrix(&transfMatrix, i);
			/* bring the points into World coordinates */
			V3PreMul(&v1, &transfMatrix);
			V3PreMul(&v2, &transfMatrix);
			l1 = distance(v1, v2);
			originalGrowthData[time][Prim[i].gIndex] = l1 * scale;
		}
	}
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
void
errorMsg( char error_text[] )
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
double
distance( Point3D p, Point3D q )
{
    return( sqrt( (p.x - q.x) * (p.x - q.x) +
				 (p.y - q.y) * (p.y - q.y) +
				 (p.z - q.z) * (p.z - q.z) ) );
}

/*
 *------------------------------------------------------------
 *------------------------------------------------------------
 */
void saveCellsFile( const char *fileName )
{
	
	FILE *fp;
	int len;
	char objFileName[128];
	int i, j;
	CELL *c;
	
	fprintf( stderr, "ExpFileName = %s\n", fileName );  
	if( (fp = fopen(fileName, "w")) == NULL )
		errorMsg("Could not open cells file to white!");
	
	if ( computingPatternFlag ){
		len = strlen( fileName );
		strncpy( objFileName, fileName, len - 3 );
		objFileName[len-3]='\0';
	}
	else{
		len = strlen( sessionFileName );
		strncpy( objFileName, sessionFileName, len - 8 );
		objFileName[len-8]='\0';
	}
	
	fprintf( fp, "%s\n", objFileName );  
	
	for( i = 0; i < NumberFaces; i++){
		fprintf( fp, "%d %d\n", i, faces[i].ncells );
		c = faces[i].head->next;
		while ( c != faces[i].tail ){
			fprintf( fp, "%d %lg %lg %lg\n", c->ctype,
					c->x, c->y, c->z );
			c = c->next;
		}
	}
	
	fclose( fp );
	
	/* inform the user */
	printf("Cells file saved on file %s\n", fileName );
}
/*
 *------------------------------------------------------------
 *------------------------------------------------------------
 */
int loadCellsFile( const char *inpFileName )
{
	FILE *fp;
	int len;
	char objFileName[128], fileName[128];
	int i, j;
	CELL *c;
	CELL_TYPE t;
	double x, y, z;
	Point3D p;
	Point2D q;
	int whichPolygon, ncells, totalNumberOfCells;
	
	if( (fp = fopen(inpFileName, "r")) == NULL )
		errorMsg("Could not open cells file to read!");
	
	len = strlen( sessionFileName );
	/*fprintf( stderr, "len %d\n", len );*/
	strncpy( objFileName, sessionFileName, len - 8 );
	objFileName[len-8]='\0';
	
	fscanf( fp, "%s\n", &fileName );
	if ( strcmp( fileName, objFileName ) ){
		fprintf( stderr, "fileName = %s objFileName = %s\n",
				fileName, objFileName);
		return FALSE;
	}
	
	totalNumberOfCells = 0;
	for( i = 0; i < NumberFaces; i++){
		fscanf( fp, "%d %d\n", &whichPolygon, &ncells);
		/*fprintf( stderr, "%d %d\n", whichPolygon, i );*/
		if ( i != whichPolygon ){
			fprintf( stderr, "Face read from file = %d current face = %d\n", whichPolygon, i );
			errorMsg("Faces do not match in read cells!");
		}
		faces[i].ncells = ncells;
		totalNumberOfCells += ncells;
		for ( j = 0; j < faces[i].ncells; j++){
			fscanf( fp, "%d %lg %lg %lg\n", &t, &(p.x), &(p.y), &(p.z) );
			/*fprintf( stderr, "%d %lg %lg %lg\n", t, p.x, p.y, p.z );*/
			c = createCell(p, i, t, 0);
			insertOnListOfCells(c, faces[i].head, faces[i].tail);
			/* Compute this cell's barycentric coordinates */
			compBarycentricCoord( c );
			
		}
	}
	fclose( fp );
	
	fprintf( stderr, "Loaded Number of Cells:\n");
	fprintf( stderr, "[C] = %d [D] = %d [E]= %d [F] = %d\n",
			nCellsType[C], nCellsType[D],
			nCellsType[E], nCellsType[F] );
	
	return totalNumberOfCells;
}
/*
 *------------------------------------------------------------
 *------------------------------------------------------------
 */
void saveObjFile( const char *fileName )
{
	FILE *fp;
	int i, j, len;
	char objFileName[128];
	
	if( (fp = fopen(fileName, "w")) == NULL )
		errorMsg("Could not open input obj file to write!");
	
	printf("==========================\n");
	printf("Saving obj file into file %s\n\n", fileName);
	
	len = strlen( sessionFileName );
	strncpy( objFileName, sessionFileName, len - 8 );
	objFileName[len-8]='\0';
	
	fprintf(fp, "# Object saved from the 'onca' program\n");
	fprintf(fp, "# Marcelo Walter (marcelow@cs.ubc.ca)\n");
	fprintf(fp, "# The original object file is %s\n\n", objFileName );
	
	fprintf(fp, "# Vertices info\n");
	for(i = 0; i < NumberVertices; i++){
		/* MARCELO */
		fprintf(fp, "v %3.6f %3.6f %3.6f\n",
				vert[i].pos.x, vert[i].pos.y, vert[i].pos.z);
		/*vertDisplay[i].x, vertDisplay[i].y, vertDisplay[i].z );*/
	}
	
	fprintf(fp, "# Normals info\n");
	for(i = 0; i < NumberNormals; i++){
		fprintf(fp, "vn %3.6f %3.6f %3.6f\n",
				vertn[i].x, vertn[i].y, vertn[i].z);
	}
	fprintf(fp, "\n");
	fprintf(fp, "# Faces info\n");
	if ( NumberNormals > 0 ){
		for(i = 0; i < NumberFaces; i++){
			fprintf(fp, "f ");
			for(j = 0; j < faces[i].nverts; j++){
				fprintf(fp, "%d//%d ", faces[i].v[j]+1, faces[i].vn[j]+1 );
			}
			fprintf(fp, "\n");
		}
	}
	else{
		for(i = 0; i < NumberFaces; i++){
			fprintf(fp, "f ");
			for(j = 0; j < faces[i].nverts; j++){
				fprintf(fp, "%d ", faces[i].v[j]+1 );
			}
			fprintf(fp, "\n");
		}
	}
	
	fclose( fp );
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
 *-----------------------------------------------------------
 *	NEWELL'S METHOD FOR COMPUTING THE PLANE EQUATION OF
 *	A POLYGON
 *	Filippo Tampieri
 *	Cornell University
 *
 *	This code adapted from Graphics Gems III
 *-----------------------------------------------------------
 */

/*
 **  PlaneEquation--computes the plane equation of an arbitrary
 **  3D polygon using Newell's method.
 **
 **  Entry:
 **      faceIndex - index into the faces array
 **  Exit:
 **      plane  - normalized (unit normal) plane equation
 */

void PlaneEquation(int faceIndex, Point4D *plane)
{
	int i, nverts;
	Point3D refpt, normal, u, v;
	float len;
	
	nverts = faces[faceIndex].nverts;
	
	/*  compute the polygon normal and a reference point on
	 *  the plane. Note that the actual reference point is
	 *  refpt / nverts
	 */
	V3Zero( &normal );
	V3Zero( &refpt );
	
	for(i = 0; i < nverts; i++) {
		V3Assign( &u, vert[faces[faceIndex].v[i]].pos);
		V3Assign( &v, vert[faces[faceIndex].v[(i + 1) % nverts]].pos);
		
		normal.x += (u.y - v.y) * (u.z + v.z);
		normal.y += (u.z - v.z) * (u.x + v.x);
		normal.z += (u.x - v.x) * (u.y + v.y);
		
		V3AddPlus( &refpt, &u);
	}
	/* normalize the polygon normal to obtain the first
	 * three coefficients of the plane equation
	 */
	len = V3Length( &normal );
	plane->a = normal.x / len;
	plane->b = normal.y / len;
	plane->c = normal.z / len;
	/* compute the last coefficient of the plane equation */
	len *= nverts;
	plane->d = -V3Dot( &refpt, &normal ) / len;
}


/*
 *-----------------------------------------------------------------
 *	This routine goes over the animation data file and
 *	reads it into the array originalAnimData
 *	This is the OLD one, where the animation file
 *	didn't have any comments
 *-----------------------------------------------------------------
 */
void openParseAnimData1( int *r, int *c )
{
	FILE *fpin;
	int rows, columns, i, j, primNumber;
	
	if( (fpin=fopen(animDataFileName,"r")) == NULL )
		errorMsg("Could not open input animation file to read!");
	
	printf("==========================\n");
	printf("Animation data loaded from file %s\n\n", animDataFileName);
	
	fscanf(fpin, "%d %d %d\n",
		   &absRotValuesFlag, &rows, &columns);
	fprintf(stderr, "rows = %d columns = %d\n", rows, columns);
	
	/* This is not very smart but I'm in a hurry.
     The rows and columns information given in the file
     are for the actual number of rows and columns in
     the animation file. Since the first 2 numbers
     in the animation file are NOT related to the
     actual angles used in the animation, I'm subtracting
     2 from the given number of columns to reflect
     the real size of the internal animation array */
	
	columns -= 2;
	
	/* get space for animation data */
	originalAnimData = allocate2DArrayFloat( rows, columns );
	
	for(i=0; i < rows; i++){
		fscanf(fpin,"%d",&primNumber);
		/*printf("prim number = %d\n",primNumber);*/
		if (Prim[primNumber].type != CYLINDER)
			errorMsg("Trying to assign animation info to a Landmark!");
		else{
			Prim[primNumber].animation = i;
			fscanf(fpin,"%d",&(Prim[primNumber].axis));
			/*printf("Axis = %d\n",prim[primNumber].axis);*/
		}
		for(j=0; j < columns; j++){
			fscanf(fpin, "%f", &(originalAnimData[i][j]));
		}
	}
	
	fclose(fpin);
	
	FINAL_ANIM_TIME = columns - 1;
	
	printf("Final Animation Time = %d\n", FINAL_ANIM_TIME);
	/* print the growth file for debugging purposes */
	printArray(originalAnimData, rows, columns);
	
	*r = rows;
	*c = columns;
}
