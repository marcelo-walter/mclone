/*
 *------------------------------------------------------------
 *	menus.c
 *	Marcelo Walter
 *
 *	This module handles all OpenGL menus
 *------------------------------------------------------------
 */

#include <stdlib.h>
/*#include <forms.h>*/

#include "common.h"

#include <GL/glut.h>
//#include <GLUT/glut.h>
#include "texture.h"

#include "menus.h"

/*#include "gl2ps.h"*/


/*
 *------------------------------------------------------------
 *		External global definitions
 *------------------------------------------------------------
 */
extern char sessionFileName[128];
extern char primitivesFileName[128];
extern char outputCMfileName[128];
extern char expFileName[128];

extern flag lightFlag;
extern flag stopAnimation;
extern flag textureFlag;
extern flag showAxes;
extern flag windowSplitFlag;
extern flag showLandMarks;
extern flag growthFlag;
extern flag animFlag;
extern flag textCoordComputed;
extern flag vectorsFlag;
extern flag normalsFlag;
extern flag voronoiColorFlag;
extern flag voronoiBorders;
extern byte expFilePresent;
extern flag bwOutput;
extern flag degreeVector;	/* Thompson 07/11/2003 */

extern flag createVect;
extern flag removevect;
extern flag drawVector;
extern flag drawVectorField;
extern flag drawVerticesVectorField;

extern flag useGeodeiscDistances;
extern int WithInterpolationType;

extern VMODE viewMode;
extern RMODE renderMode;
/* mouse speed for panning */
extern float panSpeed;

extern int growthTime;
extern RMODE renderMode;
extern MMODE modelMode;

/*add by Fabiane Queiroz*/
extern flag edgesFlag;
extern flag cellPicking;
extern flag showPrimitives;
extern flag drawCells;
extern flag polygonPicking;
extern flag objectFlag;
extern flag colorPrimFlag;
extern flag gridFlag;


/* Added by Thompson at 11/12/2003 */
/*Tira*///extern GLfloat backClipPlane;

/*
 *--------------------------------------------------
 *	Local Global variables
 *--------------------------------------------------
 */


/* menu identifiers */
int mainMenuIdent, primitiveMenuIdent, printMenuIdent;
int dispMenuIdent, viewMenuIdent, renderMenuIdent;
int vectorMenuIdent;
int savingMenuIdent;

int typeDistanciaMenuIdent;
int typeVectorFieldMenuIdent;

/*
 *----------------------------------------------------------
 *	initMenu
 *	Initializes all menu options
 *----------------------------------------------------------
 */
