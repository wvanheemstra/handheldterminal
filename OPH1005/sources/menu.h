//
// menu.h
//
// header file of menu functions for the
// Opticon barcode terminals
//
// Martin Jansen
//
// The ShowTextMenu menu is based on xmenu by Simon Brouwer
// The ShowTextSelectionMenu is based on selmenu by Oscar Jacobse
//
// For questions:
// email:	support@opticon.com
// web:		www.opticon.com
//
// 23/06/2005:	Added the graphical menu items, Functions InitGraphMenu, 
//				ShowGraphMenu and ShowGraphSelection menu are added.
// 				Changed the defined OPTION values from format 
//				TXT_MENU_XXXXX to MENU_XXXXX. The stxtSelMenu structure
//				name is changed to sSelMenu because it is also used by the 
//				graphical selection menu.
//
// 23/11/2005:	Fixed a multiple selection problem when return -1L value had also an * 
//				selection sign.
//


#ifndef __MENU_HEADER__
#define __MENU_HEADER__

//-----------------------------------------------------------------------------
// IMPORTANT!!!!
//
// This menu makes use of the Input.h and Input.c files. These files can alse be 
// found in the snippet section of the compiled help manual
//
//-----------------------------------------------------------------------------

//
// Some special define options
//
#define	MENU_FIXED		0x00		// Go from min to max and from max to min
#define	MENU_LOOP		0x01		// Go beyond Max to reach min and vice versa
#define MENU_SINGLE		0x00		// Single selection menu
#define	MENU_MULTIPLE	0x02		// Multiple items can be selected
#define	MENU_RET_SEL	0x04		// return when a single item is selected.



//-----------------------------------------------------------------------------
// structure for handling a text menu item
//
// text 				- the line of text displayed per menu item
//
// (*funcitem)(void) 	- function that is executed on selection.
//						  function must have the "void function( void )" format	
//						  if this is NULL the menu returns
//
typedef struct
{
	char *text;
	void (*funcitem)(void);
}stxtMenu;


//-----------------------------------------------------------------------------
// Purpose:     Show a text style menu for one of the Opticon hand held terminals
//
// Parameters:  nIndex		- Holds the current selected index number
//
//              txtHeader	- The text shown above the menu
//
//				menuItems	- Holds the text and matching functions of the menu
//
//				nMax		- the maximum of menu items to show
//
//				nOptions	- some special options can be added, for this menu only 2 
//							  items can be used MENU_FIXED or MENU_LOOP
//
// Remarks: 	When txtHeader is NULL then one additional menu line is displayed
//
// Returns:     None
//
void ShowTextMenu( int *nIndex, const char* txtHeader, stxtMenu *menuItems, int nMax, int nOptions );


//-----------------------------------------------------------------------------
// structure for handling a selection menu item
//
// text 				- the line of text displayed per menu item
//
// value			 	- value that is placed in lValue parameter of the 
//						  ShowTextSelectionMenu() function
//
//						  Remark:
//						  + if nOptions has MENU_MULTIPLE then value is
//						  	XOR(ed) with the lValue;
//							
//						  +	if nOption has not MENU_MULTIPLE the value is
//							copied into the ulValue;							
//
//						  + value == -1L will return (exit) from the 
//							ShowTextSelectionMenu() function.
//
typedef struct
{
	char *text;
	long value;
}sSelMenu;


//-----------------------------------------------------------------------------
// Purpose:     Show a text style menu for selecting a single or multiple 
//				items for one of the Opticon hand held terminals
//
// Parameters:  nIndex		- Holds the current selected index number
//
//              txtHeader	- The text shown above the menu
//
//				menuSelItems- Holds the text and value of the menu
//
//				nMax		- the maximum of menu items to show
//
//				nOptions	- some special options can be added, the nOptions 
//							  items can be OR(ed)	
//							  items can be used MENU_FIXED or MENU_LOOP
//							  this can be OR(ed) with either MENU_MULTIPLE	
//							  or MENU_RET_SEL 
//
//				lValue		- Hold the final value depending on Single or
//							  Multiple selection
//		
// Remarks: 	When txtHeader is NULL then one additional menu line is displayed
//				Possible nOptions combinations are:
//				MENU_FIXED|MENU_MULTIPLE 	
//				MENU_LOOP|MENU_MULTIPLE 
//				MENU_FIXED|MENU_RET_SEL 	
//				MENU_LOOP|MENU_RET_SEL
//
// Returns:     None
//
void ShowTextSelectionMenu( int *nIndex, const char* txtHeader, sSelMenu *menuSelItems, int nMax, int nOptions, long *lValue );

//const definition for max amount of layers
#define MAX_LAYERS 	10


//-----------------------------------------------------------------------------
// structure for handling a graphical menu item
//
// text 				- the line of text displayed per menu item
//
// picture				- picture representing the menu item
//						  Whe picture is NULL then the exitsign is shown	
//
// (*functitem)(void) 	- function that is executed on selection.
//						  function must have the "void function( void )" format	
//						  if this is NULL the menu returns
//
typedef struct
{
	char *text;
	const unsigned char *picture;
	void (*funcitem)(void);
}sgraphMenu;

//-----------------------------------------------------------------------------
// Purpose:     Initializes the menu_layers 
//
// Parameters:  none 
//
// Remarks: 	THIS FUNCTION MUST BE USED THE MAIN LOOP ONLY 
//				AND ONLY ONCE!!!!!
//
// Returns:     None
//
void InitGraphMenu( void );

//-----------------------------------------------------------------------------
// Purpose:     Show a Graphical style menu for one of the Opticon hand held terminals
//
// Parameters:  menuItems	- Holds the text, image and matching functions of the menu
//
//				nMax		- the maximum of menu items to show
//
// Remarks: 	None
//
// Returns:     None
//
void ShowGraphMenu( sgraphMenu *menuItems, int nMax );

//-----------------------------------------------------------------------------
// Purpose:     Show a graphical style menu for selecting a single or multiple 
//				items for one of the Opticon hand held terminals
//
// Parameters:  menuSelItems- Holds the text and value of the menu
//
//				nMax		- the maximum of menu items to show
//
//				nOptions	- some special options can be added, the nOptions 
//							  items can be OR(ed)	
//							  MENU_SINGLE, MENU_MULTIPLE and MENU_RET_SEL
//
//				lValue		- Hold the final value depending on Single or
//							  Multiple selection
//		
// Remarks: 	Possible nOptions combinations are:
//				MENU_MULTIPLE 	
//				MENU_SINGLE
//				MENU_SIGNLE|MENU_RET_SEL 	
//
// Returns:     None
//
void ShowGraphSelectionMenu( sSelMenu *menuSelItems, int nMax, int nOptions, long *lValue );


#endif // __MENU_HEADER__
