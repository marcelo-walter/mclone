/*
 *  texture.c
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/7/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */

/*
 *--------------------------------------------------------
 *	texture.c
 *	Routines related with the computation of texture
 *	coordinates and write output optik format file
 *--------------------------------------------------------
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <magick/api.h>

#include "../common.h"
#include "../simulation/relax.h"
#include "texture.h"

#ifdef GRAPHICS
//#include <GLUT/glut.h>
#include <GL/glut.h>
#endif

/*
 *------------------------------------------------------------
 *	External definitions
 *------------------------------------------------------------
 */
//int round( double value ); // Cadu Moreira 13/05/02
extern float **allocate2DArrayFloat( int r, int c );
//extern int DecodeHeader( int *xRes, int *yRes, int *zRes, FILE *fp );
//extern int DecodeCCLine( int xRes, int zRes, ColChar scanLine[PNG_RESOLUTION], FILE *fp ); 
// "RLE_RESOLUTION" before. Cadu Moreira 30/04/02
extern flag voronoiComputed;

extern char argvv[256]; // from main.c // Cadu Moreira - 06/05/02

/*
 *------------------------------------------------------------
 *	Local definitions
 *------------------------------------------------------------
 */
/* flag to tell if we computed or not texture coordinates */
flag textCoordComputed = FALSE;

TEXTARRAY textArray[MAX_N_TEXTURES];

int totalNumberTextures = 0;

/*
 *------------------------------------------------------------
 *	Reads in the textures
 *------------------------------------------------------------
 */
void readAllTextures( char *fileName )
{
	
	FILE *fpTexture;
	int numberTextRead = 0;
	int i, cylID, numParam, textureMatch;
	char line[255], input[255];
	char textName[255], param[255];
	
	fprintf( stderr, "\n==========================================\n");
	fprintf( stderr, "Pre-processing texture database...\n");
	fprintf( stderr, "==========================================\n");
	
	if ( (fpTexture = fopen( fileName, "r" )) == NULL )
		errorMsg("Could not open texture file to read!");
	
	while (fscanf (fpTexture, "%s", input) != EOF)
	{
		if (!strcmp (input, COMMENT))
		{
			Getline(line, 256, fpTexture);
			/* fprintf( stderr, "%s\n", line ); */ // Commented by Thompson Lied - 23/10/2003
		}
		else if( !strcmp(input, CYLINDERID) )
		{
			fscanf(fpTexture,"%d %d", &cylID, &numParam );
			fprintf( stderr, "CylID = %d numParam = %d\n", cylID, numParam );
			
			/*
			 * Before we assign the texture information, I want to
			 * make sure that the specified primitive is a Cylinder
			 */
			if ( Prim[cylID].type !=  CYLINDER )
				errorMsg( "Texture file trying to assign texture information to a FEATURE!");
			
			for( i = 0; i < numParam; i++ )
			{
				// fscanf(fpTexture,"%s %s", textName, param ); // Comented by Cadu Moreira - 08/05/02
				fscanf(fpTexture,"%s %s", &param, &textName );
				textureMatch = -1;
				
				/*
				 * Before we commit to this texture we check if the
				 * texture was not already defined. The match is done
				 * through name comparison
				 */
				textureMatch = textureAlreadyDefined ( textName, numberTextRead );
				
				if( textureMatch != -1 )
				{
					assignText2Prim( cylID, param, textureMatch );
					//fprintf( stderr, "Param = %s\n", param );
					fprintf( stderr, "Texture already defined = %s\n", textName );
					fprintf( stderr, "Param = %s TextureID = %d \n\n", param, textureMatch );
				}
				else
				{
					/* copy the name of the texture into the texture data structure */
					strcpy( textArray[numberTextRead].textName, textName );
					/*
					 * Assign this texture to the primitive data structure
					 */
					
					/* Changed by Thompson Peter Lied - 24/10/2003 */					
					//assignText2Prim( cylID, param, textName, numberTextRead );
					assignText2Prim( cylID, param, numberTextRead );
					
					fprintf( stderr, "TextName = %s \nParam = %s TextureID = %d \n\n",
							textName, param, numberTextRead );
					
					/* Keep track on how many textures we have */
					numberTextRead++;
					if ( numberTextRead > MAX_N_TEXTURES )
						errorMsg( "Too many textures to read in..." );
				}
			}
		}
		else
		{
			fprintf( stderr, "%s\n", input );
			errorMsg("Got token not readable in input texture file...");
		}
	}
	
	/* Transfer the info on how many textures we have to the global var */
	totalNumberTextures = numberTextRead;
	//totalNumberTextures = numberTextRead-1;
	
	/* Informs the user */
	fprintf( stderr, "\n==========================================\n");
	if ( totalNumberTextures == 0 )
	{
		fprintf( stderr, "There are no textures defined in %s\n", fileName );
		printf( "There are no textures defined in %s\n", fileName );
	}
	else
	{
		fprintf( stderr, "Read in %d textures from file %s\n",
				totalNumberTextures, fileName );
		/*printf( "Read in %d textures from file %s\n",
		 totalNumberTextures, fileName );*/
	}
	//fprintf( stderr, "==========================================\n\n");
	fclose( fpTexture );
	
	/*
	 * Open and load into memory all textures
	 */
	//totalNumberTextures = totalNumberTextures - textureID;
	if ( totalNumberTextures > 0 )
		openLoadAllTextures();
}