void initMenus( void )
{
	
    mainMenuIdent = glutCreateMenu( mainMenu );
    primitiveMenuIdent = glutCreateMenu( primitiveMenu );
    printMenuIdent = glutCreateMenu( printMenu );
    dispMenuIdent = glutCreateMenu( dispMenu );
    viewMenuIdent = glutCreateMenu( viewMenu );
    renderMenuIdent = glutCreateMenu( renderMenu );
	vectorMenuIdent = glutCreateMenu( vectorFieldMenu );
	savingMenuIdent = glutCreateMenu( savingMenu );
	
	typeDistanciaMenuIdent = glutCreateMenu( typeDistance );
	
	
    /* Create primitive submenu */
    glutSetMenu( primitiveMenuIdent );
    glutAddMenuEntry("Cylinder", CYLINDER);
    glutAddMenuEntry("LandMark", LANDMARK);
    glutAddMenuEntry("Sphere", SPHERE);
	
    /* Render submenu */
    glutSetMenu( renderMenuIdent );
    glutAddMenuEntry( "Fill", FILL);
    glutAddMenuEntry( "Wire", WIRE);
    glutAddMenuEntry( "Halo", HALO);
    glutAddMenuEntry( "Edges", EDGES);
	
	/* Create type distance submenu*/
	glutSetMenu( typeDistanciaMenuIdent );
    glutAddMenuEntry("Straight Path", 0);
    glutAddMenuEntry("Straight Path using angle", 1);
	if (useGeodeiscDistances)
	{
		glutAddMenuEntry("Geodesic Path", 2);
		glutAddMenuEntry("Geodesic Path using angle", 3);
	}
	
	/*Created by Fabiane Qeiroz at 23/09/2010 
	 Create type Vector Field*/
	glutSetMenu( typeVectorFieldMenuIdent );
	glutAddMenuEntry("Faces Vector Field (Default)", 0);
	glutAddMenuEntry("Vertices Vector Field", 1);
	
    /* Display submenu */
    glutSetMenu( dispMenuIdent );
    glutAddMenuEntry( "Primitives", 0);
    glutAddMenuEntry( "LandMarks", 1);
    glutAddMenuEntry( "Axes", 2);
    glutAddMenuEntry( "Object", 3);
    glutAddMenuEntry( "Color", 4);
    glutAddMenuEntry( "Grid", 5);
    glutAddMenuEntry( "Vectors", 6);
    glutAddMenuEntry( "BW output", 7);  /* Thompson 05/07/2002 */
    glutAddMenuEntry( "Voronoi Borders", 8);
    glutAddMenuEntry( "Cells", 9);
	glutAddMenuEntry( "Degree Vector", 10); /* Thompson 07/11/2003 */
	glutAddMenuEntry( "Normal Vector", 11); /* Vin?cius 17/09/2004 */
	
	
    /* Print information submenu */
    glutSetMenu( printMenuIdent );
    glutAddMenuEntry("Vertices", 0);
    glutAddMenuEntry("Faces", 1);
    glutAddMenuEntry("Primitives", 2);
    glutAddMenuEntry("Active Prim", 3);
    glutAddMenuEntry("Edges", 4);
    glutAddMenuEntry("Cells", 5);
	
    /* type of view: top, side, front or perspective submenu */
    glutSetMenu( viewMenuIdent );
    glutAddMenuEntry("Top", TOP);
    glutAddMenuEntry("Side", SIDE);
    glutAddMenuEntry("Front", FRONT);
    glutAddMenuEntry("Perspective", PERSPECTIVE);
	
	/* Vector field submenu */
    glutSetMenu( vectorMenuIdent );
	glutAddMenuEntry("Create Vector", 0);
    //glutAddMenuEntry("Initial Point", 0);
    //glutAddMenuEntry("Final Point", 1);
    glutAddMenuEntry("Draw Control Vectors", 1);
	glutAddMenuEntry("Print List of Vectors", 2);
	glutAddMenuEntry("Draw Faces Vector Field", 3);
	 glutAddMenuEntry("Draw Vertices Vector Field", 4);
	glutAddMenuEntry("Remove Vector", 5);
	
	glutAddSubMenu("Type Distance", typeDistanciaMenuIdent );
	
	/* Saving submenu */
	glutSetMenu( savingMenuIdent );
	glutAddMenuEntry("Write Optik", 0);
    glutAddMenuEntry("Write Vertigo", 1);
    glutAddMenuEntry("Save Exp File", 2);
    glutAddMenuEntry("Save Session File", 3);
    glutAddMenuEntry("Save Prim File", 4);
    glutAddMenuEntry("Save Inventor File", 5);
    glutAddMenuEntry("Save Obj File", 6);
	glutAddMenuEntry("Save Pattern File", 7);
	glutAddMenuEntry("Save Control Vectors Field", 8);
	glutAddMenuEntry("Save Vectors Field", 9);
	
	glutSetMenu( mainMenuIdent );
	/* sub menus */
    glutAddSubMenu("Create Primitive", primitiveMenuIdent);
    glutAddSubMenu("Print Info", printMenuIdent);
    glutAddSubMenu("Rendering", renderMenuIdent);
    glutAddSubMenu("View", viewMenuIdent);
    glutAddSubMenu("Display", dispMenuIdent);
	glutAddSubMenu("Vector Field", vectorMenuIdent);
	glutAddSubMenu("Saving", savingMenuIdent);
	
    /* individual options */
    glutAddMenuEntry("Zoom out", 0);
    glutAddMenuEntry("Light", 1);
    glutAddMenuEntry("4 Windows", 2);
    glutAddMenuEntry("Parametrize", 3);
    glutAddMenuEntry("Texture", 4);
    glutAddMenuEntry("Compute Voronoi", 5);
    glutAddMenuEntry("Cell Picking", 6);
    glutAddMenuEntry("Poly Picking", 7);
    glutAddMenuEntry("Random Cells", 8);
    glutAddMenuEntry("Duplicate Prim", 9);
    glutAddMenuEntry("Grow", 10);
    glutAddMenuEntry("Animate", 11);
    glutAddMenuEntry("Exit", 12);
	
    /* specify which mouse button activates the menu */
    glutAttachMenu( GLUT_RIGHT_BUTTON );
}

