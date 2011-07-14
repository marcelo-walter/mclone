/*
 *--------------------------------------------------------
 *	PatternObject.c
 *	Routines related with the object and material
 *	files to write a .obj and .mtl format file
 *--------------------------------------------------------
 */

#ifndef _PATTERN_OBJECT_H_
#define _PATTERN_OBJECT_H_

#include <stdio.h>

#include "Types.h"

/*
 *------------------------------------------------------------
 *  Global definitions
 *------------------------------------------------------------
 */

extern int mtl_flag;
extern int opt_flag;

extern char mtl_file[256];     /* mtl file name */



/*-----PROTOTYPES----*/
void saveOptimizerPatternFile( FILE *fp );
void savePatternFile( const char *filename );
void saveMtl( const char *mtl_file );

int CmpVertex(const void *elem0, const void *elem1);
void saveOptVertices( FILE *fpObj, long int total_vertices, TVertex *vertexNew );
void saveOptFaces( FILE *fpObj, long int *exchange );

void optimizeVoronoiPoligons( void );
void optimizeVoronoiPoligons2( void );


#endif //_PATTERN_OBJECT_H_