/*
 *------------------------------------------------------------
 *	
 *------------------------------------------------------------
 */
int textureAlreadyDefined ( char *textName, int numberTextRead )
{
	
	int i;
	
	for( i = 0; i < numberTextRead; i++ )
	{
		if ( !strcmp( textName, textArray[i].textName ) )
			return i;
	}
	return -1;
}

/*
 *------------------------------------------------------------
 *
 *------------------------------------------------------------
 */
//void assignText2Prim( int cylID, char param[255], char textName[255], int textID  )
/* Changed by Thompson Peter Lied - 24/10/2003 */
void assignText2Prim( int cylID, char param[255], int textID  )
{
	int id;
	
	id = 0;
	
	if ( !strcmp( param, EXISTC ) )
	{
		id = EXIST_C;
	}
	else if ( !strcmp( param, EXISTD ) )
	{
		id = EXIST_D;
	}
	else if ( !strcmp( param, COLORC ) )
	{
		id =  COLOR_C;
	}
	else if ( !strcmp( param, COLORD ) )
	{
		id = COLOR_D;
	}
	else if ( !strcmp( param, REPRAD ) )
	{
		id =  REP_RAD;
	}
	else if ( !strcmp( param, FORCE ) )
	{
		id = W_FORCE;
	}
	else if ( !strcmp( param, SPLITRATEC ) )
	{
		id = SPLIT_RATE_C;
	}
	else if ( !strcmp( param, SPLITRATED ) )
	{
		id = SPLIT_RATE_D;
	}
	else if ( !strcmp( param, ADHESION ) )
	{
		id = ADH;
	}
	else
	{
		fprintf( stderr, "Cylinder ID = %d Param = %s TextId = %d \n",
				cylID, param , textID );
		errorMsg( "Can not find parameter!" );
	}
	
	/* Copy which parameter this texture is controlling */
	//fprintf(stderr, "  cylID = %d  id = %d textId = %d\n", cylID, id, textID);
	Prim[cylID].textID[id] = textID;
	printf("Cilindro %d recebe  texID %d de parametro %d \n", cylID, textID, id );
	textArray[textID].textID = textID;
	textArray[textID].cylID = cylID;
}
/*
 *------------------------------------------------------------
 *	Open and load all texture files
 *------------------------------------------------------------
 */
void openLoadAllTextures( void )
{
	int i;
	printf("Numero total de texturas: %d \n",totalNumberTextures);
	//fprintf(stderr, "openLoadAllTextures... \n");
	//fprintf( stderr, "totalNumberTextures = %d \n", totalNumberTextures);
	
	//textArray[numberTextRead].textID
	//fprintf(stderr,"numberTextRead = %d \n", textArray[totalNumberTextures].textID+1 );
	
	//fprintf( stderr, "\n==========================================\n");
	fprintf( stderr, "Processing texture database...\n");
	fprintf( stderr, "==========================================\n");
	
	for( i = 0; i < totalNumberTextures; i++ )
	{
		//fprintf(stderr, "openLoadOneTextureFileRLE = %d \n", i);
		openLoadOneTextureFileRLE( i );
	}
	fprintf( stderr, "\n" );
	printf( "\n" );
	
}
/*
 *------------------------------------------------------------
 *	Free space occupied by textures
 *------------------------------------------------------------
 */
void freeTextures( void )
{
	int i;
	
	for ( i = 0; i < totalNumberTextures; i++ )
		free( textArray[i].textureImage );
}


/*
 *------------------------------------------------------------
 * Added by Cadu Moreira
 *------------------------------------------------------------
 */
/*int round( double value )
 {
 int i;
 double frac;
 
 frac = modf( value, &i );
 i = (int) floor(value);
 if( frac < 0.5 )
 return( i );
 else
 return ( i+1 );
 }*/


/*
 *------------------------------------------------------------
 *	Open and load one texture file
 *------------------------------------------------------------
 */
