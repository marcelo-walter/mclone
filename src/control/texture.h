/*
 *  texture.h
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/7/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */

/*
 *-----------------------------------------
 * texture.h
 *-----------------------------------------
 */

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "../data/Types.h"
#include "../data/Macros.h"

/* for the parsing of the texture data file */
#define EXISTC 		"EXISTC"
#define EXISTD 		"EXISTD"
#define COLORC 		"COLORC"
#define COLORD 		"COLORD"
#define REPRAD 		"REPRAD"
#define FORCE  		"FORCE"
#define SPLITRATEC 	"SPLITRATEC"
#define SPLITRATED	"SPLIRATED"
#define ADHESION	"ADHESION"
#define	COMMENT		"#"
#define	CYLINDERID	"CYLINDERID"

/* define resolution for z axis: 1 == gray scale, 3 == RGB - Cadu Moreira 14/05/02 */
#define ZResolution 3


/*
 *------------------------------------------------------------
 *	Local definitions
 *------------------------------------------------------------
 */

/* flag to tell if we computed or not texture coordinates */
extern flag textCoordComputed;

extern TEXTARRAY textArray[MAX_N_TEXTURES];

extern int totalNumberTextures;


/*
 *------------------------------------------------------------
 *	Local prototypes
 *------------------------------------------------------------
 */
void 		writeInventorOutput( const char *filename );
ColChar 	bilinearInterp( double u, double v, int textIdent );
void 		readAllTextures( char *fileName );
void 		compTextCoordinates( void );
double 		computeV( Point3D  p, int adjust );
int 		checkFaceBoundary( int whichFace, int whichPrim );
void 		findFacesAtBoundary( void );
void 		findV( Point3D p0, Point3D p1, Point3D p2, double *v0,
				  double *v1, double *v2);
void 		writeVertigoOutput( char *sessionfilename );
void 		writeInventorOutput( const char *filename );
void 		writeOptikOutput( char *sessionfilename );
void 		openLoadAllTextures( void );
void 		openLoadOneTextureFile( int textID );
void 		openLoadOneTextureFileRLE( int textID );
void 		freeTextures( void );
/* Changed by Thompson Peter Lied - 24/10/2003 */
/*void 		assignText2Prim( int cylID, char param[255], char textName[255], int textID);*/
void assignText2Prim( int cylID, char param[255], int textID );

int textureAlreadyDefined ( char *textName, int numberTextRead );

#endif //_TEXTURE_H_
