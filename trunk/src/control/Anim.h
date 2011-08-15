/*
 *------------------------------------------------------------
 *	Anim.h
 *	Ricardo Binsfeld
 *
 *	This file contains animation routines
 *------------------------------------------------------------
 */

#ifndef _ANIM_H_
#define _ANIM_H_

#include "../data/Types.h"
#include "Growth.h"

/*
 * From genericUtil.c
 *------------------------------------------------------------
 *		Local global definitions for Animation
 *------------------------------------------------------------
 */

/* animation data array */
extern float **animData;
extern float **originalAnimData;
extern int FINAL_ANIM_TIME;

/* how to interpret the values in the animation file,
 either as relative or absolute */
extern flag absRotValuesFlag;


/*
 *------------------------------------------------------------
 *		Local prototypes for Animation
 *------------------------------------------------------------
 */
void openParseAnimData( int *r, int *c );
void openParseAnimData1( int *r, int *c );
void linearInterpAnimData( int rowsAnimData, int columnsAnimData, int firstTime);

#endif //_ANIM_H_
