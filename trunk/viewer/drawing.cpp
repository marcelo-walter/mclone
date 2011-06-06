/*
 *------------------------------------------------------------
 *	drawing.c
 *	Marcelo Walter
 *
 *	This module contains auxiliary 'drawing' routines
 *	in OpenGL
 *------------------------------------------------------------
 */

#include "drawing.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "graph.h"
#include "ui.h"

#include "../main.h"
#include "../temporary.h"

#include "../util/interfaceVoronoi.h"
#include "../util/transformations.h"
#include "../data/cellsList.h"
#include "../control/Growth.h"
#include "../control/primitives.h"
#include "../control/texture.h"
#include "../control/vectorField.h"
#include "../simulation/relax.h"

/*
 *-----------------------------------------------
 *	Local definitions
 *-----------------------------------------------
 */

flag sceneFlag = FALSE;
flag shadowFlag = FALSE;
flag modeFlag = TRUE;
flag vectorsFlag = FALSE;	/* show or not the vectors
 that define a local coordinate
 system for the polygon */
flag normalsFlag = FALSE;	/* Vin?cius 17/09/2004 */
flag voronoiBorders = TRUE;

flag bwOutput = FALSE;
flag degreeVector = FALSE;	/* Thompson 07/11/2003 */

flag drawVector = TRUE;
flag drawVectorField = FALSE;
flag drawVerticesVectorField = FALSE;

/* color definitions */
float backColor[XYZ] = {0.0, 0.0, 0.0};
float foreColor[XYZ] = {1.0, 0.0, 0.0};
float colorBlue[XYZ] = {0.0, 0.0, 1.0};
float colorRed[XYZ] = {1.0, 0.0, 0.0};
/*float colorRed[XYZ] = {0.0, 0.0, 0.0};*/
/* float colorWhite[XYZ] = {0.0, 0.0, 0.0};*/
float colorWhite[XYZ] = {0.4, 0.4, 0.4}; //Confere
float colorGrey[XYZ] = {0.3, 0.3, 0.3};
float colorGreen[XYZ] = {0.0, 1.0, 0.0};
float colorYellow[XYZ] = {1.0, 1.0, 0.0};
float colorBrown[XYZ] = {0.41, 0.24, 0.14};
/*float colorBrown[XYZ] = {0.5, 0.5, 0.5};*/
float colorMagenta[XYZ] = {0.0, 0.9, 0.9};
float colorDarkYellow[XYZ] = {0.988, 0.596, 0.298};
float colorBuff[XYZ] = {0.847, 0.439, 0.1255};
float colorMediumBrown[XYZ] = {0.506, 0.3647, 0.2353};

/*
 *-----------------------------------------------
 *-----------------------------------------------
 */
void setVertexDisplayShadow(int ix, double y)
{
	glVertex3d(vertDisplay[ix].x, y, vertDisplay[ix].z);
}

/*
 *----------------------------------------------------------
 *	Calls all the drawing routines
 *----------------------------------------------------------
 */
void drawAll(MMODE mmode, VMODE vmode, RMODE rmode, int t)
{
    if (gridFlag) 
		glCallList(MY_GRID_LIST);
    if ( sceneFlag ) /*glCallList( MY_SCENE_LIST );*/
    	drawScene();
	
    /*
     * draw Voronoi and check if we are moving the trackball
     * If so we don't draw the voronoi to speed up camera
     * positioning
     */
    if ( voronoiComputed )
	{
    	if ( mouseMovingFlag )
		{
			objectFlag = TRUE;
      	}
      	else
		{
			if ( voronoiFlag )
			{
				objectFlag = FALSE;
	  			drawVoronoi();
			}
			if ( voronoiBorders ) 
				drawVoronoiBorders();
      	}
    }
	
    if ( objectFlag )
	{
    	switch(rmode)
		{
      		case FILL:
				if ( mouseMovingFlag ) 
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				else 
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glEnable(GL_CULL_FACE);
				drawObject();
				glDisable(GL_CULL_FACE);
				break;
      		case WIRE:/* basic wireframe mode */
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				/*glDisable(GL_DEPTH_TEST);
				 glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				 glLineWidth(3.f);*/
				drawObject();
				/*glLineWidth(1.f);
				 glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				 glEnable(GL_DEPTH_TEST);*/
				break;
			case HALO:
				/* draw wide lines into depth buffer */
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
				glLineWidth(9.f);
				drawObject();
				/* draw narrow lines into color with depth test on */
				glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
				glLineWidth(3.f);
				glDepthFunc(GL_LEQUAL);
				drawObject();
				glDepthFunc(GL_LESS);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glLineWidth(1.f);
				break;
			default:
				break;
      	}
    }
    /* The shadow is just a hsck which draws the whole
	 giraffe squashed into a plane with y constant
	 and equal to the minimum value in the set of
	 vertices */
    if ( shadowFlag ) 
		drawShadows( minSet.y );
    /* write some useful info onto the screen */
    if ( modeFlag ) 
	{
		drawMode(mmode, vmode, polygonPicking, t);
		drawMode(mmode, vmode, cellPicking, t);
    }
    /* draw the primitives */
    if( showPrimitives || showLandMarks )
    	drawPrimitives( NumberPrimitives, activePrim, t );
    /* draws the cells */
    if( drawCells && cellsCreated ) 
		drawPoints();
	
	/* Thompson 07/11/2003 */
	if( degreeVector )
	{
		drawDegreeVector();
	}
	
	if (drawVectorField )
	{
		drawVectorsField();
	}
	
	/*Added by Fabiane Queiroz*/
	if (drawVerticesVectorField)
	{
		drawVerticesVectorsField();
	}
	
	if( drawVector )
	{
		/* draw orientation vectors */
		drawOrientationVectors();
	}
	
	if (createVect)
	{
		drawCentroids();
	}
}
/*
 *----------------------------------------------------------
 *	Draws the voronoi diagram
 *----------------------------------------------------------
 */
