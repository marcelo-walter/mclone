/*
 *  interfacedistance.h
 *  Mclone3D
 *
 *  Created by Fabiane Queiroz on 6/7/10.
 *  Copyright 2010 UFPE. All rights reserved.
 *
 */

/*
 *--------------------------------------------------------------
 * Added by Vinicius Mello at 22/03/2005
 * Used to compare between straight and geodesic path.
 *--------------------------------------------------------------
 */
typedef struct GeodesicDistance
	{
		int Path1, Path2;
		float Distance;
	} VECTORGEODESICPATH;

void ReallocMemory(int NewLenght);
void AllocMemory(int Lenght);
//void CreatePathDistances(char *NomeArquivo, int Inicio);
void CreatePathDistances(char *NomeArquivo);
