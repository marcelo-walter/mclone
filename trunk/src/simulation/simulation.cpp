/*
 *---------------------------------------------------------
 *      simulation.c
 *---------------------------------------------------------
 */

#include "simulation.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifdef GRAPHICS
#ifdef __APPLE__
    #include "GLUT/glut.h"
#else
    #include "GL/glut.h"
#endif
#endif

#include "control/vectorField.h"
#include "control/wingEdge.h"
#include "control/primitives.h"
#include "control/Growth.h"
#include "util/heap.h"
#include "util/genericUtil.h"
#include "util/distPoints.h"
#include "util/transformations.h"
#include "data/cellsList.h"
#include "data/Object.h"
#include "data/Point3D.h"
#include "data/Matrix4.h"
#include "data/cells.h"
#include "data/Parameters.h"
#include "data/fileManager.h"
#include "relax.h"
#include "morph.h"

/*
 *--------------------------------------------------
 *      Local Global variables
 *--------------------------------------------------
 */

int computingPatternFlag = 0; //From Globals.h

/*Add by Fabiane Queiroz*/
flag    originalScale = TRUE; //LOCAL

FILE *fpRates; //LOCAL


/*
 *--------------------------------------------------------------
 *
 *
 *--------------------------------------------------------------
 */
/* Modified by Fabiane Queiroz in 25/11/2009 */

void patFormAndGrowthSimulation( RELAXMODE rMode )
{
    EVENT *a;
    float time, deltaTime;
    int countIterations = 0;


    /*Add by Fabiane Queiroz in 28/11/2009*/
        // Array lFaces;
        // lFaces.array= NULL;

    Array listFaces;
    int  max = 1;
    listFaces.array = (int*)malloc(sizeof(int)*max);
    listFaces.array = NULL;
    listFaces.size = 0;

#ifdef GRAPHICS
    /* change cursor into a watch */
    glutSetCursor( GLUT_CURSOR_WAIT );
#endif


    /*
         * Init the necessary information to run the pattern
         * formation and growth together:
         * - change the model to an embryo
         * - create the initial set of cells
         * - do the initial relaxation
         * - compute the first round of splitting
         *   rates for cells
         */
    initPatFormAndGrowth(); //removed rMode parameter //Bins


    printf("Number of days = %f\n", finalTime );
    deltaTime = 1.0 / (float) iterPerUnitTime;
    printf("DeltaTime: %f\n", deltaTime);
    time = 0.0;

    while( time < finalTime )
    {
                a = HeapRemove( myheap );
                while((time <= a->eventTime) && (time < finalTime))
                {

                        /*fprintf( stderr, "Handling relaxation event. Current time = %f\n", time );*/
                        //fprintf( stderr, "Total Number of Cells = %d\n", NumberCells );

                        if(time<=40)
                                relaxation( 1, rMode, &listFaces );

                        time += deltaTime;
                        // printf("time: %f \n ",time);
                        countIterations++;

                        /*
                         * The notion of one day here is given by
                         * the variable 'iterPerUnitTime'. Every time
                         * we reached that count a 'day' has passed
                         */
                        if ( countIterations == iterPerUnitTime )
                        {
                                countIterations = 0;    /* reset the loop counter */
                                fprintf( stderr, "\n\nCurrent day is %d\n", (int)(time + 0.5) );
                                //printf( "\nCurrent day is %d\n", (int)(time + 0.5) );

                                /*Add by Fabiane Queiroz
                                 Change the primitives scale.*/
                                if(originalScale == FALSE)
                                {
                                        int j;
                                        printf("Back to Original Scale in (simulation.c)...\n");
                                        for(j=1; j < NumberPrimitives; j++)
                                        {
                                                checkPrimitivesFile();
                                        }
                                }

                                /*
                                 * Before we can change the embryo, we have to
                                 * recompute the cell's baricentric coordinates
                                 * in order to be able to remap them to the new
                                 * model
                                 */

                                updtBaryCentricCoord();
                                /*
                                 * Now we change the geometric model to current time
                                 */

                                printf("Old Area of Object: %f \n", TotalAreaOfObject);


                                computeBodyEmbryo( (int)(time + 0.5));

                                //morph((int)(time + 0.5));

                                /*Add by Fabiane Queiroz: Change the primitives scale.*/
                                int j;
                                //float oldScaleX, oldScaleY, oldScaleZ;
                                printf("Changing Scale in (simulation.c)...\n");
                                for(j=1; j < NumberPrimitives; j++)
                                {
                                        if(Prim[j].type == CYLINDER){

                                                Prim[j].scale[X] = (Prim[j].scale[X]+Prim[j].h);
                                                Prim[j].scale[Y] = (Prim[j].scale[Y]+Prim[j].r);
                                                Prim[j].scale[Z] = (Prim[j].scale[Z]+Prim[j].r);

                                                Prim[j].trans[X] = Prim[j].trans[X] + Prim[j].center.x;
                                                Prim[j].trans[Y] = Prim[j].trans[Y] + Prim[j].center.y;
                                                Prim[j].trans[Z] = Prim[j].trans[Z] + Prim[j].center.z;

                                        }

                                }



                                /*
                                 * Find new area of object and update previous and
                                 * current values for areas of triangles
                                 */
                                TotalAreaOfObject = triangleAreas();
                                partialSumArea( TotalAreaOfObject );

                                compRadiusRepulsion( TRUE );

                                fprintf( stderr, "New Area of Object = %f Total Number of Cells = %d\n",
                                                TotalAreaOfObject, NumberCells );
                                fprintf( stderr, "By type: %d C cells %d D cells\n",
                                                nCellsType[C], nCellsType[D] );


                                /*
                                 * Every time the model changes, we have to recompute
                                 * the rates of division. The pointer to a file is a tmp
                                 * debug. I am printing into a file the computed rates
                                 */

                                /*For cow, the division and anisotropy effect stop at day 5 (window between  day 22 and 62)*/

                                compSplittingRates( fpRates, (int)(time + 0.5) );

                                 if(time>40)
                                 {

                                        Ani = 0.0;
                                 }



                                /*
                                 * Re-map the cells according to the current geometric
                                 * model
                                 */

                                morphCells();



                                /*
                                 * Recompute the cells coordinates in the polygon space
                                 */
                                compCellsPolygonCoordinates(&listFaces);


                        }
                }
        //      fprintf( stderr, "Handling split event time = %f split = %f\n",
                // time, a->eventTime );
                if(time<=40){
                handleSplit( a->info );
                free ( a );
                }

    }

    /* I think I need this here for the Voronoi part */
    compCellsPolygonCoordinates(&listFaces);

    /*
         * The simulation has ended. We can clear the heap.
         * There might be SPLIT events left but that's ok
         */
    myheap = ClearHeap( myheap );

    /* Close the file with the rates info */
    fclose ( fpRates );

#ifdef GRAPHICS
    /* change cursor back to original left arrow format */
    glutSetCursor( GLUT_CURSOR_LEFT_ARROW );
#endif
}
/*
 *--------------------------------------------------------------
 *
 *--------------------------------------------------------------
 */
