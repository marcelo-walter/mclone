/*
 *--------------------------------------------------------
 *	PatternObject.c
 *	Routines related with the object and material
 *	files to write a .obj and .mtl format file
 *--------------------------------------------------------
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "../common.h"
//#include "relax.h"
//#include "texture.h"


/*
 *------------------------------------------------------------
 *	External definitions
 *------------------------------------------------------------
 */
extern flag     voronoiComputed;


/*
 *------------------------------------------------------------
 *  Global definitions
 *------------------------------------------------------------
 */

int mtl_flag = TRUE;
int opt_flag = FALSE;
char mtl_file[256];     /* mtl file name */
long int vsize, total_vertices;


long int *exchange = NULL;
long int *sort = NULL;
TVertex *vertex = NULL;
TVertex  *vertexNew = NULL;



/*---------------------*/
void savePatternFile( const char *filename );
void saveMtl( const char *mtl_file );

int CmpVertex(const void *elem0, const void *elem1);
void optimizer( FILE *fp );
void saveVertices( FILE *fpObj );
void saveFaces( FILE *fpObj );





/*
 *------------------------------------------------------------
 *	Writes the pattern file and material library
 *  in .obj format.
 *
 *  Added by Thompson Peter Lied in 23/08/2002
 *------------------------------------------------------------
 */
