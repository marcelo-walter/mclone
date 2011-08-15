/*
 *---------------------------------------------------------
 *
 *	main.c
 *	Start: october 17, 1995
 *	Marcelo Walter
 *	This program implements the MClone system
 *
 *---------------------------------------------------------
 */

/*
 *--------------------------------------------------
 *	Includes
 *--------------------------------------------------
 */

#include "main.h"

#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>

#include <sys/time.h>	/* for the time of the day function */

#include "control/vectorField.h"
#include "control/wingEdge.h"
#include "control/Growth.h"
#include "control/Anim.h"
#include "control/texture.h"
#include "control/primitives.h"
#include "simulation/relax.h"
#include "simulation/simulation.h"
#include "simulation/forces.h"
#include "simulation/morph.h"
#include "util/heap.h"
#include "util/heapTri.h"
#include "util/printInfo.h"
#include "util/distPoints.h"
#include "util/genericUtil.h"
#include "util/random.h"
#include "data/Object.h"
#include "data/cells.h"
#include "data/cellsList.h"
#include "data/Parameters.h"
#include "data/Matrix4.h"
#include "data/fileManager.h"
#include "distance/interfacedistance.h"

#ifdef GRAPHICS
#include "viewer/ui.h"
#endif

/*
 *--------------------------------------------------
 *	Local Defined Global variables
 *--------------------------------------------------
 */

/* Added by Thompson Peter Lied at 31/10/2003 */
char argvv[256]; // execution path //LOCAL

/* names of possible object directories */
char **pathDir; //LOCAL
/* number of directories defined in the env variable ONCA_PATH */
int numberOfDir; //LOCAL


/*
 *-------------------------------------------------
 *	Main
 *--------------------------------------------------
 */
