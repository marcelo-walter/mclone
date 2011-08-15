/*
 *	menus.h
 */

#ifndef _MENUS_H_
#define _MENUS_H_

/*
 *--------------------------------------------------
 *	Local Global variables
 *--------------------------------------------------
 */
/* menu identifiers */
extern int mainMenuIdent, primitiveMenuIdent, printMenuIdent;
extern int dispMenuIdent, viewMenuIdent, renderMenuIdent;
extern int vectorMenuIdent;
extern int savingMenuIdent;

extern int typeDistanciaMenuIdent;
extern int typeVectorFieldMenuIdent;


/*
 *--------------------------------------------------
 *	Local Prototypes
 *--------------------------------------------------
 */
void mainMenu(int item);
void primitiveMenu( int item );
void dispMenu( int item );
void typeDistance( int item );
void viewMenu( int item );
void initMenus( void );
void printMenu(int item);
void renderMenu(int item);
void saveInventorFile( void );
void vectorFieldMenu(int item); /* Added by Thompson Peter Lied at 03/12/2003 */
void savingMenu(int item); /* Added by Thompson Peter Lied at 08/03/2004 */

#endif //_MENUS_H_