void drawVoronoi( void )
{
	int nVorVerts, whichFace;
	int i, j, k, t;
	Point3D q;
	CELL *c;
	float currentColor[XYZW], actualColor[RGB];
	/* for texture stuff */
	int prim1, textID;
	double u, v;
	ColChar textValue;
	
	glPolygonMode(GL_FRONT, GL_FILL);
	/*glPolygonMode(GL_BACK,  GL_LINE);*/
	glGetFloatv( GL_CURRENT_COLOR, currentColor );
	
	/*
	 * Loops over all faces
	 */
	for( whichFace = 0; whichFace < NumberFaces; whichFace++)
	{
		if ( cellPicking )
			glPushName( whichFace );
		/* I am using the normal vector to the face to
		 shade all voronoi polygons for this face */
		glNormal3d( faces[whichFace].planeCoef.a,
				   faces[whichFace].planeCoef.b,
				   faces[whichFace].planeCoef.c );
		
		for ( i = 0; i < faces[whichFace].nVorPol; i++)
		{
			if ( cellPicking )
				glPushName( i );	  /* for the picking part */
			
			nVorVerts = faces[whichFace].vorFacesList[i].faceSize;
			c = faces[whichFace].vorFacesList[i].site;
			if ( c != NULL )
			{
				/*
				 * MARCELO: This is the same case when I am saving the
				 * inventor file. There are faces without any cells
				 * and the color of that face should be some average from
				 * the neighbors. So far I am assigning a default color
				 * from the cell C
				 */
				actualColor[R] = Ortcell[c->ctype].color[R];
				actualColor[G] = Ortcell[c->ctype].color[G];
				actualColor[B] = Ortcell[c->ctype].color[B];
				/*
				 * If there is texture information, we might want to
				 * change the color of the cells based on that
				 * The texture ID for color is number 2
				 */
				if ( texturePresent )
				{
					prim1 = faces[whichFace].prim1;
					if ( prim1 != FALSE )
					{
						//fprintf( stderr, "\t prim1 = %d \n", prim1 );
						// Commented by Thompson at 27/10/2003
						textID = Prim[prim1].textID[COLOR_C]; 
						
						// The next 12 lines was added by Thompson at 27/10/2003
						/* cell_type = C */
						if( c->ctype == 0 && Prim[prim1].textID[COLOR_C] != -1 )
						{
							textID = Prim[prim1].textID[COLOR_C];
							//fprintf(stderr, "Prim %d Cell %d -> textId = %d \n", prim1, c->ctype, textID);
						}
						/* cell_type = D */
						if( c->ctype == 1 && Prim[prim1].textID[COLOR_D] != -1 )
						{
							textID = Prim[prim1].textID[COLOR_D];
							//fprintf(stderr, "Prim %d Cell %d -> textId = %d \n", prim1, c->ctype, textID);
						}
						
						//fprintf( stderr, "\t textID = %d \n", textID );
						if ( textID < totalNumberTextures && textID != -1 )
						{                                                   
							u = c->p1.u;
							v = c->p1.v;
							/*fprintf( stderr, "u = %f v = %f\n", u, v );*/
							textValue =  bilinearInterp( u, v, textID );
							//fprintf( stderr, "\t (%f %f %f)\n", textValue.redC, textValue.greenC, textValue.blueC );
							
							actualColor[R] = textValue.redC / 255.0;
							actualColor[G] = textValue.greenC / 255.0;
							actualColor[B] = textValue.blueC / 255.0;
							//fprintf(stderr, " -> Prim %d Cell %d -> textId = %d \n", prim1, c->ctype, textID);
							/* Added by Thompson Peter Lied at 30/10/2003 */
							textID = -1;
						}
						
						/*fprintf( stderr, "actualColor (%f %f %f)\n",
						 actualColor[R], actualColor[G], actualColor[B] );*/
					}
				}
				
				if (!lightFlag)
				/*glColor3fv( Ortcell[c->ctype].color );*/
					glColor3fv( actualColor );
				
				/*  if I need to smooth shade according to the
				 *  individual vertices colors, I have to use
				 *  glMaterialfv instead of glColor
				 */
				else
				/*glMaterialfv( GL_FRONT, GL_DIFFUSE, Ortcell[c->ctype].color);*/
					glMaterialfv( GL_FRONT, GL_DIFFUSE, actualColor);
				
				/*fprintf( stderr, "%f %f %f\n", actualColor[R],
				 actualColor[G], actualColor[B] );*/
			}
			else
			{
				if (!lightFlag)
					glColor3fv( Ortcell[C].color );
				else
					glMaterialfv( GL_FRONT, GL_DIFFUSE, Ortcell[C].color);
				/*fprintf( stderr, "%f %f %f\n", Ortcell[C].color[R],
				 Ortcell[C].color[G], Ortcell[C].color[B] );*/
			}
			
			/*
			 * Finally draw the polygons
			 */
			glBegin(GL_POLYGON);
			for (j = 0; j < nVorVerts; j++)
			{
				k = faces[whichFace].vorFacesList[i].vorPts[j];
				mapFromPolySpace(whichFace, faces[whichFace].vorPoints[k].x,
								 faces[whichFace].vorPoints[k].y, &q);
				/* mapPointOntoPlane( &q, whichFace); */
				
				glVertex3f( q.x, q.y, q.z);
			}
			glEnd();
			
			if ( cellPicking )
				glPopName();
		}
		if ( cellPicking )
			glPopName();
	}
	
	glColor4fv(currentColor);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}



/*
 *----------------------------------------------------------
 *----------------------------------------------------------
 */
void drawVoronoiBorders( void )
{
	
	int nVorVerts, whichFace;
	int i, j, k;
	Point3D q;
	float currentColor[XYZW];
	
	glGetFloatv( GL_CURRENT_COLOR, currentColor );
	
	/*fprintf( stderr, "Inside draw Voronoi borders the color is %f %f %f\n",
	 currentColor[0], currentColor[1], currentColor[2] );*/
	for( whichFace = 0; whichFace < NumberFaces; whichFace++){
		for ( i = 0; i < faces[whichFace].nVorPol; i++){
			nVorVerts = faces[whichFace].vorFacesList[i].faceSize;
			glColor3fv( backColor );
			glBegin(GL_LINE_LOOP);
			for (j = 0; j < nVorVerts; j++){
				k = faces[whichFace].vorFacesList[i].vorPts[j];
				mapFromPolySpace(whichFace, faces[whichFace].vorPoints[k].x,
								 faces[whichFace].vorPoints[k].y, &q);
				glVertex3f( q.x, q.y, q.z );
			}
			glEnd();
		}
	}
	glColor4fv( currentColor );
}
/*
 *----------------------------------------------------------
 *	This function draws the object specified
 * 	by the user. There is only one object at
 *	a time
 *----------------------------------------------------------
 */


void drawObject( void )
{
    int i, j;
	
    float c[XYZW];
    /*double y, miny;*/
    float currentColor[XYZW];
	
    /* test for the texture stuff */
    int vertIndex, prim1Index, textID;
    double textValue, u, v;
	
    /*miny = 1e30;*/
	
    glPushMatrix();
    /* draw faces first */
    for ( i = 0; i < NumberFaces; i++ )
	{
        if( polygonPicking )
		    glPushName(i);	/* for the picking part */
	
		
        glBegin(GL_POLYGON);
        /* draw vertices */
        for(j=0; j < faces[i].nverts; j++)
		{
            setColor( c , faces[i].v[j], i); // uncommented by Cadu in 05/11
			
			
			
            // printf("Vertice: %f, %f, %f\n", c[0], c[1], c[2]); // inserted by Cadu in 05/11
            /* Test for the texture stuff */
            /*vertIndex = faces[i].v[j];
			 prim1Index = vert[vertIndex].prim1;
			 if ( prim1Index == 2 ){
			 textID = Prim[prim1Index].textID[4];
			 u = vert[vertIndex].p1.u;
			 v = vert[vertIndex].p1.v;
			 textValue = bilinearInterp( u, v, textID );
			 c[X] *= textValue;
			 c[Y] *= textValue;
			 c[Z] *= textValue;
			 }*/
			
			
			
            if (!lightFlag)
			{
			    glColor3fv( c );
			}
            /*  if I need to smooth shade according to the
             *  individual vertices colors, I have to use
             *  glMaterialfv instead of glColor
             */
            else
				glMaterialfv( GL_FRONT, GL_DIFFUSE, c);
			    //glMaterialfv( GL_FRONT, GL_SPECULAR, c);
           
			
			/* check if there is any normal specified */
            if( NumberNormals > 0)
			    setNormal(faces[i].vn[j]);
            else
			/* I am using the normal vector to the face to
			 shade all voronoi polygons for this face */
	            glNormal3d( faces[i].planeCoef.a,
						   faces[i].planeCoef.b,
						   faces[i].planeCoef.c );
            
			/* if I want texture */
            if( NumberTextureVertices > 0 && textureFlag)
			{
	            setTexture(faces[i].vt[j], 2.0);
			}
			
            /* finally draws the vertex */
            /*y = */
			/*if(i== 11){
				glColor3fv( colorYellow );
			}*/
			teste(faces[i].v[j]);
            /*if ( y < miny ) miny = y;*/
        } /* go to next vertex */
		
	    glEnd();
		
	    if ( polygonPicking )
		    glPopName();
    } /* go to next face */
	
	if ( edgesFlag )
	    drawEdgesObject();
	drawVerticesObject();
    if ( showAxes )
	{
        glGetFloatv( GL_CURRENT_COLOR, currentColor );
        glColor3fv( backColor );
        glCallList(MY_AXES_LIST);
        glColor4fv( currentColor );
    }
	
    if ( defineLandMarks )
	    defLandMarks();
	
	if ( vectorsFlag )
	    drawVectors();
	
	if ( normalsFlag )
	    drawNormals();
	
    /* The shadow is just a hack which draws the whole
	 giraffe squashed into a plane with y constant
	 and equal to the minimum value in the set of
	 vertices */
    if ( shadowFlag )
	    drawShadows( minSet.y );
	
    glPopMatrix();
}
/*
 *----------------------------------------------------------
 * Added by Thompson Peter Lied at 07/11/2003
 *----------------------------------------------------------
 */