void openLoadOneTextureFileRLE( int textID )
{
	printf("Carregando Textura ID %d \n", textID);
	Image *images;       // Cadu Moreira 06/05/02
	ImageInfo *image_info;      // Cadu Moreira 06/05/02
	//Exception_Info exception; // Cadu Moreira 06/05/02
	ExceptionInfo exception; // Cadu Moreira 06/05/02
	PixelPacket *scanLine;   // Cadu Moreira 13/05/02
	double factor = 255.0 / 65535.0; // Cadu Moreira 13/05/02
	// FILE *fpIn;	   // commented by Cadu Moreira 14/05/02
	int xRes, yRes;
	const int zRes = ZResolution;      // Cadu Moreira 06/05/02
	int i, j, k;
	// ColChar scanLine[PNG_RESOLUTION]; // "RLE_RESOLUTION" before. Cadu Moreira 30/04/02 // commented by Cadu Moreira - 13/04/02
	
	/* Open the file */
	/*if ( (fpIn = fopen( textArray[textID].textName, "r" )) == NULL )
	 errorMsg("Could not open texture file to read!");*/
	/* Decode the header */
	/* if (DecodeHeader (&xRes, &yRes, &zRes, fpIn ) == FALSE)
	 {
	 fclose( fpIn );
	 fprintf( stderr, "Texture ID = %d\n", textID );
	 errorMsg("Error during texture header decoding (texture.c)");
	 } */
	/*else
	 {
	 fprintf ( stderr, "Loading texture %d into memory:\nname = %s size(%d %d %d)\n",
	 textID, textArray[textID].textName, xRes, yRes, zRes );
	 printf ( "Loading texture %d into memory:\nname = %s size(%d %d %d)\n",
	 textID, textArray[textID].textName, xRes, yRes, zRes );*/
	/* MARCELO. YOU HAVE TO THINK ABOUT THIS! */
	/*textArray[textID].m = yRes;
	 textArray[textID].n = xRes;
	 */
	
	
	/* Cadu Moreira - 06/05/02 */
	/* Initializing ImageMagick Structures */
	InitializeMagick( argvv );
	
	GetExceptionInfo( &exception );
	image_info=CloneImageInfo((ImageInfo *) NULL);
	//(void) strcpy( image_info->filename, "black.jpeg" );
	(void) strcpy( image_info->filename, textArray[textID].textName );
	//fprintf( stderr, "imgInfo->filename = %s \n", image_info->filename);
	
	
	/* Decode Header, Set Structures */
	images=ReadImage(image_info,&exception);
	
	if( exception.severity != UndefinedException )
	{
		fprintf(stderr, "Failed to read original image %s \n \tError during texture header decoding \n %d \n",
				image_info->filename, exception.severity );
		exit( 1 );
	}
	else if( images == (Image *)NULL )
		exit( 1 ) ;
	
	else
	{
		// yRes = (int) floor( imgTexture->y_resolution ); // Cadu Moreira - 13/05/02
		// xRes = (int) floor( imgTexture->x_resolution ); // Cadu Moreira - 13/05/02
		
		yRes = images->rows;
		xRes = images->columns;
		
		
		fprintf ( stderr, "Loading texture %d into memory:\nname = %s size(%d %d %d)\n",
				 textID, textArray[textID].textName, xRes, yRes, zRes );
		//printf ( "Loading texture %d into memory:\nname = %s size(%d %d %d)\n",
		//textID, textArray[textID].textName, xRes, yRes, zRes );
		
		
		textArray[textID].m = yRes;
		textArray[textID].n = xRes;
		
		
		/*
		 * Now that I know the resolution of the texture
		 * I can allocate the space for it
		 */
		textArray[textID].textureImage = (ColChar **) malloc(yRes * sizeof(ColChar *));
		//textArray[textID].textureImage = (ColChar **) malloc(10 * sizeof(ColChar *));
		if ( textArray[textID].textureImage == NULL )
			errorMsg("No space left on device for textures (texture.c)!");
		else
		{
			for(i=0; i < yRes; i++)
			{
				textArray[textID].textureImage[i] = (ColChar *)malloc(xRes * sizeof (ColChar));
				//textArray[textID].textureImage[i] = (ColChar *)malloc(10 * sizeof (ColChar));
				if ( textArray[textID].textureImage[i] ==  NULL )
					errorMsg("No space left on device for textures (texture.c)!");
			}
		}
		
		/* Below, four debug lines - Cadu Moreira 14/05/02 */
		// a = GetImagePixels( images, 0, 0, xRes, 1 );
		// a++;
		// printf("X res.: %d\n",xRes);
		// printf("Y res.: %d\n",yRes);
		// printf("BlueC.: %d\n", a->blue);
		
		/*
		 * Read the image in
		 */
		for(j = 0; j < yRes; j++)
		{
			// if ( DecodeCCLine( xRes, zRes, scanLine, fpIn ) == FALSE ){ // commented by Cadu Moreira - 13/05/02
			
			// *GetImagePixels ( Image *image, const long x, const long y,
			// const unsigned long columns, const unsigned long rows );
			// The command below is wrong
			//scanLine = GetImagePixels( images, 0, j, xRes, j + 1 );
			scanLine = GetImagePixels( images, 0, j, xRes, 1 );
			//printf("vermelho = %d \n ", scanLine->blue);
			if ( scanLine == NULL )
			{
				fprintf(stderr, "scanLine == NULL -> Line = %d \n", j);
			}
			else
			{
				//for( k = 0; k < xRes; k++)
				for( i = 0; i < xRes; i++)
				{
					/* Commented by Cadu Moreira - 13/05/02 */
					/* textArray[textID].textureImage[j][k].redC = scanLine[k].redC;     */
					/* textArray[textID].textureImage[j][k].greenC = scanLine[k].greenC; */
					/* textArray[textID].textureImage[j][k].blueC = scanLine[k].blueC;   */
					/* textArray[textID].textureImage[j][k].alphaC = scanLine[k].alphaC; */
					
					/*
					 scanLine->red = 1.0;
					 scanLine->green = 1.0;
					 scanLine->blue = 0.0;
					 scanLine->opacity = 0.0;
					 */
					/* Added by Cadu Moreira 13/05/02 */
					/* Modified by Fabiane Queiroz 09/12/09 */
					/*double scanLineR, scanLineG, scanLineB, scanLineO;
					 scanLineR = round(scanLine->red * factor);
					 scanLineG = round(scanLine->green * factor);
					 scanLineB = round(scanLine->blue * factor);
					 scanLineO = round(scanLine->opacity * factor);*/
					
					//scanLine->red = round(scanLine->red * factor);
					//scanLine->green = round(scanLine->green * factor);
					//scanLine->blue = round(scanLine->blue * factor);
					//scanLine->opacity = round(scanLine->opacity * factor);
					
					// Commented by Thompson Peter Lied 28/11/2002
					// Change j and k
					//textArray[textID].textureImage[j][k].redC = scanLine->red;
					//textArray[textID].textureImage[j][k].greenC = scanLine->green;
					//textArray[textID].textureImage[j][k].blueC = scanLine->blue;
					//textArray[textID].textureImage[j][k].alphaC = scanLine->opacity; // CADU VERIFY: opacity == alpha
					
					textArray[textID].textureImage[i][j].redC = scanLine->red;
					textArray[textID].textureImage[i][j].greenC = scanLine->green;
					textArray[textID].textureImage[i][j].blueC = scanLine->blue;
					textArray[textID].textureImage[i][j].alphaC = scanLine->opacity; // CADU VERIFY: opacity == alpha
					
					/*textArray[textID].textureImage[i][j].redC = scanLineR;
					 textArray[textID].textureImage[i][j].greenC = scanLineG;
					 textArray[textID].textureImage[i][j].blueC = scanLineB;
					 textArray[textID].textureImage[i][j].alphaC = scanLineO;*/
					
					// Added by Thompson in 27/11/2002
					//fprintf(stderr, "[%d %d %d %d] ", scanLine->red, scanLine->green, scanLine->blue, scanLine->opacity);
					//if( i == xRes-1 )
					//	fprintf(stderr, "\n" );
					
					scanLine++;
					
					/*************/
				}
			}
		}
		
		/* 2 debug lines - Cadu Moreira 14/05/02 */
		// printf("X res.: %d\n",xRes);
		// printf("Y res.: %d\n",yRes);
		
		/* Print the file for debugging purposes - commented by Cadu Moreira 14/05/02 */
		/*for(j = 0; j < yRes; j++)
		 {
		 for( i = 0; i < xRes; i++)
		 {
		 fprintf(stderr, "[%d,%d,%d] ",
		 textArray[textID].textureImage[i][j].redC,
		 textArray[textID].textureImage[i][j].greenC,
		 textArray[textID].textureImage[i][j].blueC );
		 }
		 printf("\n");
		 }*/
	}
	
	/* close the file */
	// fclose( fpIn ); // commented by Cadu Moreira 13/05/02
	// printf("terminou\n"); // debug line - Cadu Moreira 14/05/02
}


