/*
 *  primitives.c
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/8/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */


/*
 *--------------------------------------------------
 *	primitives.c
 *--------------------------------------------------
 */

#include "primitives.h"

#include <math.h>
#include <stdlib.h>

#ifdef GRAPHICS
#include "GL/glut.h"
#include "../viewer/graph.h"
#endif

#include "../data/Globals.h"

#include "Anim.h"
#include "Growth.h"
#include "../data/cellsList.h"
#include "../data/Object.h"
#include "../data/Point3D.h"
#include "../data/Matrix4.h"
#include "../util/genericUtil.h"
#include "../util/transformations.h"

#include "../temporary.h"


/*
 *---------------------------------------------------------
 *	Local Prototypes and variables
 *---------------------------------------------------------
 */
char primitivesFileName[128]; //From main.h

/* which primitive is currently active */
int activePrim=WORLD;

/* parametrization was computed or not */
flag parametOKFlag = FALSE;	


/*
 *--------------------------------------------------
 *	
 *--------------------------------------------------
 */
void createLandMark( void )
{
	
	Prim[NumberPrimitives].type = LANDMARK;
	
	/* assign default values for the 2 lines */
	Prim[NumberPrimitives].q1.x = -5.0;
	Prim[NumberPrimitives].q1.y = 0.0;
	Prim[NumberPrimitives].q1.z = 0.0;
	Prim[NumberPrimitives].q2.x = 5.0;
	Prim[NumberPrimitives].q2.y = 0.0;
	Prim[NumberPrimitives].q2.z = 0.0;
	
	/* sets default orientation of landmark */
	assignValue(Prim[NumberPrimitives].rot, 0.0);
	
	/* sets default translation value for the landmark */
	assignValue(Prim[NumberPrimitives].trans, 0.0);
	
	/* sets default scaling for landmark */
	assignValue(Prim[NumberPrimitives].scale, 1.0);
	
	/* make active the primitive just created */
	activePrim = NumberPrimitives;
    
	/* keep track of how many primitives we have */
	NumberPrimitives++;
}

/*
 *----------------------------------------------------------
 *	createPrimitive
 *	This procedure creates a primitive of a given type
 *	The user can select which type of primitive he
 *	wants.
 *
 * 	This routine assumes that the user has already 
 *  	selected or picked the polygons which will be
 *  	controlled by such primitive.
 *
 *	There is some computation to help the user position
 *	the primitive. We compute, for all the polygons, 
 *	selected:
 *	- center of gravity
 *	- maximum and minimum (x, y, z) coordinates
 *	- orientation given by the eigenvectors of the
 *	  moment of Inertia matrix
 *----------------------------------------------------------
 */
void createPrimitive(int type)
{
	int i;
	
	/* assign the center of primitive to be zero */
	V3Zero(&(Prim[NumberPrimitives].center));
	
	assignValue(Prim[NumberPrimitives].trans, 0.0);
	
	/* sets orientation of primitive */
	assignValue(Prim[NumberPrimitives].rot, 0.0);
	
	/* sets scaling of primitive */
	assignValue(Prim[NumberPrimitives].scale, 1.0);
	/* radius of cylinder */
	Prim[NumberPrimitives].r = 1.0;
	
	/* height of cylinder */
	Prim[NumberPrimitives].h = 1.0;
	
	for( i = 0; i < N_TEXT_PER_PRIM; i++ ){
		Prim[NumberPrimitives].textID[i] = -1;
	}
	
	/* assign indexes into growth data */
	/* I HAVE TO IMPLEMENT A WAY FOR THE USER INTERACTIVELY
	 SPECIFY THIS */
	Prim[NumberPrimitives].landMarkR = NumberPrimitives;
	Prim[NumberPrimitives].landMarkH = NumberPrimitives + 1;
	
	/* assign the parent primitive. I still have to fix this!!! */
	Prim[NumberPrimitives].parentPrim = NumberPrimitives-1;
	
	/* sets default sine and  cosine values */
	V3Zero( &(Prim[NumberPrimitives].cosine));
	V3Zero( &(Prim[NumberPrimitives].sine));
	
	if (type == CYLINDER) Prim[NumberPrimitives].type = CYLINDER;
	else Prim[NumberPrimitives].type = SPHERE;
	
	/* make active the primitive just created */
	activePrim = NumberPrimitives;
	
	/* keep track of how many primitives we have */
	NumberPrimitives++;
}

/*
 *----------------------------------------------------------
 *	Given an active primitive, this routine will
 *	create another primitive exactly equal to the
 *	active one
 *----------------------------------------------------------
 */
