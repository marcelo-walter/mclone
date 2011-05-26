/*
 *	main.h
 */

/* for the simulation. Default final time */
#define	FINAL_TIME	10

int main(int argc, char *argv[]);
void usage(void);
void print_info(void);
void processGroups(void);
void preProcess(void);
void initParam( byte parFilePresent );
void createFaceSystem(int faceIndex);
void preProcessFaces( void );
void preProcessVertices( void );
void printFinalNumberOfCells( void );
/*Modified by Fabiane Queiroz in 25/11/2009*/
void runNonInteractive( PFMODE mode, RELAXMODE rMode );

void checkCCWordering( int whichFace, int howManyVert );
void computeGeometricCenter( int whichFace, int howManyVert );
void compMappingMatrices( int whichFace );
void getMemAdjacentFacesList( int whichFace, int howManyAdjFaces );
void setEnv( void );
void freeAll( void );
