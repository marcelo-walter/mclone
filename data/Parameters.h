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

extern float finalTime, currentTime;

extern int 	NumberCells; //From Globals.h

extern double 	wrp;	/* weight for the repulsive radius */ //From relax.h
extern int 	iterPerUnitTime; //From relax.h

/* global weight for relaxation forces: steady and initial */
/* double 	wForce, initWForce; */
extern double wd, initWForce;  /* Added by Thompson Peter Lied in 16/07/2002 */ //From relax.h

/* weights for anisotropic effects */
extern double wa;  /* Added by Thompson Peter Lied in 16/07/2002 */ //From relax.h

/* initial number of steps for the relaxation */
extern int 	initNumRelax; //From relax.h

/* orientation in degrees for the anisotropy */
extern float orientation; //From relax.h

/* store adhesion values between the different types of cells */
extern float adhes[HOW_MANY_TYPES][HOW_MANY_TYPES];

/* store probabilities for switching types */
extern float swTypes[HOW_MANY_TYPES][HOW_MANY_TYPES];

/*
 * Ortcell holds the information about splitting rates for
 * cells and probabilities for a given type to occur.
 * These probabilities are only used when there is no
 * external input experiment file
 */
extern TYPE Ortcell[HOW_MANY_TYPES]; //From Globals.h

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
 * --------------------------------------------------------------------------
 *	Prototypes
 * --------------------------------------------------------------------------
 */
void readParametersFile( const char *fileName );
void printParamForUser( void );


#endif //_PARAMETERS_H_

