/*
 *---------------------------------------------------------
 * 	simulation.c
 *---------------------------------------------------------
 */

#ifndef _SIMULATION_H_
#define _SIMULATION_H_

#include <stdio.h>

#include "../data/Types.h"

/*
 *--------------------------------------------------
 *	Local Global variables
 *--------------------------------------------------
 */
extern float finalTime, currentTime;
extern int 	updateTime;

/*Add by Fabiane Queiroz*/
extern flag    originalScale;

extern FILE *fpRates;


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
void 	initPatFormAndGrowth( ); //Remove parameter  RELAXMODE rMode by Bins

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
