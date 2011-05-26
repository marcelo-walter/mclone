/*
 *---------------------------------------------------------
 *
 *	readInpFile.c
 *	Marcelo Walter
 *	This file has the routine which reads an .obj
 *	file
 *---------------------------------------------------------
 */

#include <string.h>
#include <ctype.h>

#include "common.h"
#include "readInpFile.h"

/*
 *--------------------------------------------------
 *	Local Global variables
 *--------------------------------------------------
 */ 
Vertex 	*vert;
Point3D *vertDisplay;
Point3D *origVert;
Point3D *vertn;
Point2D *vertt;
Face	*faces;
VECTORARRAY	*vectors;
Edge 	*edges;

/*add by Fabiane Queiroz*/
//VECTORARRAY *vectors;


/*
 *--------------------------------------------------
 * auxiliary routine for 'readObject'
 *--------------------------------------------------
 */
int myatoi(char **p)
{
    int j=0;
    char num[15];
	
    while (isdigit (**p)){
		num[j++] = **p;
		*p += 1;
    }
    num[j] = '\0';
    return atoi(num);
}
/*
 *-------------------------------------------------------
 *	This routine reads an object file in wavefront
 *	format (*.obj). The format is something like:
 *
 *	# This is a comment
 *	# This is a vertex specification
 *	v 0.1 0.1 0.1
 *
 *	# This is a normal vertex specification
 *	vn 0.1 0.1 0.1
 *
 *	# This is a texture vertex specification
 *	vt 0.1 0.2
 *
 *	# This is a face specification with 3 vertices
 *	f 1/1/1 2/2/3 3/4/2
 *	# This is a face specification with 4 vertices
 *	f 1/2/3 2/3/1 3/4/5 4/6/78
 *
 *	The format is:
 *	vertex/texture vertex/normal vertex
 *	
 *	Possible combinations:
 *	1 2 3 4			-> only vertices
 *	1//1 2//2 3//3 4//4	-> vertices and normal vertices
 *	1/1 2/2 3/3 4/4		-> vertices and texture vertices
 *	
 *	To find the number of vertices, normal vertices, texture
 *	vertices and polygons we first read the whole file
 *	and count the number of times the letters "v", "vn", 
 *	"vt" and "f" appear. We then allocate space for the
 *	exact necessary number of elements.
 *
 *	MARCELO REMEMBER: This is different from the previous
 *	version. In the previous version I was looking in the
 *	object file (.obj) for lines starting with the pound sign (#)
 *	which means that the line is a comment. It seems
 *	that the original .obj file from Wavefront used to include
 *	as a comment the total number of all elements (vertices, 
 *	normal vertices,  texture vertices and polygons).
 *      On the other hand, (.obj) files saved from inside Alias
 *	do not include these comment lines,  therefore I am reading
 *	the whole file first in order to gather the information
 *	on how many elements there are in the file.
 *	
 *	The previous main.c file is the one which reads objects
 *	in the old style
 *
 *------------------------------------------------------------------
 */