/*
 *------------------------------------------------------------
 *	Computes bilinear interpolation
 *------------------------------------------------------------
 */
ColChar bilinearInterp( double u, double v, int textIdent )
{
	
	int m, n;
	int i, j, inext, jnext;
	double x, y, cx, cy;
	ColChar c;
	
	m = textArray[textIdent].m;
	n = textArray[textIdent].n;
	//fprintf(stderr, "m = %d  n = %d \n", m, n );
	
	/*
	 * Compute integer and frac parts of the coordinates
	 */
	/* first in the u direction */
	if ( u == 1.0 )
	{
		i = m - 1;
		x = 0.0;
		cx = 1.0;
	}
	else
	{
		i = (m - 1) * u;	/* I am using integer truncation here */
		x = (m - 1) * u - i;
		cx = 1.0 - x;
	}
	/* now in the v direction */
	if ( v == 1.0 )
	{
		j = 0;
		y = 0.0;
		cy = 1.0;
	}
	else
	{
		j = n * v;	/* I am using integer truncation here */
		y = n * v - j; /* fractional part */
		cy = 1.0 - y;
	}
	/*
	 * For the u coordinate, we do not wrap around. We replicate
	 * the last value
	 */
	inext = (i + 1);
	if ( inext == m )
		inext = m - 1;
	jnext = (j + 1)%n;
	
	c.redC = cx * cy * textArray[textIdent].textureImage[i][j].redC +
	x * cy * textArray[textIdent].textureImage[inext][j].redC     +
	x *  y * textArray[textIdent].textureImage[inext][jnext].redC +
	cx *  y * textArray[textIdent].textureImage[i][jnext].redC;
	
	c.greenC = cx * cy * textArray[textIdent].textureImage[i][j].greenC +
	x * cy * textArray[textIdent].textureImage[inext][j].greenC     +
	x *  y * textArray[textIdent].textureImage[inext][jnext].greenC +
	cx *  y * textArray[textIdent].textureImage[i][jnext].greenC;
	
	c.blueC = cx * cy * textArray[textIdent].textureImage[i][j].blueC +
	x * cy * textArray[textIdent].textureImage[inext][j].blueC     +
	x *  y * textArray[textIdent].textureImage[inext][jnext].blueC +
	cx *  y * textArray[textIdent].textureImage[i][jnext].blueC;
	
	c.alphaC = cx * cy * textArray[textIdent].textureImage[i][j].alphaC +
	x * cy * textArray[textIdent].textureImage[inext][j].alphaC     +
	x *  y * textArray[textIdent].textureImage[inext][jnext].alphaC +
	cx *  y * textArray[textIdent].textureImage[i][jnext].alphaC;
	
	return ( c );
	
}
/*
 *------------------------------------------------------------
 *	Computes texture coordinates from the
 *	cylinders
 *------------------------------------------------------------
 */