/*
 *----------------------------------------------------------
 *	Type Distance Menu - type the distance used by RBF
 *----------------------------------------------------------
 */
void typeDistance(int item)
{
	WithInterpolationType = item;
    SetaTipoRBF();
	glutPostRedisplay();
}



/*
 *----------------------------------------------------------
 *	View Menu - type of projection
 *----------------------------------------------------------
 */
void viewMenu(int item)
{
    viewMode = (VMODE) item;
    glutPostRedisplay();
}

/*
 *----------------------------------------------------------
 *	Render Sub Menu
 *----------------------------------------------------------
 */
void renderMenu(int item)
{
    switch(item){
		case FILL:
			renderMode = (RMODE) item;
			edgesFlag = TRUE;
			break;
		case WIRE:
			renderMode = (RMODE) item;
			edgesFlag = FALSE;
			break;
		case HALO:
			renderMode = (RMODE) item;
			break;
		case EDGES:
			edgesFlag = !edgesFlag;
		default:
			break;
    }
    glutPostRedisplay();
}
/*
 *----------------------------------------------------------
 *	Display Sub Menu
 *----------------------------------------------------------
 */
void dispMenu(int item)
{
	switch(item){
		case 0:
			showPrimitives = !showPrimitives;
			break;
		case 1:
			showLandMarks = !showLandMarks;
			break;
		case 2:
			showAxes = !showAxes;
			break;
		case 3:
			objectFlag = !objectFlag;
			break;
		case 4:
			colorPrimFlag = !colorPrimFlag;
			break;
		case 5:
			gridFlag = !gridFlag;
			break;
		case 6:
			vectorsFlag = !vectorsFlag;
			break;
		case 7:
			if(!bwOutput ) 
				glClearColor( 1, 1, 1, 0 );
			else 
				glClearColor( 0.9, 0.9, 0.9, 0 );
			//glClearColor( 0.7, 0.7, 0.7, 0 );
			bwOutput = !bwOutput;
			break;
		case 8:
			/*voronoiColorFlag = !voronoiColorFlag;*/
			voronoiBorders = !voronoiBorders;
			break;
		case 9:
			drawCells = !drawCells;
			break;
		case 10: /* Thompson 07/11/2003 */
			degreeVector = !degreeVector;
			break;
		case 11:
			normalsFlag = !normalsFlag;
			break;
		default:
			break;
	}
	glutPostRedisplay();
}
/*
 *----------------------------------------------------------
 *	Primitive Sub Menu
 *----------------------------------------------------------
 */
void primitiveMenu(int item)
{
	switch(item){
		case CYLINDER:
			createPrimitive(CYLINDER);
			break;
		case LANDMARK:
			/* createPrimitive(LANDMARK); */
			createLandMark();
			break;
		case SPHERE:
			createPrimitive(SPHERE);
			break;
		default:
			break;
	}
	
	glutPostRedisplay();
}
/*
 *----------------------------------------------------------
 *	PrintMenu
 *----------------------------------------------------------
 */
void printMenu(int item)
{
	switch(item){
		case 0:
			printVertexInfo();
			break;
		case 1:
			printFacesInfo();
			break;
		case 2:
			printInfoPrim( NumberPrimitives );
			break;
		case 3:
			printActivePrim();
			break;
		case 4:
			printEdges();
			break;
		case 5:
			printCellsInfo();
			/*printCellsTimeSplit();*/
			break;
		default:
			break;
	}
	glutPostRedisplay();
}
/*
 *----------------------------------------------------------
 *	MainMenu
 *----------------------------------------------------------
 */
