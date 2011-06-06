/*
 *  cells.h
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/7/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */

/*
 *---------------------------------------------------------
 *
 *      cells.h
 *      Marcelo Walter
 *---------------------------------------------------------
 */

#ifndef _CELLS_H_
#define _CELLS_H_

#include "../common.h"

#define COR_TYPE_C_R 	0.4078 	/* 104, 60, 30 brown */
#define COR_TYPE_C_G	0.2353
#define COR_TYPE_C_B	0.1176

#define COR_TYPE_D_R	0.9882	/* 252, 152, 76 yellow */
#define COR_TYPE_D_G	0.596
#define COR_TYPE_D_B	0.298

#define COR_TYPE_E_R 	0.847	/* 216, 112, 32 buff */
#define COR_TYPE_E_G	0.4392
#define COR_TYPE_E_B	0.1255

#define COR_TYPE_F_R	1.0	/* 255, 255, 255 white */
#define COR_TYPE_F_G	1.0
#define COR_TYPE_F_B	1.0

#define INIT_PROB_C	0.2
#define INIT_PROB_D	0.8
#define INIT_PROB_E	0.0
#define INIT_PROB_F	0.0

#define INIT_RATE_DIV_C	4
#define INIT_RATE_DIV_D	60
#define INIT_RATE_DIV_E	0
#define INIT_RATE_DIV_F	0

/*
 *---------------------------------------------------------------
 *	Local global variables
 *---------------------------------------------------------------
 */
/* store adhesion values between the different types of cells */
extern float adhes[HOW_MANY_TYPES][HOW_MANY_TYPES];

/* store probabilities for switching types */
extern float swTypes[HOW_MANY_TYPES][HOW_MANY_TYPES];


/*
 * Prototypes
 */
void initAdhesInfo( void );
CELL_TYPE pickType( void );
void pickRandomPosition( CELL *c, Point3D *p );
double fran(double min, double max);
float getTimeSplit( float mean );
void assignPosition2Cell( CELL *c, Point3D p );
void assignPrevPosition2Cell( CELL *c, Point3D p );


#endif //_CELLS_H_
