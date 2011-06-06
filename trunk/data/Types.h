/*
 *----------------------------------------------------------
 *	Types.h
 *	This file contains all types definitions
 *----------------------------------------------------------
 */

#ifndef _TYPES_H_
#define _TYPES_H_

#include "../data/Macros.h"

/* define some useful types */
typedef unsigned char byte;
typedef byte flag;

/* Type of random distribution used */
typedef enum{RNONE, POISSON, EXPONENTIAL} RNDDIST;

/* modeling 'mode' variable */
typedef enum{MNONE, TRANS, ROT, SCALE }	MMODE;

/* viewing mode */
typedef enum{TOP,  SIDE, FRONT, PERSPECTIVE, VNONE } VMODE;

/* rendering mode */
typedef enum{FILL, WIRE, HALO, BACKFACE, FRONTLINES, EDGES} RMODE;

/* types of primitives */
typedef enum{CYLINDER, LANDMARK, SPHERE} TYPE_OF_PRIM;

/* voronoi cells with respect to the polygon they belong */
enum{IN, OUT, BORDER, INDEX0, UNKNOW};

/* type of simulation. With or without growth */
typedef enum{WITHGROWTH, WOUTGROWTH} PFMODE;

/* Added by Fabiane Queiroz in 25/11/2009 */
/* Local Relaxation, With or without local Relaxixation */
typedef enum{GLOBALRELAX, LOCALRELAX} RELAXMODE;

/* Possible parameters controlled by textures */
enum{
	EXIST_C,
    EXIST_D,
    COLOR_C,
    COLOR_D,
    REP_RAD,
    W_FORCE,
    SPLIT_RATE_C,
    SPLIT_RATE_D,
    ADH
};

/*
 *---------------------------------------------------
 * Which faces to consider for neighbors:
 *
 * NOSECFACES: only the primary faces, ie, the ones
 * which share an edge with the face in question
 *
 * STRIPFAN: map all faces which share a vertex
 * to the plane of the face in question. Analogy
 * with a hand-held fan
 *
 * TURK: Turk's suggestion. Sequence of rotations
 *---------------------------------------------------
 */
typedef enum{NOSECFACES, STRIPFAN, TURK} NEIGHMODE;

typedef struct{
	unsigned char alphaC;
	unsigned char blueC;
	unsigned char greenC;
	unsigned char redC;
} ColChar;

/* Texture array */
typedef struct textarray{
	int 	m, n;		/* size of texture */
	char 	textName[256];	/* pointer to the texture file name */
	float **v;		/* actual texture contents */
	ColChar **textureImage;	/* actual values */
	/* Added by Thompson Peter Lied in 31/10/2003 */
	int cylID;
	int textID;		/* array of texture ids */
}TEXTARRAY;

typedef struct point2d{
	double x,y;
}Point2D;

typedef struct point3d{
	double x,y,z;
}Point3D;

typedef struct point4d{
	double a,b,c,d;
}Point4D;

typedef struct param3d{
	double u, v, w;
}Param3D;

typedef struct param2d{
	double u, v;
}Param2D;

typedef enum{C,D,E,F} CELL_TYPE;

typedef struct{
	float	color[RGB];
	float	prob;
	float	rateOfDivision;
	float	relRate;	
}TYPE;

typedef struct Matrix4Struct {	// 4-by-4 matrix
	double element[4][4];
}Matrix4;

/* Holds an event structure */
typedef struct tevent{
	int faceIndex;
	int edgeIndex;
	int vertIndex;
	float distance;
	Matrix4 t;
}HEAPTRI_ELEMENT;

typedef struct theap{
	HEAPTRI_ELEMENT *array;
	int   *p;
	int   *q;
	int   size;
	int   max;
}HEAPTRI;

/*Added by Fabiane Queiroz at 01/12/2009*/
typedef struct Array{
	int   *array;  
	int   size; 
}Array;

/* Added by Thompson at 19/03/2004 */
/* Have vector on faces: none, primary, secondary or tertiary */
typedef enum{NONE, PRIM, SEC, TER} NEIGHBORHOOD;

/*
 *---------------------------------------------------------------
 * CELL structure
 *---------------------------------------------------------------
 */
typedef struct cell{
	double    x, y, z;	/* position of the cell */
	double    xp, yp, zp;	/* previous position of this cell */
	Param3D   bary;	/* barycentric coordinates of this cell */
	byte 	    prim1, prim2;	/* which primitives are associated with
	 this cell */
	Param2D   p1, p2;	/* (u,v) coordinates for primitive 1 and primitive 2 */
	double    fx, fy;	/* force in x and y direction */
	int	    whichFace;	/* which polygon does the cell belong to */
	double    px, py;	/* position of this cell in the polygon's XY
	 space used to compute the voronoi */
	CELL_TYPE ctype;     	/* cell type */
	float     tsplit;	/* time for next mitosis to occur */
	byte 	    gen;	/* tells how many times the cell
	 has divided itself */
	struct cell *next;    /* pointer to the next cell */
}CELL;