void updtBaryCentricCoord( void )
{

        int i;
        CELL *c;

        for( i = 0; i < NumberFaces; i++ ){
                c = faces[i].head->next;
                while( c != faces[i].tail ){
                        compBarycentricCoord( c );
                        c = c->next;
                }
        }
}
/*
 *--------------------------------------------------------------
 *
 *--------------------------------------------------------------
 */
/* Modified by Fabiane Queiroz in 25/11/2009 */
void initPatFormAndGrowth( ) //Remove parameter  RELAXMODE rMode by Bins
{
    /*Add by Fabiane Queiroz in 28/11/2009*/
    Array listFaces;
    //listFaces.array = malloc(sizeof(int)*max);

    listFaces.array = NULL;
    listFaces.size = 0;

    printf("initPatFormAndGrowth\n");
        /*
         * This flags that I am in the process of computing the pattern
         * together with growth. It will be useful in some other
         * areas of the code. MANY THINGS DEPEND ON THIS FLAG!!
         *
         * I will try to list here place where this flag is used:
         * - when morphing the object, I interpolate the values
         *   for landmarks between EMBRYOAGE and NEWBORNAGE
         * - when creating new cells, I do not want to change
         *   the value for the radius of repulsion
         */
        computingPatternFlag = TRUE;

        /* Debug file */
        if ( (fpRates = fopen( "ratesDivison", "w" )) == NULL)
                errorMsg("Could not open rates file to write!");

        /* build the parametrization */
        fprintf( stderr, "Parameterizing the model...");
        parametrize();
        fprintf( stderr, "Done!\n" );

        /*
         * If the model has been parameterized, we can proceed and
         * change it to some time BEFORE embryo. The value -1
         * flags to change the model to SOME AGE before EMBRYO
         */
        if ( parametOKFlag ){
                 computeBodyEmbryo( -1 );
        }
        else
                errorMsg("Trying to run simulation without parametrizing the model!");

        /*
         *
         */
        TotalAreaOfObject = triangleAreas();
        partialSumArea( TotalAreaOfObject );
        fprintf( stderr, "\nOld Area of Object = %f\n", TotalAreaOfObject );

        /*
         * Now we change the geometric model to the EMBRYO age
         */


        /*Add by Fabiane
         If the model has scale changes*/
        if(originalScale == FALSE){

                int j;
                float oldScaleX, oldScaleY, oldScaleZ;
                printf("Back to Original Scale in (simulation.c)...\n");
                for(j=1; j < NumberPrimitives; j++){
                        // Prim[j].scale[0] = (Prim[j].scale[0]/Prim[j].h);
                        //Prim[j].scale[1] = (Prim[j].scale[1]/Prim[j].r);
                        // Prim[j].scale[2] = (Prim[j].scale[2]/Prim[j].r);
                        checkPrimitivesFile();
                        // printf("Teste da escala (newScaleX): %f\n",Prim[j].scale[X] );
                        // }
                }
        }

        computeBodyEmbryo( EMBRYOAGE );
        TotalAreaOfObject = triangleAreas();
        partialSumArea( TotalAreaOfObject );
        fprintf( stderr, "New Area of Object = %f\n", TotalAreaOfObject );


        int j;
        float oldScaleX, oldScaleY, oldScaleZ;
        printf("...Changing Scale in (simulation.c)...\n");
        for(j=1; j < NumberPrimitives; j++){
                //printf("R da Primitiva: %f H da Primitiva: %f\n",Prim[j].r, Prim[j].h);
                if(Prim[j].type == CYLINDER){

                        Prim[j].scale[X] = (Prim[j].scale[X]+Prim[j].h);
                        Prim[j].scale[Y] = (Prim[j].scale[Y]+Prim[j].r);
                        Prim[j].scale[Z] = (Prim[j].scale[Z]+Prim[j].r);

                        Prim[j].trans[X] = Prim[j].trans[X] + Prim[j].center.x;
                        Prim[j].trans[Y] = Prim[j].trans[Y] + Prim[j].center.y;
                        Prim[j].trans[Z] = Prim[j].trans[Z] + Prim[j].center.z;

                }

        }
        if(originalScale == TRUE)
                originalScale = FALSE;
        //debug
        //savePrimitivesFile("teste.prim", NumberPrimitives);
        //saveObjFile( "teste.obj" );
        //saveCellsFile("teste.cm");
        //morphVectorField();
        /*
         * I can now create the cells in the new embryonic model
         * The same routine 'createRandomCells' is used, but
         * we pass a flag indicating that this time we are
         * using the embryo model. The difference is that
         * with growth I DO NOT yet schedule the first round
         * of splitting events
         */
        if ( cellsCreated ){
                fprintf( stderr, "\tYou can not create the cells twice!\n");
                beep();
                return;
        }
        else{
                printf("Creating Random Cells (simulation.c)...\n");
                createRandomCells( NumberCells, TRUE );
                printf( "Initial Number of Cells:\n[C] = %d [D] = %d [E] = %d [F] = %d\n",
                           nCellsType[C], nCellsType[D],
                           nCellsType[E], nCellsType[F] );
                printf("Done!\n");


                /*#ifdef GRAPHICS
                 drawCells = TRUE;
                 fl_set_button( myformCM->drawRandomPtsButton, drawCells );
                 showLandMarks = FALSE;
                 showAxes = FALSE;
                 #endif*/
        }

        /* Assign primitive information to the cells */
        // assignPrim2AllCells(NULL);
        assignPrim2AllCells(&listFaces);

        /* Check cell's parametrization */
        checkCellsParametrization();

        /* we have to recompute the radius of repulsion */
        compRadiusRepulsion( TRUE );

        /* Do the first round of relaxation */
        double temp;
        temp = Ani;
        Ani = 0.0;
        initialRelaxation();
        Ani = temp;



        /*
         * We can now compute the first round of relaxation rates,
         * based on the polygon's areas
         */
        compSplittingRates( fpRates, 0 );

        /*
         * Now we can start the simulation. First I have to include
         * the first round of splits. The time for split here is
         * computed from the growth information, based on how
         * much each polygon increased and how we have to adapt
         * the splitting rates to compensate for that
         */
        assignTimeToSplit();

        /* Put on the heap the first round of splitting events */
        postSplitEventsOnQueue();



    //printf("saving test files...\n");
    //savePrimitivesFile("teste.prim", NumberPrimitives);
    //saveObjFile( "teste.obj" );
        // saveCellsFile("teste.cm");
    printf("initPatFormAndGrowth Done....\n\n");
}