void duplicatePrimitive( void )
{
	
	if (activePrim == WORLD){
		printf("There is no active primitive...\n");
		return;
	}
	else{
		/* set up information common to any type of primitive */
		Prim[NumberPrimitives].type = Prim[activePrim].type;
		
		assignValueVector( Prim[NumberPrimitives].rot, Prim[activePrim].rot);
		assignValueVector( Prim[NumberPrimitives].trans, Prim[activePrim].trans);
		assignValueVector( Prim[NumberPrimitives].scale, Prim[activePrim].scale);
		
		if (Prim[activePrim].type == CYLINDER){
			Prim[NumberPrimitives].r = Prim[activePrim].r;
			Prim[NumberPrimitives].h = Prim[activePrim].h;
			Prim[NumberPrimitives].parentPrim = Prim[activePrim].parentPrim;
			Prim[NumberPrimitives].landMarkR = Prim[activePrim].landMarkR;
			Prim[NumberPrimitives].landMarkH = Prim[activePrim].landMarkH;
			
			V3Assign( &(Prim[NumberPrimitives].center), Prim[activePrim].center);
			V3Assign( &(Prim[NumberPrimitives].cosine), Prim[activePrim].cosine);
			V3Assign( &(Prim[NumberPrimitives].sine), Prim[activePrim].sine);
			
		}
		else if (Prim[activePrim].type == LANDMARK){
			Prim[NumberPrimitives].gIndex =  Prim[activePrim].gIndex;
			Prim[NumberPrimitives].gIndex =  Prim[activePrim].gIndex;
			
			Prim[NumberPrimitives].dus = Prim[activePrim].dus;
			Prim[NumberPrimitives].dvs = Prim[activePrim].dvs;
			Prim[NumberPrimitives].dws = Prim[activePrim].dws;
			
			V3Assign( &(Prim[NumberPrimitives].q1), Prim[activePrim].q1);
			V3Assign( &(Prim[NumberPrimitives].q2), Prim[activePrim].q2);
			V3Assign( &(Prim[NumberPrimitives].p1), Prim[activePrim].p1);
			V3Assign( &(Prim[NumberPrimitives].p2), Prim[activePrim].p2);
		}
        
		/* make the just created primitive the active one */
		activePrim = NumberPrimitives;
		
		/* keep track of how many primitives we have */
		NumberPrimitives++;
	}
}

/*
 *---------------------------------------------------
 * 	For each primitive, checks which vertex
 * 	is being controlled by that primitive, ie,
 * 	the vertex is 'inside' the space defined by
 *	the primitive
 *
 * 	This should happen AFTER the primitive is
 *	properly placed and scaled
 *---------------------------------------------------
 */
/*Modified by Fabiane Queiroz at 01/12/2009*/
void parametrize( void )
{
	int i;
	CELL *c;
	Array lFaces;
	lFaces.array = NULL;
	
#ifdef GRAPHICS
	/* change cursor into a watch */
	glutSetCursor( GLUT_CURSOR_WAIT );
#endif
	
	if ( !primFilePresent ){
		beep();
		fprintf( stderr, "There is no primitive file to parameterize the model!\n");
		return;
	}
	
	/* mark all vertices without primitive */
	for(i=0; i < NumberVertices; i++){
		vert[i].prim1 = FALSE;
		vert[i].prim2 = FALSE;
		vert[i].p1.u = -1;
		vert[i].p1.v = -1;
		vert[i].p2.u = -1;
		vert[i].p2.v = -1;
	}
	
	/* mark all faces and cells without primitive */
	for(i=0; i < NumberFaces; i++){
		faces[i].prim1 = FALSE;
		faces[i].prim2 = FALSE;
		c = faces[i].head->next;
		while( c != faces[i].tail ){
			c->prim1 = FALSE;
			c->prim2 = FALSE;
			c = c->next;
		}
	}
	
	printf("Remember that primitives' parametrization starts in 2!\n");
	
	/* assign primitive information to vertices */
	printf("Parameterizing Vertices...");
	assignPrim2Vert();
	/* assign parametric (u,v) coordinates to each vertex */
	compParamCoordAllVert();
	printf("Done!\n");
	
	/* assign primitive information to faces */
	printf("Parameterizing  Faces...");
	assignPrim2Faces();
	printf("Done!\n");
	
	/* assign primitive information to cells */
	printf("Parameterizing  Cells...");
	assignPrim2AllCells(&lFaces);
	/* assign parametric (u,v) coordinates to each cell */
	compParamCoordAllCells(&lFaces);
	printf("Done!\n\n");
	
	/* check that all points have at least
     one primitive associated */
	checkParametrization();
	
#ifdef GRAPHICS
	/* init color information to paint object accordingly.
     Vertices belonging to different primitives will be
     painted differently */
	initColorArray();
#endif
	
	/* compute, for each landmark, the values of
     4 points which define a given landmark
     in the primitives' coordinate system */
	compLandMarks();
	
	/* check land marks information */
	checkLandMarks();
	
	/* test the landmarks */
	/* testLandMarks(); */
	
	parametOKFlag = TRUE;
	
#ifdef GRAPHICS
	/* change cursor back to original left arrow format */
	glutSetCursor( GLUT_CURSOR_LEFT_ARROW );
#endif
	
	
}

/*
 *------------------------------------------------------
 *	Assign primitive information to ONE cell
 *------------------------------------------------------
 */
