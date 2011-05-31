/*
 *----------------------------------------------------------
 *	Globals.h
 *	This file contains all global variables
 *	definitions
 *----------------------------------------------------------
 */

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#ifdef MAIN
#define Global
#define Init( var, value) var = value
#else
#define Global extern
#define Init( var, value) var
#endif


Global double 	Init( TotalAreaOfObject, 0 );
Global double	Init( InitAreaOfObject, 0 );
Global int 	Init( Seed, -11 );
Global int 	Init( NumberVoronoiVert, 0 );
Global int 	Init( NumberVertices, 0 );
Global int 	Init( NumberTextureVertices, 0 );
Global int 	Init( NumberNormals, 0 );
Global int 	Init( NumberFaces, 0 );
Global int 	Init( NumberVectors, 0 );
Global int 	Init( NumberEdges, 0 );
Global int 	Init( NumberGroups, 0 );
Global int 	Init( NumberCells, 0 );
Global int 	Init( NumberPrimitives, 1 );
Global int	Init( computingPatternFlag, 0 );
/*
 * How to consider the neighboring faces for a given
 * face. The possible values are: PRIMFACES, STRIPFAN, TURK
 * The default value is to consider only the primary
 * faces, ie, the faces which share an edge with the
 * face being computed
 */
Global NEIGHMODE	Init( neighborsFaceMode, NOSECFACES );

Global RNDDIST 	Init( RandomTypeDist, RNONE );
Global PFMODE	Init( pfMode, WOUTGROWTH );

/* Added by Fabiane Queiroz in 25/11/2009 */
/* Local Relaxation */

Global RELAXMODE	Init( relaxMode, GLOBALRELAX );

/*
 *---------------------------------------------------------
 * 	Here are the definitions when I don't want to
 *	Initialize the Global Variables
 *---------------------------------------------------------
 */
/*
 * Ortcell holds the information about splitting rates for
 * cells and probabilities for a given type to occur.
 * These probabilities are only used when there is no
 * external input experiment file
 */
#ifdef MAIN
#define Global
Global TYPE Ortcell[HOW_MANY_TYPES];
#else
#define Global extern
Global TYPE Ortcell[HOW_MANY_TYPES];
#endif

/*
 * data structure for the primitives
 */
#ifdef MAIN
#define Global
Global Primitive Prim[N_OF_PRIM+1];
#else
#define Global extern
Global Primitive Prim[N_OF_PRIM+1];
#endif

#endif //_GLOBALS_H_