/*
 *--------------------------------------------------------------
 *      For each cell we are going to assign the first round
 *      of splitting events based on the growth data available
 *--------------------------------------------------------------
 */
void assignTimeToSplit( void )
{
        int i;  /* faces loop counter */
        CELL *c;

        for( i = 0; i < NumberFaces; i ++){
                c = faces[i].head->next;
                while( c != faces[i].tail ){
                        /* get the current rate associated with this face
                         and for this particular type of cell */
                        /*printf( "type %d time before = %f ", c->ctype, c->tsplit );*/
                        if ( faces[i].rates[c->ctype] == -1 ){
                                fprintf( stderr, "Face %d rates [C] = %f [D] = %f [E] = %f [F] = %f\n",
                                                i, faces[i].rates[C], faces[i].rates[D],
                                                faces[i].rates[E], faces[i].rates[F] );
                                errorMsg("Rate not defined! This should not happen! (simulation.c)" );
                        }
                        else{
                                c->tsplit = getTimeSplit( faces[i].rates[c->ctype] );
                                if ( c->tsplit < 0 )
                                        errorMsg("Split time negative! (simulation.c)");
                                /*printf("%d %lg\n", c->ctype, c->tsplit );*/
                        }
                        c = c->next;
                }
        }
}
/*
 *-------------------------------------------------------------
 *-------------------------------------------------------------
 */