void savePatternFile( const char *filename )
{
    FILE *fpObj;
    int i, j, k, len, whichFace, nVorVerts, faceIndex;
	char outObjFileName[128] = "";
	char mtl_file[256];     /* mtl file name */
    Point3D q;
	
	int vert_index;
	CELL *c;
	CELL_TYPE current_type = C;
	
	
	
	fprintf( stderr, "\n" );
	
    if ( !voronoiComputed )
        errorMsg("There is no voronoi computed to save as an Object file!");
	
    /* get rid of the obj.session*/
    len = strlen( filename );
    strncpy( outObjFileName, filename, len - 3);
    outObjFileName[len - 3]='\0';
    strcat( outObjFileName, "Pat.obj" );
	
	/* Save mtl file */
	strncpy( mtl_file, filename, len - 3);
	mtl_file[len-3]='\0';
    strcat( mtl_file, ".mtl" );
	saveMtl( mtl_file );
    /* */
	
	fprintf( stderr, "Saving the pattern file into file %s... ", outObjFileName );
	
    /* open the file to write */
    if( (fpObj = fopen(outObjFileName,"w")) == NULL)
        errorMsg("Could not open inventor file to write!");
	
    fprintf( fpObj, "\n \n               \t\t\t\t\t\t\t\t \n"); /* linhas em branco para o rewind */
    fprintf( fpObj, "# Object saved from the 'onca' program \n");
    fprintf( fpObj, "# Thompson Peter Lied and Marcelo Walter \n" );
    fprintf( fpObj, "#\t{tlied|marcelow}@exatas.unisinos.br \n\n");
	
	fprintf( fpObj, "mtllib %s \n\n", mtl_file );
	
	
	//	if( opt_flag == FALSE )
	//	{
	/* Optimize vertices */
	/*		optimizer( fpObj );
	 opt_flag = TRUE;
	 }
	 */
	/* Save vertices info */
	//	saveVertices( fpObj );
	
	
	/* Save faces info */
	//	saveFaces( fpObj );
	
	
	
	/******************************/
	vert_index = 1;
	
    /* Writes vertices info */
	
    fprintf(fpObj, "# Vertices info\n");
    for( whichFace = 0; whichFace < NumberFaces; whichFace++)
    {
        for ( i = 0; i < faces[whichFace].nVorPol; i++)
	    {
            nVorVerts = faces[whichFace].vorFacesList[i].faceSize;
            for (j = 0; j < nVorVerts; j++)
		    {
	            k = faces[whichFace].vorFacesList[i].vorPts[j];
				faces[whichFace].vorFacesList[i].vorPtsChanged[j] = faces[whichFace].vorFacesList[i].vorPts[j];
				
	    	    mapFromPolySpace(whichFace, faces[whichFace].vorPoints[k].x,
								 faces[whichFace].vorPoints[k].y, &q);
	            fprintf( fpObj, "v %f %f %f \n", q.x, q.y, q.z);
				
                faces[whichFace].vorFacesList[i].vorPtsChanged[j] = vert_index;
				vert_index++;
            }
        }
    }
    fprintf(fpObj, "# %d vertices \n\n", vert_index );
	
	
	
    /* Writes faces info */
    fprintf(fpObj, "# Faces info\n");
    /* faceIndex = 0; */

	for( whichFace = 0; whichFace < NumberFaces; whichFace++)
    {
        for ( i = 0; i < faces[whichFace].nVorPol; i++)
	    {
			//* definiÁ„o das cores
			c = faces[whichFace].vorFacesList[i].site;
	        if( current_type != c->ctype )
	        {
                switch( c->ctype )
		        {
		            case C:
				        current_type = C;
		                fprintf( fpObj, "g cellCmtl\n" );
			            fprintf( fpObj, "usemtl cellCmtl\n" );
			            break;
						
                    case D:
					    current_type = D;
		                fprintf( fpObj, "g cellDmtl\n" );
		                fprintf( fpObj, "usemtl cellDmtl\n" );
			            break;
						
			        case E:
					    current_type = E;
			            fprintf( fpObj, "g cellEmtl\n" );
		                fprintf( fpObj, "usemtl cellEmtl\n" );
				        break;
						
			        case F:
					    current_type = F;
			            fprintf( fpObj, "g cellFmtl\n" );
		                fprintf( fpObj, "usemtl cellFmtl\n" );
				        break;
						
						/* This has to be fixed. At this point it means
						 that this cell doesn't have a type assigned to
						 it */
			        default:
					    current_type = C;
                        fprintf( fpObj, "g cellCmtl\n" );
			            fprintf( fpObj, "usemtl cellCmtl\n" );
			            break;
				}
			}

			nVorVerts = faces[whichFace].vorFacesList[i].faceSize;
			
			fprintf( fpObj, "f " );
			for (j = 0; j < nVorVerts; j++)
		    {
			    k = faces[whichFace].vorFacesList[i].vorPtsChanged[j];
                fprintf( fpObj, "%d ", k );
				
			    //fprintf( fp, "%d ", faceIndex);
	            //faceIndex++;
		    }
            fprintf( fpObj, "\n");
        }
    }
    fprintf( fpObj, "# %d faces \n\n", NumberFaces );
	
	
	//rewind( fp );
	//fprintf( fp, "# %d vertices \n", vert_index );
    //fprintf( fp, "# %d faces \n", NumberFaces );

    fclose( fpObj );
	
	fprintf( stderr, "Ok \n" );
}



/*
 ************************************************************
 *
 *  Material files (.mtl)
 *
 *  FORMAT:
 *
 *  The following is a sample format for a single material
 * from an .mtl file:
 *
 *      newmtl my_mtl
 *      Ka 0.0435 0.0435 0.0435
 *      Kd 0.1086 0.1086 0.1086
 *      Ks 0.0000 0.0000 0.0000
 *      Tf 0.9885 0.9885 0.9885
 *      d 0.6600
 *      Ns 10.0000
 *      Ni 1.19713
 *      illum 6
 *
 *
 *  SYNTAX:
 *
 *  The following syntax statements describe the identifiers
 * that apply to .mtl files.
 *
 *  d factor
 *      facto is the amount this material dissolves into the
 *      background. A factor 1.0 is fully opaque (default when
 *      a new material is created). A factor of 0.0 is fully
 *      dissolved (completely transparent).
 *
 *
 *
 *
 *
 *
 *
 *
 ************************************************************
 */
