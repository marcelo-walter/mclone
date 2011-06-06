/*
 *---------------------------------------------------------
 *	Parameters.c
 *	This file has the routine which reads the parameter
 *	file <obj name>.par
 *---------------------------------------------------------
 */

#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include "Types.h"

/*
 * --------------------------------------------------------------------------
 *	Variables
 * --------------------------------------------------------------------------
 */

/*
 * How to consider the neighboring faces for a given
 * face. The possible values are: PRIMFACES, STRIPFAN, TURK
 * The default value is to consider only the primary
 * faces, ie, the faces which share an edge with the
 * face being computed
 */
extern NEIGHMODE	neighborsFaceMode; //From Globals.h

extern RNDDIST 	RandomTypeDist; //From Globals.h
extern PFMODE		pfMode; //From Globals.h

extern RELAXMODE	relaxMode; //From Globals.h

/*
 * Ortcell holds the information about splitting rates for
 * cells and probabilities for a given type to occur.
 * These probabilities are only used when there is no
 * external input experiment file
 */
extern TYPE Ortcell[HOW_MANY_TYPES]; //From Globals.h

/*
 * --------------------------------------------------------------------------
 *	Prototypes
 * --------------------------------------------------------------------------
 */
void readParametersFile( const char *fileName );
void printParamForUser( void );


#endif //_PARAMETERS_H_