void assignPrim2Cell( CELL *c )
{
	
	int i;
	Point3D p, v;
	
	c->prim1 = FALSE;
	c->prim2 = FALSE;
	
	//printf("Face da célula: %d\n", c->whichFace);
	/*
	 * The loop on the primitives is starting in 2,
	 * since primitive 0 is the 'WORLD' and primitive
	 * 1 is a fake cylinder used just for animation
	 * purposes. For the purposes of parametrization
	 * I don't want cylinder 1 playing any role
	 * I have to be careful that if I don't have
	 * a fake cylinder 1 anymore, the loop should
	 * start in 1 instead
	 */
	// printf("Número de primitivas: %d\n", NumberPrimitives);
	for (i=2 /*1*/; i < NumberPrimitives; i++){
		if ( Prim[i].type == CYLINDER ){
			// printf("primitiva: %f\n",Prim[i].scale[0]);
			p.x = c->x;
			p.y = c->y;
			p.z = c->z;
			// printf("célula: %f, %f, %f\n",p.x,p.y,p.z);
			
			if( belongPrimitive(i, p, &v) ){
				/* check if celldoesn't have yet a primitive assigned */
				if ( c->prim1 == FALSE ){
					
					c->prim1 = i;
				}
				else if ( c->prim1 != i && c->prim2 == FALSE){
					
					c->prim2 = i;
				}
				//else{
				//fprintf( stderr, "Tried to assign more than 2 primitives to a cell! (%d %d %d)\n",
				//   i, c->prim1, c->prim2);
				//}
				/*fprintf( stderr, "%f %f %f %d %d\n",
				 c->x, c->y, c->z, c->prim1, c->prim2 );*/
			}else{
				// printf("célula: %f, %f, %f não pertence a primitiva %d , %f\n",p.x,p.y,p.z,i,Prim[i].scale[0]);
			}
			
			
		}		
	}
	/*
	 * If I couldn't find a primitive to assign to this cell
	 * we exit
	 */
	if ( c->prim1 == FALSE && c->prim2 == FALSE ){
		printf("célula: %f %f %f\n", c->x, c->y, c->z);
		errorMsg("Could not assign any primitive to a cell! (primitives.c)");
		
	}
		
	
}
/*
 *------------------------------------------------------
 *	Assign primitive information to the cells.
 *------------------------------------------------------
 */
/*Modified by Fabiane Queiroz in 12/01/2009*/
void assignPrim2AllCells(Array* listFaces )
{
    //savePrimitivesFile("teste1.prim", NumberPrimitives);
	//saveObjFile( "teste1.obj" );
	//saveCellsFile("teste1.cm");
	int i;
	CELL *c;
	
	/*int nFaces;
	 
	 if(listFaces->array == NULL)
	 nFaces = NumberFaces;
	 else
	 nFaces = listFaces->size;*/
	
	//printf("AssignPrim2AllCells... \n");
	
	if ( cellsCreated )
	{
		if(listFaces->array == NULL)
		{
			// printf("\nlistFaces->array == NULL \n");
			for(i=0; i < NumberFaces; i++)
			{
				//  printf("face: %d possui %d celulas \n", i, faces[i].ncells);
		  		c = faces[i].head->next;
				/*if(faces[i].head->next == NULL || faces[i].tail == NULL){
				 printf("Oupa!\n");
				 }*/
		  		while( c != faces[i].tail )
				{
				    assignPrim2Cell( c );
					c = c->next;
		  		}
			}
		}
		else
		{
			int j;
			for(i=0; i < listFaces->size; i++)
			{
			    //printf("listFaces->array != NULL \n");
				j = listFaces->array[i];
		  		c = faces[j].head->next;
		  		while( c != faces[j].tail )
				{
					assignPrim2Cell( c );
					c = c->next;
		  		}
			}
		}		
		
		
	}
	else{
		fprintf( stderr, "Tried to assign primitives information to cells but there are no cells!\n" );
		return;
	}
}

/*
 *------------------------------------------------------
 *	Assign primitive information to the faces,
 *	according to what the vertices say. If all
 *	vertices of a face belong to the same primitive
 *	'A', that is the primitive info that the
 *	face will get; if a face has vertices that
 *	belong to different primitives, the face
 *	will be assigned the same primitives as
 *	the vertices. The way I have now I assume that
 * 	a given vertex can only belong to 2 primitives
 *------------------------------------------------------
 */
void assignPrim2Faces( void )
{
	int i, j, nVert;
	
	for(i=0; i < NumberFaces; i++){
		nVert = faces[i].nverts;
		for(j=0; j < nVert; j++){
			if ( vert[faces[i].v[j]].prim1 != 0 && vert[faces[i].v[j]].prim2 != 0){
				faces[i].prim1 = vert[faces[i].v[j]].prim1;
				faces[i].prim2 = vert[faces[i].v[j]].prim2;
				break;
			}
			else{
				if (vert[faces[i].v[j]].prim1 != 0){
					faces[i].prim1 = vert[faces[i].v[j]].prim1;
				}else if (vert[faces[i].v[j]].prim2 != 0 && faces[i].prim1 == 0){
					faces[i].prim1 = vert[faces[i].v[j]].prim2;
				}
				else faces[i].prim2 = vert[faces[i].v[j]].prim2;
				break;
			}
		}
	}
}

/*
 *---------------------------------------------------------
 *	For each vertex,  assign one or two primitives
 *	depending on the primitive information derived
 *	from the primitives bounding boxes
 *	All vertices are checked against all primitives
 *---------------------------------------------------------
 */