/*
 *---------------------------------------------------------------
 * SIMPLE CELL structure
 *---------------------------------------------------------------
 */
typedef struct scell{
	Point2D	p;		/* original cell's coordinates in 2D space */
	CELL		*originalCell;	/* pointer to the original cell */
	int		toProcess;		
	struct scell 	*next;		/* pointer to the next cell */
}SCELL;

/*
 *---------------------------------------------------------------
 * Data structures for the voronoi part
 *---------------------------------------------------------------
 */
typedef struct{
	double	x, y;		/* position of the Voronoi vertex */
	CELL		*sites[3]; 	/* the sites that generate the Voronoi vertex */
	byte 		inside;		/* flag is this point is inside of the face */
}voronoiType;

typedef struct{
	int	faceSize;		/* How many points of vorPts are valid */
	int   vorPts[MAX_N_VOR_VERT];	/* The list of indices into the array of
	 voronoiType that form the boundary of the
	 face.  Note:  an index of 0 indicates
	 a Voronoi vertex at infinity! */
	int     vorPtsChanged[MAX_N_VOR_VERT];  /* The same to the previous one
	 but with the indice of vertices rearranged */
	CELL  *site;     		/* The cell that corresponds to the face */
	byte	border;			/* flag to check if the face is at the border */
}faceType;

/*
 *---------------------------------------------------------------
 * Vertex
 *---------------------------------------------------------------
 */
typedef struct vertex{
	Point3D pos;		/* original position of the vertex */
	Point3D dir;		/* Added by Thompson Peter Lied at 09/12/2003 */
	/* This point indicates the vector's direction */
	byte vec;			/* vertex was hit by a picking sweep */
	byte prim1, prim2;	/* which primitives are associated with
	 this vertex */
	byte hit;		/* vertex was hit by a picking sweep */
	int edge;		/* pointer to the first edge that uses
	 this vertex */
	Param2D   p1, p2;	/* (u,v) coordinates for primitive 1 and primitive 2 */
	byte nNeighFaces;	/* how many neighboring faces there are */
	int *neighFaces;	/* all faces which share this vertex */
	/*add By Fabiane Queiroz*/
	Point3D EndOfVector3D;
	Point2D beginOfProjectvector, endOfProjectVector;
}Vertex;

/*
 *---------------------------------------------------------------
 * Edge
 *---------------------------------------------------------------
 */
typedef struct edge{
	int vstart,
	vend;	/* pointer to start and end  vertices for this edge */
	int epcw,	/* pointers to previous and next, clock and */
	epccw,	/* counter clockwise edges */
	encw,
	enccw;
	int pf,	/* pointer to previous and next face */
	nf;
	Matrix4 pn, np;/* pn rotates a point from pf->nf;
	 np is the inverse of pn */
}Edge;

/*
 *---------------------------------------------------------------
 * Primitives
 *---------------------------------------------------------------
 */
typedef struct primitive{
	int type;		/* type of primitive: cylinder or feature */
	/* only for cylinders */
	Point3D center;	/* center of primitive */
	float r, h;		/* radius and heigth */
	Point3D cosine;	/* future use. I want to pre-compute the
	 orientation for the primitives */
	Point3D sine;	
	int  parentPrim;	/* index For the parent primitive */
	int landMarkR;	/* index for the radius feature */
	int landMarkH;	/* index for the heigth feature */
	int animation;	/* if != -1 means that this primitive
	 has a set of rotations defined such
	 that it will be 'animated' according
	 to the specifications in a companion
	 file with the general name *.anim.
	 Index into the animation array (animData)
	 */
	int axis;		/* axis for the animation. In the future
	 I might need information for the 3 axes.
	 For now, since I now that for a given
	 primitive I'm only rotating about one
	 of the 3 axis I can get around with this
	 flag only */
	int textID[N_TEXT_PER_PRIM];		/* array of texture ids */
	/* only for landMarks */
	int gIndex;		/* index into the growth data array */
	int primInd;		/* index for primitive */
	Point3D q1,q2;	/* original points given by the user
	 (in model's original coordinates) */
	Point3D p1,p2;	/* same points in primitive's
	 coordinates */
	double lengthW;	/* length of the landmark in WORLD
	 coordinates */
	double lengthP;	/* length of the landmark in primitive
	 space coordinates */
	double dus,dvs,dws;	/* used to compute faster the radius
	 and height of the primitives according
	 to the landmark info */
	
	/* for both */
	float rot[XYZW];
	float trans[XYZ];
	float scale[XYZ];
	/* rates of growth for each type of cell.
     This is used when I grow the pattern together
     with the embryo growing. I AM NOT USING THIS
     ANYMORE!!! */
	float rates[HOW_MANY_TYPES];     
}Primitive;

