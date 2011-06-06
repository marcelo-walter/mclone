//extern "C"
//{
#include "interfacedistance.h"
//}

#include <stdio.h>

#include "surface.h"

#include "../data/Globals.h"
#include "../data/Object.h"

//Vertex 	*vert;
//Face	*faces;
//VECTORARRAY *vectors;

/*---VARIABLES---*/
// geodesic computation
int fini, fend;

int LenghtVetorGeodesic, PosicaoFinalVetorGeodesicPath;
VECTORGEODESICPATH *ArrayGeodesicPath;

/*
 OBS: O vetor ArrayGeodesicPath esta tendo sua memoria alocada, porem nao esta sendo liberada.
 PS: The vector ArrayGeodesicPath is reciving memory, but this memory is not being free.
 */

void ReallocMemory(int NewLenght)
{
	VECTORGEODESICPATH *old = ArrayGeodesicPath;
	
	LenghtVetorGeodesic = NewLenght;
	
	ArrayGeodesicPath = (VECTORGEODESICPATH *) realloc(old, sizeof(VECTORGEODESICPATH) * NewLenght);
}

void AllocMemory(int Lenght)
{
	LenghtVetorGeodesic = Lenght;
	
	ArrayGeodesicPath = (VECTORGEODESICPATH *) malloc(sizeof(VECTORGEODESICPATH) * Lenght);
}