void drawDegreeVector( void )
{
	int i;
	float currentColor[XYZW];
	float x, y, z, newx, newy;
	Point2D q;
	Point3D p;
	
	//fprintf(stderr, "\t-> Degree Vector\n");
  	
	glGetFloatv( GL_CURRENT_COLOR, currentColor );
	
	
	for( i = 0; i < NumberFaces; i++)
	{ 
		glPushMatrix();
		
		//x = vert[faces[i].v[0]].pos.x;
		//y = vert[faces[i].v[0]].pos.y;
		//z = vert[faces[i].v[0]].pos.z;
		// //mapOntoPolySpace( whichFace, x, y, z, &q );
		//mapOntoPolySpace( i, x, y, z, &q );
		//fprintf(stderr, "V1 -> x = %f  y = %f \n", q.x, q.y);
		
		x = vert[faces[i].v[1]].pos.x;
		y = vert[faces[i].v[1]].pos.y;
		z = vert[faces[i].v[1]].pos.z;
		// //mapOntoPolySpace( whichFace, x, y, z, &q );
		mapOntoPolySpace( i, x, y, z, &q );
		//fprintf(stderr, "V2 -> x = %f  y = %f \n", q.x, q.y);
		
		newx = q.x*0.866025 - q.y*0.5;
		newy = q.x*0.5 + q.y*0.866025;
		
		mapFromPolySpace( i, newx, newy, &p);
		
		
		glColor3fv( colorBuff );	
		
		glBegin( GL_LINES );
		glVertex3f( vert[faces[i].v[0]].pos.x,
				   vert[faces[i].v[0]].pos.y,
				   vert[faces[i].v[0]].pos.z );
		glVertex3f( p.x, p.y, p.z );
		glEnd();
		glPopMatrix();
	}
	glColor4fv( currentColor );
}

void drawNormals( void )
{
	int i;
	Point3D normal;
	
	for( i = 0; i < NumberFaces; i++)
	{
		glPushMatrix();
    	normal.x = faces[i].planeCoef.a;
		normal.y = faces[i].planeCoef.b;
		normal.z = faces[i].planeCoef.c;
		
		glTranslatef( faces[i].center3D.x, faces[i].center3D.y, faces[i].center3D.z);
		//glTranslatef( vert[faces[i].v[0]].pos.x,vert[faces[i].v[0]].pos.y,vert[faces[i].v[0]].pos.z);
		
		glScalef(0.3, 0.3, 0.3);
		
		glColor3f( 1.0, 0.0, 0.0 );
		
		glBegin( GL_LINES );
		glVertex3f( 0.0, 0.0, 0.0 );
		glVertex3f( normal.x, normal.y, normal.z);
		glEnd();
		
		glColor3f( 0.0, 1.0, 0.0 );
		
		glBegin( GL_LINES );
		glVertex3f( 0.0, 0.0, 0.0 );
		glVertex3f( faces[i].EixoReferencia.x, faces[i].EixoReferencia.y, faces[i].EixoReferencia.z );
		glEnd();
		
		glColor3f( 0.0, 0.0, 1.0 );
		
		glBegin( GL_LINES );
		glVertex3f( 0.0, 0.0, 0.0 );
		glVertex3f( faces[i].MatrixMapTo3D.element[0][1], faces[i].MatrixMapTo3D.element[1][1], faces[i].MatrixMapTo3D.element[2][1] );
		glEnd();
		
		glPopMatrix();
	}
}
/*
 *----------------------------------------------------------
 *----------------------------------------------------------
 */
void drawVectors( void )
{
	int i;
	Point3D normal;
	GLfloat zero[XYZ] = {0.0, 0.0, 0.0};
	
	glColor3fv( colorGreen );
	for( i = 0; i < NumberFaces; i++)
	{
		glPushMatrix();
    	normal.x = faces[i].planeCoef.a;
		normal.y = faces[i].planeCoef.b;
		normal.z = faces[i].planeCoef.c;
		
		/*glTranslatef( faces[i].center.x, faces[i].center.y, faces[i].center.z);*/
		glTranslatef( vert[faces[i].v[0]].pos.x,vert[faces[i].v[0]].pos.y,vert[faces[i].v[0]].pos.z);
		
		drawAxes(faces[i].v1, faces[i].v2, normal, zero);
		glPopMatrix();
	}
}
/*
 *----------------------------------------------------------
 *----------------------------------------------------------
 */
void drawAxes( Point3D v1, Point3D v2, Point3D v3, GLfloat zero[XYZ])
{
	float currentColor[XYZW];
	
	glGetFloatv( GL_CURRENT_COLOR, currentColor );
	//glColor3fv( backColor );
	glColor3fv( colorMagenta );
	
	glPushMatrix();
	glScalef(0.5, 0.5, 0.5);
	glBegin( GL_LINES );
	glVertex3fv( zero );
	glVertex3f( v1.x, v1.y, v1.z);
	glEnd();
	glBegin( GL_LINES );
	glVertex3fv( zero );
	glVertex3f( v2.x, v2.y, v2.z);
	glEnd();
	glBegin( GL_LINES );
	glVertex3fv( zero );
	glVertex3f( v3.x, v3.y, v3.z);
	glEnd();
	glPopMatrix();
	
	glColor4fv( currentColor );
}
/*
 *----------------------------------------------------------
 *	Auxiliary routine for drawObject above
 *	'ix' is the index into the vertex array
 *----------------------------------------------------------
 */
double teste(int ix)
{
	
	int *n;
	
	/* (ix + 4) = vertex line in obj // commented by Cadu
	 */
	
	//n = nnoprimVertex;
	//nnoprimVertex = nnoprimVertex + ix;
	
	if ( nnoprimVertex ) {
		n = nnoprimVertex;
		nnoprimVertex = nnoprimVertex + ix;
		
		//if ( (vertDisplay[ix].x == vertDisplay[*nnoprimVertex].x) && (vertDisplay[ix].y == vertDisplay[*nnoprimVertex].y) && (vertDisplay[ix].z == vertDisplay[*nnoprimVertex].z) ) {
		//glColor3d( 1.0, 0.0, 0.0 );
		
		//}
		
	}
	
	glVertex3d(vertDisplay[ix].x, vertDisplay[ix].y, vertDisplay[ix].z);
	
	if( nnoprimVertex ) 
		nnoprimVertex = n;
	
	//glColor3fv( backColor );
	
	//printf("Vertice(x,y,z): %f, %f, %f\n", vertDisplay[0].x, vertDisplay[0].y, vertDisplay[0].z);
	
	/*if ( nnoprimVertex ) {
	 printf("Deve ser o mesmo que o de cima(x,y,z): %f, %f, %f\n", vertDisplay[*nnoprimVertex].x, vertDisplay[*nnoprimVertex].y, vertDisplay[*nnoprimVertex].z );
	 
	 }*/
	
	//while( nnoprimVertex ) {
	//printf("Vertex without primitive: %d\n",*nnoprimVertex);
	//nnoprimVertex++;
	//}
	
	//verifyVertexPrimitive(); // line added by Cadu in 29/10
	//printf("Face( ix ): %d\n",ix);
	//printf("Vertices(x, y, z): %f  %f  %f\n",vertDisplay[ix].x, vertDisplay[ix].y, vertDisplay[ix].z);
	//glColor3d( 0.0, 0.0, 0.0 ); // line added by Cadu in 29/10
	
	
	return( vertDisplay[ix].y );
}
/*
 *----------------------------------------------------------
 *----------------------------------------------------------
 */
void setNormal(int ix)
{
    glNormal3d(vertn[ix].x, vertn[ix].y, vertn[ix].z);
}
/*
 *----------------------------------------------------------
 *----------------------------------------------------------
 */
void setTexture(int ix, float scale)
{
    glTexCoord2f(vertt[ix].x*scale, vertt[ix].y*scale);
}

/*
 *----------------------------------------------------------
 * set color of vertices
 *----------------------------------------------------------
 */
