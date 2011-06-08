/*
 *---------------------------------------------------------
 *	FileManager.cpp
 *	This file has the routine which reads the parameter manager
 *	file <obj name>.mclone
 *---------------------------------------------------------
 */

#include "fileManager.h"

/*
 * --------------------------------------------------------------------------
 *	Variables - File Names
 * --------------------------------------------------------------------------
 */

char sessionFileName[128]; //From main.h

/* name of the output CM file */
char outputCMfileName[128]; //From main.h

char outputObjFileName[128]; //From main.h

char expFileName[128]; //From main.h

char primitivesFileName[128]; //From main.h

char textureFileName[128]; //From main.h

char vectorcoordFileName[128]; /*Save the vector final coordenates in a file to calculate de erro later*/

char vectorFileName[128];

/* default growth data array */
char animDataFileName[128];

/* default growth data array */
char growthDataFileName[128];

/*
 * --------------------------------------------------------------------------
 *	Variables - Flags
 * --------------------------------------------------------------------------
 */

/* flag if there is or not an experiment file <file name>.cm */
flag expFilePresent = FALSE; //From main.h

/* Flags if there is or not a primitive file specified */
flag primFilePresent = FALSE; //From main.cpp

/* Flags if there is or not a texture file <file name>.txtr */
flag texturePresent = FALSE; //from main.c

/* Added bu Vinicius at 21/07/2005 */
/* This flag say that we loaded the file that have all the vectors for each face */
flag vectorfieldPresent = FALSE; //From main.h

flag vectorPresent = FALSE;

flag animFlag = FALSE; //From transformation.cpp

/*
 * Grow or not the model. The idea is that sometimes
 * I want to only animate the model (such as the horse
 * walking) without actually growing it at the same
 * time
 */
flag growthFlag = TRUE;


/*
 *----------------------------------------------------------
 *  Functions
 *----------------------------------------------------------
 */

