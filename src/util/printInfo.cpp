/*
 *  printInfo.c
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/10/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */

#include "printInfo.h"

#include <stdio.h>

#include "data/cellsList.h"
#include "data/Object.h"
#include "control/primitives.h"

/*
 *-----------------------------------------------------------------
 *	Prints information about the current active primitive
 *-----------------------------------------------------------------
 */
void printActivePrim( void )
{

	int i;

	printf("Primitive Number = %d\n", activePrim);

	if (Prim[activePrim].type == CYLINDER){
		printf("Cylinder\n");
		printf("Center "); printPoint3D(Prim[activePrim].center);
		printf("h=%f r=%f\n", Prim[activePrim].h, Prim[activePrim].r);
		printf("Index for Parent = %d\n", Prim[activePrim].parentPrim);
		printf("Index for LandMarkR = %d\n", Prim[activePrim].landMarkR);
		printf("Index for LandMarkH = %d\n", Prim[activePrim].landMarkH);
		printf("Has animation information associated: %d\n", Prim[activePrim].animation);
		printf("Textures IDs: ");
		for( i = 0; i < N_TEXT_PER_PRIM; i++ )
			printf("%d ", Prim[activePrim].textID[i] );
		printf("\n");
	}
	else if (Prim[activePrim].type == LANDMARK){
		printf("LandMark\n");
		printf("Growth index: %d\n", Prim[activePrim].gIndex);
		printf("Primitive index = %d\n",Prim[activePrim].primInd);
		printf("Length in WORLD coordinates = %f\n", Prim[activePrim].lengthW);
		printf("Length in CYLINDER coordinates = %f\n", Prim[activePrim].lengthP);
		printf("== Original points definition ==\n");
		printf("q1 "); printPoint3D(Prim[activePrim].q1);
		printf("q2 "); printPoint3D(Prim[activePrim].q2);
		printf("== Primitive space points definition ==\n");
		printf("p1 "); printPoint3D(Prim[activePrim].p1);
		printf("p2 "); printPoint3D(Prim[activePrim].p2);

	}
	printf("Translation x=%f y=%f z=%f\n",
		   Prim[activePrim].trans[X],
		   Prim[activePrim].trans[Y],
		   Prim[activePrim].trans[Z]);
	printf("Rotation x=%f y=%f z=%f\n",
		   Prim[activePrim].rot[X],
		   Prim[activePrim].rot[Y],
		   Prim[activePrim].rot[Z]);
	printf("Scale x=%f y=%f z=%f\n",
		   Prim[activePrim].scale[X],
		   Prim[activePrim].scale[Y],
		   Prim[activePrim].scale[Z]);
	printf("\n");
}
/*
 *-----------------------------------------------------------------
 *-----------------------------------------------------------------
 */
void printArray(float **p, int rows, int columns)
{
	int i,j;

	for(i=0; i < rows; i++){
		for(j=0; j < columns; j++){
			printf("%3.3f ", p[i][j]);
		}
		printf("\n");
	}
}

/*
 *------------------------------------------------------------
 *	Print information about the primitives
 *------------------------------------------------------------
 */
void printInfoPrim(int nPrim)
{
	int i, j;

	printf("============================\n");
	printf("    Primitives Information  \n");
	printf("============================\n");
	for(i = 1; i < nPrim; i++ ){
		if (Prim[i].type == CYLINDER){
			printf("Cylinder\n");
			printf("Primitive number = %d\n", i);
			printf("Center "); printPoint3D(Prim[i].center);
			printf("h=%f r=%f\n", Prim[i].h, Prim[i].r);
			printf("Index for Parent = %d\n", Prim[i].parentPrim);
			printf("Index for LandMarkR = %d\n", Prim[i].landMarkR);
			printf("Index for LandMarkH = %d\n", Prim[i].landMarkH);
			printf("Animation information = %d\n", Prim[i].animation);
			printf("Textures IDs: ");
			for( j = 0; j < N_TEXT_PER_PRIM; j++ )
				printf("%d ", Prim[i].textID[j] );
			printf("\n");
		}
		else  if (Prim[i].type == LANDMARK){
			printf("LandMark\n");
			printf("Primitive number = %d\n", i);
			printf("Growth index: %d\n", Prim[i].gIndex);
			printf("Primitive index = %d\n",Prim[i].primInd);
			printf("Length in WORLD coordinates = %f\n", Prim[i].lengthW);
			printf("Length in CYLINDER coordinates = %f\n", Prim[i].lengthP);
			printf("== Original points definition ==\n");
			printf("q1 "); printPoint3D(Prim[i].q1);
			printf("q2 "); printPoint3D(Prim[i].q2);
			printf("== Primitive space points definition ==\n");
			printf("p1 "); printPoint3D(Prim[i].p1);
			printf("p2 "); printPoint3D(Prim[i].p2);
		}
		else printf("Sphere\n");
		printf("Translation x=%f y=%f z=%f\n",
			   Prim[i].trans[X],
			   Prim[i].trans[Y],
			   Prim[i].trans[Z]);
		printf("Rotation x=%f y=%f z=%f\n",
			   Prim[i].rot[X],
			   Prim[i].rot[Y],
			   Prim[i].rot[Z]);
		printf("Scale x=%f y=%f z=%f\n",
			   Prim[i].scale[X],
			   Prim[i].scale[Y],
			   Prim[i].scale[Z]);
		/* printf("Ratios: x = %f y = %f z = %f\n",
		 Prim[i].ratio[X], Prim[i].ratio[Y], Prim[i].ratio[Z]); */
		printf("\n");
	}
}