void compSplittingRatesForOneFace( int whichFace )
{
        double deltaN, k, tmp;
        double deltaNTypes[HOW_MANY_TYPES];

        deltaN = (faces[whichFace].absArea - faces[whichFace].prevAbsArea) / AreaOneCell;

        if ( deltaN < 0 ){
                //printf( "Got negative rate of splitting...\n" );
                // fprintf( stderr, "Got negative rate of splitting...\n");
                deltaN = fabs( deltaN );
        }
        tmp =
    Ortcell[C].relRate * faces[whichFace].nCellsType[C] +
    Ortcell[D].relRate * faces[whichFace].nCellsType[D] +
    Ortcell[E].relRate * faces[whichFace].nCellsType[E] +
    Ortcell[F].relRate * faces[whichFace].nCellsType[F];

        k = (deltaN / tmp) ;
        //printf("Valor de K = %f",k);

        deltaNTypes[C] = (k*3) * faces[whichFace].nCellsType[C] * Ortcell[C].relRate;
        deltaNTypes[D] = (k*3) * faces[whichFace].nCellsType[D] * Ortcell[D].relRate;
        deltaNTypes[E] = (k*3) * faces[whichFace].nCellsType[E] * Ortcell[E].relRate;
        deltaNTypes[F] = (k*3) * faces[whichFace].nCellsType[F] * Ortcell[F].relRate;

        /* store the rates in the face data structure */
        if ( faces[whichFace].nCellsType[C] != 0 )
                faces[whichFace].rates[C] = (faces[whichFace].nCellsType[C] / deltaNTypes[C] );
        else
                faces[whichFace].rates[C] = -1;

        if ( faces[whichFace].nCellsType[D] != 0 )
                faces[whichFace].rates[D] = (faces[whichFace].nCellsType[D] / deltaNTypes[D] );
        else
                faces[whichFace].rates[D] = -1;

        if ( faces[whichFace].nCellsType[E] != 0 )
                faces[whichFace].rates[E] = faces[whichFace].nCellsType[E] / deltaNTypes[E];
        else
                faces[whichFace].rates[E] = -1;

        if ( faces[whichFace].nCellsType[F] != 0 )
                faces[whichFace].rates[F] = faces[whichFace].nCellsType[F] / deltaNTypes[F];
        else
                faces[whichFace].rates[F] = -1;

        /*if ( faces[whichFace].rates[C] != -1 && faces[whichFace].rates[D] != -1 )
         printf( "%lg %lg\n", faces[whichFace].rates[C], faces[whichFace].rates[D] );*/


#ifdef VERBOSE
        fprintf( stderr, "deltaN = %lg\n", deltaN );
        fprintf( stderr, "relRates [C] = %lg [D] = %lg\n",
                        Ortcell[C].relRate, Ortcell[D].relRate );
        fprintf( stderr, "tmp = %lg k = %lg\n", tmp, k );
        fprintf( stderr, "Deltas [C] = %lg [D] = %lg\n",
                        deltaNTypes[C], deltaNTypes[D] );
        fprintf( stderr, "Face %d Prev Area = %lg Curr Area = %lg\n",
                        whichFace, faces[whichFace].prevAbsArea, faces[whichFace].absArea );
        fprintf( stderr, "Number of cells [C] = %d [D] = %d\n",
                        faces[whichFace].nCellsType[C], faces[whichFace].nCellsType[D] );
        fprintf( stderr, "Rates [C] = %lg [D] = %lg\n\n",
                        faces[whichFace].rates[C], faces[whichFace].rates[D] );
#endif
}
/*
 *--------------------------------------------------------------
 *      Every time we grow the object, we recompute the
 *      rates of splitting for the different types of
 *      cells and store this information with the faces
 *      data structure
 *--------------------------------------------------------------
 */
void compSplittingRates( FILE *fpOut, int day )
{
        int i;  /* faces loop counter */
        double deltaN, k, tmp;
        double deltaNTypes[HOW_MANY_TYPES];
        double avgRateC, avgRateD;

        avgRateC = 0.0;
        avgRateD = 0.0;
        for( i = 0; i < NumberFaces; i ++){
                /*
                 * We only have to deal with faces that have cells
                 * defined
                 */
                if ( faces[i].ncells != 0 ){
                        compSplittingRatesForOneFace( i );
                        avgRateC += faces[i].rates[C];
                        avgRateD += faces[i].rates[D];
                }
                else{
#ifdef VERBOSE
                        fprintf( stderr, "Face %d did not have any cells defined!\n\n", i );
#endif
                }
        }
        fprintf( fpOut, "%d %lg %lg\n", day, avgRateC, avgRateD );
}

/*
 *--------------------------------------------------------------
 * I DON'T THINK I NEED TO USE THIS
 *--------------------------------------------------------------
 */