void compTextCoordinates( void )
{
	int i, j, nVert;
	Point3D p, p1, p2;
	double v, v1, v2;
	
	/* change cursor into a watch */
#ifdef GRAPHICS
	glutSetCursor( GLUT_CURSOR_WAIT );
#endif
	
	/* this routine finds all faces at the boundary of
	 the texture space (cross from 1->0) and deals
	 with them */
	
	findFacesAtBoundary();
	
	for(i=0; i < NumberFaces; i++)
	{
		nVert = faces[i].nverts;
		if ( nVert > 3)
			errorMsg("Don't know yet how to handle polygons which are not 3-sided!");
		else
		{
			if ( !faces[i].boundary )
			{
				/* we only have to deal with the faces NOT at the boundary */
				for(j=0; j < nVert; j++)
				{
					/* I have to detect how many primitives are associated
					 with this vertex. For now the two only possibilities
					 are 1 or 2 primitives */
					if ( vert[faces[i].v[j]].prim1 != 0 && vert[faces[i].v[j]].prim2 != 0)
					{
						/* the vertex has two primitives associated */
						compCylinderCoord(vert[faces[i].v[j]].prim1,
										  vert[faces[i].v[j]].pos, &p1);
						v1 = computeV(p1, FALSE);
						compCylinderCoord(vert[faces[i].v[j]].prim2,
										  vert[faces[i].v[j]].pos, &p2);
						v2 = computeV(p2, FALSE);
						
						/* interpolates between the 2 values */
						p.x = p1.x + ((p2.x - p1.x) / 2.0);
						v = v1 + ((v2 - v1) / 2.0);
					}
					else
					{
						if (vert[faces[i].v[j]].prim1 != 0)
						{
							/* the above test is just to make sure that all vertices
							 have primitive info associated */
							compCylinderCoord(vert[faces[i].v[j]].prim1,
											  vert[faces[i].v[j]].pos, &p);
							v = computeV(p, FALSE);
						}
						else
							errorMsg("There is a vertex without a primitive associated!");
					}
					vertt[faces[i].vt[j]].x = p.x;
					vertt[faces[i].vt[j]].y = v;
				}
			}
		}
	}
	
	/* change cursor back to original left arrow format */
#ifdef GRAPHICS
	glutSetCursor( GLUT_CURSOR_LEFT_ARROW );
#endif
	
	/* set flag TRUE for texture coordinates were computed */
	textCoordComputed = TRUE;
	
}

/*
 *------------------------------------------------------------
 *------------------------------------------------------------
 */
void findFacesAtBoundary( void )
{
	int i, j, prim, indexIntoTextureArray=0;
	int numberVerticesAtBoundary=0;
	double v0, v1, v2;
	Point3D p0, p1, p2;
	
	for(i=0; i < NumberFaces; i++)
	{
		/* Marcelo remember, if you change the way you
		 are defining the primitives here you also
		 have to change inside the compTextCoordFaceAtBoundary()
		 function */
		if (faces[i].prim1 != 0)  
			prim = faces[i].prim1;
		else if (faces[i].prim2 != 0 ) 
			prim = faces[i].prim2;
		
		if ( checkFaceBoundary( i , prim ) )
		{
			faces[i].boundary = TRUE;
			numberVerticesAtBoundary += faces[i].nverts;
		}
		else
		{
			faces[i].boundary = FALSE;
		}
	}
	printf("Number of vertices at the boundary is %d\n",numberVerticesAtBoundary);
	/* now for each face that it is at the boundary I need
	 to allocate space for new texture coordinates */
	vertt = (Point2D *) realloc(vertt, sizeof (Point2D) *
								(NumberTextureVertices + numberVerticesAtBoundary));
	if ( vertt == NULL)
		errorMsg("Not enough space for new texture vertices in texture.c!");
	
	/* now I have to rearrange the texture information as it's
	 related to the faces */
	printf("Old number of texture vertices %d\n", NumberTextureVertices);
	
	indexIntoTextureArray = NumberTextureVertices;
	for(i=0; i < NumberFaces; i++)
	{
		/* check if it is a face that needs to be fixed */
		if ( faces[i].boundary == TRUE )
		{
			for(j=0; j < faces[i].nverts; j++)
			{
				faces[i].vt[j] = indexIntoTextureArray;
				vertt[faces[i].vt[j]].x = -1;
				vertt[faces[i].vt[j]].y = -1;
				indexIntoTextureArray++;
			}
		}
	}
	/* update new number of texture coordinates */
	NumberTextureVertices = indexIntoTextureArray;
	printf("New number of texture vertices %d\n", NumberTextureVertices);
	
	/* now finally compute the actual text coordinates for these
	 vertices */
	for(i=0; i < NumberFaces; i++)
	{
		/* this part below only works for TRIANGLES!!! */
		if ( faces[i].boundary == TRUE )
		{
			compCylinderCoord(prim, vert[faces[i].v[0]].pos, &p0);
			compCylinderCoord(prim, vert[faces[i].v[1]].pos, &p1);
			compCylinderCoord(prim, vert[faces[i].v[2]].pos, &p2);
			
			/* find 'v' for the 3 vertices */
			findV( p0, p1, p2, &v0, &v1, &v2);
			
			vertt[faces[i].vt[0]].x = p0.x;
			vertt[faces[i].vt[1]].x = p1.x;
			vertt[faces[i].vt[2]].x = p2.x;
			
			vertt[faces[i].vt[0]].y = v0;
			vertt[faces[i].vt[1]].y = v1;
			vertt[faces[i].vt[2]].y = v2;
		}
	}
}