void setColor(float c[], int vertIndex, int faceIndex)
{
    if ( colorPrimFlag && !polygonPicking ){
		/* paint polygons according to the primitives they belong to */
		if ( vert[vertIndex].prim1 != 0 && vert[vertIndex].prim2 != 0)
			assignValueVector(c,  colorBlue );
		
		else if ( vert[vertIndex].prim1 != 0)
			assignValueVector(c,  colPrim[vert[vertIndex].prim1] );
		
		else if ( vert[vertIndex].prim2 != 0)
			assignValueVector(c,  colPrim[vert[vertIndex].prim2] );
		
		else{
			/* if (!faces[faceIndex].hit) assignValueVector(c, colorBrown );*/
			/* if (!faces[faceIndex].hit) assignValueVector(c, colorRed );*/
			/* if (!faces[faceIndex].hit) assignValueVector(c, colorDarkYellow );*/
			/* if (!faces[faceIndex].hit) assignValueVector(c, colorMediumBrown );*/
			if (!faces[faceIndex].hit){
				if ( !bwOutput ) assignValueVector(c, colorWhite);
				else assignValueVector(c, backColor );
			}
			/* if (!faces[faceIndex].hit) assignValueVector(c, backColor ); */
			else{ printf("face: %d\n", faceIndex); assignValueVector( c, colorGrey);}
		}
    }
    else{
		if (!faces[faceIndex].hit){
			if ( !bwOutput ) assignValueVector(c, colorBrown);
			else assignValueVector(c, backColor );
		}
		else{
			if ( !bwOutput ) assignValueVector( c, colorGrey);
			else assignValueVector(c, backColor );
		}
		
    }
    c[W] = 1.0; /* if the color is being used as material */
}

/*
 *----------------------------------------------------------
 *	drawEdgesObject
 *	This function is an auxiliary function for drawObject
 *	It draws the edges conecting polygons by drawing
 *	individual edges connecting vertices
 *----------------------------------------------------------
 */
void drawEdgesObject1(void)
{
	int i, j;
	
	glPushMatrix();
	
	/* Draw edges connecting faces */
	glLineWidth(2.0);
	glColor3fv( colorWhite );
	for (i=0; i < NumberFaces; i++){
		glBegin(GL_LINES);
		for(j=0; j < faces[i].nverts-1; j++){
			glPushName(j);
			teste(faces[i].v[j]);
			teste(faces[i].v[j+1]);
		}
		glEnd();
		glPopName();
		/* draw the last pair */
		glPushName(j+1);
		glBegin(GL_LINES);
		teste(faces[i].v[0]);
		teste(faces[i].v[faces[i].nverts-1]);
		glEnd();
		glPopName();
	}
	glPopMatrix();
}

/*
 *----------------------------------------------------------
 *	drawEdgesObject
 *	This function is an auxiliary function for drawObject
 *	It draws the edges conecting polygons
 *----------------------------------------------------------
 */
void drawEdgesObject(void)
{
    int i, j;
    float currentColor[XYZW];
	
    glGetFloatv( GL_CURRENT_COLOR, currentColor );
    glPushMatrix();
    /* Draw edges connecting faces */
    glLineWidth(2.0);
    /* To change the color for printing */
    /*glColor3fv( colorWhite );*/
    glColor3fv( backColor );
    for (i=0; i < NumberFaces; i++){
		glPushName(i);
		glBegin(GL_LINE_LOOP);
		for(j=0; j < faces[i].nverts; j++)
			teste(faces[i].v[j]);
		glEnd();
		glPopName();
    }
    glPopMatrix();
    glColor4fv( currentColor );
}

void drawVerticesObject(void)
{
    int i, j;
    float currentColor[XYZW];
	
    glGetFloatv( GL_CURRENT_COLOR, currentColor );
    glPushMatrix();
    /* Draw edges connecting faces */
    glLineWidth(2.0);
    /* To change the color for printing */
    /*glColor3fv( colorWhite );*/
    glColor3fv( colorBlue );
    for (i=0; i < NumberFaces; i++){
		glPushName(i);
		glBegin(GL_POINTS);
		for(j=0; j < faces[i].nverts; j++)
		{
				//if(faces[i].v[j] == 272)
				//teste(faces[i].v[j]);
		}
		glEnd();
		glPopName();
    }
    glPopMatrix();
    glColor4fv( currentColor );
}
/*
 *----------------------------------------------------------
 *	Draws distribution of random points as defined in the
 *	cells array
 *----------------------------------------------------------
 */
void drawPoints( void )
{
	int i;
	CELL *c;
	float currentColor[XYZW];
	float newColor[XYZ];
	
	glPushMatrix();
	glGetFloatv( GL_CURRENT_COLOR, currentColor );
	glPointSize(5.0);
	
	for(i=0; i < NumberFaces; i++)
	{
		c = faces[i].head->next;
		while(c != faces[i].tail)
		{
			newColor[X] = Ortcell[c->ctype].color[X]*0.5;
			newColor[Y] = Ortcell[c->ctype].color[Y]*0.5;
			newColor[Z] = Ortcell[c->ctype].color[Z]*0.5;
			glColor3fv( newColor );
			
			//glColor3fv( colorBlue );
			glBegin(GL_POINTS);
			glVertex3d( c->x, c->y, c->z );
			//glVertex3d( -0.18013, -0.553858, 0.190039 );
			glEnd();
			/* draw the points as small circles */
			/*glPushMatrix();
			 glTranslatef(c->x, c->y , c->z);
			 glCallList(MY_CIRCLE_LIST);
			 glPopMatrix();*/
			c = c->next;
		}
	}
	
	glColor3fv( colorBlue );
	glBegin(GL_POINTS);
	glVertex3d( 0.061060,3.252496, 0.645913 );
	//glVertex3d( 0.218113, -0.568722, 0.193168 );
	glEnd();
	
	glColor4fv( currentColor );
	glPopMatrix();
}

/*
 *----------------------------------------------------------
 *       drawArc
 *       Draws a circular arc of radius "radius"
 *       which starts at "theta" degrees from the
 *       x-axis and spans over "alpha" degrees
 *       from "theta". The arc is drawn with "nseg"
 *       number of segments
 *----------------------------------------------------------
 */
void drawArc(float radius, float theta, float alpha, int nseg)
{
    int i;
    GLfloat p[XYZ];
    GLfloat n[XYZ];
    GLfloat step;
	
    step = alpha / (float)nseg;
    p[X] = radius * cos(deg2rad(theta));
    p[Z] = radius * sin(deg2rad(theta));
    p[Y] = n[Y] = 0.0;
	
    glBegin(GL_LINES);
    for(i=1; i<=nseg; i++){
		n[X] = radius * cos(deg2rad(theta + step*i));
		n[Z] = radius * sin(deg2rad(theta + step*i));
		glVertex3fv( p );
		glVertex3fv( n );
		p[X] = n[X];
		p[Z] = n[Z];
    }
    glEnd();
}

/*
 *----------------------------------------------------------
 *	builds a display list for displaying the axes
 *----------------------------------------------------------
 */
void buildAxes(void)
{
    static float axes[4][XYZ] = {
		{0.0, 0.0, 0.0},
		{1.0, 0.0, 0.0},
		{0.0, 1.0, 0.0},
		{0.0, 0.0, 1.0}
    };
	
    glNewList(MY_AXES_LIST, GL_COMPILE);
	
    /*glLineStipple(4, 0xAAAA);*/
    glLineStipple(1, 0x00FF);
    glEnable(GL_LINE_STIPPLE);
	
    glPushMatrix();
    glScalef(2.0, 2.0, 2.0);
    glTranslatef(0.0, 0.0, 0.0);
	
    glRasterPos3f(axes[1][0], axes[1][1], axes[1][2]);
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'X');
    glBegin( GL_LINES );
	glVertex3fv( axes[0] );
	glVertex3fv( axes[1] );
    glEnd();
	
    glRasterPos3f(axes[2][0], axes[2][1], axes[2][2]);
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'Y');
    glBegin( GL_LINES );
	glVertex3fv( axes[0] );
	glVertex3fv( axes[2] );
    glEnd();
	
    glRasterPos3f(axes[3][0], axes[3][1], axes[3][2]);
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'Z');
    glBegin( GL_LINES );
	glVertex3fv( axes[0] );
	glVertex3fv( axes[3] );
    glEnd();
	
    glPopMatrix();
	
    glDisable(GL_LINE_STIPPLE);
    glEndList();
	
}
/*
 *----------------------------------------------------------
 *	builds a display list with coordinates for a circle
 *	with a 100 segments
 *	To draw a circle invoke 'glCallList(MY_CIRCLE_LIST)'
 *	Example taken from the OpenGL book
 *----------------------------------------------------------
 */
