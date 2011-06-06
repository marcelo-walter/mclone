/*
 *  morph.c
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/8/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */


/*
 *------------------------------------------------------------
 *	morph.c
 *	Marcelo Walter
 *
 *	This module contains the morphing part of
 *	the shape transformation
 *------------------------------------------------------------
 */

#include "morph.h"

#include <stdlib.h>
#include <math.h>

#ifdef GRAPHICS
//#include <GLUT/glut.h>
#include "GL/glut.h"
#endif

#include "../temporary.h"

//#include "../main.h"
#include "simulation.h"
#include "relax.h"
#include "../control/wingEdge.h"
#include "../control/primitives.h"
#include "../control/Growth.h"
#include "../data/cellsList.h"
#include "../util/genericUtil.h"

#include "../util/transformations.h"

/*
 *----------------------------------------------------------
 * Do the actual morphing, that is, for each vertex
 * computes the new position according to the growth
 * information.
 *
 * INPUT: 	t -> time for transformation
 *----------------------------------------------------------
 */
void morph( int t )
{
	
	Array listFaces;
    int  max = 1;
    listFaces.array = (int*)malloc(sizeof(int)*max);
    	
    listFaces.array = NULL;
    listFaces.size = 0;
	
	int i;
	// int j;
	
	/* If we want the primitives also to be morphed, we
	 * have to call morphPrimitives below.
	 * Because of the way I am doing now, I have to keep
	 * the morphing of the primitives BEFORE the morphing
	 * of the vertices
	 */
	
	if ( !growthFlag )
		morphPrimitives( FINAL_GROWTH_TIME );
	else
		morphPrimitives( t );
	
	/* Morph the vertices */
	morphVertices( t );
	
	//morphVectorField();
	
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
	
	/* recompute the triangle areas for placing random cells */
	TotalAreaOfObject = triangleAreas();
	fprintf( stderr, "\nNew Area of Object = %f\n", TotalAreaOfObject );
	
	/* Morph the cells */
	if ( cellsCreated )
		morphCells();
	
	/* I need this here for the Voronoi part */
	compCellsPolygonCoordinates(&listFaces);
	
	/* display the changes */
#ifdef GRAPHICS
	glutPostRedisplay();
#endif
}

/*
 *------------------------------------------------------------
 *	Morph the Vertices
 *------------------------------------------------------------
 */
void morphVertices( int t )
{
	
	Point3D v1, v2, v;
	double w1, w2, d1, d2;
	int i;
	//for (i = 0; i < 20; i++){
	for (i = 0; i < NumberVertices; i++)
	{
		//printf("\nteste: %lf, %lf, %lf\n", origVert[i].x, origVert[i].y, origVert[i].z); 
		/* case where the vertex has 2 associated primitives */
		if ( vert[i].prim1 != 0 && vert[i].prim2 != 0)
		{
			d1 = applyGrowthInformation(vert[i].prim1,
										origVert[i],
										&v1,
										t );
			
			
			d2 = applyGrowthInformation(vert[i].prim2,
										origVert[i],
										&v2,
										t );
			
			if( d1 > 1.0 || d2 > 1.0)
				errorMsg("Distance greater than 1! (morph.c)");
			else
			{
				d1 = 1.0 - d1;
				d2 = 1.0 - d2;
			}
			/* find and apply weights */
			w1 = d1 / (d1 + d2);
			w2 = d2 / (d1 + d2);
			
			// printf("w1 = %f, w2= %f\n", w1, w2 );
			
			/* finally transfer the info to new data structure */
			vert[i].pos.x = v1.x * w1 + v2.x * w2;
			vert[i].pos.y = v1.y * w1 + v2.y * w2;
			vert[i].pos.z = v1.z * w1 + v2.z * w2;
			
			
#ifdef GRAPHICS
			vertDisplay[i].x = vert[i].pos.x;
			vertDisplay[i].y = vert[i].pos.y;
			vertDisplay[i].z = vert[i].pos.z;
#endif
			
		}else
		{
			/* the vertex has only one primitive associated.
			 * It will always be 'prim1' and therefore I don't have to
			 * check if 'prim2' also has information
			 */
			if (vert[i].prim1 != 0)
			{
				// printf("\nvertice to be transformed teste: %lf, %lf, %lf\n", origVert[i].x, origVert[i].y, origVert[i].z);
				applyGrowthInformation( vert[i].prim1, origVert[i], &v1, t );
				
				
				
				if (i == 1)
				{
					// printf("XXXX: %.25lf", v1.x);
					// printf(" YYY: %.25lf", v1.y);
					//  printf(" ZZZ: %.25lf \n", v1.z);
				}
				vert[i].pos.x = v1.x;
				vert[i].pos.y = v1.y;
				vert[i].pos.z = v1.z;
				
				
#ifdef GRAPHICS
				vertDisplay[i].x = vert[i].pos.x;
				vertDisplay[i].y = vert[i].pos.y;
				vertDisplay[i].z = vert[i].pos.z;
#endif
			} 
			else errorMsg("There is a vertex without a primitive associated in morph.c!");
		}
	}
}

