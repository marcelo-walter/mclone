/*
 *---------------------------------------------------------
 *	FileManager.h
 *	This file has the routine which reads the parameter manager
 *	file <obj name>.mclone
 *---------------------------------------------------------
 */

#ifndef _FILE_MANAGER_H_
#define _FILE_MANAGER_H_

#include "Types.h"

/*
 * --------------------------------------------------------------------------
 *	Variables - File Names
 * --------------------------------------------------------------------------
 */

extern char sessionFileName[128]; //From main.h

/* name of the output CM file */
extern char outputCMfileName[128]; //From main.h

extern char outputObjFileName[128]; //From main.h

extern char expFileName[128]; //From main.h

extern char primitivesFileName[128]; //From main.h

extern char textureFileName[128]; //From main.h

extern char vectorcoordFileName[128]; /*Save the vector final coordenates in a file to calculate de erro later*/

extern char vectorFileName[128];

/* default growth data file name */
extern char animDataFileName[128];

/* default growth data file name */
extern char growthDataFileName[128];

/*
 * --------------------------------------------------------------------------
 *	Variables - Flags
 * --------------------------------------------------------------------------
 */

/* flag if there is or not an experiment file <file name>.cm */
extern flag expFilePresent; //From main.h

/* Flags if there is or not a primitive file */
extern flag primFilePresent; //From main.h

/* Flags if there is or not a texture file <file name>.txtr */
extern flag texturePresent;

/* Added bu Vinicius at 21/07/2005 */
/* This flag say that we loaded the file that have all the vectors for each face */
extern flag vectorfieldPresent; //From main.h

extern flag vectorPresent;

extern flag animFlag; //From transformation.h
/*
 * Grow or not the model. The idea is that sometimes
 * I want to only animate the model (such as the horse
 * walking) without actually growing it at the same
 * time
 */
extern flag growthFlag;

/*
 * --------------------------------------------------------------------------
 *	Prototypes
 * --------------------------------------------------------------------------
 */



#endif //_FILE_MANAGER_H_

