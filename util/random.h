/*
 *  random.h
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/7/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */

/*
 * 	random.h
 */

#ifndef _RANDOM_H_
#define _RANDOM_H_

/*
 *---------------------------------------------------
 *	DEFINES
 *---------------------------------------------------
 */

#define M1 259200
#define IA1 7141
#define IC1 54773
#define RM1 (1.0/M1)
#define M2 134456
#define IA2 8121
#define IC2 28411
#define RM2 (1.0/M2)
#define M3 243000
#define IA3 4561
#define IC3 51349

/*
 *---------------------------------------------------
 *	PROTOTYPES	
 *---------------------------------------------------
 */
float gammln(float xx);
float gasdev(int *idum);
float ran1(int *idum);
float gamdev(int ia, int *idum);
float poidev(float xm, int *idum);

#endif //_RANDOM_H_

