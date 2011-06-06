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

#include "utilGraph.h"

#include <stdio.h>

#include <GL/glut.h>

#include "graph.h"
#include "drawing.h"

#include "../data/Globals.h"

#include "../util/genericUtil.h"

/*
 *----------------------------------------------------------
 *	saveSessionFile
 *----------------------------------------------------------
 */
void saveSessionFile( char *outFile )
{
	FILE *fp;
	
	/* change cursor to a watch */
#ifdef GRAPHICS
	glutSetCursor( GLUT_CURSOR_WAIT );
#endif
	
	if( (fp = fopen(outFile,"w")) == NULL)
		errorMsg("Could not open session file to write!");
	fprintf( fp, "%d\n", Prim[WORLD].type );
	fprintf( fp, "%f %f %f\n",
			Prim[WORLD].trans[X],
			Prim[WORLD].trans[Y],
			Prim[WORLD].trans[Z]);
	fprintf( fp, "%f %f %f %f\n",
			Prim[WORLD].rot[X], Prim[WORLD].rot[Y],
			Prim[WORLD].rot[Z], Prim[WORLD].rot[W] );
	fprintf( fp, "%f %f %f\n",
			Prim[WORLD].scale[X],
			Prim[WORLD].scale[Y],
			Prim[WORLD].scale[Z] );
	fprintf(fp, "%d %d\n", Prim[WORLD].landMarkR, Prim[WORLD].landMarkH );
	fprintf(fp, "%d\n", Prim[WORLD].parentPrim );
	fprintf(fp, "%d\n", renderMode );
	fprintf(fp, "%d\n", showLandMarks );
	fprintf(fp, "%d\n", showPrimitives );
	/* Added by Thompson at 04/12/2003 */
	fprintf(fp, "%d\n", voronoiBorders );
	fprintf(fp, "%d\n", drawCells );
	/* end */
	
	fclose( fp );
	
	/* change cursor back to normal */
#ifdef GRAPHICS
	glutSetCursor( GLUT_CURSOR_LEFT_ARROW );
#endif
	
	/* inform the user */
	printf("Session saved on file %s\n", outFile);
}

/*
 *----------------------------------------------------------
 *	loadSession
 *	If the file does not exists or could not be
 *	open this routine will return -1
 *----------------------------------------------------------
 */
int loadSessionFile( char *inFile )
{
	FILE *fp;
	int rm, sL, sP;
	int vb, dc;
	
	if( (fp=fopen(inFile,"r")) == NULL) return(-1);
	
	fscanf(fp, "%d\n", &(Prim[WORLD].type));
	fscanf( fp, "%f %f %f\n",
		   &(Prim[WORLD].trans[X]),
		   &(Prim[WORLD].trans[Y]),
		   &(Prim[WORLD].trans[Z]));
	fscanf( fp, "%f %f %f %f\n",
		   &(Prim[WORLD].rot[X]), &(Prim[WORLD].rot[Y]),
		   &(Prim[WORLD].rot[Z]), &(Prim[WORLD].rot[W]) );
	fscanf( fp, "%f %f %f\n",
		   &(Prim[WORLD].scale[X]),
		   &(Prim[WORLD].scale[Y]),
		   &(Prim[WORLD].scale[Z]) );
	fscanf( fp, "%d %d\n", &(Prim[WORLD].landMarkR),&(Prim[WORLD].landMarkH));
	fscanf( fp, "%d\n", &(Prim[WORLD].parentPrim));
	fscanf( fp, "%d\n", &rm);
	fscanf( fp, "%d\n", &sL);
	fscanf( fp, "%d\n", &sP );
	/* Added by Thompson at 04/12/2003 */
	fscanf( fp, "%d\n", &vb );
	fscanf( fp, "%d\n", &dc );
	
	voronoiBorders = (flag) vb;
	drawCells = (flag) dc;
	/* end */
	
	renderMode = (RMODE) rm;
	showLandMarks = (flag) sL;
	showPrimitives = (flag) sP;
	
	fclose( fp );
	
	Prim[WORLD].center.x = 0.0;
	Prim[WORLD].center.y = 0.0;
	Prim[WORLD].center.z = 0.0;
	Prim[WORLD].h = 1.0;
	Prim[WORLD].r = 1.0;
	Prim[WORLD].gIndex = -1;
	
	/* help message for the user */
	printf("==========================\n");
	printf("\tSession loaded from file %s\n\n", inFile);
}