int main( int argc, char *argv[] )
{
	char inFileName[256],
	parFileName[128],
	wingedEdgeFileName[128],
	vectorfieldFileName[128],
	*s,
	distanceFileName[128]; /* By Vinicius 18/03/2005*/
	int 	rg,
	cg,
	ra,
	ca;
	byte 	objFile = FALSE,
	parFile = FALSE,
	primFile = FALSE;
	int 	i, len;
	struct timeval pt;		/* for the random seed */
	struct timezone zpt;
	/* keep track of total simulation times */
	int initTimeSimulation, finalTimeSimulation, totalTimeSimulation;

	//int InicioGPath;

	/* we need at least the name of an object file */
	if ( argc < 2 )
		usage();

	/* parse the command line options */
	for(i = 1; i < argc; i++ )
	{
		if( *argv[i] == '-')
		{
			s = argv[i] + 1;
		  	switch(*s)
			{
					/* option to create the file with geodesic path distance or open the file that already have the distances*/
					/*To create Geodesic Path (in viewer) starting of a face index*/
				case 'd':
					i++;
					if( i >= argc )
						break;
					strcpy( distanceFileName, argv[i] );

					/* If you want to specify a face index where you star to calculate de geodesic distance, you can you this.
					 i++;
					 InicioGPath = atoi(argv[i]);
					 */
					
					useGeodeiscDistances = TRUE;
					break;
					/* option to read a <file>.cm, ie, a file with
					 the cell's descriptions */
		  		case 'e':
					i++;
					if( i >= argc )
						break;
					strcpy( expFileName, argv[i] );
					expFilePresent = TRUE;
					break;
					/* option to read the object description */
		 		case 'f':
					i++;
					if( i >= argc )
						break;
					strcpy( inFileName, argv[i] );
					objFile = TRUE;
					break;
					/* print the help or "usage" for the program */
		  		case 'h':
					usage();
					break;
					/*
					 * Option to input a primitive file <name>.prim
					 */
		  		case 'm':
					i++;
					if( i >= argc ) 
						break;
					strcpy( primitivesFileName, argv[i] );
					primFile = TRUE;
					primFilePresent = TRUE;
					break;
					/*
					 * Option to input a parameter file <name>.par
					 */
		  		case 'p':
					i++;
					if( i >= argc ) 
						break;
					strcpy( parFileName, argv[i] );
					parFile = TRUE;
					break;
			  	case 't':
					i++;
					if ( i >= argc ) 
						break;
					strcpy( textureFileName, argv[i] );
					texturePresent = TRUE;
					break;
					/*
					 * The use of this flag (vectorfieldPresent) is
					 * not finished yet! It is here for in the future
					 * add one direction per cell.
					 */
				case 'c':
					i++;
					if ( i >= argc ) 
						break;
					strcpy( vectorfieldFileName, argv[i] );
					fprintf( stderr, "Field Vector file = %s \n", vectorfieldFileName);
					vectorfieldPresent = TRUE;
					break;
					/* added by Thompson in 10/03/2004 */
					//#ifdef VECTORFIELD
			  	case 'v':
					i++;
					if ( i >= argc )
						break;
					strcpy( vectorFileName, argv[i] );
					fprintf( stderr, "Control\'s Vectors file = %s \n", vectorFileName);
					vectorPresent = TRUE;
					break;
					//#endif
		  		default:
					printf( "Bad command-line parameter: %s\n", argv[i] );
					usage();
					exit( 1 );
					break;
			}
		}
		else 
		{
			printf( "Bad command-line parameter: %s\n", argv[i] );
		  	usage();
		  	exit( 1 );
		}
	}
	
	/*
	 * Search the env variable (ONCA_PATH) for the possible
	 * directories to find the .obj files
	 */
	// setEnv();
	
	/*
	 * Before I check the env variable path, I want to make
	 * sure that the file being given is not given with
	 * the full path
	 */
	/*if ( inFileName[0] == '/') foundInFile = TRUE;
	 
	 else{*/
	/*
	 * At this point the variable 'numberOfDir' holds the
	 * number of possible directories to look for objects
	 * and the variable 'pathDir' has the names of
	 * the directories
	 */
	/*for(i = 0; i < numberOfDir; i++){
	 strcpy( fullInFileName, pathDir[i] );
	 strcat( fullInFileName, inFileName );
	 if( ( fp = fopen( fullInFileName, "r" )) != NULL ){
	 strcpy( inFileName, fullInFileName );
	 foundInFile = TRUE;
	 fclose( fp );
	 fprintf( stderr, "Found file %s\n", fullInFileName );
	 break;
	 }
	 }
	 }*/
	
	// if ( !foundInFile ) errorMsg("The file name does not exist!");
	
	/*
	 * Remove the 'obj' from the inFileName and copy
	 * to sessionFileName, primitivesFileName and
	 * wingedEdgeFilename strings
	 */
	len = strlen( inFileName );
	strncpy( sessionFileName, inFileName, len - 4);
	sessionFileName[len-4]='\0';
	/* add ".session" to the session file name */
	strcat( sessionFileName,".session" );
	
	if ( !primFile )
	{
		strncpy( primitivesFileName, inFileName, len - 4);
		primitivesFileName[len-4]='\0';
		/* add ".prim" to the primitives file name */
		strcat( primitivesFileName,".prim" );
		fprintf( stderr, "\n\nPrimitive file name %s\n", primitivesFileName );
	}
	
	/* winged-edge info file name */
	strncpy( wingedEdgeFileName, inFileName, len - 4);
	wingedEdgeFileName[len-4]='\0';
	/* add ".we" to the winged edge file name */
	strcat( wingedEdgeFileName,".we" );
	
	/*
	 * Get initial time to monitor simulation time
	 */
	gettimeofday( &pt, &zpt);
	initTimeSimulation = pt.tv_sec;
	Seed = pt.tv_sec/pt.tv_usec;
	
	/* initialization of the random number generator */
	ran1( &Seed );
	
	/* Initialize the default type of random distribution */
	RandomTypeDist = POISSON;
	
	if( objFile )
	{
		/* Read the object */
		readObject( inFileName );
		//int j;
		//for (j = 0; j < NumberVertices; j++)
		// printf("\ntestando: %lf, %lf, %lf\n", origVert[j].x, origVert[j].y, origVert[j].z); 
		/* If there is a parameter file we read it */
		if ( parFile )
		{
			readParametersFile( parFileName );
		}
		
		/* if needed include group information with the face information */
		/* processGroups(); */
		
		/* pre-process the geometry database */
		fprintf(stderr, "\nPre-processing the geometric database...");
		preProcess();
		fprintf( stderr, "Done!\n\n" );
		
		/*Added by Vinicius at 16/12/2004*/
		/*If we have a vector field, then we must calculate the distance between them.*/
		/*if (calculatesDistances)
		 {
		 len = strlen( inFileName );
		 strcpy(distanceFileName, inFileName);
		 
		 distanceFileName[len-3] = 'd';
		 distanceFileName[len-2] = 'i';
		 distanceFileName[len-1] = 's';
		 distanceFileName[len] = '\0';
		 //CreatePathDistances(distanceFileName, InicioGPath);
		 CreatePathDistances(distanceFileName);
		 }*/
		
		if (vectorfieldPresent)
		{
			loadVectorField(vectorfieldFileName);
		}
		
		/*
		 * Added by Vinicius at 16/12/2004
		 *
		 * If we have a vector field, then we must calculate the distance between them.
		 * If we already have the distance, CreatePathDistances will load the file .dis (distance)
		 */
		//	#ifdef VECTORFIELD
		if (useGeodeiscDistances)
		{
			//CreatePathDistances(distanceFileName, InicioGPath);
			CreatePathDistances(distanceFileName);
		}
		//#endif
		/*End Geodesic Distance create/load*/
		
		/* build winged edge data structure if we do not have already one */
		checkWingEdgeFile( wingedEdgeFileName );
		
		/* initializes a lot of stuff */
		initParam( parFile );
		
		/* the primitives and session file is read inside InitGraphics
		 since it specifies the initial camera position */
		if ( checkPrimitivesFile() )
		{
			/*
			 * There is a file to read the primitives from
			 * <obj file name>.prim
			 *
			 * The  numbering of primitives in this file starts
			 * in 1. The first group of data stored in this file
			 * is not being used. I just kept there for compatibility
			 * reasons and not-so-wise early design decision.
			 * All parameters related to the display (camera position,
			 * show primitives or not, etc...) is saved on a file
			 * named <obj file name>.session which is read from
			 * inside 'InitGraphics' defined in ui.c
			 */
			/* parse the growth data. Get the number of columns
			 * and rows in the growth data
			 */
			openParseGrowthData( &rg, &cg );
			
			/* parse the animation data. Get the number of rows
			 * in the animation data
			 */
			openParseAnimData( &ra, &ca );
			
			/* interpolate the values for growth and
			 animation information. TRUE means it's
			 the first time we are calling this
			 function */
			/* splineInterpGrowthData( rg, cg, TRUE); */
			linearInterpGrowthData( rg, cg, TRUE);
			/* splineInterpAnimData( ra, ca, TRUE); */
			linearInterpAnimData( ra, ca, TRUE);
		}
		
		/* Added by Thompson at 09/12/2003 */
		/* Declared in vectorField.c */
		
		create_global_list(); /* initalizes the vectors field list */
		
		if( vectorPresent )
		{
			//fprintf( stderr, "Field Vector file = %s \n", vectorFileName);
			readVectorFile( vectorFileName );
			
		}

		if ( texturePresent )
		{
			/* read in the texture file */
			readAllTextures( textureFileName );
			
		}
		
		/* if there is an experiment file present
		 we read it in */
		if ( expFilePresent ) 
			readExpFile( expFileName );
		
		/* Added by Thompson Peter Lied in 31/10/2003 */
		/* init the orientation vector */
		//compOrientation();
		
		/* anisotropy effects */
		comp_aniso_effect();
		/* end */
		
		/*
		 * Never ending loop. Control is transferred to GLUT if
		 * in GRAPHICS mode. Otherwise we are running
		 * non-interactive
		 */
		
#ifdef GRAPHICS
		InitGraphics( argc, argv );
		
#else
		/* I am running a non-interactive version */
		runNonInteractive( pfMode, relaxMode );
		
		/* Free all memory used */
		freeAll();
		
		/* Number of times cells changed faces */
		printf( "Number of times cells changed faces: %d\n", nChangeFaces );
		
		/* print log information about the timing it took to finish the simulation */
		gettimeofday( &pt, &zpt);
		finalTimeSimulation = pt.tv_sec;
		totalTimeSimulation = finalTimeSimulation - initTimeSimulation;
		printf( "Total time %f minutes (ie, %d seconds)\n",
			   totalTimeSimulation / 60.0, totalTimeSimulation);
		printf( "Total number of force computation events: %d\n",
			   totalNumOfForcesComputation );
		printf( "Number of times the total number of neighbors was below 4: %d\n",
			   nOfNeighBelowIdeal );
		printf( "Percentage %2.2f%%\n",
			   ((float) nOfNeighBelowIdeal/(float)totalNumOfForcesComputation)*100.0 );
#endif
	}	
	/* we don't have an object. We need one! */
	else usage();
}


