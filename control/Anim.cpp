/*
 *------------------------------------------------------------
 *	Anim.h
 *	Ricardo Binsfeld
 *
 *	This file contains animation routines
 *------------------------------------------------------------
 */

#include "Anim.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "primitives.h"
#include "../util/genericUtil.h"
#include "../util/printInfo.h"
#include "../data/fileManager.h"

#ifdef GRAPHICS
#include "GL/glut.h"
#endif


/*
 *------------------------------------------------------------
 *		Local global definitions for Animation
 *------------------------------------------------------------
 */
/* animation data array */
float **animData;
float **originalAnimData;
int FINAL_ANIM_TIME;

/* how to interpret the values in the animation file,
 either as relative or absolute */
flag absRotValuesFlag = FALSE;


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
		errorMsg("Could not open input animation file to read! (Anim.cpp)");

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
				errorMsg("Trying to assign animation info to a Landmark! (Anim.cpp)");
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
			errorMsg("Got token not readable in input anim file... (Anim.cpp)");
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
		errorMsg("Could not open input animation file to read! (Anim.cpp)");

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
			errorMsg("Trying to assign animation info to a Landmark! (Anim.cpp)");
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
			errorMsg("There is more animation data than growth...(Anim.cpp)\n");
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