/*
 *---------------------------------------------------------
 *	weigthing function. Produces a smooth decaying
 *	function as follows:
 *
 *	y > 0: The function goes from 1 to 0 as y goes
 *	from h/2 to h/2 + HEIGHT_INCR_FACTOR
 *
 *	y < 0: The function goes from 1 to 0 as y goes
 *	from -h/2 to -h/2 - HEIGHT_INCR_FACTOR
 *--------------------------------------------------------
 */
double weightFunction(double y, double h)
{
	double aux, yprime;
	
	yprime = y + h/2.0;
	
	if ( yprime >= 0 && yprime <= h) return(1.0);
	
	else{
		if ( yprime < 0){
			aux = (yprime + h)/HEIGHT_INCR_FACTOR;
			return(-2 * (aux * aux * aux) - 3 * (aux * aux) + 1);
		}
		else{
			aux = (yprime - h)/HEIGHT_INCR_FACTOR;
			return(2 * (aux * aux * aux) - 3 * (aux * aux) + 1);
		}
	}
	
}

/*
 *---------------------------------------------------------
 *	This routine is auxiliary to the morph routine.
 *	It gets the vertex identified by 'vertIndex'
 *	plus the primitive associated with the vertex
 *	identified by 'primIndex' and do the following:
 *
 *	- transform the vertex coordinates into the
 *	primitive's coordinate system
 *	- apply the growth information to the vertex
 *	- undo the primitive transformation, that is,
 *	the vertex is expressed in world coordinates
 *	again
 *
 *	OUTPUT - The vertex distance from the axis
 *	of the primitive used to weight the contributions
 *	when a vertex has more than one primitive
 *	associated with it
 *--------------------------------------------------------
 */

double applyGrowthInformation( int primIndex,
							  Point3D vertexToBeTransformed,
							  Point3D *vReturn, int t )
{
	// printf("\nMORPH VERTICES:\n");
	Matrix4 gm, rm, tm, inverse;
	
	Point3D v;
	
	double d, h;
	
	int i;
	
	Matrix4 m;
	
	
	
	/* get vertex to be transformed */
	
	V3Assign( &v, vertexToBeTransformed );
	
	// printf("\nvertice to be transformed: %lf, %lf, %lf\n", v.x, v.y, v.z);
	
	/* get the matrix which will bring the
	 
     vertex into primitive coordinate space */
	
	buildInvTransfMatrix(&inverse, primIndex);
	
    
	
	/* express vertex into primitive coordinate space */
	
	V3PreMul(&v, &inverse);
	
	
	
	/* compute distance to primitive axis */
	
    
	d = sqrt(v.y * v.y + v.z * v.z);
	
	
	/*
	 
	 *------------------------------------------------------------
	 
	 * Here is where I should change for the weigthing scheme
	 
	 *
	 
	 * h =  (1.0 - HEIGHT_INCR_FACTOR) * Prim[primIndex].scale[X];
	 
	 * d *= weightFunction(v.x, h);
	 
	 *------------------------------------------------------------
	 
	 */
	
	/*
	 * If the point is not in the master primitive
	 * space, I have to traverse the hierarchy
	 */
	i =  primIndex;
	while( Prim[i].parentPrim != -1){
		followMasterPrimitive( i, &m, t, &v);
		i = Prim[i].parentPrim;
		//break;
	}
	
	/*
	 
	 * At this point I assume that the vertex
	 
	 * is in MASTER PRIMITIVE space and therefore
	 
	 * the only transformations still to be
	 
	 * applied are the ones relative to the master
	 
	 */
	
	fromMaster2World( &v );
	
	
	/* copy new vertex position into destination variable */
	
	vReturn->x = v.x;
	
	vReturn->y = v.y;
	
	vReturn->z = v.z;
	
	//printf ("\nAiiiiiiiiiiiiiiiiiiiiiiii\n");
	
	return( d );	/* returns the distance */
}