void saveMtl( const char *mtl_file )
{
    int number_of_vert, i, j, k, l;
    int cont, length;
	double d[XY];
	FILE *fpMtl;
	
	
	if(( fpMtl = fopen( mtl_file, "w" )) == NULL )
	{
        printf( "Could not open Material Library ( %s ) to write! \n", mtl_file );
        exit(1);
    }
	
	fprintf( stderr, "Saving the pattern file into file %s... ", mtl_file );
	
	
    fprintf( fpMtl, "# \n" );
	fprintf( fpMtl, "# Wavefront material file \n" );
    fprintf( fpMtl, "# \n" );
	fprintf( fpMtl, "# Saved by OncaViewer \n" );
	fprintf( fpMtl, "# More info, mail to \n" );
	fprintf( fpMtl, "# \t {marcelow|tlied}@exatas.unisinos.br \n" );
	fprintf( fpMtl, "# \n" );
	
    /* global material */
    fprintf( fpMtl, "newmtl MAT_999C94 \n" );
	fprintf( fpMtl, "\tKa 0.117647 0.117647 0.117647 \n" );
	fprintf( fpMtl, "\tKd 0.752941 0.752941 0.752941 \n" );
	fprintf( fpMtl, "\tKs 0.752941 0.752941 0.752941 \n" );
	/* fprintf( fpMtl, "\tTf 0.988500 0.988500 0.988500 \n" ); */
    /* fprintf( fpMtl, "\td 0.6600 \n" ); */
	fprintf( fpMtl, "\tNs 8 \n" );
	/* fprintf( fpMtl, "\tNi 1.19713 \n" ); */
	fprintf( fpMtl, "\tillum 0 \n" );
	fprintf( fpMtl, "\n" );
	
	
	/* material for cell C */
	fprintf( fpMtl, "newmtl cellCmtl \n" );
    fprintf( fpMtl, "\tKa 0.117647 0.117647 0.117647 \n" );
	fprintf( fpMtl, "\tKd %f %f %f \n", Ortcell[C].color[R],
			Ortcell[C].color[G], Ortcell[C].color[B] );
	fprintf( fpMtl, "\tKs %f %f %f \n", Ortcell[C].color[R],
			Ortcell[C].color[G], Ortcell[C].color[B] );
	fprintf( fpMtl, "\tNs 8 \n" );
	fprintf( fpMtl, "\tillum 0 \n" );
	fprintf( fpMtl, "\n" );
	
	
	/* material for cell D */
	fprintf( fpMtl, "newmtl cellDmtl \n" );
	fprintf( fpMtl, "\tKa 0.117647 0.117647 0.117647 \n" );
    fprintf( fpMtl, "\tKd %f %f %f \n", Ortcell[D].color[R],
			Ortcell[D].color[G], Ortcell[D].color[B] );
    fprintf( fpMtl, "\tKs %f %f %f \n", Ortcell[D].color[R],
			Ortcell[D].color[G], Ortcell[D].color[B] );
	fprintf( fpMtl, "\tNs 8 \n" );
	fprintf( fpMtl, "\tillum 0 \n" );
	fprintf( fpMtl, "\n" );
	
	
    /* material for cell E */
	fprintf( fpMtl, "newmtl cellEmtl \n" );
	fprintf( fpMtl, "\tKa 0.117647 0.117647 0.117647 \n" );
    fprintf( fpMtl, "\tKd %f %f %f \n", Ortcell[E].color[R],
			Ortcell[E].color[G], Ortcell[E].color[B] );
    fprintf( fpMtl, "\tKs %f %f %f \n", Ortcell[E].color[R],
			Ortcell[E].color[G], Ortcell[E].color[B] );
	fprintf( fpMtl, "\tNs 8 \n" );
	fprintf( fpMtl, "\tillum 0 \n" );
	fprintf( fpMtl, "\n" );
	
	
	/* material for cell F */
	fprintf( fpMtl, "newmtl cellFmtl \n" );
	fprintf( fpMtl, "\tKa 0.117647 0.117647 0.117647 \n" );
    fprintf( fpMtl, "\tKd %f %f %f \n", Ortcell[F].color[R],
			Ortcell[F].color[G], Ortcell[F].color[B] );
    fprintf( fpMtl, "\tKs %f %f %f \n", Ortcell[F].color[R],
			Ortcell[F].color[G], Ortcell[F].color[B] );
	fprintf( fpMtl, "\tNs 8 \n" );
	fprintf( fpMtl, "\tillum 0 \n" );
	fprintf( fpMtl, "\n" );
	fprintf( fpMtl, "\n" );
	
	
    fclose( fpMtl );
	
	fprintf( stderr, "Ok \n" );
	
}






