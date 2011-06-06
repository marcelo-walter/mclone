/*
 * temporary.h
 *
 *  Created on: 02/06/2011
 *      Author: rlbinsfeld
 */

#ifndef _TEMPORARY_H_
#define _TEMPORARY_H_

#include "data/Types.h"

/*
 *--------------------------------------------------
 *	Local Defined Global variables
 *--------------------------------------------------
 */
extern char sessionFileName[128]; //From main.h

/* name of the output CM file */
extern char outputCMfileName[128]; //From main.h

extern char expFileName[128]; //From main.h

/* Flags if there is or not a primitive file */
extern flag primFilePresent; //From main.h

extern int *nnoprimVertex; //From main.h

/*
 *--------------------------------------------------
 *	Local Prototypes
 *--------------------------------------------------
 */
void computeGeometricCenter( int whichFace, int howManyVert ); //from main.h
void compMappingMatrices( int whichFace );//from main.h


#endif /* TEMPORARY_H_ */
