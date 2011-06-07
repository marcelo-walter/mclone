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
 *	Variables
 * --------------------------------------------------------------------------
 */

extern char sessionFileName[128]; //From main.h

/* name of the output CM file */
extern char outputCMfileName[128]; //From main.h

extern char expFileName[128]; //From main.h

extern char textureFileName[128]; //From main.h

extern char outputObjFileName[128]; //From main.h

/* flag if there is or not an experiment file <file name>.cm */
extern byte expFilePresent; //From main.h

/* Added bu Vinicius at 21/07/2005 */
/* This flag say that we loaded the file that have all the vectors for each face */
extern flag vectorfieldPresent; //From main.h

/*
 * --------------------------------------------------------------------------
 *	Prototypes
 * --------------------------------------------------------------------------
 */



#endif //_FILE_MANAGER_H_

