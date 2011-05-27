/*
 *-----------------------------------------------------------
 *	util.h
 *-----------------------------------------------------------
 */


/* for the parsing of the growth data file */
#define	DATA	"data"
#define	SIZE	"size"
#define	COMMENT	"#"
#define	MONTH	0

/* local prototypes */
void saveCellsFile( const char *fileName );
int loadCellsFile( const char *inpFileName );
int *myIntMalloc( int quantity );
void readExpFile( const char *fileName );
float **allocate2DArrayFloat( int r, int c );
char **allocate2DArrayChar( int r, int c );
byte **allocate2DArrayByte( int r, int c );