void assignPrim2Vert( void )
{
	int i, j;
	Point3D v;
	
	/*
	 * The loop on the primitives is starting in 2,
	 * since primitive 0 is the 'WORLD' and primitive
	 * 1 is a fake cylinder used just for animation
	 * purposes. For the purposes of parametrization
	 * I don't want cylinder 1 playing any role
	 * I have to be careful that if I don't have
	 * a fake cylinder 1 anymore, the loop should
	 * start in 1 instead
	 */
	for (i=2 /*1*/; i < NumberPrimitives; i++){
		/* only the primitives themselves will
		 parametrize the vertices */
		if( Prim[i].type != LANDMARK){
			for(j=0; j < NumberVertices; j++){ 	/* for each vertex */
				if( belongPrimitive(i, vert[j].pos, &v) ){
					/* check if vertex doesn't have yet a primitive assigned */
					if ( vert[j].prim1 == FALSE){
						vert[j].prim1 = i;
					}
					else if ( vert[j].prim1 != i && vert[j].prim2 == FALSE){
						vert[j].prim2 = i;
					}
					/*else{
					 fprintf(stderr, "Tried to assign more than 2 primitives to a vertex!\n");
					 fprintf(stderr, "Vertex %d\n",j);
					 fprintf(stderr, "Primitives: %d %d %d\n",
					 i, vert[j].prim1, vert[j].prim2);
					 }
					 */
				}	
			}
		}
	}	
}

/*
 *------------------------------------------------------
 *	Check that each primitive has appropriate
 *	landmarks information associated
 *------------------------------------------------------
 */
void checkLandMarks( void )
{
	int i;
	
	/* check the master primitive outside the loop */
	if ( Prim[1].parentPrim != -1 ||
		Prim[1].landMarkR  == -1 ||
		Prim[1].landMarkH  == -1)
		errorMsg("Master cylinder is not correct!");
	if ( Prim[1].parentPrim > NumberPrimitives ||
		Prim[1].landMarkR  > NumberPrimitives ||
		Prim[1].landMarkH  > NumberPrimitives)
		errorMsg("Master cylinder has wrong landmark information associated!");
	
	for(i=2; i < NumberPrimitives; i++){
		if (Prim[i].type == CYLINDER){
			if ( Prim[i].parentPrim == -1 ||
				Prim[i].landMarkR  == -1 ||
				Prim[i].landMarkH  == -1)
				errorMsg("A cylinder has wrong landmark information associated!");
			if ( Prim[i].parentPrim >= NumberPrimitives ||
				Prim[i].landMarkR  >= NumberPrimitives ||
				Prim[i].landMarkH  >= NumberPrimitives)
				errorMsg("A cylinder has wrong landmark information associated!");
			
		}
		else if (Prim[i].type == LANDMARK){
			if (Prim[i].gIndex == -1 ||
				Prim[i].primInd == -1 ||
				Prim[i].primInd >= NumberPrimitives)
				errorMsg("A landmark has wrong primitive and/or growth information associated!");
		}
	}
}
/*
 *------------------------------------------------------
 *	Test the landmarks procedure
 *------------------------------------------------------
 */
void testLandMarks( void )
{
	int i, lm1, lm2;
	double h, r, hSquare;
	double l1, l2, l1s, l2s;
	double a1,a2,b1,b2;
	Point3D v1, v2, v3, v4;
	Matrix4 transfMatrix1, transfMatrix2;
	
	for(i=1; i < NumberPrimitives; i++){
		if (Prim[i].type == CYLINDER){
			lm1 = Prim[i].landMarkR;
			lm2 = Prim[i].landMarkH;
			/* get transformation matrix to bring the
			 points into World coordinates */
			buildTransfMatrix(&transfMatrix1, lm1);
			buildTransfMatrix(&transfMatrix2, lm2);
			
			V3Assign( &v1, Prim[lm1].q1);
			V3Assign( &v2, Prim[lm1].q2);
			V3Assign( &v3, Prim[lm2].q1);
			V3Assign( &v4, Prim[lm2].q2);
			
			/* get transformation matrix to bring the
			 points into World coordinates */
			buildTransfMatrix(&transfMatrix1, lm1);
			buildTransfMatrix(&transfMatrix2, lm2);
			
			/* bring the points into World coordinates */
			V3PreMul(&v1, &transfMatrix1);
			V3PreMul(&v2, &transfMatrix1);
			V3PreMul(&v3, &transfMatrix2);
			V3PreMul(&v4, &transfMatrix2);
			
			l1 = distance(v1, v2);
			l2 = distance(v3, v4);
			
			/* l1 = distance(Prim[lm1].p2, Prim[lm1].p1);
			 l2 = distance(Prim[lm2].p2, Prim[lm2].p1); */
			l1s = l1 * l1;
			l2s = l2 * l2;
			printf("l1 = %f l2 = %f\n", l1, l2);
			
			a1 = Prim[lm1].dus + Prim[lm1].dvs;
			b1 = Prim[lm1].dws;
			a2 = Prim[lm2].dus + Prim[lm2].dvs;
			b2 = Prim[lm2].dws;
			
			/* hSquare =  -(l1s * a2 - a1 * l2s) / (-a2 * b1 + a1 * b2 ); */
			/* printf("h square = %f\n",hSquare); */
			
			h = sqrt( -(l1s * a2 - a1 * l2s) / (-a2 * b1 + a1 * b2 ));
			/* I'm not too sure yet why I don't have to divide it by 2 here!! */
			r = sqrt( (-l2s * b1 + b2 * l1s) / (-a2 * b1 + a1 * b2));
			
			printf("r = %f h = %f\n", r, h);
			
		}
	}
}
/*
 *-------------------------------------------------
 *	For each landmark, compute the 4 points
 *	in primitives' coordinates, according to the
 *	transformation matrices.
 *	First bring the point from original landmark
 *	space to world space and from world space
 *	to primitive space
 *-------------------------------------------------
 */
