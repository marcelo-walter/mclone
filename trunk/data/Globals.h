/*
 *----------------------------------------------------------
 *	Globals.h
 *	This file contains all global variables
 *	definitions
 *----------------------------------------------------------
 */

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

//#include "Types.h"

/*
#ifdef MAIN
#define Global
#define Init( var, value) var = value
#else
#define Global extern
#define Init( var, value) var
#endif
*/

//Global double 	Init( TotalAreaOfObject, 0 ); //Object
//Global double	Init( InitAreaOfObject, 0 ); //Object
//Global int 	Init( Seed, -11 ); //CellsList
//Global int 	Init( NumberVoronoiVert, 0 ); //InterfaceVoronoi
//Global int 	Init( NumberVertices, 0 ); //Object
//Global int 	Init( NumberTextureVertices, 0 ); //Object
//Global int 	Init( NumberNormals, 0 ); //Object
//Global int 	Init( NumberFaces, 0 ); //Object
//Global int 	Init( NumberVectors, 0 ); //Don't used
//Global int 	Init( NumberEdges, 0 ); //Object
//Global int 	Init( NumberGroups, 0 ); //Object
//Global int 	Init( NumberCells, 0 ); //CellsList
//Global int 	Init( NumberPrimitives, 1 ); //Primitives
//Global int	Init( computingPatternFlag, 0 ); //Simulation ?
/*
 * How to consider the neighboring faces for a given
 * face. The possible values are: PRIMFACES, STRIPFAN, TURK
 * The default value is to consider only the primary
 * faces, ie, the faces which share an edge with the
 * face being computed
 */
//Global NEIGHMODE	Init( neighborsFaceMode, NOSECFACES ); //Parameters

//Global RNDDIST 	Init( RandomTypeDist, RNONE ); //Parameters
//Global PFMODE	Init( pfMode, WOUTGROWTH ); // Parameters

/* Added by Fabiane Queiroz in 25/11/2009 */
/* Local Relaxation */

//Global RELAXMODE	Init( relaxMode, GLOBALRELAX ); //Parameters

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
/*
#ifdef MAIN
#define Global
Global TYPE Ortcell[HOW_MANY_TYPES]; //Parameters
#else
#define Global extern
Global TYPE Ortcell[HOW_MANY_TYPES];
#endif
*/

/*GLOBALRELAX
 * data structure for the primitives
 * /
#ifdef MAIN
#define Global
Global Primitive Prim[N_OF_PRIM+1];
#else
#define Global extern
Global Primitive Prim[N_OF_PRIM+1];
#endif*/

#endif //_GLOBALS_H_