/*-----------------------------------------------------------------
 *
 * Added by Thompson Peter Lied in 03/01/2003
 *
 *-----------------------------------------------------------------*/

/*
 * Compare vertex -> qsort
 */
int CmpVertex(const void *elem0, const void *elem1)
{
    int *val0=(int *)elem0;
    int *val1=(int *)elem1;
	
    if (vertex[*val0].x>vertex[*val1].x)
    {
        return -1;
    }
    else if (vertex[*val0].x<vertex[*val1].x)
    {
        return 1;
    }
    else
    {
        if (vertex[*val0].y>vertex[*val1].y)
        {
            return -1;
        }
        else if (vertex[*val0].y<vertex[*val1].y)
        {
            return 1;
        }
        else
        {
            if (vertex[*val0].z>vertex[*val1].z)
            {
                return -1;
            }
            else if (vertex[*val0].z<vertex[*val1].z)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }
}



/*
 *  Optimize vertices
 */
void optimizer( FILE *fp )
{
	int i, j, k, l;
	int nVorVerts, whichFace;
	int cont, length;
	double d[XY];
	long int countNew;
    int newIndex;
	Point3D q;
	
	
	vsize = 0;
	for( whichFace = 0; whichFace < NumberFaces; whichFace++ )
    {
        for ( i = 0; i < faces[whichFace].nVorPol; i++)
	    {
            nVorVerts = faces[whichFace].vorFacesList[i].faceSize;
            for (j = 0; j < nVorVerts; j++)
		    {
				faces[whichFace].vorFacesList[i].vorPtsChanged[j] = faces[whichFace].vorFacesList[i].vorPts[j];
                faces[whichFace].vorFacesList[i].vorPtsChanged[j] = vsize;
				vsize++;
            }
        }
    }
	
	
    vertex = malloc(sizeof(TVertex)*vsize);
	
	
	l = 0;
    for( whichFace = 0; whichFace < NumberFaces; whichFace++)
    {
        for ( i = 0; i < faces[whichFace].nVorPol; i++)
	    {
            nVorVerts = faces[whichFace].vorFacesList[i].faceSize;
            for (j = 0; j < nVorVerts; j++)
		    {
	            k = faces[whichFace].vorFacesList[i].vorPts[j];
				
	    	    mapFromPolySpace(whichFace, faces[whichFace].vorPoints[k].x,
								 faces[whichFace].vorPoints[k].y, &q);
				vertex[l].x = q.x;
				vertex[l].y = q.y;
				vertex[l].z = q.z;
				
				l++;
            }
        }
    }
	
	
	// Make ordenation array
    sort = malloc( sizeof(long int)*vsize );
    for ( i = 0; i < vsize; i++)
    {
        sort[i] = i;
		//printf("Sort[i] = %d  i = %d \n", sort[i], i);
    }
	
    //fprintf( stderr, "\nSorting sort array... " );
    qsort( sort, vsize, sizeof(long int), CmpVertex );
    //fprintf( stderr, "Ok \n" );
	
	
    /* Optimizing sort array and creating exchange table */
    exchange = malloc(sizeof(long int)*vsize);
    countNew = vsize;
    exchange[sort[0]] = 0;
    //fprintf(stderr, "Optimizing sort array and creating exchange table... ");
    for ( i = 1; i < vsize; i++)
    {
        if( (vertex[sort[i-1]].x == vertex[sort[i]].x) &&
		   (vertex[sort[i-1]].y == vertex[sort[i]].y) &&
		   (vertex[sort[i-1]].z == vertex[sort[i]].z) )
        {
            exchange[sort[i]] = exchange[sort[i-1]];
	        sort[i] = sort[i-1];
            countNew--;
        }
        else
        {
            exchange[sort[i]] = exchange[sort[i-1]]+1;
        }
		//fprintf(stderr, "%d  %ld  %d  %d\n", i, exchange[i], sort[i], exchange[sort[i]]);
    }
	//fprintf( stderr, "Ok \n" );
	
	
    /* Generates new array of vertices */
    vertexNew = malloc(sizeof(TVertex)*countNew);
	
	
    vertexNew[0] = vertex[sort[0]];
	j = 1;
    for( i = 1; i < vsize; i++)
    {
        if (sort[i-1] != sort[i])
        {
            vertexNew[j] = vertex[sort[i]];
			j++;
        }
    }
	total_vertices = j;
	
}


/*
 *  Save Vertices
 */
void saveVertices( FILE *fpObj )
{
    int j;
	
    for( j = 0; j < total_vertices; j++)
    {
		fprintf( fpObj, "v %f %f %f \n", vertexNew[j].x, vertexNew[j].y, vertexNew[j].z );
    }
	
	fprintf( fpObj, "# %d vertices \n", total_vertices );
	fprintf( fpObj, "\n" );
	
}



/*
 *  Save Faces
 */
void saveFaces( FILE *fpObj )
{
    int i, j, k;
	int whichFace, nVorVerts;
	int newIndex;
	int nFaces;
	CELL *p;
	CELL_TYPE current_type = C;
	
	
	fprintf( fpObj, "# Faces info\n");
	
	nFaces = 0;
	
	/* Save faces info */
	for( whichFace = 0; whichFace < NumberFaces; whichFace++)
    {
        for ( i = 0; i < faces[whichFace].nVorPol; i++)
	    {
            nVorVerts = faces[whichFace].vorFacesList[i].faceSize;
			
			/* Color definitions */
			p = faces[whichFace].vorFacesList[i].site;
			
			//if( mtl_flag )
			if( current_type != p->ctype )
			{
				switch( p->ctype )
				{
					case C:
						current_type = C;
						fprintf( fpObj, "g cellCmtl\n" );
						fprintf( fpObj, "usemtl cellCmtl\n" );
						break;
						
					case D:
						current_type = D;
						fprintf( fpObj, "g cellDmtl\n" );
						fprintf( fpObj, "usemtl cellDmtl\n" );
						break;
						
					case E:
						current_type = E;
						fprintf( fpObj, "g cellEmtl\n" );
						fprintf( fpObj, "usemtl cellEmtl\n" );
						break;
						
					case F:
						current_type = F;
						fprintf( fpObj, "g cellFmtl\n" );
						fprintf( fpObj, "usemtl cellFmtl\n" );
						break;
						
					default:
						current_type = C;
						fprintf( fpObj, "g cellCmtl\n" );
						fprintf( fpObj, "usemtl cellCmtl\n" );
						break;
				}
			}
			
			fprintf( fpObj, "f " );
			for( j = 0; j < nVorVerts; j++)
			{
				k = faces[whichFace].vorFacesList[i].vorPtsChanged[j];
				newIndex = exchange[k] + 1;
				
				fprintf( fpObj, "%d ", newIndex );
			}
			fprintf( fpObj, "\n");
			nFaces++;
		}
		
    }
	fprintf( fpObj, "# %d faces \n", nFaces );
	
	
}