void compLandMarks( void )
{
	int i;
	Point3D v1, v2;
	Matrix4 transfMatrix;
	Matrix4 primMatrix;
	
	for(i=1; i < NumberPrimitives; i++){
		/* we only do that for landmarks */
		if (Prim[i].type == LANDMARK){
			/* get the 2 points */
			V3Assign( &v1, Prim[i].q1);
			V3Assign( &v2, Prim[i].q2);
			/* get transformation matrix to bring the
			 points into World coordinates */
			buildTransfMatrix(&transfMatrix, i);
			/* bring the points into World coordinates */
			V3PreMul(&v1, &transfMatrix);
			V3PreMul(&v2, &transfMatrix);
			/* compute the landmark length in WORLD coordinates */
			Prim[i].lengthW = distance(v1,v2);
			/* get transformation matrix to bring the
			 points into primitive space */
			buildInvTransfMatrix(&primMatrix, Prim[i].primInd);
			/* bring the points into primitive space */
			V3PreMul(&v1, &primMatrix);
			V3PreMul(&v2, &primMatrix);
			Prim[i].lengthP = distance(v1,v2);
			/* store these values in the primitive data structure */
			V3Assign( &(Prim[i].p1), v1);
			V3Assign( &(Prim[i].p2), v2);
			/* do some saving time computations needed later;
			 the squared difference of all 3 coordinates */
			Prim[i].dws = (Prim[i].p2.x - Prim[i].p1.x) * 
			(Prim[i].p2.x - Prim[i].p1.x);
			Prim[i].dvs = (Prim[i].p2.y - Prim[i].p1.y) *
			(Prim[i].p2.y - Prim[i].p1.y);
			Prim[i].dus = (Prim[i].p2.z - Prim[i].p1.z) *
			(Prim[i].p2.z - Prim[i].p1.z);
		}
	}
}
/*
 *-------------------------------------------------
 *	checks if every vertex has at least
 *	one primitive associated with it
 *-------------------------------------------------
 */
void checkParametrization(void)
{
	int i;
	int j = 0; // added by Cadu in 29/10
	int *n;    // added by Cadu in 29/10
	CELL *c;
	
	for(i=0; i < NumberVertices; i++){
		if ( vert[i].prim1 == FALSE && vert[i].prim2 == FALSE )
			fprintf( stderr, "Vertex %d does not have a primitive assigned!\n", i);
		
		// Lines below added by Cadu in 29/10
		if( j == 0) {
			nnoprimVertex = (int*)malloc( sizeof( int ) );
			*nnoprimVertex = i;
		}
		else {
			nnoprimVertex = (int*)realloc( nnoprimVertex, sizeof( int ) * (j + 1) );
			n = nnoprimVertex;
			nnoprimVertex = nnoprimVertex + j;
			*nnoprimVertex = i;
			nnoprimVertex = n;
		}
		j++;
		/******************/
	}
	
	for(i=0; i < NumberFaces; i++){
		if ( faces[i].prim1 == FALSE && faces[i].prim2 == FALSE )
			fprintf(stderr, "Face %d does not have a primitive assigned!\n", i);
	}
	
	/* Check cell's parametrization */
	checkCellsParametrization();
	
}

/*
 *-------------------------------------------------
 *	checks if every cells has at least
 *	one primitive associated with it
 *-------------------------------------------------
 */
void checkCellsParametrization( void )
{
	int i;
	CELL *c;
	
	if ( cellsCreated ){
		for( i = 0; i < NumberFaces; i++ ){
			c = faces[i].head->next;
			while( c != faces[i].tail ){
				if ( c->prim1 == FALSE && c->prim2 == FALSE )
					fprintf( stderr, "Cell(%f %f %f) does not have a primitive assigned! (primitives.c)\n",
							c->x, c->y, c->z );
				c = c->next;
			}
		}
	}	
}

/*
 *----------------------------------------------------------
 *	Check if the received vertex is inside the
 *	bounding box of the received primitive
 *	Returns TRUE if yes and FALSE otherwise
 *	and also returns the vertex coordinates in
 *	primitive space values 
 *----------------------------------------------------------
 */