double computeTotalAreaObject( void )
{
        int whichFace, whichVert;
        double partArea, areaSum;
        Point3D v1, v2, v3;
        int vcount;
        double totalArea = 0.0;

        for(whichFace = 0;  whichFace < NumberFaces; whichFace++){
                vcount = faces[whichFace].nverts;
                areaSum = 0.0;

                for(whichVert = 0; whichVert < vcount-2; whichVert++){
                        V3Sub(&(vert[faces[whichFace].v[whichVert+1]].pos),
                                  &(vert[faces[whichFace].v[0]].pos),
                                  &v1);
                        V3Sub(&(vert[faces[whichFace].v[whichVert+2]].pos),
                                  &(vert[faces[whichFace].v[0]].pos),
                                  &v2);
                        V3Cross( &v1,  &v2,  &v3);

                        /* Warning: This procedure computes the areas multiplied
                         * by 2. To get the actual areas we have to divide the
                         * result of V3Length by 2
                         */
                        partArea = V3Length( &v3 ) / 2.0;
                        areaSum += partArea;
                }
                /* keep trackof the total area of object */
                totalArea += areaSum;
        }
        return totalArea;
}
/*
 *--------------------------------------------------------------
 *      For each cell, this procedure computes the cell's 3D
 *      position in the new model based on its barycentric
 *      coordinates
 *--------------------------------------------------------------
 */
void
morphCells( void )
{
        int i;
        CELL *c;

        for( i = 0; i < NumberFaces; i++ ){
                c = faces[i].head->next;
                while( c != faces[i].tail ){
                        fromBarycentricCoord( c );
                        c->xp = c->x;
                        c->yp = c->y;
                        c->zp = c->z;
                        c = c->next;
                }
        }
}



/*
 *--------------------------------------------------------------
 *      This routine morphs the geometric description of
 *      the object to the age given ('day' input parameter)
 *      I am using day as integer since I am counting the
 *      simulation in days
 *--------------------------------------------------------------
 */
void computeBodyEmbryo( int day )
{
        printf("ComputeBodyEmbryo, dia: %d\n", day);
        int i;
        //Point3D v1, v2, v;
        //double w1, w2, d1, d2;


        /* first we have to morph the primitives */
        morphPrimitives( day );

        /* Morph the vertices */
        morphVertices( day );

        /*Morph vector Field*/
        morphVectorField();
        saveVectors("newaaaaaaaaaaaaaaaaaaaaa");
        //saveVectorField("aaaaaaaaaaaaaaaaaaaaaaaaaa");

        /*
         * I have to recompute the orientation system on the face
         */

        for(i=0; i < NumberFaces; i++){
                compMappingMatrices( i );
                computeGeometricCenter( i, faces[i].nverts );
        }


        /*
         * Finally, recompute the matrices that rotate the faces
         * about the edges
         */
        for( i = 0; i < NumberEdges; i++ ){
                compMatEdges( i );
        }


        /* display the changes */
#ifdef GRAPHICS
        glutPostRedisplay();
#endif
}


/*
 *--------------------------------------------------------------
 *
 *--------------------------------------------------------------
 */
/*Added by Fabiane Queiroz in 25/11/2009*/
int findElement(Array* listFaces, int e)
{
        int i;
        for( i = 0 ; i< listFaces->size ; i++)
        {
                if(e == listFaces->array[i])
                        return 1;
        }
        return 0;
}

/*
 *--------------------------------------------------------------
 *
 *--------------------------------------------------------------
 */