/*
 *-----------------------------------------------------
 *	Convert from Master space into World space
 *-----------------------------------------------------
 */
void fromMaster2World( Point3D *vReturn )
{
    //printf("\nFROM MASTER TO WORLD:\n");
	Matrix4 m, tm, rm, gm, sm;
	
	Point3D v;
	
	
	
	/* get vertex to be transformed */
	
	v.x = vReturn->x;
	
	v.y = vReturn->y;
	
	v.z = vReturn->z;
	
	
	
	buildRotMatrix( &rm, MASTER_PRIM);
	
	buildGrowthMatrix( &gm, MASTER_PRIM);
	
	buildTranslMatrix( &tm, MASTER_PRIM);
	
	
	
#ifdef DEBUG
	
	printf("\n============From Master to World coordinates=========\n");
	
#endif
	
	
	
	/* #ifdef RELATIVE
	 
	 buildScaleMatrix( &sm, MASTER_PRIM);
	 
	 V3PreMul(&v, &sm);*/
	
#ifdef DEBUG
	
	//printMatrixInfo("master scaling matrix", sm, v);
	
#endif
	
	/*#endif*/
	
	
	
	V3PreMul(&v, &gm);
	
#ifdef DEBUG
	
	printMatrixInfo("master growth matrix", gm, v);
	
#endif
	
	
	
	V3PreMul(&v, &rm);
	
#ifdef DEBUG
	
	printMatrixInfo("master rotation matrix", rm, v);
	
#endif
	
	
	
	V3PreMul(&v, &tm);
	
#ifdef DEBUG
	
	printMatrixInfo("master translation matrix", tm, v);
	
#endif
	
	
	
	/* copy new vertex position into destination variable */
	
	vReturn->x = v.x;
	
	vReturn->y = v.y;
	
	vReturn->z = v.z;
	
}


/*
 *----------------------------------------------------------
 *
 *
 *----------------------------------------------------------
 */
void followMasterPrimitive( int primIndex, Matrix4 *m, int tim,
						   
						   Point3D *vReturn)