void mainMenu(int item)
{
	int i;
	
	switch(item){
		case 0:
			/* HACK! Moves the camera back by globalScale amount */
			Prim[WORLD].trans[Z] *= Prim[WORLD].cosine.z;
			showLandMarks = FALSE;
			showPrimitives = FALSE;
			panSpeed *= 16.0;
			break;
		case 1:
			lightFlag = !lightFlag;
			break;
		case 2:
			windowSplitFlag = !windowSplitFlag;
			break;
		case 3:
			parametrize();
			break;
		case 4:
			textureFlag = !textureFlag;
			if ( textureFlag )
			{
				if ( !textCoordComputed )
				{
					compTextCoordinates();
				}
				glEnable(GL_TEXTURE_2D);
			}
			else
				glDisable(GL_TEXTURE_2D);
			break;
		case 5:
			computeVoronoi();
			break;
		case 6:    /* added by Thompson 21/05/2002 */
			cellPicking = !cellPicking;
			if( cellPicking )
				glutSetCursor( GLUT_CURSOR_INFO );
			else
				glutSetCursor( GLUT_CURSOR_LEFT_ARROW );
			break;
		case 7:{
			polygonPicking = !polygonPicking;
			if ( polygonPicking ) 
			{glutSetCursor( GLUT_CURSOR_INFO );}
			else {glutSetCursor( GLUT_CURSOR_LEFT_ARROW );}
		}
			break;
		case 8:
			drawCells = TRUE;
			createRandomCells( NumberCells, FALSE );
			break;
		case 9:
			duplicatePrimitive();
			break;
		case 10:
			growthFlag = !growthFlag;
			break;
		case 11:
			animFlag = !animFlag;
			break;
		case 12:
			exit(0);
		default:
			break;
	}
	glutPostRedisplay();
}


/*
 *----------------------------------------------------------
 *	savingMenu
 *  Added by Thompson Peter Lied at 08/03/2004 
 *----------------------------------------------------------
 */
void savingMenu(int item)
{ 
	switch(item)
	{
		case 0:
			writeOptikOutput( sessionFileName );
			break;
		case 1:
			writeVertigoOutput( sessionFileName );
			break;
		case 2:
			saveCellsFile( expFileName );
			break;
		case 3:
			saveSessionFile( sessionFileName );
			break;
		case 4:
			savePrimitivesFile( primitivesFileName, NumberPrimitives);
			break;
		case 5:
			if ( expFilePresent )
				writeInventorOutput( expFileName );
			else
				writeInventorOutput( "foo.iv" );
			/* saveInventorFile(); */
			break;
		case 6:
			saveObjFile( sessionFileName );
			break;
			/* Added by Thompson Peter Lied in 23/08/2002 */
			/* This function writes the pattern file and material library
			 */
		case 7:
			savePatternFile( sessionFileName );
			break;
			/* end */
		case 8:
			saveVectors( sessionFileName );
			break;
		case 9:
			saveVectorField( sessionFileName );
			break;
		default:
			break;
	}
	glutPostRedisplay();
	
}

/*
 *----------------------------------------------------------
 *	Vector Field Sub Menu
 *
 *	Added by Thompson Peter Lied at 03/12/2003
 *----------------------------------------------------------
 */
void vectorFieldMenu(int item)
{
	switch(item)
	{
		case 0:
			if( createVect )
			{
				printString( 0, " ==> A vector alredy was created!");
			}
			else
			{
				createVect = TRUE;
				showAxes = FALSE; //Because we dont want to hit de axes printed
				drawVector = TRUE;
				/* Incluir cursor e indicar que estah criando o ponto inicial ou final */
			}
			break;
		case 1:
			drawVector = !drawVector;
			break;
		case 2:
			printListOfVectors();
			break;
			/*Tira*//*case 3:
			 //neighborhood();
			 vectorField = !vectorField;
			 break;*/
		case 3:
			drawVectorField = !drawVectorField;
			break;
		case 4:
			drawVerticesVectorField = !drawVerticesVectorField;
			break;
		case 5:
			removevect = TRUE;
		default:
			break;
	}
	glutPostRedisplay();
}

/* Added by Thompson Peter Lied in 9/12/2002 */
void saveInventorFile( void )
{
	const char *ff;
	char *cwd;
	
	ff = (char *) malloc( sizeof ( char) * 512 );
	
	if ((ff = getcwd(NULL, 512)) == NULL){
		errorMsg("Could not get current working directory!");
		exit(2);
	}
	
	if ( ff != NULL ){
		printf("Inventor file name = %s \n", ff);
		
		writeInventorOutput( ff );
	}
	
}