/*Modified by Fabiane Queiroz in 25/11/2009*/
void pureSimulation( RELAXMODE rMode )
{
        //printf("Simulating in %d mode. \n", rMode);

        EVENT *a;
        float time, deltaTime;
        int countIterations = 0;

        float old_save_time = 0;

        /*Add by Fabiane Queiroz in 28/11/2009*/
        Array listFaces;
        int  max = 1;
        listFaces.array = (int*)malloc(sizeof(int)*max);
        int i = 0;
        listFaces.array = NULL;
        listFaces.size = 0;

        printf("Final time = %3.2f\n", finalTime );
        deltaTime = 1.0 / (float)iterPerUnitTime;
        time = 0.0;

        while( time < finalTime )
        {

                a = HeapRemove( myheap );

                //printf("Time = %3.2f, Event_Type = %d, event_time =  %3.2f \n", time, a->whichEvent, a->eventTime );


                while( (time <= a->eventTime) && (time < finalTime) )
                {

                        //if(rMode == GLOBALRELAX)
                        //relaxation( 1, rMode, NULL );
                        //else
                        relaxation( 1, rMode, &listFaces );

                        time += deltaTime;
                        countIterations++;
                        /*
                         * The notion of one day here is given by
                         * the variable 'iterPerUnitTime'. Every time
                         * we reached that count a 'day' has passed
                         */
                        if ( countIterations == iterPerUnitTime )
                        {
                                compRadiusRepulsion( TRUE );
                                countIterations = 0;    /* reset the loop counter */

                                /*if listFace is not NULL, clear it*/
                                i =0;
                                if(listFaces.array != NULL && rMode == LOCALRELAX)
                                {
                                        free(listFaces.array);
                                        listFaces.array = (int*)malloc(sizeof(int)*max);
                                        listFaces.array = NULL;
                                }

                                fprintf( stderr, "Current day is %d (out of %d)\n", (int)(time + 0.5), (int)finalTime );
                                printf( "Current day is %d\n", (int)(time + 0.5) );

                                fprintf( stderr, "Total Number of Cells = %d [C] = %d [D] = %d [E] = %d [F] = %d \n",
                                                NumberCells, nCellsType[C], nCellsType[D], nCellsType[E], nCellsType[F] );
                                printf( "Total Number of Cells = %d [C] = %d [D] = %d [E] = %d [F] = %d \n",
                                           NumberCells, nCellsType[C], nCellsType[D], nCellsType[E], nCellsType[F] );

                        }
                }


                /*fprintf( stderr, "split\n");*/

                /*Cell Division Process*/
                handleSplit( a->info );

                /*Add By Fabiane Queiroz*/
                /* load the listFaces for the local relaxation process*/
                if(rMode == LOCALRELAX)
                {
                        /*find new cell face and store it in listFaces*/
                        /*if listFace is null alocate it*/
                        if(listFaces.array == NULL){
                                free(listFaces.array);
                                listFaces.array = (int*)malloc(sizeof(int)*max);
                        }


                        /*Find a cell face and add its in the listFaces */
                        int wFace = a->info->whichFace;
                        if(!findElement(&listFaces, wFace))
                        {
                                listFaces.array[i] = wFace;
                                i += 1;
                                listFaces.array = (int*)realloc(listFaces.array,sizeof(int)*(i+1));
                        }       listFaces.size = i;

                        /*Find the primary neighbors of face wFace and add its to the listFaces*/
                        int k,adjFace;
                        for( k = 0; k < faces[wFace].nPrimFaces; k++)
                        {
                                adjFace = faces[wFace].primFaces[k];
                                if(!findElement(&listFaces, adjFace))
                                {
                                        listFaces.array[i] = adjFace;
                                        i+=1;
                                        listFaces.array = (int*)realloc(listFaces.array,sizeof(int)*(i+1));
                                        listFaces.size = i;
                                }

                        }


                        /*Find the secondary neighbors of face wFace and add its to the listFaces*/
                        int wVertex,nSecFaces;
                        for(k = 0; k < 3; k++)
                        {       // only 3 vertices for each face
                                wVertex = faces[wFace].v[k];
                                nSecFaces = vert[wVertex].nNeighFaces - 3;
                                listFaces.array = (int*)realloc(listFaces.array,sizeof(int)*(i+1));
                                listFaces.size = i;
                                int l;
                                for(l = 0; l< nSecFaces; l++)
                                {
                                        adjFace = vert[wVertex].neighFaces[l];
                                        if(!findElement(&listFaces, adjFace))
                                        {
                                                listFaces.array[i] = adjFace;
                                                i+=1;
                                                listFaces.array = (int*)realloc(listFaces.array,sizeof(int)*(i+1));
                                                listFaces.size = i;
                                        }
                                }
                        }
                }

                /*
                if( ((int)time % (int)5) == 0 and time != old_save_time){
                        old_save_time = time;
                        // save CM file
                        printf("Saving cells file... %f", time);
                        //fprintf( stderr, "###ExpFileName = %s\n", outputCMfileName);
                        char newCmFileName[128];
                        int len = strlen( outputCMfileName );
                        strncpy( newCmFileName, outputCMfileName, len - 3 );
                        newCmFileName[len-3]='\0';
                        strcat( newCmFileName,"_T" );
                        char cTime[32];
                        sprintf(cTime,"%d",(int)time);
                        strcat( newCmFileName, cTime );
                        strcat( newCmFileName,".cm" );
                        saveCellsFile( newCmFileName);
                        printf("Done!\n");
                }
                */

                free( a );

        }

        /*
         * The simulation has ended. We can clear the heap.
         * There might be SPLIT events
         */
        myheap = ClearHeap( myheap );

}
/*
 *--------------------------------------------------------------
 *
 *--------------------------------------------------------------
 */
void initSimulationParam( float finaltime, float currenttime )
{
        finalTime = finaltime;
        currentTime = currenttime;
}


/*
 *-------------------------------------------------------------------
 *      Put the first round of split events on the heap
 *-------------------------------------------------------------------
 */
