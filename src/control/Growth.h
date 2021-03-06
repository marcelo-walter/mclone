/*
 *------------------------------------------------------------
 *	Growth.h
 *	Ricardo Binsfeld
 *
 *	This file contains growth routines
 *------------------------------------------------------------
 */

#ifndef _GROWTH_H_
#define _GROWTH_H_

#include "data/Types.h"

/*
 * Begin from genericUtil.c
 *------------------------------------------------------------
 *		Local global definitions for Growth
 *------------------------------------------------------------
 */
/*  indexes the growth array. 'growthTime' points to
 *  the current time we want the growth data, eg,
 *  3 months, 18 months and so on
 */
extern int growthTime;
extern int FINAL_GROWTH_TIME;

/* growth data array */
extern float **growthData;
extern float **originalGrowthData;

/* default totalnumber of frames */
extern int numberOfInBet;

extern flag keepAspectFlag;


/*
 *------------------------------------------------------------
 *		Local prototypes for Growth
 *------------------------------------------------------------
 */
void openParseGrowthData( int *r, int *c );
void includeUserAgeintoGrowthArray( int time, float scale );
void linearInterpGrowthData( int rowsGrowthData, int columnsGrowthData, int firstTime);

#endif //_GROWTH_H_