/*
 *---------------------------------------------------------
 *	Free all memory used
 *---------------------------------------------------------
 */
void freeAll( void )
{
	
	free( vert );
	free( vertDisplay );
	free( origVert );
	free( vertn );
	free( vertt );
	free( faces );
	free( edges );
	
	freeTextures();
}

/*
 *---------------------------------------------------------
 *	runNonInteractive
 *---------------------------------------------------------
 */
/*Modified by Fabiane Queiroz in 25/11/2009 : add the Local Relaxation Process*/
void runNonInteractive( PFMODE mode, RELAXMODE rMode )
{
	int len;
	
	printf("\n\tRunning in non-interactive mode...");
	
	if ( mode == WOUTGROWTH ){
		printf("and WITHOUT growth!\n" );
		/* build the parametrization */
		parametrize();
		/* creates the random cells */
		if ( !expFilePresent ){
			printf("Creating Random Cells... (main.c) \n");
			createRandomCells( NumberCells, FALSE );
			printf( "Initial Number of Cells:\n[C] = %d [D] = %d [E] = %d [F] = %d\n",
				   nCellsType[C], nCellsType[D],
				   nCellsType[E], nCellsType[F] );
			printf("Done!\n");
			
			/* Perform the initial number of relaxations */
			initialRelaxation();
		}
		
		
		/* do the simulation */
		printf("Simulation started...\n");
		pureSimulation(rMode);
		printf("Done!\n");
	}
	else if ( mode == WITHGROWTH ){
		printf("and WITH growth!\n" );
		
		/* Do the simulation with pattern formation */
		patFormAndGrowthSimulation(rMode);
		
		/* get the name and save the object file */
		len = strlen( outputCMfileName );
		strncpy( outputObjFileName, outputCMfileName, len - 3);
		outputObjFileName[len-3]='\0';
		/* add ".obj" to the file name */
		strcat( outputObjFileName,".obj" );
		/*
		 * I have to save also the geometric model which
		 * corresponds to the final time
		 */
		
		saveObjFile( outputObjFileName );
		strncpy( outputObjFileName, outputCMfileName, len - 3);
		outputObjFileName[len-3]='\0';
		strcat( outputObjFileName,".prim" );
		savePrimitivesFile(outputObjFileName, NumberPrimitives);
		
		
	}
	
	/* print final number of cells */
	printFinalNumberOfCells(nCellsType);
	
	/* save CM file */
	printf("Saving cells file...");
	//fprintf( stderr, "###ExpFileName = %s\n", outputCMfileName);
	saveCellsFile( outputCMfileName );
	printf("Done!\n");
	
	/* compute voronoi diagram */
	/*printf("Computing Voronoi...");
	 computeVoronoi();
	 printf("Done!\n");
	 */
	
	/* save inventor file */
	/*fprintf( stderr, "Saving inventor file...\n");
	 printf("Saving inventor file...");
	 writeInventorOutput( outputCMfileName );
	 printf("Done!\n");
	 */
	
	/* inform the user the simulation is over */
	fprintf( stderr, "Simulation done!\n" );
}

