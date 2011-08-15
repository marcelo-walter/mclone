/*
 *----------------------------------------------------------
 *	Macros.h
 *	This file contains all Macros definitions
 *----------------------------------------------------------
 */

#ifndef _MACROS_H_
#define _MACROS_H_

/* useful constants */

#define PI 		3.141592654
#define TWOPI		6.2831853
#define TRUE 		1
#define FALSE 		0
#define ON 		1
#define BUFSIZE    	256
#define	X		0
#define	Y		1
#define	Z		2
#define	W		3
#define XY		2
#define	XYZ		3
#define	XYZW	4
#define R		0
#define G		1
#define B		2
#define RGB		3
#define RLE_RESOLUTION 	4096

/* return values for the intersect routine - Graphics Gems III - p. 199 */
#define DONT_INTERSECT 0
#define DO_INTERSECT   1
#define COLLINEAR      2

#define	N_OF_PRIM	100

#define HOW_MANY_TYPES  4

/* these are specific for the giraffe */
//#define NEWBORNAGE 	      	1
//#define EMBRYOAGE 		0
//#define TOTALGESTATIONTIME	457
//#define STARTPFTIME		36

/* Added by Fabiane Queiroz: these are specific for the cow */
#define TOTALGESTATIONTIME	40//285
#define STARTPFTIME		22
#define NEWBORNAGE 	    1
#define EMBRYOAGE 		0

#define	MAX_N_TEXTURES 	50
#define	N_TEXT_PER_PRIM 10	/* Max number of texture per cylinder */

/*
 * Primitive "0" is all objects in the scene
 * which I call WORLD
 */
#define	WORLD		0

//#define DEBUG           1

/*
 * I am not doing this in a smart way. I was using 20 and
 * it was not enough. Then I increased to 40 and it seems
 * fine
 */
#define MAX_N_VOR_VERT	40

/* how much to increase the heap for triangles list each time */
#define HEAPTRI_GROW_FACTOR 	5

/*
 * Since Michael's code has some same definitions as myself
 * I am including this directive here which mean compile Michael
 * McAllister code
 */
#ifndef _COMPILE_MM_CODE

#define EPSILON 	1.0e-10
#define EPSILON1	1.0e-9
/* useful geometry stuff */
#define MyAbs(x) ( (x) >= 0 ? (x) : -(x))
#define Zero(x) (MyAbs(x) < EPSILON)
#define Equal(A,B) Zero((A)-(B))
#define Negative(x) ((!Zero(x)) && ((x) < 0.0))
#define Positive(x) ((!Zero(x)) && ((x) > 0.0))
#define GreaterEqualZero(x) ( Zero(x) || Positive(x) )

#define ZeroSp(x) (MyAbs(x) < EPSILON1)
#define NegativeSp(x) ((!ZeroSp(x)) && ((x) < 0.0))
#define PositiveSp(x) ((!ZeroSp(x)) && ((x) > 0.0))
#define GreaterEqualZeroSp(x) ( ZeroSp(x) || PositiveSp(x) )

#endif

#endif //_MACROS_H_