{
	
	int parentIndex;
	
	Matrix4 g, r, t, s;			/* primitive matrices */
	
	Matrix4 gpi, rpi, tpi, spi,
	
	gp, rp, tp, sp, inverseParent;	/* parent matrices */
	
	Matrix4 result;
	
	Point3D v;
	
	
	
	/* for test of rotation */
	
	Matrix4 testRotation;
	
	
	
	/* this is the test to include rotations */
	
	buildTestRot( &testRotation, primIndex, tim );
	
	
	
	loadIdentity(m);
	
    
	
	/* get vertex to be transformed */
	
	v.x = vReturn->x;
	
	v.y = vReturn->y;
	
	v.z = vReturn->z;
	
	
	
	/* index for the parent primitive */
	
	parentIndex = Prim[primIndex].parentPrim;
	
	if (parentIndex == -1)
		
		errorMsg("Bug in hierarchy of primitives. Trying to acess undefined primitive!");
	
	
	
	/*  primitive matrices. Go from primitive space
	 
	 into world space */
	
	buildGrowthMatrix(&g, primIndex);
	
	buildTranslMatrix(&t, primIndex);
	
	buildRotMatrix(&r, primIndex);
	
	
	
	/* #ifdef RELATIVE
	 
	 buildScaleMatrix(&s, primIndex);
	 
	 #endif */
	
	
	
	/* parent matrices. Go from world space into
	 
     primitive space */
	
	buildInvGrowthMatrix(&gpi, parentIndex);
	
	buildInvTranslMatrix(&tpi, parentIndex);
	
	buildInvRotMatrix(&rpi, parentIndex);
	
	buildInvScaleMatrix(&spi, parentIndex);
	
	
	
	buildGrowthMatrix(&gp, parentIndex);
	
	buildTranslMatrix(&tp, parentIndex);
	
	buildRotMatrix(&rp, parentIndex);
	
	buildScaleMatrix(&sp, parentIndex);
	
	buildInvTransfMatrix(&inverseParent, parentIndex);
	
	
	
	/* #ifdef RELATIVE */
	
	/* apply scaling */
	
	/* V3PreMul(&v, &s); */
	
#ifdef DEBUG
	
	printf("\n====== %d -> %d =======\n",primIndex,parentIndex);
	
	//printMatrixInfo("Primitive scaling", s, v);
	
#endif
	
	/* #endif */
	
	
	
	/* apply growth */
	
	V3PreMul(&v, &g);
	
#ifdef DEBUG
	
	printMatrixInfo("Primitive growth", g, v);
	
#endif
	
	
	
	/* test with the rotation stuff */
	
	V3PreMul(&v, &testRotation);
	
	
	
	/* undo rotation */
	
	V3PreMul(&v, &r);
	
#ifdef DEBUG
	
	printMatrixInfo("Primitive rotation", r, v);
	
#endif
	
	
//Begin ADD after XXX .......................................................................

	V3PreMul(&v, &rpi);
	
#ifdef DEBUG
	
	printMatrixInfo("Parent inverse of rotation", rpi, v);
	
#endif
	
	
	
	// apply inverse of parent growth
	
	V3PreMul(&v, &gpi);
	
#ifdef DEBUG
	
	printMatrixInfo("Parent inverse of growth", gpi, v);
	
#endif
	
	
	
	V3PreMul(&v, &sp);
	
#ifdef DEBUG
	
	printMatrixInfo("Parent direct scaling", sp, v);
	
#endif

	
	
	V3PreMul(&v, &rp);
	
#ifdef DEBUG
	
	printMatrixInfo("Parent direct rotation", rp, v);
	
#endif

	//End ADD after XXX .......................................................................
	

	V3PreMul(&v, &t);
	
#ifdef DEBUG
	
	printMatrixInfo("Primitive translation", t, v);
	
#endif
	
	
	
	/* Instead of multiplying each individual matrix
	 
     I'm multiplying for the whole inverse transformation */
	
	// V3PreMul(&v, &inverseParent);
	
	
	
	V3PreMul(&v, &tpi); 
	
#ifdef DEBUG
	
	printMatrixInfo("Parent inverse translation", tpi, v);
	
#endif
	
	
	
	V3PreMul(&v, &rpi); 
	
#ifdef DEBUG
	
	printMatrixInfo("Parent inverse rotation", rpi, v);
	
#endif
	
	
	
	V3PreMul(&v, &spi); //Modified after from gpi to spi XXX .............................
	
#ifdef DEBUG
	
	printMatrixInfo("Parent inverse scaling", spi, v);
	
#endif
	
	
	
	/* copy new vertex position into destination variable */
	
	vReturn->x = v.x;
	
	vReturn->y = v.y;
	
	vReturn->z = v.z;
	
	
	
}

/*
 *-----------------------------------------------------------
 *	This morphs the primitives
 *-----------------------------------------------------------
 */