void postSplitEventsOnQueue( void )
{
        EVENT a;
        int i;
        CELL *c;
        static int oneTimeOnly = 0;

        /* this is just a safety measure to make sure that I was
     calling this routine only once */
        oneTimeOnly++;

        if ( oneTimeOnly > 1 )
                errorMsg( "Went twice to form split events queue!");

        for(i = 0; i < NumberFaces; i++){
                c = faces[i].head->next;
                while( c != faces[i].tail ){
                        /* includes split event in the heap */
                        a.whichEvent = SPLIT;
                        a.eventTime =  c->tsplit;
                        a.info = c;
                        HeapInsert( myheap, &a );
                        /*printf("cell(%5.3f %5.3f %5.3f) type %d at time %f\n",
                         c->x, c->y, c->z, c->ctype, a->eventTime );*/
                        c = c->next;
                }
        }
}
/*
 *--------------------------------------------------------------
 * I AM NOT USING THIS YET!!
 *--------------------------------------------------------------
 */
double compRateOfSplitting( CELL *c )
{
        int prim1, prim2;
        double d1, d2, w1, w2;
        Matrix4 inverse1, inverse2;
        Point3D v1, v2;


        /*
         * Some cells might not belong to any primitive, since
         * the covering of the primitives is decided on a vertex
         * basis. This means that in these cases I will assign
         * primitive information to a cell based on the FACE
         * the cell lies on
         */

        if ( c->prim1 == FALSE ) prim1 = faces[c->whichFace].prim1;
        else  prim1 = c->prim1;
        if ( c->prim2 == FALSE ) prim2 = faces[c->whichFace].prim2;
        else prim2 = c->prim2;

        if ( prim1 != FALSE && prim2 != FALSE ){
                /* get the matrices which will bring the
                 vertex into primitive coordinate space */
                buildInvTransfMatrix( &inverse1, prim1 );
                buildInvTransfMatrix( &inverse2, prim2 );
                v1.x = c->x;
                v1.y = c->y;
                v1.z = c->z;
                v2.x = c->x;
                v2.y = c->y;
                v2.z = c->z;
                /* express vertex into primitive coordinate space */
                V3PreMul( &v1, &inverse1 );
                V3PreMul( &v2, &inverse2 );
                /*
                 * In some cases the cells could be "outside" the
                 * volume defined by the cylinders. Therefore we
                 * need 2 ways of defining the weights, depending on
                 * if any of the distances is greater than 1
                 */
                d1 = sqrt( v1.y * v1.y + v1.z * v1.z );
                d2 = sqrt( v2.y * v2.y + v2.z * v2.z );

                if ( d1 > 1.0 || d2 > 1.0){
                        /* find and apply weights */
                        w1 = d1 / (d1 + d2);
                        w2 = d2 / (d1 + d2);
                        w1 = 1.0 - w1;
                        w2 = 1.0 - w2;
                }
                else{
                        d1 = 1.0 - d1;
                        d2 = 1.0 - d2;
                        /* find and apply weights */
                        w1 = d1 / (d1 + d2);
                        w2 = d2 / (d1 + d2);
                }

                if ( w1 < 0.0 || w2 < 0.0 ){
                        fprintf( stderr, "w1 = %d w2 = %d\n", w1, w2 );
                        errorMsg("Negative weights in simulation.c!" );
                }

                return ( Prim[prim1].rates[Z] * w1 + Prim[prim2].rates[Z] * w2 );
        }
        else if ( prim1 != FALSE ){
                return ( Prim[prim1].rates[Z] );

        }else if ( prim2 != FALSE )
                return ( Prim[prim2].rates[Z] );
        else{
                fprintf( stderr, "cell (%lg %lg %lg)\n", c->x, c->y, c->z );
                fprintf( stderr, "face %d prim1 %d prim2 %d\n", c->whichFace, prim1, prim2 );
                errorMsg("There is a cell without primitive information associated! (simulation.c)");
        }
}

/*
 *-----------------------------------------------------------------
 *      Given the landmarks and primitives definitions
 *      this routine will compute the daily rate of
 *      growth for each primitive (cm/day) and store
 *      that information with the primitives data
 *      structure.
 *
 *      I AM NOT YET USING THIS! (May 20, 1998)
 *-----------------------------------------------------------------
 */
void compRatesOfGrowth( void )
{

        int i;
        int lmR, lmH;
        double deltaD, deltaH, deltaA;
        double dNewBorn, hNewBorn, dEmbryo, hEmbryo;

        for(i=1; i < NumberPrimitives; i++){
                /* we only compute ratios for the cylinders */
                if ( Prim[i].type == CYLINDER){
                        lmR = Prim[i].landMarkR;
                        lmH = Prim[i].landMarkH;
                        dNewBorn = growthData[NEWBORNAGE][Prim[lmR].gIndex];
                        hNewBorn = growthData[NEWBORNAGE][Prim[lmH].gIndex];
                        dEmbryo = growthData[EMBRYOAGE][Prim[lmR].gIndex];
                        hEmbryo = growthData[EMBRYOAGE][Prim[lmH].gIndex];
                        deltaD = ( dNewBorn - dEmbryo ) / ( TOTALGESTATIONTIME - STARTPFTIME );
                        deltaH = ( hNewBorn - hEmbryo ) / ( TOTALGESTATIONTIME - STARTPFTIME );
                        deltaA = deltaD * deltaH * PI;
                        Prim[i].rates[X] = deltaH;
                        Prim[i].rates[Y] = deltaD;
                        Prim[i].rates[Z] = TotalAreaOfObject / deltaA;
                        /*fprintf( stderr, "Primitive %d deltaD = %f deltaH = %f deltaA = %f rate = %f\n",
                         i, deltaD, deltaH, deltaA, TotalAreaOfObject / deltaA  );*/

                }
        }
}
/*
 *-------------------------------------------------------------------
 *      Put relaxation events on the heap
 *      I AM NOT USING THIS ANYMORE. IN DUE TIME
 *      IT SHOULD BE REMOVED
 *      (TODAY'S DATE DEC. 17, 1997)
 *-------------------------------------------------------------------
 */
