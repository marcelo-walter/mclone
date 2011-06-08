/*
 *------------------------------------------------------------
 *	Growth.c
 *	Ricardo Binsfeld
 *
 *	This file contains growth routines
 *------------------------------------------------------------
 */

#include "Growth.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "primitives.h"
#include "../util/genericUtil.h"
#include "../util/transformations.h"
#include "../data/Point3D.h"
#include "../data/Matrix4.h"
#include "../data/fileManager.h"

#ifdef GRAPHICS
#include "GL/glut.h"
#endif


/*
 *------------------------------------------------------------
 *		Local global definitions for Growth
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

/* default totalnumber of frames */
int numberOfInBet = 0;

flag keepAspectFlag = FALSE;

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
	int i;
	char line[255];
	float max, globalScale;
	int indexIntoGrowthArray;

	max = -1.0;
	growthTime = 0;

	if( (fpin=fopen(growthDataFileName,"r")) == NULL )
		errorMsg("Could not open input growth file to read! (Growth.cpp)");

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
			errorMsg("Got token not readable in input growth file...(Growth.cpp)");
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