/*
 *------------------------------------------------------------
 *	Print a single 3D point
 *------------------------------------------------------------
 */
void printPoint3D( Point3D p)
{
	printf("x: %lg y: %lg z: %lg\n", p.x, p.y, p.z);
}
/*
 *------------------------------------------------------------
 *	Print a single 2D point
 *------------------------------------------------------------
 */
void printPoint2D( Point2D p)
{
	printf("x: %lg y: %lg\n", p.x, p.y);
}
/*
 *------------------------------------------------------------
 *	Print faces information
 *------------------------------------------------------------
 */
void printFacesInfo(void)
{
	int i, j;


	printf("=======================\n");
	printf("    Faces Information  \n");
	printf("=======================\n");
	/* print the faces */
	for (i=0; i < NumberFaces; i++){
		printf("Face[%d] ", i);
		printf("Number of Vertices = %d\n", faces[i].nverts);
		printf("Absolut Area = %3.2lg Previous Absolut Area = %3.2lg\n",
			   faces[i].absArea, faces[i].prevAbsArea);
		printf("Relative Area = %3.2lg\n", faces[i].relArea );
		printf("Partial Sum Area = %lg\n", faces[i].partArea);
		printf("Geometry vertices: ");
		for(j=0; j < faces[i].nverts; j++){
			printf("%d ",faces[i].v[j]);
		}
		printf("\n");
		printf("Texture vertices: ");
		for(j=0; j < faces[i].nverts; j++){
			printf("%d ",faces[i].vt[j]);
		}
		printf("\n");
		printf("prim1 = %d prim2 = %d\n", faces[i].prim1, faces[i].prim2);
		printf("Plane coef: a = %3.2f b = %3.2f c = %3.2f d = %3.2f\n", faces[i].planeCoef.a,
			   faces[i].planeCoef.b, faces[i].planeCoef.c, faces[i].planeCoef.d);
		printf("v1: %3.2f %3.2f %3.2f\n", faces[i].v1.x, faces[i].v1.y, faces[i].v1.z);
		printf("v2: %3.2f %3.2f %3.2f\n", faces[i].v2.x, faces[i].v2.y, faces[i].v2.z);
		printf("o2p:\n");
		printMatrix( faces[i].p2o );
		printMatrix( faces[i].o2p );
		printf("p2o:\n");
		printf("center: %3.2f %3.2f %3.2f\n",
			   faces[i].center3D.x, faces[i].center3D.y, faces[i].center3D.z);
		printf("Edge pointer %d\n", faces[i].edge);
		printf("%d adjacent faces: ", faces[i].nPrimFaces);
		for(j=0; j < faces[i].nPrimFaces; j++){
			printf("face %d with edge %d and angle %3.2f\n",
				   faces[i].primFaces[j], faces[i].primEdges[j], faces[i].rotAngles[j]);
		}
		printf("\n");
		printf("%d cells for this face:\n", faces[i].ncells);
		printf("C[%d] D[%d] E[%d] F[%d]\n",
			   faces[i].nCellsType[C], faces[i].nCellsType[D],
			   faces[i].nCellsType[E], faces[i].nCellsType[F]);
		printListOfCells( faces[i].head, faces[i].tail);
		printf("\n");
	}

}

/*
 *------------------------------------------------------------
 *	Print vertices information
 *------------------------------------------------------------
 */
void printVertexInfo(void)
{
	int i, j;

	printf("==========================\n");
	printf("    Vertices Information  \n");
	printf("==========================\n");
	printf("vertex \tx \ty \tz \tprim1 \tprim2 \tedge \tadjFaces\n");
	/* print the vertices */
	for ( i = 0; i < NumberVertices; i++ ){
		printf("%d \t%2.2f \t%2.2f \t%2.2f \t%d \t%d \t%d",
			   i, vert[i].pos.x, vert[i].pos.y, vert[i].pos.z,
			   vert[i].prim1, vert[i].prim2, vert[i].edge);
		printf("\t");
		for( j = 0; j < vert[i].nNeighFaces; j++)
			printf("%d ", vert[i].neighFaces[j] );
		printf("\n");
	}

	/*printf("text vertex \tu \tv\n");
	 for (i=0; i < NumberTextureVertices; i++){
	 printf("%d \t%2.2f \t%2.2f\n", i, vertt[i].x, vertt[i].y);
	 }*/
}