void updtRelaxEventsQueue(int inittime, int finaltime)
{

        EVENT *a;
        int i, time;

        errorMsg("Should not be in updtRelaxEventsQueue!");

        if ((a = (EVENT *) malloc(sizeof(EVENT)))== NULL)
                errorMsg("Problem with malloc in updtRelaxEventsQueue!");

        for(time = inittime; time < finaltime; time++){
                /* Put on the heap the relaxation events */
                for(i = 0; i < iterPerUnitTime; i++){
                        a->whichEvent = RELAX;
                        a->eventTime = time + 1.0*i/(float)iterPerUnitTime;
                        a->info = NULL;
                        HeapInsert( myheap, a);
                }
        }
        free(a); a = NULL;
}

/*
 *------------------------------------------------------------------------
 * RECOMPUTE relative areas of sub-triangles that form a convex polygon.
 * The original computation of these areas is in the 'triangleAreas()'
 * function defined in 'distpoints.c'
 * The main difference is that this routine here does not allocate
 * space to store the areas per face and the areas are computed
 * NOT from the original model but rather from the modified 'embryonic'
 * version of the original 'adult' model.
 *
 * AS OF MAY 14, 1998 I AM NOT USING THIS ANYMORE. IN DUE TIME
 * I SHOULD BE ABLE TO REMOVE. THE REASON IS THAT THIS ROUTINE
 * WAS DOING BASICALLY THE SAME AS ANOTHER ONE IN DISTPOINTS.C
 * CALLED 'TRIANGLEAREAS()'
 *-------------------------------------------------------------------------
 */
double
recomputeTriangleAreas( void )
{
        int whichFace, whichVert;
        float areaSum;
        Point3D v1, v2, v3;
        int vcount;
        double totalArea = 0.0;

        for(whichFace = 0;  whichFace < NumberFaces; whichFace++){
                vcount = faces[whichFace].nverts;
                areaSum = 0.0;

                for(whichVert = 0; whichVert < vcount-2; whichVert++){
                        V3Sub(&(vert[faces[whichFace].v[whichVert+1]].pos),
                                  &(vert[faces[whichFace].v[0]].pos),
                                  &v1);
                        V3Sub(&(vert[faces[whichFace].v[whichVert+2]].pos),
                                  &(vert[faces[whichFace].v[0]].pos),
                                  &v2);
                        V3Cross( &v1,  &v2,  &v3);

                        /* Warning: This procedure computes the areas multiplied
                         * by 2. To get the actual areas we have to divide the
                         * result of V3Length by 2
                         */
                        faces[whichFace].areas[whichVert] = V3Length( &v3 ) / 2.0;
                        areaSum += faces[whichFace].areas[whichVert];
                }
                /* store the area of polygon with the face information */
                faces[whichFace].absArea = areaSum;
                /* store the previous area as the current area, since they
                 are the same right now */
                faces[whichFace].prevAbsArea = areaSum;
                /* make the sub-areas relative (0,1) */
                for(whichVert = 0; whichVert < vcount-2; whichVert++)
                        faces[whichFace].areas[whichVert] /= areaSum;

                /* keep trackof the total area of object */
                totalArea += faces[whichFace].absArea;
        }
        return totalArea;
}

/*
 *---------------------------------------------------------------------
 * tmp debug info. Will mark a cell to follow it. I AM NOT USING THIS!!
 *---------------------------------------------------------------------
 */
void markOneCellToFollow( void )
{
        int whichFace, counter, nCellsThisFace;
        CELL *c;

        whichFace = 0;
        do{
                nCellsThisFace = faces[whichFace].ncells;
                whichFace++;
        }while( nCellsThisFace < 2 );

        c = faces[whichFace].head->next;
        counter = 0;
        while( c != faces[whichFace].tail ){
                counter++;
                if ( counter == (nCellsThisFace / 2) ){
                        /* I need to set up the info for the cell here
                         I used to have a field called 'toFollow' associated
                         with all cells. This field would be FALSE for all cells
                         EXCEPT this one cell we want to follow */
                        fprintf( stderr, "\nMarked cell %d on face %d to follow!\n\n",
                                        counter, whichFace );
                        printf( "\nMarked cell %d on face %d to follow!\n\n",
                                   counter, whichFace );
                }
                c = c->next;
        }
}