void morphPrimitives( int t )
{
	
	int i, j;
	Matrix4 m, tm, rm, gm;
	Point3D v;
	
	//int oldH,oldR;
	
	/* the loop below starts in 1 since primitive 0
	 *  is the WORLD (camera transformation)
	 */
	for(i=1; i < NumberPrimitives; i++){
		V3Zero(&v);
		
#ifdef DEBUG
		printf("\n==========================\n");
		
#endif
		
		if (Prim[i].type == CYLINDER){
			//printf("primitiva: %d de h = %f e r= %f\n", i,Prim[i].h,Prim[i].r);
			growRadiusHeight( i, &(Prim[i].h), &(Prim[i].r), t);
			
			j = i;
			while( Prim[j].parentPrim != -1){
				followMasterPrimitive( j, &m, t, &v);
				j = Prim[j].parentPrim;
			}
			/* convert from Master to World coordinate system */
			fromMaster2World( &v );
			
			/*new position of primitive*/
			Prim[i].center.x = v.x - Prim[i].trans[X];
			Prim[i].center.y = v.y - Prim[i].trans[Y];
			Prim[i].center.z = v.z - Prim[i].trans[Z];
			
		}else if (Prim[i].type == LANDMARK){
			
		}else
			errorMsg("Got unknown primitive type in morph primitives"); 
	}
}
/*
 *----------------------------------------------------------
 *
 *
 *----------------------------------------------------------
 */
