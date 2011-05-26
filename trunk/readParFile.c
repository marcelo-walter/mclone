/*
 *---------------------------------------------------------
 *	readParFile.c
 *	This file has the routine which reads the parameter
 *	file <obj name>.par
 *---------------------------------------------------------
 */

#include <stdlib.h>

#include "common.h"

/*
 *	Prototypes
 */
void readParametersFile( const char *fileName );
void printParamForUser( void );

extern float 	adhes[HOW_MANY_TYPES][HOW_MANY_TYPES];
extern float 	swTypes[HOW_MANY_TYPES][HOW_MANY_TYPES];
extern char 	outputCMfileName[128];
extern char expFileName[128];
extern float 	finalTime, currentTime;
extern int 	updateTime;
/* Added by Thompson Peter Lied
 15/07/2002 */
extern double 	wa;
extern double 	wd, initWForce;
/* end */
extern int 	iterPerUnitTime, initNumRelax;
extern double 	wrp;
/* Added by Thompson Peter Lied at 31/10/2003 */
extern float orientation;

/* Added by Fabiane Queiroz in 15/11/2009 */

/*
 *----------------------------------------------------------
 *----------------------------------------------------------
 */
void
readParametersFile( const char *fileName )
{
	
	FILE *fp;
	double sumOfRatesOfSplitting;
	
	printf( "================================================\n" );
	printf( "\tReading parameters from file %s\n", fileName );
	printf( "================================================\n" );
	
	if( (fp = fopen( fileName, "r" )) == NULL )
		errorMsg("Could not open parameter file to read!");
	
	fscanf( fp,"%d %d\n", &iterPerUnitTime, &initNumRelax );
	fscanf( fp,"%lg\n", &wrp );
	fscanf( fp,"%d\n", &NumberCells );
	fscanf( fp,"%f %d %d %d %d\n", &finalTime, &pfMode, &RandomTypeDist, &neighborsFaceMode, &relaxMode );
	fscanf( fp,"%f %f %f %f\n",
		   &(Ortcell[C].prob),
		   &(Ortcell[D].prob),
		   &(Ortcell[E].prob),
		   &(Ortcell[F].prob));
	fscanf( fp,"%f %f %f %f\n",
		   &(Ortcell[C].rateOfDivision),
		   &(Ortcell[D].rateOfDivision),
		   &(Ortcell[E].rateOfDivision),
		   &(Ortcell[F].rateOfDivision) );
	/*fscanf(fp,"%lg %lg %lg %lg\n", &wForce, &wx, &wy, &initWForce ); */
	/* changed by Thompson Peter Lied in 15/07/2002 */
	fscanf(fp,"%lg %lg %lg\n", &wd, &wa, &initWForce );
	fscanf(fp,"%f %f %f %f\n",&adhes[C][C],&adhes[C][D],&adhes[C][E],&adhes[C][F]);
	fscanf(fp,"%f %f %f %f\n",&adhes[D][C],&adhes[D][D],&adhes[D][E],&adhes[D][F]);
	fscanf(fp,"%f %f %f %f\n",&adhes[E][C],&adhes[E][D],&adhes[E][E],&adhes[E][F]);
	fscanf(fp,"%f %f %f %f\n",&adhes[F][C],&adhes[F][D],&adhes[F][E],&adhes[F][F]);
	fscanf(fp,"%f %f %f %f\n",&swTypes[C][C],&swTypes[C][D],&swTypes[C][E],&swTypes[C][F]);
	fscanf(fp,"%f %f %f %f\n",&swTypes[D][C],&swTypes[D][D],&swTypes[D][E],&swTypes[D][F]);
	fscanf(fp,"%f %f %f %f\n",&swTypes[E][C],&swTypes[E][D],&swTypes[E][E],&swTypes[E][F]);
	fscanf(fp,"%f %f %f %f\n",&swTypes[F][C],&swTypes[F][D],&swTypes[F][E],&swTypes[F][F]);
	fscanf(fp,"%f %f %f\n", &(Ortcell[C].color[R]), &(Ortcell[C].color[G]), &(Ortcell[C].color[B]) );
	fscanf(fp,"%f %f %f\n", &(Ortcell[D].color[R]), &(Ortcell[D].color[G]), &(Ortcell[D].color[B]) );
	fscanf(fp,"%f %f %f\n", &(Ortcell[E].color[R]), &(Ortcell[E].color[G]), &(Ortcell[E].color[B]) );
	fscanf(fp,"%f %f %f\n", &(Ortcell[F].color[R]), &(Ortcell[F].color[G]), &(Ortcell[F].color[B]) );
	fscanf(fp,"%s\n", &outputCMfileName );
  	/* Added by Thompson Peter Lied at 31/10/2003 */
	fscanf(fp,"%f\n", &orientation);
	
	fclose(fp);
	
	strcpy( expFileName, outputCMfileName );
	
	currentTime = 0;
	
	sumOfRatesOfSplitting =
	Ortcell[C].rateOfDivision +
	Ortcell[D].rateOfDivision +
	Ortcell[E].rateOfDivision +
	Ortcell[F].rateOfDivision;
	
	/*
	 * Compute the relative rates
	 */
	Ortcell[C].relRate = 1.0 - (Ortcell[C].rateOfDivision / sumOfRatesOfSplitting);
	Ortcell[D].relRate = 1.0 - (Ortcell[D].rateOfDivision / sumOfRatesOfSplitting);
	
	Ortcell[E].relRate = Ortcell[E].rateOfDivision / sumOfRatesOfSplitting;
	Ortcell[F].relRate = Ortcell[F].rateOfDivision / sumOfRatesOfSplitting;
	
	/* inform the user about the parameters for this simulation */
	printParamForUser();
}
/*
 *----------------------------------------------------------
 *----------------------------------------------------------
 */