/*
 *---------------------------------------------------------------
 * Faces
 *---------------------------------------------------------------
 */
typedef struct face{
	byte hit;		/* face was hit or not in a picking sweep */
	byte nverts;		/* how many vertices this face has */
	/* a given face can be associated with at most two control primitives */
	byte prim1;		/* first primitive */
	byte prim2;		/* second primitive */
	byte g;		/* group information. I'm not using this yet */
	byte boundary;	/* flag to indicate if the face is at the
	 boundary for texture or not */
	char group[10];	/* not using this yet */
	int *v;		/* list of indexes into geometry vertices array */
	int *vn;		/* list of indexes into normals vertices array */
	int *vt;		/* list of indexes into texture vertices array */
	Point3D center3D;	/* center of face in the 3D cartesian space */
	Point2D center2D;	/* center of face in the 2D space of the face */
	/*Add by Fabiane Queiroz at 24/02/210*/ 
	Point2D centerProjecVector;
	Point2D endOfProjectVector;
	double cosAngle;
	
	Point4D planeCoef;	/* store the plane coefficients for this face */
	Point3D v1, v2;	/* v1, v2 and the normal vector to the face
	 define a 3D space local to the face.
	 v1 is the vector given by the vector difference
	 between the first 2 vertices that define the face.
	 v2 is the cross product between v1 and the normal
	 vector */
	Matrix4 p2o, o2p;	/* matrices that bring a point from the polygon
	 space to the object's XYZ space and vice-versa.
	 These matrices are obtained from v1, v2 and the
	 normal vector to the best fitting plane to the
	 face */
	/*
	 * Below is for distributing random points on the surface
	 * of the object
	 */
	double relArea;	/* current area of the polygon relative to the
	 total area of the object */
	double prevAbsArea;	/* when simulating growth with pattern formation */
	double absArea;	/* current absolut area */
	double partArea;	/* sum of areas up to and including this polygon
	 normalized to be between (0, 1) */
	float *areas;		/* relative areas of sub-triangles
	 of polygon. Used by the procedure to position
	 random points in the polygons */
	/*
	 * Below is the winged-edge stuff
	 */
	int edge;		/* pointer to the first edge of this face */
	byte nPrimFaces;	/* number of neighboring faces to this face */
	int *primFaces;	/* list of adjacent faces to this face */
	int *primEdges;	/* list of adjacent edges to this face */
	double *rotAngles;	/* for each primary adjacent face, stores the angle
	 between this face and all adjacent ones */
	/*
	 * Below is the cells stuff
	 */
	int ncells;		/* how many cells this face has */
	/* how many cells of each type this face has */
	int nCellsType[HOW_MANY_TYPES];
	/* rates of growth for each type of cell.
     This is used when I grow the pattern together
     with the embryo growing */
	float rates[HOW_MANY_TYPES]; 
	int nVorPol;		/* how many voronoi polygons this face has */
	CELL *head;		/* list of cells on this face */
	CELL *tail;
	voronoiType *vorPoints;	/* points which form the voronoi polygons */
	faceType *vorFacesList;	/* list of faces of voronoi polygons for this face */
    
	/* Added by Thompson Peter Lied at 18/03/2004 */
	byte haveVector;		/* This variable indicates that the face have an orientation vector */
	
	Point3D EixoReferencia, EndOfVector3D; /* To set a common vector for all faces, Used to especify the end of vector interpolated by RBF.  */
	
	/*Added by Fabiane Queiroz*/
	Point3D projectedVerticeVector1, projectedVerticeVector2, projectedVerticeVector3;
	
	Point3D EndOfVector1, EndOfVector0; /* To storage the differen result, to compare e calculate the error. */
	double Erro;
	Matrix4 MatrixMapTo3D, MatrixMapTo2D; /* Used to create a new co-ordinated system onto face. */
}Face;


/*
 *---------------------------------------------------------------
 * For sort array
 *---------------------------------------------------------------
 */
typedef struct tagTVertex
	{
		double x, y, z;
		char line[128];
	}TVertex;


/*
 *--------------------------------------------------------------
 * Vectors array
 * Added by Thompson Peter Lied at 04/03/2004 *
 *--------------------------------------------------------------
 */
typedef struct vectors{
	Point3D vBegin, vEnd;	/* vBegin its the initial point and 
	 vEnd its the final point of the vector */
	int faceIndex;			/* Index from the face where is the vector */
	
	/*add by Fabiane queiroz*/
	Param3D baryBegin, baryEnd; /*angle whit the abscissa axis */
	
	//Edited by Ricardo: Tava cell no lugar de vectors
	struct vectors *next;    /* pointer to the next vectors */
}VECTORARRAY;


#endif //_TYPES_H_
