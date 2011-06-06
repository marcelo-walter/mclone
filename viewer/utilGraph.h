/*
 *-------------------------------------------------------
 *	utilGraph.c
 *	Useful routines when running the program
 *	in interactive mode. They used to live
 *	in util.c but since I want also a non-interactive
 *	version I moved here all routines which
 *	were dependent upon the graphics
 *-------------------------------------------------------
 */

#ifndef _UTIL_GRAPH_H_
#define _UTIL_GRAPH_H_

/*
 *----------------------------------------------------------
 *	Prototypes
 *----------------------------------------------------------
 */
void saveSessionFile( char *outFile );
int loadSessionFile( char *inFile );

#endif //_UTIL_GRAPH_H_