void readObject(char *name)
{
	FILE 	*fp;
	char 	input[255];
	char  token[25][50];
	
	int	currentVertices = 0;
	int	currentTextureVertices = 0;
	int	currentNormals = 0;
	int	currentPolygons = 0;
	int	i,j,k;
	char *tmp, group[10], oneChar;
	
	if( (fp=fopen(name,"rb")) == NULL ) errorMsg("Could not open input file to read!");
    
	/* check the number of vertices, texture vertices, normals and
     polygons */
	
	while (fscanf (fp, "%s", input) != EOF){
		
		if (!strcmp (input, VERTEX))
			NumberVertices++;
		
		else if (!strcmp (input, VERTEX_N))
			NumberNormals++;
		
		else if (!strcmp (input, POLYGON)){
			NumberFaces++;
		NumberVectors++;		}
			
			
		
		else if (!strcmp (input, VERTEX_T))
			NumberTextureVertices++;
	}
	
	/* once I have the number of faces and vertices
	 * I can compute the number of edges according
	 * to Euler's formula. This is a first estimate
	 * For reasons related to how the object is
	 * built, it can happen that the actual number
	 * of edges is slightly larger or smaller than this
	 * I am fixing the actual value for NumberEdges
	 * later when I compute the wing edge data structure
	 */
	
	NumberEdges = NumberFaces + NumberVertices - 2;
	
	fprintf(stderr, "===========================\n");
	fprintf(stderr, "    Geometric Information  \n");
	fprintf(stderr, "===========================\n");
	
	fprintf (stderr, "%d vertices\n", NumberVertices);
	fprintf (stderr, "%d normals\n", NumberNormals);
	fprintf (stderr, "%d polygons\n", NumberFaces);
	fprintf (stderr, "%d possible edges\n", NumberEdges);
	fprintf (stderr, "%d texture vertices\n", NumberTextureVertices);
	
	/* go back at beggining of file */
	rewind(fp);
	
	/* allocate space for vertices, parameterized vertices, normal vertices,
     texture vertices, and faces */
	vert  = (Vertex *) malloc (sizeof (Vertex) * (NumberVertices));
	if (vert == NULL) errorMsg("Problem with malloc in readObj! (vert)");
	
	origVert = (Point3D *) malloc (sizeof (Point3D) * (NumberVertices));
	if ( origVert == NULL) errorMsg("Problem with malloc in readObj! (origVert)");
	
	vertDisplay = (Point3D *) malloc (sizeof (Point3D) * (NumberVertices));
	if (vertDisplay == NULL) errorMsg("Problem with malloc in readObj! (vertDisplay)");
    
	if ( NumberNormals != 0 ){
		vertn = (Point3D *) malloc (sizeof (Point3D) * (NumberNormals));
		if (vertn == NULL) errorMsg("Problem with malloc in readObj (vertn)!");
	}
	
	/* I'm assigning the texture coordinates myself */
	/* if (numberTextureVertices != 0)
	 vertt = (Point2D *) malloc (sizeof (Point2D) * (numberTextureVertices)); */
	NumberTextureVertices = NumberVertices;
	vertt = (Point2D *) malloc (sizeof (Point2D) * (NumberVertices));
	
	if (vertt == NULL) errorMsg("Problem with malloc in readObj (vertt) !");
	
	faces = (Face   *) malloc (sizeof (Face) * (NumberFaces));
	if (faces == NULL) errorMsg("Problem with malloc in readObj (faces)!");
	

	
	/*add by Fabiane Queiroz*/
	vectors = (VECTORARRAY  *) malloc (sizeof (VECTORARRAY) * (NumberFaces));
	if (vectors == NULL) errorMsg("Problem with malloc in readObj (vectors)!");
	
	currentVertices = 0;
	currentPolygons = 0;
	currentNormals  = 0;
	currentTextureVertices = 0;
	while (fscanf (fp, "%s", input) != EOF){
		/* read vertices */
		
		if (!strcmp (input, VERTEX)){
			fscanf(fp, "%lg %lg %lg",
				   &vert[currentVertices].pos.x,
				   &vert[currentVertices].pos.y,
				   &vert[currentVertices].pos.z);
			origVert[currentVertices].x = vert[currentVertices].pos.x;
			//printf("x: %lf ",origVert[currentVertices].x);
			origVert[currentVertices].y = vert[currentVertices].pos.y;
			// printf("y: %lf ",origVert[currentVertices].y);
			origVert[currentVertices].z = vert[currentVertices].pos.z;
			// printf("z: %lf\n ",origVert[currentVertices].z);
			currentVertices++;
			
			/* Added by Thompson at 09/12/2003 */
			/* Initialize the vector's direction */
			//vert[currentVertices].dir.x = 0.0;
			//vert[currentVertices].dir.y = 0.0;
			//vert[currentVertices].dir.z = 0.0;
			//printf("x: %lf ",origVert[currentVertices].x);
			//printf("y: %lf ",origVert[currentVertices].y);
			//printf("z: %lf\n ",origVert[currentVertices].z);
			//int l;
			//for (l = 0; l < NumberVertices; l++)
			//printf("\ntestando: %lf, %lf, %lf\n", origVert[l].x, origVert[l].y, origVert[l].z); 
		}
		
		
		/* read groups */
		else if (!strcmp (input, GROUP)){
			fscanf (fp, "%c", &oneChar);
			if (oneChar != '\n') fscanf(fp, "%s", &group);
		}
		
		/* read normal vertices */
		else if (!strcmp (input, VERTEX_N)){
			fscanf(fp, "%lg %lg %lg",
				   &vertn[currentNormals].x,
				   &vertn[currentNormals].y,
				   &vertn[currentNormals].z);
			currentNormals++;
		}
		/* read texture vertices */
		else if (!strcmp (input, VERTEX_T)){
			/* Even though I'm reading the texture vertices here
			 I'll overwrite them later on, at the end of this
			 function */
			/* texture vertices are 2D !!!! */
			fscanf(fp, "%lg %lg",
				   &vertt[currentTextureVertices].x,
				   &vertt[currentTextureVertices].y);
			currentTextureVertices++;
		}
		/* read faces */
		else if (!strcmp (input, POLYGON) || !strcmp (input, POLYGON_O)){
			strcpy(faces[currentPolygons].group, group);
			fgets (input, 255, fp);
			tmp = input;
			while ((*tmp == ' ') || (*tmp == '\r')) tmp++;
			i = 0; j = 0;
			while (*tmp != '\n') {
				while ((*tmp != ' ')&& (*tmp != '\r') && (*tmp != '\n')) { //Adicionei o \r by BINS
					token[i][j++] = *(tmp++);
				}
				token[i][j] = '\0';	/* make each token a string */
				while ((*tmp == ' ') || (*tmp == '\r')) tmp += 1;
				i++; j = 0;
			}
			/* i has the number of tokens in the line */
			faces[currentPolygons].v = (int *) malloc (sizeof (int) * i);
			
			/* I want to allocate the space for normals and texture info
			 dynamically. The only assumption is that if a face has
			 normal and/or texture information, the number of normals specified
			 and/or texture vertices is THE SAME as the number of geometry
			 vertices specified. I don't think this assumption introduces
			 any problem...
			 */
			if ( NumberNormals != 0 )
				faces[currentPolygons].vn = (int *) malloc (sizeof (int) * i);
			
			if ( NumberTextureVertices != 0 )
				faces[currentPolygons].vt = (int *) malloc (sizeof (int) * i);
			
			for (k = 0; k < i; k++) {
				
				/* we know for sure that the first token will ALWAYS be
				 a geometry vertex */
				tmp = token[k];
				faces[currentPolygons].v[k] = myatoi(&tmp)-1;
				faces[currentPolygons].nverts = i;
				if ( faces[currentPolygons].nverts != 3 )
					errorMsg("I can only deal with triangles!" );
				
				switch (*tmp) {
					case '\0': 
					case ' ':
						break;
					case '/':
						/* at this point we have also either normal or texture
						 information specified */
						if (*(tmp+1) == '/') { /* read normal */
							tmp++; tmp++;
							faces[currentPolygons].vn[k] = myatoi(&tmp)-1;
						}else{               /* read texture */
							tmp++;
							faces[currentPolygons].vt[k] = myatoi(&tmp)-1;
							switch(*tmp){
								case '\0': case ' ': 
									break;
								case '/':	/* read normal */
									tmp++;
									faces[currentPolygons].vn[k] = myatoi(&tmp)-1;
									break;
								default:
									break;
							} 
						}
						break;
					default:
						break;
				}
			}	
			currentPolygons++;
		}
	}
	fclose(fp);
	
	/* fix the texture information. If the original file does not
     have texture information, I am assigning the same geometric
     indexes to the textures
	 */
	/* if (numberTextureVertices == 0){ */
    for( i=0; i < NumberFaces; i++){
		for ( j=0; j < faces[i].nverts; j++){
			faces[i].vt[j] = faces[i].v[j];
			vertt[faces[i].vt[j]].x = -1;
			vertt[faces[i].vt[j]].y = -1;
		}
    }
    
    
    /*} */
}