/*
 *---------------------------------------------------------
 *	initParam
 *---------------------------------------------------------
 */
void initParam( byte parFilePresent )
{
	
	/* init stuff related to the relaxation */
	if ( !parFilePresent ) initRelax();
	
	/* get the radius of repulsion. This is a global variable
     declared in relax.c */
	compRadiusRepulsion( TRUE );
	
	/* keep track of how many cells of a given type we have */
	nCellsType[C] = nCellsType[D] = nCellsType[E] = nCellsType[F] = 0;
	
	/* init default information for the different types of cells
     and simulation parameters (times) */
	if ( !parFilePresent ){
		initCellsInfo();
		/* This initializes the different times for the
		 simulation, the final time, the update time
		 (which I am NOT using anymore) and current time
		 */
		initSimulationParam( FINAL_TIME, 0 );
	}
	
	/* init heap pointer */
	myheap = HeapInit(HEAP_GROW_FACTOR);
	/* init heap for triangles */
	myheapTri = HeapInitTri(HEAPTRI_GROW_FACTOR);
}

/*
 *-----------------------------------------------------------------
 *	preProcess
 *
 *	Does some pre-processing of the geometric database
 *
 *  	1. Splits polygons into triangles and compute the
 *	   subareas of these triangles since these subareas
 *	   are needed to position random points on the polygon
 *	   I'm not actually creating these triangles in the
 *	   database, I just need their areas
 *
 *  	2. Compute the partial areas of the polygons and sort
 *	   the faces array in increasing order of polygon areas
 *	   The partial areas is such that the last polygon in
 *	   the array has partial summed area of 1
 *
 *	3. Compute the best fit plane equation for the polygons
 *	   using Newell's method
 *	
 *	4. Compute the total area of the object in order
 *	   to compute the radius of repulsion
 *----------------------------------------------------------------
 */