void growRadiusHeight( int whichPrim, float *h, float *r, int t)
{
	
	//debug
    /*int j;
	 for(j=1; j< NumberPrimitives; j++ ){
     printf("comeco de growRadiusHeight: Escala da primitiva: %f, primitiva: %d de h = %f e r= %f\n", Prim[j].scale[X], j,Prim[j].h,Prim[j].r);
     
	 }
	 printf("whichPrim: %d, h: %f, r: %f, tempo: %f \n", whichPrim, *h, *r, t);*/
	int lm1, lm2;
	double a1, a2, b1, b2;
	double l1s, l2s, lR, lH, rate;
	double hSquare, dSquare;
	double refLength, primLength, scaleFactor = -1;
	int refIndex;
	
	lm1 = Prim[whichPrim].landMarkR;
	lm2 = Prim[whichPrim].landMarkH;
	/* The idea here is to scale the body according to the
	 * growth data, but keeping the size of the adult, so
	 * that we can check the change in proportions better
	 * We will use a feature associated with the body
	 * Arbitrarily we picked the first primitive, which
	 * is usually the body and we are using the feature
	 * which controls the lenght of the body (landMarkH)
	 */
	if ( keepAspectFlag ){
		refIndex = Prim[1].landMarkH;
		refLength = growthData[FINAL_GROWTH_TIME-1][Prim[refIndex].gIndex];
		primLength = growthData[t][Prim[refIndex].gIndex];
		scaleFactor = refLength / primLength;
	}
	//computingPatternFlag = FALSE;
	if ( computingPatternFlag ){
		if ( t == -1 ){
			/*
			 * For now, I am computing the first round of splits
			 * by reducing the embryo size 4%, which corresponds
			 * more or less to one day increase in area
			 */
			lR = growthData[EMBRYOAGE][Prim[lm1].gIndex] *  0.99;/*0.96*/
			lH = growthData[EMBRYOAGE][Prim[lm2].gIndex] *  0.99;/*0.96*/
			l1s = lR * lR;
			l2s = lH * lH;
		}
		/*
		 * The rate here is given by the current date divided by the
		 * amount of days that pattern formation takes place, which
		 * is the value for finalTime
		 */
		else{
			int aux;
			if(t<= TOTALGESTATIONTIME)
			{
				rate = (double) (t) / TOTALGESTATIONTIME;
				lR = growthData[EMBRYOAGE][Prim[lm1].gIndex] + ( rate * ( growthData[NEWBORNAGE][Prim[lm1].gIndex] - growthData[EMBRYOAGE][Prim[lm1].gIndex] ) );
				lH = growthData[EMBRYOAGE][Prim[lm2].gIndex] + ( rate * ( growthData[NEWBORNAGE][Prim[lm2].gIndex] - growthData[EMBRYOAGE][Prim[lm2].gIndex] ) );
				
			}
	
			if(t>TOTALGESTATIONTIME && t<=(TOTALGESTATIONTIME+5))
			{
				rate = t/ (TOTALGESTATIONTIME+5);
				lR = growthData[NEWBORNAGE][Prim[lm1].gIndex] + ( rate * ( growthData[2][Prim[lm1].gIndex] - growthData[NEWBORNAGE][Prim[lm1].gIndex] ) );
				lH = growthData[NEWBORNAGE][Prim[lm2].gIndex] + ( rate * ( growthData[2][Prim[lm2].gIndex] - growthData[NEWBORNAGE][Prim[lm2].gIndex] ) );
			}
			if(t>(TOTALGESTATIONTIME+5) && t<=(TOTALGESTATIONTIME+10))
			   {
			   rate = t/ (TOTALGESTATIONTIME+10);
			   lR = growthData[2][Prim[lm1].gIndex] + ( rate * ( growthData[3][Prim[lm1].gIndex] - growthData[2][Prim[lm1].gIndex] ) );
			   lH = growthData[2][Prim[lm2].gIndex] + ( rate * ( growthData[3][Prim[lm2].gIndex] - growthData[2][Prim[lm2].gIndex] ) );
			   }
			   
			if(t>(TOTALGESTATIONTIME+10) && t<=(TOTALGESTATIONTIME+15))
			{
				rate = t/ (TOTALGESTATIONTIME+15);
				lR = growthData[3][Prim[lm1].gIndex] + ( rate * ( growthData[4][Prim[lm1].gIndex] - growthData[3][Prim[lm1].gIndex] ) );
				lH = growthData[3][Prim[lm2].gIndex] + ( rate * ( growthData[4][Prim[lm2].gIndex] - growthData[3][Prim[lm2].gIndex] ) );
			}
			if(t>(TOTALGESTATIONTIME+15) && t<=(TOTALGESTATIONTIME+20))
			{
				rate = t/ (TOTALGESTATIONTIME+20);
				lR = growthData[4][Prim[lm1].gIndex] + ( rate * ( growthData[5][Prim[lm1].gIndex] - growthData[4][Prim[lm1].gIndex] ) );
				lH = growthData[4][Prim[lm2].gIndex] + ( rate * ( growthData[5][Prim[lm2].gIndex] - growthData[4][Prim[lm2].gIndex] ) );
			}
			if(t>(TOTALGESTATIONTIME+20) && t<=(TOTALGESTATIONTIME+25))
			{
				rate = t/ (TOTALGESTATIONTIME+25);
				lR = growthData[5][Prim[lm1].gIndex] + ( rate * ( growthData[6][Prim[lm1].gIndex] - growthData[5][Prim[lm1].gIndex] ) );
				lH = growthData[5][Prim[lm2].gIndex] + ( rate * ( growthData[6][Prim[lm2].gIndex] - growthData[5][Prim[lm2].gIndex] ) );
			}
			if(t>(TOTALGESTATIONTIME+25))
			{
				rate = t/ (TOTALGESTATIONTIME+30);
				lR = growthData[6][Prim[lm1].gIndex] + ( rate * ( growthData[7][Prim[lm1].gIndex] - growthData[6][Prim[lm1].gIndex] ) );
				lH = growthData[6][Prim[lm2].gIndex] + ( rate * ( growthData[7][Prim[lm2].gIndex] - growthData[6][Prim[lm2].gIndex] ) );
			}
						
			l1s = lR * lR;
			l2s = lH * lH;
			
			
			//l1s = growthData[t][Prim[lm1].gIndex] * growthData[t][Prim[lm1].gIndex];
			//l2s = growthData[t][Prim[lm2].gIndex] * growthData[t][Prim[lm2].gIndex];
		}
	}
	else{
		/* I am not computing pattern plus growth. Do the usual way */
		//printf("ATENÇAO: %f %d %d \n", growthData[t][Prim[lm1].gIndex],Prim[lm1].gIndex,t);
		l1s = growthData[t][Prim[lm1].gIndex] * growthData[t][Prim[lm1].gIndex];
		l2s = growthData[t][Prim[lm2].gIndex] * growthData[t][Prim[lm2].gIndex];
	}
	/*fprintf( stderr, "Prim %d lR = %f lH = %f\n",
	 whichPrim, l1s, l2s );*/
	/* printf("prim = %d rIndex = %d hIndex = %d\n",
	 whichPrim, Prim[lm1].gIndex, Prim[lm2].gIndex);
	 printf("t = %d l1 = %f l2 = %f\n",t,
	 growthData[t][Prim[lm1].gIndex],
	 growthData[t][Prim[lm2].gIndex]);*/ 
	a1 = Prim[lm1].dus + Prim[lm1].dvs;
	b1 = Prim[lm1].dws;
	a2 = Prim[lm2].dus + Prim[lm2].dvs;
	b2 = Prim[lm2].dws;
	
	hSquare =  -(l1s * a2 - a1 * l2s) / (-a2 * b1 + a1 * b2 );
	dSquare = (-l2s * b1 + b2 * l1s) / (-a2 * b1 + a1 * b2);
	if ( hSquare < 0 || dSquare < 0 ){
		printf("lm2 = %d Prim[lm2].gIndex = %d\n", lm2, Prim[lm2].gIndex);
		printf("growth data = %f\n",growthData[t][Prim[lm2].gIndex]);
		printf("a1 = %f b1 = %f a2 = %f b2 = %f\n",a1,b1,a2,b2);
		printf("l1s = %f l2s = %f\n", l1s, l2s);
		printf("Primitive = %d\n", whichPrim);
		printf("Time = %d hSquare = %f dSquare= %f\n", t, hSquare, dSquare);
		errorMsg("Either radius or height negative...");
	}
	else{
		
		*h = sqrt(hSquare);
		/* I'm not too sure yet why I don't have to divide by 2 here */
		*r = sqrt(dSquare);
		/* this flag keeps the size proportional to the
		 adult size */
		if ( keepAspectFlag && scaleFactor != -1 ){
			
			*h *= scaleFactor;
			*r *= scaleFactor;
		}
	}
	
    //debug
    /*for(j=1; j< NumberPrimitives; j++ ){
     printf("Fim de growRadiusHeight: Escala da primitiva: %f, primitiva: %d de h = %f e r= %f\n", Prim[j].scale[X], j,Prim[j].h,Prim[j].r);
     
	 }*/
	
	
}
/*
 *-------------------------------------------------------
 * 	Test the morphing function. Does one increment
 *	of time only. If 'direction' is TRUE the time
 *	decreases; if 'direction' is FALSE the time
 *	increases
 *-------------------------------------------------------
 */
void morphOneStep( int direction )
{
	static int inct;
	int n;
	
	if ( !parametOKFlag ){
		fprintf( stderr, "\tYou have to parametrize the model first!\n");
		beep();
		return;
	}
	
	else{
		if ( direction ){
			inct = -1;
			if ( growthTime <= 0 ){
				growthTime=0;
				inct = 1;
			}
		}
		else{
			inct = 1;
			if ( growthTime >= FINAL_GROWTH_TIME ){
				growthTime=FINAL_GROWTH_TIME;
				inct = -1;
			}
		}
		growthTime += inct;
		morph( growthTime );
		n = growthData[growthTime][0];
		printf("Current time is %d\n", n);
#ifdef GRAPHICS
		glutPostRedisplay();
#endif
	}
	
}

int getGrowthData( int day )
{
	int n;
	//growthTime = day;
	n = growthData[day][0];
	//printf("Current time is %d\n", n);

	return n;
	
}