/*
 *------------------------------------------------------------
 *	Computes texture coordinates from the
 *	cylinders
 *------------------------------------------------------------
 */
void findV(Point3D p0, Point3D p1, Point3D p2, double *v0, double *v1, double *v2)
{
	
	/* for positive values we leave as it is */
	if ( p0.y < 0  && p1.y < 0)
	{
		/* both p0 and p1 are negative */
		*v0 = computeV(p0, TRUE);
		*v1 = computeV(p1, TRUE);
		*v2 = computeV(p2, FALSE);
	}
	if ( p0.y > 0 && p1.y > 0)
	{
		/* p2 is negative */
		*v0 = computeV(p0, FALSE);
		*v1 = computeV(p1, FALSE);
		*v2 = computeV(p2, TRUE);
	}
	if ( p0.y < 0  && p2.y < 0)
	{
		/* p0 and p2 are negative */
		*v0 = computeV(p0, TRUE);
		*v1 = computeV(p1, FALSE);
		*v2 = computeV(p2, TRUE);
	}
	if ( p0.y > 0  && p2.y > 0)
	{
		/* p1 is negative */
		*v0 = computeV(p0, FALSE);
		*v1 = computeV(p1, TRUE);
		*v2 = computeV(p2, FALSE);
	}
	if ( p1.y < 0  && p2.y < 0)
	{
		/* p1 and p2 are negative */
		*v0 = computeV(p0, FALSE);
		*v1 = computeV(p1, TRUE);
		*v2 = computeV(p2, TRUE);
	}
	if ( p1.y > 0  && p2.y > 0)
	{
		/* p0 is negative */
		*v0 = computeV(p0, TRUE);
		*v1 = computeV(p1, FALSE);
		*v2 = computeV(p2, FALSE);
	}
}

/*
 *------------------------------------------------------------
 * computes the 'v' coordinate based on 'y' and 'z' given
 * in primitive coordinates
 *------------------------------------------------------------
 */
double computeV( Point3D  p, int adjust)
{
	double v;
	
	v = atan2( p.y, p.z ) + PI;
	if ( adjust )
	{
		v += TWOPI;
	}
	v /= TWOPI;
	
	return(v);
}

/*
 *------------------------------------------------------------
 *	checks if a face is at the boundary of a
 *	cylinder. This only works for triangles now!!!
 *------------------------------------------------------------
 */
int checkFaceBoundary(int whichFace, int whichPrim)
{
	Point3D p0, p1, p2;
	
	compCylinderCoord( whichPrim, vert[faces[whichFace].v[0]].pos, &p0);
	compCylinderCoord( whichPrim, vert[faces[whichFace].v[1]].pos, &p1);
	compCylinderCoord( whichPrim, vert[faces[whichFace].v[2]].pos, &p2);
	
	if ( p0.z > 0 || p1.z > 0 || p2.z > 0 ) 
		return ( FALSE );
	else if ( p0.y > 0 && p1.y > 0 && p2.y > 0) 
		return ( FALSE );
	else if ( p0.y < 0 && p1.y < 0 && p2.y < 0) 
		return ( FALSE );
	else 
		return(TRUE);
}

/*
 *------------------------------------------------------------
 *	Writes a file in optik format. Splits the polygons
 *	into triangles and compute texture coordinates
 *------------------------------------------------------------
 */