void preProcess( void )
{
	preProcessFaces();
	
	preProcessVertices();
	
	/* compute sub-areas (triangles) for the polygons
     and total area of object */
	TotalAreaOfObject = triangleAreas();
	InitAreaOfObject = TotalAreaOfObject;
	
	fprintf( stderr, "\nTotal Area of Object = %f\n", TotalAreaOfObject );
	
	/* compute the partial sums of the areas of the
	 * polygons and then order the polygons from
	 * smallest to largest area. This is necessary
	 * to allow a binary search on the polygons
	 * by their area in order to place the random
	 * points
	 */
	partialSumArea( TotalAreaOfObject );
}
/*
 *-------------------------------------------------
 *	
 *--------------------------------------------------
 */
void setEnv( void )
{
	/* variable storing the path to find the objects */
	int 	lenPath,
	i,
	j,
	k;
	char 	*completeObjectPath,
	*runnerOnPath;
	
	completeObjectPath = (char *) malloc( sizeof( 256 ));
	runnerOnPath = (char *) malloc( sizeof( 256 ));
	completeObjectPath = getenv("ONCA_PATH");
	runnerOnPath = getenv("ONCA_PATH");
	/*
	 * strip the full path into individual paths
	 */
	lenPath = strlen( completeObjectPath );
	numberOfDir = 1;
	for( i = 0; i < lenPath; i++){
		if ( completeObjectPath[i] == ':') numberOfDir++;
	}
	
	pathDir = allocate2DArrayChar( numberOfDir, 256 );
	
	for(i = 0, j = 0; i < numberOfDir; i++){
		k = 0;
		while( completeObjectPath[j] != ':' && j < lenPath ){
			pathDir[i][k++] = completeObjectPath[j++];
		}
		pathDir[i][k++] = '/';
		pathDir[i][k] = '\0';
		j++;
	}
	
}