void
printParamForUser( void )
{
	
	printf("\n---------------------------------------------------------\n");
	printf("\t\tPARAMETERS FOR THIS SIMULATION\n");
	printf("---------------------------------------------------------\n");
	printf("Niterations = %d Initial Number of relaxations = %d Ncells = %d\n",
		   iterPerUnitTime, initNumRelax, NumberCells );
	printf("Weight for the repulsive radius = %3.4f\n", wrp );
	printf("Final Time = %3.2f\n", finalTime );
	printf("Growth(0) or not(1) = %d Type of Random Dist Poisson(1) Exp(2) = %d\n",
		   pfMode,  RandomTypeDist );
	printf("Secondary faces as neighbors: (0)No sec faces (1)StripFan (2)Turk = %d\n",
		   neighborsFaceMode );
	printf("Relaxation: (0)Global Relaxation (1)LocalRelaxation = %d\n",
		   relaxMode);
	/* printf("Weight for the relaxation force: Steady = %3.4f Initial = %3.4f wx = %3.4f wy = %3.4f\n",
	 wForce, initWForce, wx, wy ); */
	/* added by Thompson Peter Lied in 15/07/2002	*/
	printf("Weight for the relaxation force: Steady = %3.4f Initial = %3.4f wa = %3.4f \n",
		   wd, initWForce, wa );
	printf("Probabilities C = %3.4f D = %3.4f E = %3.4f F = %3.4f\n", Ortcell[C].prob,
		   Ortcell[D].prob, Ortcell[E].prob, Ortcell[F].prob);
	printf("Division C = %f D = %f E = %f F = %f\n", Ortcell[C].rateOfDivision,
		   Ortcell[D].rateOfDivision,
		   Ortcell[E].rateOfDivision,
		   Ortcell[F].rateOfDivision);
	printf("Adhesion CC = %2.2f CD = %2.2f CE = %2.2f CF = %2.2f\n",
		   adhes[C][C],adhes[C][D],adhes[C][E],adhes[C][F]);
	printf("Adhesion DC = %2.2f DD = %2.2f DE = %2.2f DF = %2.2f\n",
		   adhes[D][C],adhes[D][D],adhes[D][E],adhes[D][F]);
	printf("Adhesion EC = %2.2f ED = %2.2f EE = %2.2f EF = %2.2f\n",
		   adhes[E][C],adhes[E][D],adhes[E][E],adhes[E][F]);
	printf("Adhesion FC = %2.2f FD = %2.2f FE = %2.2f FF = %2.2f\n",
		   adhes[F][C],adhes[F][D],adhes[F][E],adhes[F][F]);
	printf("switch[C][C] = %2.2f [C][D] = %2.2f [C][E] = %2.2f [C][F] = %2.2f\n",
		   swTypes[C][C],swTypes[C][D],swTypes[C][E],swTypes[C][F]);
	printf("switch[D][C] = %2.2f [D][D] = %2.2f [D][E] = %2.2f [D][F] = %2.2f\n",
		   swTypes[D][C],swTypes[D][D],swTypes[D][E],swTypes[D][F]);
	printf("switch[E][C] = %2.2f [E][D] = %2.2f [E][E] = %2.2f [E][F] = %2.2f\n",
		   swTypes[E][C],swTypes[E][D],swTypes[E][E],swTypes[E][F]);
	printf("switch[F][C] = %2.2f [F][D] = %2.2f [F][E] = %2.2f [F][F] = %2.2f\n",
		   swTypes[F][C],swTypes[F][D],swTypes[F][E],swTypes[F][F]);
	printf("Color for C cells: [R] = %f [G] = %f [B] = %f\n",
		   Ortcell[C].color[R], Ortcell[C].color[G], Ortcell[C].color[B] );
	printf("Color for D cells: [R] = %f [G] = %f [B] = %f\n",
		   Ortcell[D].color[R], Ortcell[D].color[G], Ortcell[D].color[B] );
	printf("Color for E cells: [R] = %f [G] = %f [B] = %f\n",
		   Ortcell[E].color[R], Ortcell[E].color[G], Ortcell[E].color[B] );
	printf("Color for F cells: [R] = %f [G] = %f [B] = %f\n",
		   Ortcell[F].color[R], Ortcell[F].color[G], Ortcell[F].color[B] );
	printf("Output CM file: %s\n", outputCMfileName );
	printf("Orientation: %f \n", orientation);
}
