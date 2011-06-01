/*
 *---------------------------------------------------------
 * 	simulation.c
 *---------------------------------------------------------
 */

#ifndef _SIMULATION_H_
#define _SIMULATION_H_

#include "../common.h"

/*
 *--------------------------------------------------
 *	Local Global variables
 *--------------------------------------------------
 */
float 	finalTime, currentTime;
int 	updateTime;

/*Add by Fabiane Queiroz*/
extern flag    originalScale;
VECTORARRAY *NodoAux;

FILE *fpRates;

/*
 *--------------------------------------------------
 *	Local prototypes
 *--------------------------------------------------
 */
void 	initSimulationParam( float finaltime, int updtime, float currenttime );
void 	postSplitEventsOnQueue( void );
/* Added by Fabiane Queiroz in 25/11/2009 */
int    findElement(Array* listFaces, int e);
/* Modified by Fabiane Queiroz in 25/11/2009 */
void 	pureSimulation( RELAXMODE rMode );

void 	patFormAndGrowthSimulation( RELAXMODE rMode );
void 	initPatFormAndGrowth( RELAXMODE rMode);

void 	computeBodyEmbryo( int day );
void 	assignTimeToSplit( void );
void 	compRatesOfGrowth( void );
void 	morphCells( void );
double 	compRateOfSplitting( CELL *c );
double 	computeTotalAreaObject( void );
void 	compSplittingRates( FILE *fpOut, int day );
void 	compSplittingRatesForOneFace( int whichFace );
void 	updtBaryCentricCoord( void );


#endif //_SIMULATION_H_