/*
 * This procedure set a common vector, call EixoReferencia and a matrix to create a
 * new co-ordinated system.
 */
void createFaceSystem(int faceIndex)
{
	Point3D normal, axleY, v2;
	
	axleY.x = 0.0;
	axleY.y = 1.0;
	axleY.z = 0.0;
	
	normal.x = faces[faceIndex].planeCoef.a;
	normal.y = faces[faceIndex].planeCoef.b;
	normal.z = faces[faceIndex].planeCoef.c;
	
	V3Cross( &axleY, &normal,  &(faces[faceIndex].EixoReferencia) );
	V3Normalize(&(faces[faceIndex].EixoReferencia));
	
	V3Cross( &normal, &(faces[faceIndex].EixoReferencia), &v2 );
	V3Normalize(&v2);
	
	/*
	 * See compMappingMatrices(int whichFace) for more information.
	 */
	faces[faceIndex].MatrixMapTo3D.element[0][0] = faces[faceIndex].EixoReferencia.x;
	faces[faceIndex].MatrixMapTo3D.element[0][1] = v2.x;
	faces[faceIndex].MatrixMapTo3D.element[0][2] = normal.x;
	faces[faceIndex].MatrixMapTo3D.element[0][3] = faces[faceIndex].center3D.x;
    
	faces[faceIndex].MatrixMapTo3D.element[1][0] = faces[faceIndex].EixoReferencia.y;
	faces[faceIndex].MatrixMapTo3D.element[1][1] = v2.y;
	faces[faceIndex].MatrixMapTo3D.element[1][2] = normal.y;
	faces[faceIndex].MatrixMapTo3D.element[1][3] = faces[faceIndex].center3D.y;
	
	faces[faceIndex].MatrixMapTo3D.element[2][0] = faces[faceIndex].EixoReferencia.z;
	faces[faceIndex].MatrixMapTo3D.element[2][1] = v2.z;
	faces[faceIndex].MatrixMapTo3D.element[2][2] = normal.z;
	faces[faceIndex].MatrixMapTo3D.element[2][3] = faces[faceIndex].center3D.z;
	
	faces[faceIndex].MatrixMapTo3D.element[3][0] = 0.0;
	faces[faceIndex].MatrixMapTo3D.element[3][1] = 0.0;
	faces[faceIndex].MatrixMapTo3D.element[3][2] = 0.0;
	faces[faceIndex].MatrixMapTo3D.element[3][3] = 1.0;
	
	/* get the invert matrix */
	MxInvert( &(faces[faceIndex].MatrixMapTo3D), &(faces[faceIndex].MatrixMapTo2D));
	
	//MatMul(  &(faces[faceIndex].MatrixMapTo3D), &(faces[faceIndex].MatrixMapTo2D), &(tmp) );
    //checkIdentity( &tmp );
}

/*
 *------------------------------------------------
 *------------------------------------------------
 */