int  belongPrimitive( int whichPrim,
					 Point3D vertToBeTransformed,
					 Point3D *vReturn )
{
	Matrix4 m;
	Point3D v1;
	float d;
	
	/* get vertex to be transformed */
	V3Assign(&v1, vertToBeTransformed );
	
	/* bring point into primitive space */
	buildInvTransfMatrix(&m, whichPrim);
	V3PreMul(&v1, &m);
	
	/*
	 * MARCELO: I am not sure why it seems that
	 * I was NOT using the sqrt here to compute
	 * the distance. I fixed now and everything
	 * seems to be ok.
	 */
	d = sqrt( v1.y * v1.y + v1.z * v1.z );
	
	if ((d >= 0.0 ) && (d <=  1.0) && ( v1.x <= 1.0) && ( v1.x >= 0.0) ){
		vReturn->x = v1.x;
		vReturn->y = v1.y;
		vReturn->z = v1.z;
		return(TRUE);
	}
	else{
		//printf("opssss ");
	return(FALSE);}
}

/*
 *--------------------------------------------------------------
 *	This function computes the coordinates of the point
 *	received in the cylindrical space defined by the
 *	'whichprimitive' parameter. The result is returned 
 *	in the 'vReturn' argument
 *--------------------------------------------------------------
 */
void compCylinderCoord( int whichPrim,
					   Point3D vertToBeTransformed,
					   Point3D *vReturn )
{
	Matrix4 m;
	Point3D v;
	
	/* get vertex to be transformed */
	V3Assign(&v, vertToBeTransformed );
	
	/* bring point into primitive space */
	buildInvTransfMatrix(&m, whichPrim);
	V3PreMul(&v, &m);
	
	vReturn->x = v.x;
	vReturn->y = v.y;
	vReturn->z = v.z;
}
/*
 *--------------------------------------------------------------
 * This function goes over all vertices and computes a pair of
 * parametric values which define the position of the vertex
 * in the 2D cyindrical space
 *--------------------------------------------------------------
 */
void compParamCoordAllVert( void )
{
	int i;
	
	for( i = 0; i < NumberVertices; i++ ){
		compParamCoord3DPoint( i );
	}
}

/*
 *-----------------------------------------------------------
 *-----------------------------------------------------------
 */
void compParamCoord3DPoint( int vertID )
{
	Matrix4 m;
	Point3D p;
	
	if ( vert[vertID].prim1 != FALSE && vert[vertID].prim2 != FALSE ){
		p.x = vert[vertID].pos.x;
		p.y = vert[vertID].pos.y;
		p.z = vert[vertID].pos.z;
		/* bring point into first primitive space */
		buildInvTransfMatrix( &m, vert[vertID].prim1 );
		V3PreMul(&p, &m);
		compUV( p, &(vert[vertID].p1.u), &(vert[vertID].p1.v) );
		p.x = vert[vertID].pos.x;
		p.y = vert[vertID].pos.y;
		p.z = vert[vertID].pos.z;
		/* bring point into first primitive space */
		buildInvTransfMatrix( &m, vert[vertID].prim2 );
		V3PreMul(&p, &m);
		compUV( p, &(vert[vertID].p2.u), &(vert[vertID].p2.v) );
	}else if ( vert[vertID].prim1 != FALSE ){
		p.x = vert[vertID].pos.x;
		p.y = vert[vertID].pos.y;
		p.z = vert[vertID].pos.z;
		/* bring point into first primitive space */
		buildInvTransfMatrix( &m, vert[vertID].prim1 );
		V3PreMul(&p, &m);
		compUV( p, &(vert[vertID].p1.u), &(vert[vertID].p1.v) );
	}
	/*else
	 errorMsg( "Vertex without primitive assigned!");*/
}
/*
 *--------------------------------------------------------------
 * This function goes over all cells and computes a pair of
 * parametric values which define the position of the cell
 * in the 2D cyindrical space
 *--------------------------------------------------------------
 */
/*Modified by Fabiane Queiroz in 12/01/2009*/
void compParamCoordAllCells(Array* listFaces)
{
	int i;
	CELL *c;
	
	/*int nFaces;
	 
	 if(listFaces->array == NULL)
	 nFaces = NumberFaces;
	 else{
	 nFaces = listFaces->size;
	 /*int j;
	 for (j=0; j <nFaces; j++)
	 printf("Elemento = %d , Valor = %d\n", j, listFaces->array[j]);}*/
	
	
	
	//printf("Em compParamCoordAllCells: nFaces = %d \n", nFaces);
	
	if(listFaces->array == NULL)
	{
		for( i = 0; i < NumberFaces; i++ )
		{
			c = faces[i].head->next;
			while( c != faces[i].tail )
			{
		 		compParamCoordOneCell( c );
		  		c = c->next;
			}
		}
	}
	else
	{
		int j;
		for( i = 0; i < listFaces->size; i++ )
		{
			j = listFaces->array[i];
			c = faces[j].head->next;
			while( c != faces[j].tail )
			{
		 		compParamCoordOneCell( c );
		  		c = c->next;
			}
		}
	}
	
}
/*
 *--------------------------------------------------------------
 * Auxiliary routine for 'compParamCoordAllCells' above
 *--------------------------------------------------------------
 */