void buildCircle(void)
{
    GLint i;
    GLfloat cosine, sine;
	
    glNewList(MY_CIRCLE_LIST, GL_COMPILE);
    glBegin(GL_POLYGON);
    for(i=0;i<100;i++){
		cosine=cos(i*2*PI/100.0)*0.05;
		sine=sin(i*2*PI/100.0)*0.05;
		glVertex2f(cosine,sine);
    }
    glEnd();
    glEndList();
}

/*
 *----------------------------------------------------------
 *	INPUT: y is the coordinate of the shadow plane
 *----------------------------------------------------------
 */
void drawShadows( double y )
{
	float c[XYZW];
	int i,j ;
	
	glPushMatrix();
	
	c[X] = 0.0; c[Y] = 0.0; c[Z] = 0.0; c[W] = 1.0;
	glColor3fv( c );
	
	for (i=0; i < NumberFaces; i++){
		glBegin(GL_POLYGON);
		/* draw vertices */
		for(j=0; j < faces[i].nverts; j++){
			if (lightFlag) glDisable( GL_LIGHTING );
			/* finally draws the vertex */
			setVertexDisplayShadow(faces[i].v[j], y);
		} /* go to next vertex */
		glEnd();
	} /* go to next face */
	
	if (lightFlag) glEnable( GL_LIGHTING );
	
	glPopMatrix();
	
}


/*
 *----------------------------------------------------------
 *       draws a "scene"
 *----------------------------------------------------------
 */
void drawScene( void )
{
	
	float c[XYZW], c1[XYZW];
	float  lgt_pos[] = { 0.0, 0.0, -30.0, 0.0 };
	float  lgt_amb[] = { 0.5, 0.5, 0.5, 1.0 };
	float  lgt_dif[] = { 0.9, 0.9, 0.9, 1.0 };
	float zt=-40.0;
	
	zt *= Prim[WORLD].cosine.z;
	
	glPushMatrix();
	glLoadIdentity();
	glTranslatef( 0.0, 0.0, zt);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	if ( lightFlag ){
		glLightfv( GL_LIGHT1, GL_AMBIENT, lgt_amb );
		glLightfv( GL_LIGHT1, GL_DIFFUSE, lgt_dif );
		glLightfv( GL_LIGHT1, GL_POSITION, lgt_pos );
		glEnable( GL_LIGHT1 );
	}
	
	/* Draw the background plane with a blue 'fading' color */
	glBegin( GL_POLYGON );
    /* Dark Blue */
    c[X] = 0.0; c[Y] = 0.0; c[Z] = 0.2; c[W] = 1.0;
    /* Light blue */
    c1[X] = 0.0; c1[Y] = 0.0; c1[Z] = 0.95; c1[W] = 1.0;
	
    if (!lightFlag) glColor3fv( c );
    else glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, c);
	
    /* set the normal to the plane */
    glNormal3d(0, 0, -1);
    glVertex3f( -200.0, -200.0, -160 );
    glNormal3d(0, 0, -1);
    glVertex3f( 200.0, -200.0, -160 );
	
    if (!lightFlag) glColor3fv( c1 );
    else glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, c1);
    glNormal3d(0, 0, -1);
    glVertex3f(  200.0,  200.0, -160 );
    glNormal3d(0, 0, -1);
    glVertex3f( -200.0,  200.0, -160 );
	glEnd();
	
	/* Draws a floor in gray to black */
	glBegin( GL_POLYGON );
    c[X] = 0.3; c[Y] = 0.3; c[Z] = 0.3; c[W] = 1.0;
    c1[X] = 0.0; c1[Y] = 0.0; c1[Z] = 0.0; c1[W] = 1.0;
	
    if (!lightFlag) glColor3fv( c );
    else glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, c);
	
    glNormal3d(0, 0, -1);
    glVertex3f( -200.0, -30, -200.0 );
    glNormal3d(0, 0, -1);
    glVertex3f( 200.0, -30, -200.0 );
	
    if (!lightFlag) glColor3fv( c1 );
    else glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, c1);
	
    glNormal3d(0, 0, -1);
    glVertex3f( 200.0,  -30, 200.0 );
    glNormal3d(0, 0, -1);
    glVertex3f( -200.0,  -30, 200.0 );
	
	glEnd();
	glPopMatrix();
	
	if ( lightFlag ) glDisable( GL_LIGHT1 );
	
}
/*
 *----------------------------------------------------------
 *       draws 2 lines separating the 4 individual screens
 *----------------------------------------------------------
 */
void buildSeparatingLines( void )
{
	
	glNewList(MY_LINES_LIST, GL_COMPILE);
	glColor3f(0.0, 0.0, 0.0);
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0,1.0,0.0,1.0);
	
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
	glLoadIdentity();
	glBegin(GL_LINES);
	glVertex2f(0.0, 0.5);
	glVertex2f(1.0, 0.5);
	glVertex2f(0.5, 0.0);
	glVertex2f(0.5, 1.0);
	glEnd();
    glPopMatrix();
    glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glEndList();
}
/*
 *----------------------------------------------------------
 *       draws a grid
 *----------------------------------------------------------
 */
void buildGrid( void )
{
	
	
	int i;
	float currentLineWidth;
	float currentColor[XYZW];
	/* cc */
	glNewList(MY_GRID_LIST, GL_COMPILE);
	/* cc */
	glGetFloatv( GL_CURRENT_COLOR, currentColor );
	glGetFloatv(GL_LINE_WIDTH, &currentLineWidth);
	glLineWidth(0.5);
	
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0,10.0,0.0,10.0,-1.0,1.0);
	
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
	glLoadIdentity();
	glColor3f(0.6, 0.6, 0.6);
	glBegin(GL_LINES);
	for(i=0; i<10; i++){
		glVertex3f(0.0, 0.0+i, 0.0);
		glVertex3f(10.0, 0.0+i, 0.0);
		glVertex3f(0.0+i, 0.0, 0.0);
		glVertex3f(0.0+i, 10.0, 0.0);
	}
	glEnd();
	glPopMatrix();
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glLineWidth(currentLineWidth);
    glColor4fv( currentColor );
    glEndList();
}

/*
 *----------------------------------------------------------
 *	drawMode
 *	This function draws a string representing the
 *	transformation mode at the bottom of the screen
 *	The modes are: Translation,  Rotation,  and Scaling
 *	Additionally,  the user can be in 'picking' mode
 *	and in this case the previous 3 modes are disabled
 *----------------------------------------------------------
 */