void preProcessFaces( void )
{
	int i;
	/*Point3D normal, v1, v2;
	 Matrix4 tmp;*/
	
	/*
	 * Init all faces as NO hit, areas to 0 and compute
	 * polygon's plane equation
	 */
	for(i=0; i < NumberFaces; i++)
	{
		faces[i].hit = FALSE;
		faces[i].haveVector = FALSE;	/* Thompson 18/03/2004 */
		/*Tira*///faces[i].neighbors = NONE;		/* Thompson 19/03/2004 */
		faces[i].relArea = 0.0;
		faces[i].absArea = 0.0;
		faces[i].prevAbsArea = 0.0;
		faces[i].partArea = 0.0;
		faces[i].ncells = 0;
		faces[i].nCellsType[C] = 0;
		faces[i].nCellsType[D] = 0;
		faces[i].nCellsType[E] = 0;
		faces[i].nCellsType[F] = 0;
		faces[i].rates[C] = -1; 
		faces[i].rates[D] = -1;
		faces[i].rates[E] = -1; 
		faces[i].rates[F] = -1;
		faces[i].nVorPol = 0;
		/* The number of sub-triangles in a generic polygon is always
		 the number of vertices - 2 */
		faces[i].areas = (float *) malloc( sizeof (float) * (faces[i].nverts - 2));
		if ( faces[i].areas == NULL ) 
			errorMsg("No space for sub areas! (main.c)");
		
		/*
		 * Init head and tail nodes for the linked list
		 * of cells
		 */
		faces[i].head = cellAlloc();
		faces[i].tail  = cellAlloc();
		faces[i].head->next = faces[i].tail;
		faces[i].vorPoints = (voronoiType *) NULL;
		faces[i].vorFacesList = (faceType *) NULL;
		
		/* number of adjacent faces for each face.
		 * In a Winged-Edge data structure the number of adjacent faces
		 * which share an edge is assumed to be the same as the number
		 * of vertices (or at least I am assuming this!). I am
		 * defining a new variable (nPrimFaces) just to
		 * make the code more readable
		 */
		faces[i].nPrimFaces = faces[i].nverts;
		
		/* Allocate space for the adjacent faces list */
		getMemAdjacentFacesList( i , faces[i].nPrimFaces );
		
		/* compute plane coefficients for each face */
		/*PlaneEquation( i, &(faces[i].planeCoef) );*/
		
		/*
		 * Compute a local orientation system for this polygon
		 */
		compMappingMatrices( i );
		/*
		 * Compute geometric center of face. I am not using this
		 * yet
		 */
		computeGeometricCenter( i, faces[i].nverts );
		/*
		 * Check if all faces are given in counterclockwise order
		 */
		checkCCWordering( i, faces[i].nverts );
		
		/*
		 * Added by Vinicius at 22/04/2005
		 * To create a vector field using angle between this vector and the user's vector.
		 */
		createFaceSystem( i );
	}
}



/*
 *------------------------------------------------
 *------------------------------------------------
 */
void getMemAdjacentFacesList( int whichFace, int howManyAdjFaces )
{
	faces[whichFace].primFaces = (int *) malloc ( sizeof( int ) * howManyAdjFaces );
	if ( faces[whichFace].primFaces == NULL )
		errorMsg("No space left for primary faces! (main.c)");
	
	faces[whichFace].rotAngles = (double *) malloc ( sizeof( double ) * howManyAdjFaces );
	if ( faces[whichFace].rotAngles == NULL )
		errorMsg("No space left for angles of adjacent faces! (main.c)");
	
	faces[whichFace].primEdges = (int *) malloc ( sizeof( int ) * howManyAdjFaces );
	if ( faces[whichFace].primEdges == NULL )
		errorMsg("No space left for primary edges! (main.c)");
}

/*
 *------------------------------------------------
 *------------------------------------------------
 */