//extern "C" void CreatePathDistances(char *NomeArquivo, int Inicio)
//extern "C" void CreatePathDistances(char *NomeArquivo)
void CreatePathDistances(char *NomeArquivo)
{	
	int i, Star, IndexFaceStart, IndexFaceEnd;
	int *fvert; //Ponteiro de inteiros.
	FILE *arqDistance;
	
	int NumVerticesPlusNumFace, NumFacesDot3;
	
	arqDistance = fopen(NomeArquivo,"r");
	
	NumVerticesPlusNumFace = (NumberVertices + NumberFaces); //With centroides.
	NumFacesDot3 = (NumberFaces * 3); //Because we take the face that exist and put 3 new.
	
	if (arqDistance == NULL )
	{
		printf("Creating the Geodesic Path! Wait a lot of minutes!\n\n");
		arqDistance = fopen(NomeArquivo,"w");
		
		Surface* S = new Surface(NumVerticesPlusNumFace, NumFacesDot3);
		
		for(i = 0; i < NumberVertices; i++)
		{
			S->addVertex(vert[i].pos.x, vert[i].pos.y, vert[i].pos.z);
		}
		
		for(i = 0; i < NumberFaces; i++)
		{
			S->addVertex(faces[i].center3D.x, faces[i].center3D.y, faces[i].center3D.z);
		}
		
		for( i = 0; i < NumberFaces; i++)
		{
			fvert = new int[3];
			
			fvert[0] = faces[i].v[0];
			fvert[1] = faces[i].v[1];
			fvert[2] = (NumberVertices + i); //Index of the vertices centroides.
			
			S->addFace(fvert, 3);
			delete[]fvert;
			
			fvert = new int[3];
			
			fvert[0] = faces[i].v[1];
			fvert[1] = faces[i].v[2];
			fvert[2] = (NumberVertices + i); //Index of the vertices centroides.
			
			S->addFace(fvert, 3);
			delete[]fvert;
			
			fvert = new int[3];
			
			fvert[0] = faces[i].v[2];
			fvert[1] = faces[i].v[0];
			fvert[2] = (NumberVertices + i); //Index of the vertices centroides.
			
			S->addFace(fvert, 3);
			delete[]fvert;
		}
		
		S->FinishToConstruct(NumVerticesPlusNumFace);
		
		IndexFaceStart = 0;
		//IndexFaceStart = 98;
		//IndexFaceStart = Inicio;
		
		//for(fini = NumberVertices; fini < NumVerticesPlusNumFace; fini++)
		for(fini = (NumberVertices + IndexFaceStart); fini < NumVerticesPlusNumFace; fini++)
		{
			Star = fini;
			Star++;
			
			IndexFaceEnd = (IndexFaceStart+1);
			for(fend = Star; fend < NumVerticesPlusNumFace; fend++)
			{
				S->ComputeGeodesic(fini, fend);
				
				for (i = 0; i < 40; i++)
				{
					for (int j = 0; j < 50; j++)
					{
						S->CorrectGeodesic();
					}
					if (S->geodesicerror() < EPS)
					{
						i = 2000;
					}
				}
				/*
				 printf("%d -> %d = %f %f %f => %f %f %f\n", IndexFaceStart, fini, faces[IndexFaceStart].center3D.x, faces[IndexFaceStart].center3D.y, faces[IndexFaceStart].center3D.z, S->returnXVertice(fini), S->returnYVertice(fini), S->returnZVertice(fini));
				 printf("%d -> %d = %f %f %f => %f %f %f\n", IndexFaceEnd, fend, faces[IndexFaceEnd].center3D.x, faces[IndexFaceEnd].center3D.y, faces[IndexFaceEnd].center3D.z, S->returnXVertice(fend), S->returnYVertice(fend), S->returnZVertice(fend));
				 printf("         %d %d (%d %d) %f\n", fini, fend, IndexFaceStart, IndexFaceEnd, S->GeodesicDistanceCalculated());
				 */
				fprintf(arqDistance, "%d %d %f\n", IndexFaceStart, IndexFaceEnd, S->GeodesicDistanceCalculated());
				
				IndexFaceEnd++;
			}
			
			IndexFaceStart++;
			
			printf("%d - ", IndexFaceStart);
		}
	}
	else { /*Read the file that have the geodesic distances before calculated */
		fprintf(stderr, "Loading Geodesic Path... ");
		
		AllocMemory((((NumberFaces-2) * NumberFaces) - (((NumberFaces-2) * (NumberFaces-1))/2))+1);
		PosicaoFinalVetorGeodesicPath = 0;
		
		while (!feof(arqDistance))
		{
			fscanf(arqDistance, "%d %d %f\n", &ArrayGeodesicPath[PosicaoFinalVetorGeodesicPath].Path1,
				   &ArrayGeodesicPath[PosicaoFinalVetorGeodesicPath].Path2,
				   &ArrayGeodesicPath[PosicaoFinalVetorGeodesicPath].Distance);
			PosicaoFinalVetorGeodesicPath++;
			
			/*if (PosicaoFinalVetorGeodesicPath == LenghtVetorGeodesic)
			 {
			 ReallocMemory((int)(LenghtVetorGeodesic * 0.5));
			 }*/
		}
		
		//printf("\nUltima posicao: %d (%d)\n", PosicaoFinalVetorGeodesicPath, LenghtVetorGeodesic);
		
		/*Just to see if the file are complet*/
		/*
		 printf("\n\n\tStarting verification...\n");
		 PosicaoFinalVetorGeodesicPath = 0;
		 
		 for(fini = 0; fini < NumberFaces; fini++)
		 {
		 Star = fini;
		 Star++;
		 
		 for(fend = Star; fend < NumberFaces; fend++)
		 {
		 if ((ArrayGeodesicPath[PosicaoFinalVetorGeodesicPath].Path1 != fini) ||	(ArrayGeodesicPath[PosicaoFinalVetorGeodesicPath].Path2 != fend))
		 {
		 printf("ERRO em : %d %d (%d %d) [%d]\n", fini, fend, ArrayGeodesicPath[PosicaoFinalVetorGeodesicPath].Path1, ArrayGeodesicPath[PosicaoFinalVetorGeodesicPath].Path2, PosicaoFinalVetorGeodesicPath);
		 exit(1);
		 }
		 PosicaoFinalVetorGeodesicPath++;
		 }
		 }
		 printf("\t...done\n\n");
		 */
	}
	
	fprintf(stderr, "...done.\n");
	fclose(arqDistance);
}