void drawMode(MMODE mmode, VMODE vmode, int pick, int t)
{
	
	int   i, len, len1, n;				
    char  modelMode[ 64 ], pickingMode[ 64 ], v[ 64 ], str1[ 64 ];
	char  textLine[64];
	
    glColor3fv( backColor );		/* Draw text in black */
	
    if (activePrim != WORLD){
    	switch( mmode ) {
      		case ROT:
				strcpy( modelMode, "Rotate" );
				break;
     		case SCALE:
				strcpy( modelMode, "Scale" );
				break;
      		case TRANS:
				strcpy( modelMode, "Translation" );
				break;
      		default:
				modelMode[ 0 ] = '\0';
				break;
     	}
		
    } else modelMode[ 0 ] = '\0';
	
    switch( vmode )
	{
    	case TOP:
			strcpy( str1, "Top");
			break;
      	case FRONT:
			strcpy( str1, "Front");
			break;
      	case SIDE:
			strcpy( str1, "Side");
			break;
      	default:
			str1[ 0 ] = '\0';
			break;
    }
	
    /* added by Thompson in 22/05/2002 */
    pickingMode[0] = '\0';
    if( pick ) /*strcat(modelMode, " Picking")*/{
		if( polygonPicking ) 
			strcat(pickingMode, "   Polygon Picking");
        if( cellPicking )     
			strcat(pickingMode, "    Cell Picking");
    }
	
	/* added by Thompson in 05/03/2004 */
	textLine[0]  = '\0';
	if( createVect )
	{
		strcpy(textLine, "Drawn Vector");
    }
    /* I need a better way here to inform the user
	 that the growth information is not yet
	 available */
    if ( morphedFlag ){
		n = growthData[t][0];
		sprintf(v, "  %d", n);
		strcat(modelMode, v);
    }
    
    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0,1.0,0.0,1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    /* draws model mode: rotation, scaling or translation */
    draw2DText( modelMode, 0.87, 0.01, MY_FONT); /* 0.87, 0.01*/
	
    /* added by Thompson in 22/05/2002 */
    /* draws picking mode: cells or polygons */ 
    draw2DText( pickingMode, 0.40, 0.01, MY_FONT); 
	
	/* added by Thompson in 05/03/2004 */
	/* draws orientation line */ 
	draw2DText( textLine, 0.40, 0.01, MY_FONT); 
	
    /* draws the viewing mode: top, front, side */
    draw2DText( str1, 0.01, 0.95, MY_FONT);
    /* draws small 2D axes at the bottom left corner
	 of the windows (except in the perspective
	 window */
    if (vmode == VNONE || vmode == PERSPECTIVE);
    else
		drawSmall2DAxes(vmode);
    
    /* added by Thompson in 20/05/2002 */
    if( zoomflag==FALSE && rotateflag==TRUE ) 
		draw2DText( "Y", 0.95, 0.95, MY_FONT);
    if( zoomflag==TRUE && rotateflag==FALSE )
		draw2DText( "X", 0.95, 0.95, MY_FONT);
    
	
    glPopMatrix();
    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

/*
 *----------------------------------------------------------
 *	draws the small orientation 2D axes
 *	at the bottom left corner of the window
 *----------------------------------------------------------
 */
void drawSmall2DAxes( VMODE vmode )
{
    float currentLineWidth;
	
    glGetFloatv(GL_LINE_WIDTH, &currentLineWidth);
    glLineWidth(0.5);
    glBegin(GL_LINES);
	/* draw two lines crossing */
	glVertex2f(0.01, 0.08);
	glVertex2f(0.01, 0.03);
	glVertex2f(0.01, 0.03);
	glVertex2f(0.06, 0.03);
    glEnd();
    glBegin(GL_LINE_LOOP);
	/* draw first arrow */
	glVertex2f(0.06, 0.025);
	glVertex2f(0.06, 0.035);
	glVertex2f(0.07, 0.03);
    glEnd();
    glBegin(GL_LINE_LOOP);
	/* draw second arrow */
	glVertex2f(0.005, 0.08);
	glVertex2f(0.01, 0.09);
	glVertex2f(0.015, 0.08);
    glEnd();
    glLineWidth(currentLineWidth);
	
    /* writes the labels for the axes */
    switch( vmode ){
		case TOP:
			draw2DText( "Z", 0.07, 0.03, GLUT_BITMAP_HELVETICA_12);
			draw2DText( "X", 0.01, 0.09, GLUT_BITMAP_HELVETICA_12);
			break;
		case FRONT:
			draw2DText( "X", 0.07, 0.03, GLUT_BITMAP_HELVETICA_12);
			draw2DText( "Y", 0.01, 0.09, GLUT_BITMAP_HELVETICA_12);
			break;
		case SIDE:
			draw2DText( "Z", 0.07, 0.03, GLUT_BITMAP_HELVETICA_12);
			draw2DText( "Y", 0.01, 0.09, GLUT_BITMAP_HELVETICA_12);
			break;
		default:
			break;
    }
}
/*
 *----------------------------------------------------------
 *	draws text specified by 'str' in the position
 *	specified by (xpos, ypos) in the given font
 *----------------------------------------------------------
 */
void draw2DText( char *str, float xpos, float ypos, void *font)
{
    int i, len;
	
    glRasterPos2f(xpos, ypos);
    len = strlen( str );
    for( i = 0; i < len; i++ ) {
		glutBitmapCharacter( font, str[ i ] );
    }
}

/*
 *----------------------------------------------------------
 *       draws a circle drawing individual arcs
 *----------------------------------------------------------
 */
void drawCircle(int npies, float radius)
{
    GLfloat initAngle=0.0;
    GLfloat anglePie;
    GLint i;
    GLfloat center[XYZ];
    GLfloat p[XYZ];
	
    anglePie = 360.0 / (float) npies;
	
    for(i=0; i < npies; i++){
		drawArc(radius, initAngle, anglePie, 10);
		initAngle += anglePie;
    }
}
/*
 *----------------------------------------------------------
 *      drawWireCylinder1
 *	Draws a cylinder aligned with the Y-axis
 *	and centered at (0, 0, 0).
 *	Y goes from -h/2 at the bottom to h/2 at the top
 *
 *	INPUT:
 *	- radius
 *	- height
 *	- number of segments
 *----------------------------------------------------------
 */
void drawWireCylinder1(float r, float h, int nseg)
{
    GLint i;
    GLfloat p[XYZ];
    GLfloat n[XYZ];
    GLfloat angle=0.0;
    GLfloat anglePie;
	
    anglePie = 360.0 / (float) nseg;
    
    glPushMatrix();
    p[X] = r * cos(deg2rad(angle));
    p[Y] = -(h/2.0);
    /* p[Y] = 0.0; */
    p[Z] = r * sin(deg2rad(angle));
    
    /* draws straight segments connecting top and bottom parts */
    for(i=0; i < nseg; i++){
		n[X] = p[X];
		n[Y] = p[Y] + h;
		n[Z] = p[Z];
		glBegin(GL_LINES);
	    glVertex3fv( p );
	    glVertex3fv( n );
		glEnd();
		angle += anglePie;
		p[X] = r * cos(deg2rad(angle));
		p[Y] = -(h/2.0);
		/* p[Y] = 0.0; */
		p[Z] = r * sin(deg2rad(angle));
    }
	
    /* draws bottom circle */
    p[X] = r * cos(deg2rad(angle));
    p[Y] = n[Y] = -(h/2.0);
    /* p[Y] = n[Y] = 0.0; */
    p[Z] = r * sin(deg2rad(angle));
    
    for(i=0; i < nseg; i++){
		n[X] = r * cos(deg2rad(angle+anglePie));
		n[Z] = r * sin(deg2rad(angle+anglePie));
		glBegin(GL_LINES);
	    glVertex3fv( p );
	    glVertex3fv( n );
		glEnd();
		angle += anglePie;
		p[X] = n[X];
		p[Z] = n[Z];
    }
	
    /* draws top circle */
    p[X] = r * cos(deg2rad(angle));
    p[Y] = n[Y] = h/2.0;
    /* p[Y] = n[Y] = h; */
    p[Z] = r * sin(deg2rad(angle));
    
    for(i=0; i < nseg; i++){
		n[X] = r * cos(deg2rad(angle+anglePie));
		n[Z] = r * sin(deg2rad(angle+anglePie));
		glBegin(GL_LINES);
	    glVertex3fv( p );
	    glVertex3fv( n );
		glEnd();
		angle += anglePie;
		p[X] = n[X];
		p[Z] = n[Z];
    }
    glPopMatrix();
}	
/*
 *----------------------------------------------------------
 *	drawWireCylinder
 *	Draws a cylinder aligned with the X-axis
 *	X goes from 0 at the bottom to h at the top
 *
 *	INPUT:
 *	- radius
 *	- height
 *	- number of segments
 *----------------------------------------------------------
 */
void drawWireCylinder(float r, float h, int nseg)
{
    GLint i;
    GLfloat p[XYZ];
    GLfloat n[XYZ];
    GLfloat angle=0.0;
    GLfloat anglePie;
    GLfloat currentLineWidth;
	
    anglePie = 360.0 / (float) nseg;
	
    glGetFloatv(GL_LINE_WIDTH, &currentLineWidth);
    glLineWidth(0.5);
	
    glPushMatrix();
    p[Z] = r * cos(deg2rad(angle));
    p[X] = 0.0;
    p[Y] = r * sin(deg2rad(angle));
    
    /* draws straight segments connecting top and bottom parts */
    for(i=0; i < nseg; i++){
		n[Y] = p[Y];
		n[X] = p[X] + h;
		n[Z] = p[Z];
		glBegin(GL_LINES);
	    glVertex3fv( p );
	    glVertex3fv( n );
		glEnd();
		angle += anglePie;
		p[Z] = r * cos(deg2rad(angle));
		p[X] = 0.0;
		p[Y] = r * sin(deg2rad(angle));
    }
	
    /* draws bottom circle */
    p[Z] = r * cos(deg2rad(angle));
    p[X] = n[X] = 0.0;
    p[Y] = r * sin(deg2rad(angle));
    
    for(i=0; i < nseg; i++){
		n[Z] = r * cos(deg2rad(angle+anglePie));
		n[Y] = r * sin(deg2rad(angle+anglePie));
		glBegin(GL_LINES);
	    glVertex3fv( p );
	    glVertex3fv( n );
		glEnd();
		angle += anglePie;
		p[Y] = n[Y];
		p[Z] = n[Z];
    }
	
    /* draws top circle */
    p[Z] = r * cos(deg2rad(angle));
    p[X] = n[X] = h;
    p[Y] = r * sin(deg2rad(angle));
    
    for(i=0; i < nseg; i++){
		n[Z] = r * cos(deg2rad(angle+anglePie));
		n[Y] = r * sin(deg2rad(angle+anglePie));
		glBegin(GL_LINES);
	    glVertex3fv( p );
	    glVertex3fv( n );
		glEnd();
		angle += anglePie;
		p[Y] = n[Y];
		p[Z] = n[Z];
    }
    glPopMatrix();
    /* return line width to its original value */
    glLineWidth(currentLineWidth);
}
/*
 *----------------------------------------------------------
 *	drawPrimitives
 *	This function draws the control primitives
 *	The growth control primitives are defined
 *	by the growth data available
 *----------------------------------------------------------
 */
void drawPrimitives(int nPrim, int active, int t)
{
    GLint i;
	
    for(i=1; i < nPrim; i++){ /* primitive '0' is the whole 'world' */
		/* check to see if primitive is a landmark */
		if ( Prim[i].type == LANDMARK){
			if (showLandMarks) drawLandMarks(i, active, t, backColor);
		}
		if ( Prim[i].type == CYLINDER){
			if (showPrimitives) drawOnePrimitive(i, active, t, backColor);
		}
    }
}
/*
 *----------------------------------------------------------
 *	drawLandMarks
 *	INPUT:	'n' is the number of landMarks
 *		'active' is the current active landmark
 *		't' is the current time
 *----------------------------------------------------------
 */
void drawLandMarks(int which, int active, int t, float *color)
{
	GLfloat currentLineWidth;
	
	glGetFloatv(GL_LINE_WIDTH, &currentLineWidth);
	glLineWidth(3.0);
	
	/* changes color when drawing active primitive */
	if (which == active) glColor3fv( colorBlue);
	else glColor3fv( color );
	
	glPushMatrix();
	
	/* apply rotation, translation and scaling to the primitive */
	myPrimTransf(which);
	/* draws two lines defining the landmark */
	drawLine( Prim[which].q1, Prim[which].q2);
	
	glLineWidth(currentLineWidth);
	
	glPopMatrix();
}
/*
 *-----------------------------------------------------
 *
 *-----------------------------------------------------
 */
void drawOnePrimitive(int whichPrim, int active, int t, float *color)
{
	
	/* for test of rotation */
	Matrix4 testRotation;
	float myMatrix[16];
	
	/* changes color when drawing active primitive */
	if (whichPrim == active) glColor3fv( colorBlue );
	else glColor3fv( color );
	
	glPushMatrix();
	
	if ( morphedFlag ){
		printf("morphedFlag\n");
		buildTestRot( &testRotation, whichPrim, t); 
		toOpenGL( testRotation, &myMatrix[0]);
		glMultMatrixf(myMatrix);
	}
	
	/* translates the primitive to its new center
     because of growth */
	glTranslatef( Prim[whichPrim].center.x,
				 Prim[whichPrim].center.y,
				 Prim[whichPrim].center.z);
	
	/* apply rotation, translation and scaling to the primitive */
	myPrimTransf(whichPrim);
	
	/* draws cylinder with radius, height, and number of segments */
	
	drawWireCylinder( Prim[whichPrim].r, Prim[whichPrim].h, 15);
	/* draw axes */
	if (showAxes){
		/* if I want to scale the axes accordingly
		 here is where I have to apply the scaling call */
		/* glScalef(sh?,sr?,sr?); */
		glCallList(MY_AXES_LIST);
	}
	
	glPopMatrix();
}



/*
 *----------------------------------------------------------
 *	draws a line 
 *----------------------------------------------------------
 */
void drawLine(Point3D p1, Point3D p2)
{
	glBegin(GL_LINES);
  	glVertex3d(p1.x, p1.y, p1.z);
	glVertex3d(p2.x, p2.y, p2.z);
	glEnd();
}

/*
 *---------------------------------------------------------------
 *	
 *
 *---------------------------------------------------------------
 */
void defLandMarks( void )
{
	
	int i, j;
	int nOfActiveVertices=0;
	int *activeVert;
	int index, index1;
	
	
	/* if (activePrim != WORLD){ */
	
    for(i=0; i < NumberVertices; i++)
		if (vert[i].hit == TRUE) nOfActiveVertices++;
	
    activeVert = (int *) malloc(sizeof(int)*nOfActiveVertices);
	
    j=0;
    for(i=0; i < NumberVertices; i++){
		if (vert[i].hit == TRUE){
			activeVert[j] = i;
			j++;
		}
    }
    
    for(j=0; j < nOfActiveVertices; j++){
		if (index == j) drawCross(activeVert[j], backColor);
		else if (index1 == j) drawCross(activeVert[j], backColor);
		else drawCross(activeVert[j], colorYellow);
    }
    glColor3f(1.0,0.0,1.0);
    glBegin(GL_LINES);
    glVertex3d(vertDisplay[activeVert[index]].x,
			   vertDisplay[activeVert[index]].y,
			   vertDisplay[activeVert[index]].z);
    glVertex3d(vertDisplay[activeVert[index1]].x,
			   vertDisplay[activeVert[index1]].y,
			   vertDisplay[activeVert[index1]].z);
    glEnd();
    /* if (lMark[activePrim].firstLine == FALSE){
	 lMark[activePrim].q1.x = vertDisplay[activeVert[index]].x;
	 lMark[activePrim].q1.y = vertDisplay[activeVert[index]].y;
	 lMark[activePrim].q1.z = vertDisplay[activeVert[index]].z;
	 lMark[activePrim].q2.x = vertDisplay[activeVert[index1]].x;
	 lMark[activePrim].q2.y = vertDisplay[activeVert[index1]].y;
	 lMark[activePrim].q2.z = vertDisplay[activeVert[index1]].z;
	 lMark[activePrim].firstLine = TRUE;
	 }
	 }*/
}

/*
 *---------------------------------------------------------------
 *	Draw a small 3D cross at the vertex position
 *	The input parameter indicates the vertex index
 *	in the vertices data structure
 *---------------------------------------------------------------
 */
void drawCross( int i, float *color )
{ 
	
	glColor3fv( color );
	
	glBegin(GL_LINES);
	glVertex3d(vertDisplay[i].x-0.5, vertDisplay[i].y, vertDisplay[i].z);
	glVertex3d(vertDisplay[i].x+0.5, vertDisplay[i].y, vertDisplay[i].z);
	glVertex3d(vertDisplay[i].x, vertDisplay[i].y-0.5, vertDisplay[i].z);
	glVertex3d(vertDisplay[i].x, vertDisplay[i].y+0.5, vertDisplay[i].z);
	glVertex3d(vertDisplay[i].x, vertDisplay[i].y, vertDisplay[i].z-0.5);
	glVertex3d(vertDisplay[i].x, vertDisplay[i].y, vertDisplay[i].z+0.5);
	glEnd();
}


void changeCellColor( CELL *c, float r, float g, float b )
{
	/* since I have changed the way the cell information
     is defined, each cell must have a color defined
     by one of the possible types. I have to review
     this later */
	/*c->type.color[R] = r;
	 c->type.color[G] = g;
	 c->type.color[B] = b;*/
}

/*void drawVectorsField(void)
 {
 int i;
 glPointSize(6.0);
 glLineWidth(4.0);
 
 glColor3f( 0.0, 0.0, 1.0 );
 for( i = 0; i < NumberFaces; i++)
 {
 glBegin( GL_POINTS );
 glVertex3f( faces[i].center3D.x, faces[i].center3D.y, faces[i].center3D.z);
 glEnd();
 
 glBegin( GL_LINES );
 glVertex3f( faces[i].center3D.x, faces[i].center3D.y, faces[i].center3D.z);
 glVertex3f( faces[i].EndOfVector1.x, faces[i].EndOfVector1.y, faces[i].EndOfVector1.z );
 glEnd();
 }
 
 glColor3f( 1.0, 0.0, 0.0 );
 for( i = 0; i < NumberFaces; i++)
 {
 glBegin( GL_POINTS );
 glVertex3f( faces[i].center3D.x, faces[i].center3D.y, faces[i].center3D.z);
 glEnd();
 
 glBegin( GL_LINES );
 glVertex3f( faces[i].center3D.x, faces[i].center3D.y, faces[i].center3D.z);
 glVertex3f( faces[i].EndOfVector0.x, faces[i].EndOfVector0.y, faces[i].EndOfVector0.z );
 glEnd();
 }
 }*/

void drawVerticesVectorsField(void)
{
	int i;
	
	glPointSize(5.0);
	glLineWidth(2.5);
	
	for(i=0; i < NumberVertices; i++)
	{
		glColor3f( 0.0, 0.0, 1.0 );
		glBegin( GL_POINTS );
		glVertex3f( vert[i].pos.x, vert[i].pos.y, vert[i].pos.z );
		glEnd();
		
		glBegin( GL_LINES );
		glVertex3f( vert[i].pos.x, vert[i].pos.y, vert[i].pos.z );
		glVertex3f( vert[i].EndOfVector3D.x, vert[i].EndOfVector3D.y, vert[i].EndOfVector3D.z );
		glEnd();	
	}
	
	/*for (i=0; i < NumberFaces; i++)
	{
		int nVert,j;
		
		glColor3f( 0.0, 0.0, 1.0 );
		
		for (i=0; i < NumberFaces; i++)
		{
			for(j=0;j<faces[i].nverts;j++)
			{
				glBegin( GL_POINTS );
				glVertex3f( vert[faces[i].v[j]].pos.x, vert[faces[i].v[j]].pos.y, vert[faces[i].v[j]].pos.z);
				glEnd();
				
				glBegin( GL_LINES );
				glVertex3f( vert[faces[i].v[j]].pos.x, vert[faces[i].v[j]].pos.y, vert[faces[i].v[j]].pos.z);
				if(j==0)
					glVertex3f(faces[i].projectedVerticeVector1.x, faces[i].projectedVerticeVector1.y, faces[i].projectedVerticeVector1.z  );
				else if(j==1)
					glVertex3f( faces[i].projectedVerticeVector2.x, faces[i].projectedVerticeVector2.y, faces[i].projectedVerticeVector2.z );
				else if(j==2)
					glVertex3f( faces[i].projectedVerticeVector3.x, faces[i].projectedVerticeVector3.y, faces[i].projectedVerticeVector3.z );
				glEnd();
				
			}
			
			
		}
		
	}*/
	
	 

		
}


void drawVectorsField(void)
{

	int i,j;
	Point3D normal;
	
	glPointSize(5.0);
	glLineWidth(2.5);
	
	//glColor3f( 0.9, 0.9, 0.9 );
	
	for (i=0; i < NumberFaces; i++)
	{
		glColor3f( 1.0, 0.0, 0.0 );
		glBegin( GL_POINTS );
		glVertex3f( faces[i].center3D.x, faces[i].center3D.y, faces[i].center3D.z);
		glEnd();
		
		glBegin( GL_LINES );
		glVertex3f( faces[i].center3D.x, faces[i].center3D.y, faces[i].center3D.z);
		glVertex3f( faces[i].EndOfVector3D.x, faces[i].EndOfVector3D.y, faces[i].EndOfVector3D.z );
		glEnd();
		
		
		
		//float k = faces[i].cosAngle;
		//glColor3f( 0.0, k, 0.0 );
		//glPushName(i);
		//glBegin(GL_POLYGON);
		//for(j=0; j < faces[i].nverts; j++)
		//teste(faces[i].v[j]);
		//glEnd();
		//glPopName();
	}
	
	
		
		
		
		
		  					
	
	
	
}

void drawCentroids(void)
{
	int i;
	glPointSize(2.0);
	glColor3f( 1.0, 0.0, 0.0 );
	for( i = 0; i < NumberFaces; i++)
	{
		glBegin( GL_POINTS );
		glVertex3f( faces[i].center3D.x, faces[i].center3D.y, faces[i].center3D.z);
		glEnd();
	}
}
/*
 *----------------------------------------------------------
 *	Draws the orientation vectors
 *----------------------------------------------------------
 */
void drawOrientationVectors()
{
 	VECTORARRAY *v;
	
	v = headV->next;
	
	glPointSize(6.0);
	glLineWidth(5.0);
	while (v != zedV)
	{		
		glBegin(GL_POINTS);
		glColor3f( 0.0, 0.0, 1.0 );
		glVertex3f( v->vBegin.x, v->vBegin.y, v->vBegin.z );
		glColor3f( 1.0, 0.0, 0.0 );	
		glVertex3f( v->vEnd.x, v->vEnd.y, v->vEnd.z );
		glEnd();
		
		glColor3f( 0.0, 1.0, 0.0 );
		
		glBegin(GL_LINES);
		glVertex3f( v->vBegin.x, v->vBegin.y, v->vBegin.z );
		glVertex3f( v->vEnd.x, v->vEnd.y, v->vEnd.z );
		glEnd();
		
		v = v->next;
	}
}

/*
 *
 */	
void drawVect( float coords1[], float coords2[] )
{
	glBegin(GL_LINES);
	glVertex3f( coords1[0], coords1[1], coords1[2] );
	glVertex3f( coords2[0], coords2[1], coords2[2] );
	glEnd();
	
}

/*
 *----------------------------------------------------------
 * Added by Thompson Peter Lied at 18/03/2004
 * Not so good!
 *----------------------------------------------------------
 */
void drawArrow( void )
{
	int i=0;
	float x, y, z, newxp, newyp, newxn, newyn;
	Point2D q;
	Point3D pp, pn;
	VECTORARRAY *v;
	
	glLineWidth(3.0);
	glColor3f( 0.0, 0.0, 1.0 );
	
	v = headV->next;
	
	while (v != zedV)
	{		
		glPushMatrix();
		
		x = v->vBegin.x;
		y = v->vBegin.y;
		z = v->vBegin.z;
		mapOntoPolySpace( i, x, y, z, &q );
		
		/*
		 x = v->vEnd.x;
		 y = v->vEnd.y;
		 z = v->vEnd.z;
		 mapOntoPolySpace( i, x, y, z, &q );
		 */
		
		/* + 15? */
		newxp = q.x*cos(15) - q.y*sin(15);
		/*newxp = q.x*0.866025 - q.y*0.5;*/
		newyp = q.x*sin(15) - q.y*cos(15);
	  	/*newyp = q.x*0.5 + q.y*0.866025;*/
		mapFromPolySpace( i, newxp, newyp, &pp);
		
		/* - 15? */
		newxn = q.x*cos(345) - q.y*sin(345);
		/*newxn = q.x*0.866025 - q.y*(-0.5);*/
		newyn = q.x*sin(345) - q.y*cos(345);
	  	/*newyn = q.x*(-0.5) + q.y*0.866025;*/
		mapFromPolySpace( i, newxn, newyn, &pn);
		
		glBegin(GL_LINES);
		glVertex3f( v->vEnd.x, v->vEnd.y, v->vEnd.z );
		glVertex3f( pp.x, pp.y, pp.z );
		
		glVertex3f( v->vEnd.x, v->vEnd.y, v->vEnd.z );
		glVertex3f( pn.x, pn.y, pn.z );
		glEnd();
		
		glPopMatrix();
		
		i++;
		v = v->next;
	}
}