void writeOptikOutput( char *sessionfilename )
{
	FILE *fp;
	int i, j, len, whichPrim, nVert;
	char optikFileName[128] = "";
	char objectName[15] = "";
	char tmpString[128] = "";
	Point3D p, p1, p2;
	double v, v1, v2;
	
	/* make sure we have computed the texture coordinates */
	/* THIS SEEMS TO BE BREAKING THE CODE!!! */
	if ( !textCoordComputed ) compTextCoordinates();
	
	/* change cursor into a watch */
#ifdef GRAPHICS
	glutSetCursor( GLUT_CURSOR_WAIT );
#endif
	
	/* get rid of the obj.session */
	len = strlen( sessionfilename );
	strncpy( optikFileName, sessionfilename, len - 8 );
	strncpy( tmpString, sessionfilename, len - 8 );
	tmpString[len - 8]='\0';
	optikFileName[len - 8]='\0';
	strcat( optikFileName, ".optik" );
	strcat( tmpString, ".obj" );
	printf("Optik file name is %s\n", optikFileName);
	
	/* inits object name */
	strcpy(objectName, "tri_");
	
	/* open the file to write */
	if( (fp = fopen(optikFileName,"w")) == NULL)
		errorMsg("Could not open optik file to write!");
	
	fprintf(fp,"/""* Optik script output from the growth program\n");
	fprintf(fp,"/""* by Marcelo Walter\n");
	fprintf(fp,"/""* Original .obj file %s\n", tmpString);
	fprintf(fp,"/""*\n");
	
	for(i=0; i < NumberFaces; i++)
	{
		nVert = faces[i].nverts;
		if ( nVert > 3)
		{
			printf("Face %d has %d vertices\n",i,faces[i].nverts);
			/* I have to convert all faces with more than 3 sides to
			 triangles */
			fclose(fp);
			errorMsg("Don't know yet how to handle polygons which are not 3-sided!");
		}
		else
		{
			fprintf(fp,"add object tri_%d triangle ",i);
			for(j=0; j < nVert; j++)
			{
				fprintf(fp,"%f %f %f ",
						vert[faces[i].v[j]].pos.x,
						vert[faces[i].v[j]].pos.y,
						vert[faces[i].v[j]].pos.z);
			}
			if ( NumberNormals > 0)
			{
				/* we have normals therefore we output them
				 as well */
				for(j=0; j < nVert; j++)
				{
					fprintf(fp,"%f %f %f ",
							vertn[faces[i].vn[j]].x,
							vertn[faces[i].vn[j]].y,
							vertn[faces[i].vn[j]].z);
				}
			}
			fprintf(fp,"\n");
			fprintf(fp,"texture tri_%d texture_name ",i);
			
			for(j=0; j < nVert; j++)
			{
				fprintf(fp,"%f %f ", vertt[faces[i].vt[j]].x, vertt[faces[i].vt[j]].y);
			}
			fprintf(fp,"\n");
			fprintf(fp,"surface tri_%d surface_name\n",i);
		}
	}
	fclose(fp);
	
	/* change cursor back to original left arrow format */
#ifdef GRAPHICS
	glutSetCursor( GLUT_CURSOR_LEFT_ARROW );
#endif
}

/*
 *------------------------------------------------------------
 *	Writes the object file in vertigo format.
 *------------------------------------------------------------
 */
void writeVertigoOutput( char *sessionfilename )
{
	FILE *fp;
	int i, j, len, nVert;
	char vertigoFileName[128] = "";
	
	/* change cursor into a watch */
#ifdef GRAPHICS
	glutSetCursor( GLUT_CURSOR_WAIT );
#endif
	
	/* get rid of the obj.session */
	len = strlen( sessionfilename );
	strncpy( vertigoFileName, sessionfilename, len - 8 );
	vertigoFileName[len - 8]='\0';
	strcat( vertigoFileName, ".pol" );
	printf("Vertigo file name is %s\n", vertigoFileName);
	
	/* open the file to write */
	if( (fp = fopen(vertigoFileName,"w")) == NULL)
		errorMsg("Could not open vertigo file to write!");
	
	fprintf(fp,"object polygon\n");
	fprintf(fp,"\tvertices\n");
	
	for(i = 0; i < NumberVertices; i++)
	{
		fprintf(fp,"\t(%f %f %f)\n",
				vert[i].pos.x,
				vert[i].pos.y,
				vert[i].pos.z );
	}
	/* write down the face conectivity info */
	for(i = 0; i < NumberFaces; i++)
	{
		fprintf(fp,"\tface ");
		nVert = faces[i].nverts;
		for( j = 0; j < nVert; j++)
		{
			fprintf(fp,"%d ", (faces[i].v[j])+1);
		}
		fprintf(fp,"\n");
	}
	
	fprintf(fp,"endobject\n");
	fclose(fp);
	
	/* change cursor back to original left arrow format */
#ifdef GRAPHICS
	glutSetCursor( GLUT_CURSOR_LEFT_ARROW );
#endif
}


/*
 *------------------------------------------------------------
 *	Writes the pattern file in open Inventor format.
 *------------------------------------------------------------
 */
