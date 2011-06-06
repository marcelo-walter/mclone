/*
 *	main.h
 */

#ifndef _MAIN_H_
#define _MAIN_H_

#include "data/Types.h"

/* for the simulation. Default final time */
#define	FINAL_TIME	10


/*
 *--------------------------------------------------
 *	Local Defined Global variables
 *--------------------------------------------------
 */
//char sessionFileName[128]; moved to temporary.h
//char primitivesFileName[128];
/* name of the output CM file */
//char outputCMfileName[128]; moved to temporary.h
extern char outputObjFileName[128];
//extern char expFileName[128];
extern char textureFileName[128];

/* Added by Thompson Peter Lied at 31/10/2003 */
extern char argvv[256]; // execution path

/*
 * various flags
 */
extern flag fillingFlag;	/* draw or not filled polygons */
extern flag edgesFlag;		/* draw or not edges */

/* flag if there is or not an experiment file <file name>.cm */
extern byte expFilePresent;


/* Added bu Vinicius at 21/07/2005 */
/* This flag say that we loaded the file that have all the vectors for each face */
extern flag vectorfieldPresent;

extern flag calculatesDistances;
//flag calculatesDistances = TRUE;

extern flag useGeodeiscDistances;

/* keep the maximum and minimum coordinates of
 the geometric model */
extern Point3D minSet, maxSet;

/* names of possible object directories */
extern char **pathDir;
/* number of directories defined in the env variable ONCA_PATH */
extern int numberOfDir;



int main(int argc, char *argv[]);
void usage(void);
void processGroups(void);
void preProcess(void);
void initParam( byte parFilePresent );
void createFaceSystem(int faceIndex);
void preProcessFaces( void );
void preProcessVertices( void );
/*Modified by Fabiane Queiroz in 25/11/2009*/
void runNonInteractive( PFMODE mode, RELAXMODE rMode );

void checkCCWordering( int whichFace, int howManyVert );
//void computeGeometricCenter( int whichFace, int howManyVert ); //moved to temporary.c
//void compMappingMatrices( int whichFace );//moved to temporary.c
void getMemAdjacentFacesList( int whichFace, int howManyAdjFaces );
void setEnv( void );
void freeAll( void );


#endif //_MAIN_H_