/*
 *------------------------------------------------------------
 * 	This is an auxiliary routine for when I am
 *	debugging the transformations
 *------------------------------------------------------------
 */
void printMatrixInfo(char *s, Matrix4 m, Point3D v)
{
	printf("%s:\n", s);
	printMatrix(m);
	printf("After: %lf %lf %lf\n\n",v.x,v.y,v.z);
}

/*
 *--------------------------------------------------
 * Prints a matrix
 *-------------------------------------------------
 */
void
printMatrix(Matrix4 m)
{
	int i, j;

	/*printf("===========================\n");*/
	for(i=0;i<4;i++){
		for(j=0;j<4;j++){
			printf( "[%d][%d] = %lg ", i, j, m.element[i][j] );
		}
		printf( "\n" );
	}
	printf( "\n" );
}

/*
 *------------------------------------------------------------
 *	Print edges data structure
 *------------------------------------------------------------
 */
void printEdges( void )
{

	int i;

	printf("==========================\n");
	printf("    Edges Information     \n");
	printf("==========================\n");

	/* print the edges */
	printf("edge \tvstart \tvend \tepcw \tepccw \tencw \tenccw \tpf \tnf\n");
	for( i = 0; i < NumberEdges; i++){
		printf("%d \t%d \t%d \t%d \t%d \t%d \t%d \t%d \t%d\n",
			   i, edges[i].vstart, edges[i].vend, edges[i].epcw,
			   edges[i].epccw, edges[i].encw, edges[i].enccw,
			   edges[i].pf, edges[i].nf);
	}
	for( i = 0; i < NumberEdges; i++){
		printf("edge %d pf->nf matrix:\n", i);
		printMatrix(edges[i].pn);
		printf("nf->pf matrix:\n");
		printMatrix(edges[i].np);
	}
}

/*
 *------------------------------------------------------------
 *	Print cells info
 *------------------------------------------------------------
 */
void printCellsInfo( void )
{
	int i;

	for( i = 0; i < NumberFaces; i++ ){
		if ( faces[i].ncells != 0 ){
			printf("Face %d\n", i );
			printListOfCells( faces[i].head, faces[i].tail );
		}
	}
}

/*
 *------------------------------------------------------------
 *	Print cells info
 *------------------------------------------------------------
 */
void printCellsTimeSplit( void )
{
	int i;
	CELL *c;

	printf( "#type C splitTime\n");
	for( i = 0; i < NumberFaces; i++ ){
		if ( faces[i].ncells != 0 ){
			c = faces[i].head->next;
			while ( c != faces[i].tail){
				fprintf( stderr, "%f %f %f %d %d\n", c->x, c->y, c->z, c->prim1, c->prim2 );
				c = c->next;
			}
		}
	}
	/*
	 printf( "#type D splitTime\n");
	 for( i = 0; i < NumberFaces; i++ ){
	 if ( faces[i].ncells != 0 ){
	 c = faces[i].head->next;
	 while ( c != faces[i].tail){
	 if ( c->ctype == D )
	 printf( "%f\n", c->tsplit );
	 c = c->next;
	 }
	 }
	 }*/
}


/*
 *------------------------------------------------------------
 *	Print tmp list of cells
 *------------------------------------------------------------
 */
void printTmpListCells( int whichFace,  SCELL *h, SCELL *t )
{

	SCELL *runner;

	printf("face %d\n", whichFace );
	runner = h->next;
	while( runner != t ){
		printf( "%lg %lg\n", runner->p.x, runner->p.y );
		runner = runner->next;
	}
}

/*
 *---------------------------------------------------------
 *	printFinalNumberOfCells
 *---------------------------------------------------------
 */
void printFinalNumberOfCells( int *nCellsType )
{
	float totalNumberCells;

	totalNumberCells = nCellsType[C] +
    nCellsType[D] +
    nCellsType[E] +
    nCellsType[F];

	printf( "Final Number of Cells: %d (by printInfo.c)\n", (int) totalNumberCells );
	printf( "[C] = %d (%3.3f%%) [D] = %d (%3.3f%%) [E]= %d (%3.3f%%) [F] = %d (%3.3f%%)\n",
		   nCellsType[C], ((float) nCellsType[C]/totalNumberCells)*100.0,
		   nCellsType[D], ((float) nCellsType[D]/totalNumberCells)*100.0,
		   nCellsType[E], ((float) nCellsType[E]/totalNumberCells)*100.0,
		   nCellsType[F], ((float) nCellsType[F]/totalNumberCells)*100.0 );
	printf( "Average number of cells per face: %3.3f\n",
		   totalNumberCells / NumberFaces );
}

/*
 * Mode:
 *	0 - stderr
 *	1 - stdout
 *	2 - stdin
 */
void printString( int mode, char *string )
{
	if( mode == 0)
		fprintf( stderr, "%s \n", string );
}