void compParamCoordOneCell( CELL *c )
{
	Matrix4 m;
	Point3D v;
	
	/*fprintf( stderr, "Face %d prim1 = %d prim2 = %d\n",
	 c->whichFace, c->prim1, c->prim2 );*/
	
	if ( c->prim1 != FALSE && c->prim2 != FALSE ){
		/* get vertex to be transformed */
		v.x = c->x;	
		v.y = c->y;
		v.z = c->z;
		/* bring point into first primitive space */
		buildInvTransfMatrix( &m, c->prim1 );
		/*fprintf( stderr, "processing prim %d ", c->prim1 );*/
		V3PreMul(&v, &m);
		compUV( v, &(c->p1.u), &(c->p1.v) );
		v.x = c->x;	
		v.y = c->y;
		v.z = c->z;
		/* bring point into second primitive space */
		buildInvTransfMatrix( &m, c->prim2 );
		/*fprintf( stderr, "processing prim %d\n", c->prim2 );*/
		V3PreMul(&v, &m);
		compUV( v, &(c->p2.u), &(c->p2.v) );
	}
	else if ( c->prim1 != FALSE ){
		/* get vertex to be transformed */
		v.x = c->x;	
		v.y = c->y;
		v.z = c->z;
		/* bring point into first primitive space */
		buildInvTransfMatrix( &m, c->prim1 );
		/*fprintf( stderr, "processing prim %d\n", c->prim1 );*/
		V3PreMul(&v, &m);
		compUV( v, &(c->p1.u), &(c->p1.v) );
	}
	else if ( c->prim2 != FALSE ){
		/* get vertex to be transformed */
		v.x = c->x;	
		v.y = c->y;
		v.z = c->z;
		/* bring point into first primitive space */
		buildInvTransfMatrix( &m, c->prim2 );
		/*fprintf( stderr, "processing prim %d\n", c->prim1 );*/
		V3PreMul(&v, &m);
		compUV( v, &(c->p2.u), &(c->p2.v) );
	}
	else{
		fprintf( stderr, "cell %f %f %f with primitives %d and %d\n",
				c->x, c->y, c->z, c->prim1, c->prim2);
		fprintf( stderr, "Face %d has %d and %d for primitives\n",
				c->whichFace, faces[c->whichFace].prim1, faces[c->whichFace].prim2 );
		errorMsg( "Cell without primitive assigned! (primitives.c)");
	}
}

/*
 *------------------------------------------------------------------
 *	Given a 3D position of a cell in the cylindrical
 *	coordinate space (0 <= x <= 1 and 0 <= sqrt(y*y + z*z) <= 1
 * 	returns a pair of (u,v) coordinates
 *------------------------------------------------------------------
 */
void compUV( Point3D p, double *u, double *v )
{
	double d, uu, vv;
	
	d = sqrt( p.y * p.y + p.z * p.z );
	
	if ((d >= 0.0 ) && (d <= (1.0 + 1.0e-4) ) && ( p.x <= (1.0 + 1.0e-4)) && ( p.x >= 0.0) ){
		uu = p.x;
		vv = atan2( p.z, p.y ) / (2 * M_PI );
		if ( vv < 0 ) vv += 1.0;
		*u = uu;
		*v = vv;
		return;
	}
	else{
		fprintf( stderr, "x = %lg y = %lg z = %lg d = %lg\n",
				p.x, p.y, p.z, d );
		errorMsg( "3D coordinates outside cylindrical space!" );
	}
}


/*
 * From genericUtil.c
 *----------------------------------------------------------
 *	savePrimitivesFile
 *----------------------------------------------------------
 */
void savePrimitivesFile( char *outFile, int nPrim )
{
	FILE *fp;
	int i;

	/* change cursor to a watch */
#ifdef GRAPHICS
	glutSetCursor( GLUT_CURSOR_WAIT );
#endif

	if( (fp = fopen(outFile,"w")) == NULL)
		errorMsg("Could not open primitives file to write!");

	fprintf(fp, "%s\n", growthDataFileName);
	fprintf(fp, "%s\n", animDataFileName);
	fprintf(fp, "%d\n", nPrim);
	for(i=0; i < nPrim; i++){
		fprintf( fp, "#### %d\n", i );
		fprintf( fp, "%d\n", Prim[i].type);
		fprintf( fp, "%f %f %f\n",
				Prim[i].trans[X],
				Prim[i].trans[Y],
				Prim[i].trans[Z]);
		if ( i == WORLD){
			fprintf( fp, "%f %f %f %f\n",
					Prim[i].rot[X], Prim[i].rot[Y],
					Prim[i].rot[Z], Prim[i].rot[W] );
		}
		else{
			fprintf( fp, "%f %f %f\n",
					rad2deg( Prim[i].rot[X] ),
					rad2deg( Prim[i].rot[Y] ),
					rad2deg( Prim[i].rot[Z] ) );
		}
		fprintf( fp, "%f %f %f\n",
				Prim[i].scale[X],
				Prim[i].scale[Y],
				Prim[i].scale[Z] );
		/* write info specific to a CYLINDER or LANDMARK */
		if ( Prim[i].type == CYLINDER ){
			fprintf( fp, "%d %d\n", Prim[i].landMarkR, Prim[i].landMarkH );
			fprintf( fp, "%d\n", Prim[i].parentPrim );
		}
		else if ( Prim[i].type == LANDMARK){
			fprintf(fp, "%d\n", Prim[i].gIndex);
			fprintf(fp, "%d\n", Prim[i].primInd);
		}
		else errorMsg("Trying to write unknown primitive from primitives file!");
		/*fprintf(fp, "\n");*/
	}
	/* close the file */
	fclose(fp);
	/* change cursor back to normal */
#ifdef GRAPHICS
	glutSetCursor( GLUT_CURSOR_LEFT_ARROW );
	/* inform the user */
#endif
	printf("\tPrimitives saved on file %s\n", outFile);

}