void
checkCCWordering( int whichFace, int howManyVert )
{
	int i;
	Point2D p, v;
	Point2D q0, q1, q2;
	
	for(i = 0; i  < howManyVert - 2; i += 2 ){
		mapOntoPolySpace( whichFace,
						 vert[faces[whichFace].v[i]].pos.x,
						 vert[faces[whichFace].v[i]].pos.y,
						 vert[faces[whichFace].v[i]].pos.z,
						 &q0);
		mapOntoPolySpace( whichFace,
						 vert[faces[whichFace].v[((i+1)%howManyVert)]].pos.x,
						 vert[faces[whichFace].v[((i+1)%howManyVert)]].pos.y,
						 vert[faces[whichFace].v[((i+1)%howManyVert)]].pos.z,
						 &q1 );
		mapOntoPolySpace( whichFace,
						 vert[faces[whichFace].v[((i+2)%howManyVert)]].pos.x,
						 vert[faces[whichFace].v[((i+2)%howManyVert)]].pos.y,
						 vert[faces[whichFace].v[((i+2)%howManyVert)]].pos.z,
						 &q2 );
		p.x = q2.x - q0.x;
		p.y = q2.y - q0.y;
		v.x = q1.x - q0.x;
		v.y = q1.y - q0.y;
		
		if ( (v.x * p.y - p.x * v.y) < 0.0 ){
			fprintf( stderr, "face %d z = %lg\n", whichFace, v.x * p.y - p.x * v.y );
			errorMsg("Face not given in clockwise order! (main.c)");
		}
	}
}
/*
 *------------------------------------------------
 *------------------------------------------------
 */
void preProcessVertices( void )
{
	int i;
	
	/* init parametrization info for vertices as no parametrization */
	for(i=0; i < NumberVertices; i++){
		vert[i].prim1 = 0; vert[i].prim2 = 0;
		/* copies into the display list the original vertices */
		vertDisplay[i].x = vert[i].pos.x;
		vertDisplay[i].y = vert[i].pos.y;
		vertDisplay[i].z = vert[i].pos.z;
		/* keep track of max and min in vertices set */
		minMax(vert[i].pos, &maxSet, &minSet);
		
		/* initialize 'hit' field as FALSE for all vertices */
		vert[i].hit = FALSE;
	}
}

/*
 *--------------------------------------------------
 *	This routine gets the group information
 *	provided in the original obj file.
 * 	The group info is a string with the name
 * 	of the group. I translate this name
 * 	into a numeric value.
 * 	The grouping information is stored
 *	together with each face, both the string
 *	and the numeric values
 *--------------------------------------------------
 */
void
processGroups( void )
{
	int i,ii,currentg;
	char group[50][20];
	int flag=TRUE;
	
	/* initialize array of names of groups */
	for(ii=0;ii<50;ii++) strcpy(group[ii],"blah");
	
	currentg = 0;
	strcpy(group[NumberGroups],faces[0].group);
	faces[0].g = currentg;
	for (i=0; i < NumberFaces; i++){
		for(ii=0;ii<50;ii++){
			if(!strcmp(faces[i].group,group[ii])){ /* found the same group */
				flag=FALSE;
				currentg = ii;
			}
		}
		if(flag){	/* found a different group */
			NumberGroups++;
			strcpy(group[NumberGroups],faces[i].group);
			currentg = NumberGroups;
		}
		faces[i].g = currentg;
		flag=TRUE;
	}
	/* printf("The total number of groups is %d\n", numberOfGroups); */
}	


/*
 *--------------------------------------------------
 *--------------------------------------------------
 */
void usage( void )
{
#ifdef GRAPHICS
	printf("Usage: onca.sgi ");
#else
	printf("Usage: onca.irix ");
#endif
	printf("\n-f <name of an input object description file> (MANDATORY!)\n");
	printf("-e <name of an experiment file (*.cm)> (OPTIONAL!)\n");
	printf("-p <name of a parameter file (*.par)> (OPTIONAL!)\n");
	printf("-m <name of a primitive file (*.prim)> (OPTIONAL!)\n");
	printf("-t <name of a texture file (*.txtr)> (OPTIONAL!)\n");
	printf("-d <name of a geodesic distance file (*.dis)> (OPTIONAL!)\n");
	printf("-v <name of a control\'s vectors file (*.cvf)> (OPTIONAL!)\n");
	printf("-c <name of a vector field file (*.vf)> (OPTIONAL!)\n");
	printf("-h Help\n");
	exit(1);
}
