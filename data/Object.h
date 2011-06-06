/*
 *-------------------------------------------------------
 *	Object.h
 *-------------------------------------------------------
 */

#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "Types.h"

/* for the parsing of the input file */
#define VERTEX		"v"
#define VERTEX_N	"vn"
#define VERTEX_T	"vt"
#define POLYGON		"f"
#define POLYGON_O	"fo"
#define INFO		"#"
#define GROUP		"g"
#define USEMTL		"usemtl"
#define	W_VERTICES	"vertices"
#define	W_NORMALS	"normals"
#define	W_POLYGONS	"elements"
#define	W_TEXTURE	"texture"

/*
 *--------------------------------------------------
 *	Local Global variables
 *--------------------------------------------------
 */
extern Vertex 	*vert;
extern Point3D *vertDisplay;
extern Point3D *origVert;
extern Point3D *vertn;
extern Point2D *vertt;
extern Face	*faces;
extern VECTORARRAY	*vectors;
extern Edge 	*edges;

/*
 * prototypes
 */
void readObject(char *name);
void saveObjFile( const char *fileName );
int myatoi(char **p);


#endif //_OBJECT_H_