/*
 * From genericUtil.c
 *----------------------------------------------------------
 *	loadPrimitives
 *	If the file does not exists or could not be
 *	open this routine will return -1
 *----------------------------------------------------------
 */
int loadPrimitivesFile( char *inFile )
{
	FILE *fp;
	int i, j, nPrim;
	char line[255];

	if( (fp=fopen(inFile,"r")) == NULL) return(-1);

	/* read name of the file with the growth data */
	fscanf(fp, "%s\n", growthDataFileName);
	/* read name of the file with the animation data */
	fscanf(fp, "%s\n", animDataFileName);
	/* read number of primitives */
	fscanf(fp, "%d\n", &nPrim );

	for(i=0; i < nPrim; i++){
		Getline( line, 255, fp );
		/*printf( "Read Line %s\n", line );*/
		fscanf(fp, "%d\n", &(Prim[i].type));
		/*printf( "Read Prim %d\n", Prim[i].type );*/
		if ( Prim[i].type == CYLINDER || Prim[i].type == LANDMARK ){
			Prim[i].center.x = 0.0;
			Prim[i].center.y = 0.0;
			Prim[i].center.z = 0.0;
			Prim[i].h = 1.0;
			Prim[i].r = 1.0;
			fscanf(fp, "%f %f %f\n",
				   &(Prim[i].trans[X]),
				   &(Prim[i].trans[Y]),
				   &(Prim[i].trans[Z]));
			if ( i == WORLD){
				fscanf(fp, "%f %f %f %f\n",
					   &(Prim[i].rot[X]), &(Prim[i].rot[Y]),
					   &(Prim[i].rot[Z]), &(Prim[i].rot[W]) );
			}
			else{
				fscanf(fp, "%f %f %f\n",
					   &(Prim[i].rot[X]),
					   &(Prim[i].rot[Y]),
					   &(Prim[i].rot[Z]));
				/* convert rotation values into radians */
				Prim[i].rot[X] = deg2rad( Prim[i].rot[X] );
				Prim[i].rot[Y] = deg2rad( Prim[i].rot[Y] );
				Prim[i].rot[Z] = deg2rad( Prim[i].rot[Z] );
			}
			fscanf(fp, "%f %f %f\n",
				   &(Prim[i].scale[X]),
				   &(Prim[i].scale[Y]),
				   &(Prim[i].scale[Z]));
			/* read info specific to a CYLINDER or LANDMARK */
			if ( Prim[i].type == CYLINDER){
				fscanf(fp, "%d %d\n", &(Prim[i].landMarkR),&(Prim[i].landMarkH));
				fscanf(fp, "%d\n", &(Prim[i].parentPrim));
				Prim[i].gIndex = -1;
				for( j = 0; j < N_TEXT_PER_PRIM; j++ )
					Prim[i].textID[j] = -1;

			}
			else if ( Prim[i].type == LANDMARK){
				fscanf(fp, "%d\n", &(Prim[i].gIndex));
				fscanf(fp, "%d\n", &(Prim[i].primInd));
				Prim[i].parentPrim = -1;
				Prim[i].landMarkR = -1; Prim[i].landMarkH = -1;
				Prim[i].q1.x = -5.0;
				Prim[i].q1.y = 0.0;
				Prim[i].q1.z = 0.0;
				Prim[i].q2.x = 5.0;
				Prim[i].q2.y = 0.0;
				Prim[i].q2.z = 0.0;
			}
		}
		else{
			fprintf( stderr, "Primitive file %s\n", inFile );
			errorMsg("Trying to read unknown primitive from primitives file!");
		}
		fscanf(fp, "\n");
	}

	/* close the file */
	fclose(fp);

	/* show primitives */
	/*showPrimitives = TRUE;*/

	/* make sure that all primitives don't have animation
     information associated. Later on when processing
     the animation file, the primitives with animation
     information associated will have this flag switched
     to TRUE */
	for(i=1; i < nPrim; i++)
		Prim[i].animation = -1;

	/* help message for the user */
	//printf("==========================\n");
//	printf("Primitives loaded from file %s\n\n", inFile);

	/* returns the number of primitives */
	return( nPrim );
}

/*
 * From genericUtil.c
 *----------------------------------------------------------
 *	This routine will check if there exists a primitives
 *	file and if so will load it beforehand
 *	Return the number of primitives defined in the
 *	file. Returns TRUE if there is a
 *	primitive file (since I always have WORLD as the
 *	first primitive) and FALSE otherwise
 *----------------------------------------------------------
 */
int checkPrimitivesFile(void)
{
	int n;

	n = loadPrimitivesFile( primitivesFileName );
	if( n != -1){
		NumberPrimitives = n;
		primFilePresent = TRUE;	/* set the flag */
		return TRUE;
	}
	else{
		NumberPrimitives = 1;
		fprintf(stderr,"This object has no primitives file associated with it!\n");
		fprintf(stderr,"It just means that there are no primitives defined!\n");
		return FALSE;
	}
}