void writeInventorOutput( const char *filename )
{
	FILE *fp;
	int i, j, k, len, whichFace, nVorVerts, faceIndex;
	char ivFileName[128] = "";
	Point3D q;
	CELL *c;
	/*
	 FILE *fpFOO;
	 */
	
	/* change cursor into a watch */
#ifdef GRAPHICS
	glutSetCursor( GLUT_CURSOR_WAIT );
#endif
	
	if ( !voronoiComputed )
		errorMsg("There is no voronoi computed to save as an Inventor file!");
	
	/* get rid of the obj.session*/
	len = strlen( filename );
	strncpy( ivFileName, filename, len - 3);
	ivFileName[len - 3]='\0';
	strcat( ivFileName, ".iv" );
	printf("Inventor file name is %s\n", ivFileName);
	
	/* open the file to write */
	if( (fp = fopen(ivFileName,"w")) == NULL)
		errorMsg("Could not open inventor file to write!");
	
	/*
	 fpFOO = fopen( "foo.geom","w" );
	 */
	
	fprintf( fp, "#Inventor V2.0 ascii\n");
	fprintf( fp, "Separator {\n");
	
	/* write the type of Material Binding */
	fprintf( fp, "\tMaterialBinding {\n");
	/*fprintf( fp, "\t\tvalue	PER_VERTEX_INDEXED\n");*/
	fprintf( fp, "\t\tvalue	PER_FACE_INDEXED\n");
	fprintf( fp, "\t}\n");
	
	/* write the material diffuse colors */
	fprintf( fp, "\tMaterial {\n");
	fprintf( fp, "\t\tdiffuseColor   [\n");
	fprintf( fp, "\t\t%f %f %f ,\n",Ortcell[C].color[R],
			Ortcell[C].color[G],
			Ortcell[C].color[B] );
	fprintf( fp, "\t\t%f %f %f ,\n",Ortcell[D].color[R],
			Ortcell[D].color[G],
			Ortcell[D].color[B] );
	fprintf( fp, "\t\t%f %f %f ,\n",Ortcell[E].color[R],
			Ortcell[E].color[G],
			Ortcell[E].color[B] );
	fprintf( fp, "\t\t%f %f %f ,\n",Ortcell[F].color[R],
			Ortcell[F].color[G],
			Ortcell[F].color[B] );
	fprintf( fp, "\t\t]\n");
	fprintf( fp, "\t}\n");
	
	/* write the vertices index */
	fprintf( fp, "\tCoordinate3 {\n");
	fprintf( fp, "\tpoint	[\n");
	for( whichFace = 0; whichFace < NumberFaces; whichFace++)
	{
		/*
		 fprintf( fpFOO, "Face %d has %d vor polygons\n",  whichFace, faces[whichFace].nVorPol );
		 */
		for ( i = 0; i < faces[whichFace].nVorPol; i++)
		{
			nVorVerts = faces[whichFace].vorFacesList[i].faceSize;
			/*
			 fprintf( fpFOO, "%d voronoi polygon has %d vertices\n", i, nVorVerts );
			 */
			for (j = 0; j < nVorVerts; j++)
			{
				k = faces[whichFace].vorFacesList[i].vorPts[j];
				/*
				 fprintf( fpFOO, "%d ", k );
				 */
				mapFromPolySpace(whichFace, faces[whichFace].vorPoints[k].x,
								 faces[whichFace].vorPoints[k].y, &q);
				fprintf( fp, "\t%f %f %f,\n", q.x, q.y, q.z);
			}
			/*
			 fprintf( fpFOO, "\n");
			 */
		}
	}
	fprintf( fp, "\t]\n");
	fprintf( fp, "\t}\n");
	
	/* write the faces index */
	faceIndex = 0;
	fprintf( fp, "\tIndexedFaceSet {\n");
	fprintf( fp, "\tcoordIndex [\n");
	for( whichFace = 0; whichFace < NumberFaces; whichFace++)
	{
		for ( i = 0; i < faces[whichFace].nVorPol; i++)
		{
			nVorVerts = faces[whichFace].vorFacesList[i].faceSize;
			fprintf( fp, "\t" );
			for (j = 0; j < nVorVerts; j++)
			{
				fprintf( fp, "%d, ", faceIndex);
				faceIndex++;
			}
			fprintf( fp, "-1,\n");
		}
	}
	fprintf( fp, "\t]\n");
	
	/* write the material indexes per vertex */
	fprintf( fp, "\tmaterialIndex [\n");
	for( whichFace = 0; whichFace < NumberFaces; whichFace++)
	{
		for ( i = 0; i < faces[whichFace].nVorPol; i++)
		{
			c = faces[whichFace].vorFacesList[i].site;
			if ( c != NULL )
			{
				if ( c->ctype == C || c->ctype == D ||
					c->ctype == E || c->ctype == F )
					fprintf( fp, "%d,\n", c->ctype );
			}
			/* This has to be fixed. At this point it means
			 that this cell doesn't have a type assigned to
			 it */
			else fprintf( fp, "%d,\n", C);
		}
	}
	fprintf( fp, "\t]\n");
	
	fprintf( fp, "\t}\n");
	
	fprintf(fp,"}\n");
	fclose(fp);
	/*
	 fclose( fpFOO );
	 */
	
	/* change cursor back to original left arrow format */
#ifdef GRAPHICS
	glutSetCursor( GLUT_CURSOR_LEFT_ARROW );
#endif
}

/*
 *------------------------------------------------------------
 *	Open and load one texture file
 *	THIS WAS THE OLD WAY OF READING IN A TEXTURE
 *	ONLY FLOATS. I AM NOT USING THIS ANYMORE
 *	JUNE 30, 1998
 *------------------------------------------------------------
 */
void openLoadOneTextureFile( int textID )
{
	
	FILE *fpIn;
	int i, j, rows, columns;
	
	/* get space for texture */
	textArray[textID].v = allocate2DArrayFloat( textArray[textID].m, textArray[textID].n );
	rows = textArray[textID].m;
	columns = textArray[textID].n;
	
	if ( (fpIn = fopen( textArray[textID].textName, "r" )) == NULL )
		errorMsg("Could not open texture file to read!");
	
	for( i = 0; i < rows; i++ )
	{
		for( j = 0; j < columns; j++ )
		{
			fscanf( fpIn, "%f", &(textArray[textID].v[i][j]) );
		}
	}
	
	fclose( fpIn );
	
	/* For debugging purposes only */
	/*for( i = 0; i < rows; i++ )
	 {
	 for( j = 0; j < columns; j++ )
	 {
	 fprintf( stderr, "%1.1f ", textArray[textID].v[i][j] );
	 }
	 fprintf( stderr, "\n");
	 }
	 fprintf( stderr, "\n");*/
}


