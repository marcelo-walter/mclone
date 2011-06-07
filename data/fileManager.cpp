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
 *	Variables
 * --------------------------------------------------------------------------
 */

char sessionFileName[128]; //From main.h

/* name of the output CM file */
char outputCMfileName[128]; //From main.h

char expFileName[128]; //From main.h

char outputObjFileName[128]; //From main.h

char textureFileName[128]; //From main.h

/* flag if there is or not an experiment file <file name>.cm */
byte expFilePresent = FALSE; //From main.h

/* Added bu Vinicius at 21/07/2005 */
/* This flag say that we loaded the file that have all the vectors for each face */
flag vectorfieldPresent = FALSE; //From main.h


/*
 *----------------------------------------------------------
 *  Functions
 *----------------------------------------------------------
 */

