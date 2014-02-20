//
// input.h
//
// header file of some input functions for the
// opticon barcode terminals
//
// Martin Jansen
//
// KeyboardInput function is based on the get_string() function from
// Oscar Jacobse
//
// For questions:
// email:	support@opticon.com
// web:		www.opticon.com
//
// 03/06/2005: 	Bug fix in display_input() function, cursor position was
//				not correctly calculated.
//
// 08/06/2005: 	Added function ScanBarcodeSymbol(), ScanOrKeyboardInputSymbol(), 
//				Added INPUT_PRINT define (all printable characters)
//				Added Automatic cursor on when selecting Keyboard input
//				Automatic setecho(ON) and setecho(OFF) keyboardinput with INPUT_ALPHA or NPUT_PRINT
//				Changed display_input function to indicate difference between full string and max display_length
//
// 20/06/2005: 	Added KeyboardNumeric function for input a whole numeric values displaying is done from
//				left to right to give numeric input a more natural feeling
//
// 

#ifndef __INPUT_H__
#define __INPUT_H__

#define INPUT_FLOAT		0x01
#define INPUT_NUM		0x02
#define INPUT_ALPHA		0x04
#define INPUT_NEGATIVE	0x08
#define INPUT_PRINT		0x10
#define INPUT_ALL		(INPUT_FLOAT | INPUT_NUM | INPUT_ALPHA | INPUT_NEGATIVE | INPUT_PRINT )

#define INPUT_SHOW_DEFAULT	0x80

#define INPUT_BCR_MAX	50

#define DEFAULT			0x200 	// default return value
#define KEYBOARD		0x400	// used when input is done by keyboard
#define SCANNED			0x800	// used when input is scanned

//-----------------------------------------------------------------------------
// Purpose:     Wait until any key is pressed
//
// Parameters:  None
//
// Returns:     The key that is pressed
//
int WaitForKey( void );

//-----------------------------------------------------------------------------
// Purpose:     Wait until one of the keys from the list is pressed
//
// Parameters:  num			- amount of keys in the list
//
//              ...			- The list of the keys to react on
//
// Returns:     The key that was pressed from the list
//
int WaitForKeys( int num, ... );

//-----------------------------------------------------------------------------
// Purpose:     Scan a barcode
//
// Parameters:  string		- holds the scanned barcode
//
//              min_length	- the minimal length the barcode at least needs to be
//
//				max_length	- the maximum length the barcode may be
//
// Returns:     OK on success, ERROR when any other key is pressed
//
int ScanBarcode( char* string, int min_length, int max_length);

//-----------------------------------------------------------------------------
// Purpose:     Scan a barcode
//
// Parameters:  string		- holds the scanned barcode
//
//              min_length	- the minimal length the barcode at least needs to be
//
//				max_length	- the maximum length the barcode may be
//
//				nCodeId		- returns the matching code id of the barcode (see lib.h for matchin code ID
//
// Returns:     OK on success, ERROR when any other key is pressed
//
int ScanBarcodeSymbol( char* string, int min_length, int max_length, int *nCodeId);

//-----------------------------------------------------------------------------
// Purpose:     Input a string of data by the keyboard
//
// Parameters:  string		- holds the typed data string
//
//              min_length	- the minimal length the data at least needs to be
//
//				max_length	- the maximum length the data may be
//
//				typ			- orred parameter for FLOAT, NUMERIC, ALPHA, NEGATIVE
//
//				x			- x position on the display
//
//				y			- y position on the display
//
//				display_length - the amount of characters to display before starting to scroll data
//
//				num			- amount of exception keys
//
//				...			- list of exception keys
//
// Remarks 		BS_KEY cannot be used as an exception key
//
// Returns:     EOF when no exception keys are used, else returns the exception key
//
int KeyboardInput( char* string, int min_length, int max_length, int typ, int x, int y, int display_length, int num, ... );

//-----------------------------------------------------------------------------
// Purpose:     Input a string of data by the keyboard, or by a scanned barcode
//
// Parameters:  string		- holds the typed data string
//
//              min_length	- the minimal length the data at least needs to be
//
//				max_length	- the maximum length the data may be
//
//				typ			- orred parameter for FLOAT, NUMERIC, ALPHA, NEGATIVE
//
//				x			- x position on the display
//
//				y			- y position on the display
//
//				display_length - the amount of characters to display before starting to scroll data
//
//				display_height - the amount lines fitting on the screen
//
// Returns:     SCANNED on scanned barcode, KEYBOARD on a typed string or CLR_KEY when CLR_KEY was pressed
//
int ScanOrKeyboardInput( char* string, int min_length, int max_length, int typ, int x, int y, int display_length, int display_height  );


//-----------------------------------------------------------------------------
// Purpose:     Input a string of data by the keyboard, or by a scanned barcode
//
// Parameters:  string		- holds the typed data string
//
//              min_length	- the minimal length the data at least needs to be
//
//				max_length	- the maximum length the data may be
//
//				typ			- orred parameter for FLOAT, NUMERIC, ALPHA, NEGATIVE
//
//				x			- x position on the display
//
//				y			- y position on the display
//
//				display_length - the amount of characters to display before starting to scroll data
//
//				nCodeId		- the scanned barcode code id (see lib.h for matching info)
//
// Returns:     SCANNED on scanned barcode, KEYBOARD on a typed string or CLR_KEY when CLR_KEY was pressed
//
int ScanOrKeyboardInputSymbol( char* string, int min_length, int max_length, int typ, int x, int y, int display_length, int *nCodeId );


//-----------------------------------------------------------------------------
// Purpose:     Input a numeric string of data by the keyboard negative values allowed
//
// Parameters:  string		- holds the typed data string
//
//				max_length	- maximum number of digits without the minus sign (-) if type 
//							  also has INPUT_NEGATIVE then max length is increase by 1
//
//				typ			- orred parameter for INPUT_NUMERIC, INPUT_NEGATIVE, INPUT_SHOW_DEFAULT
//
//				x			- x position on the display
//
//				y			- y position on the display
//
//				display_length - the amount of characters to display before starting to scroll data
//
//				num			- amount of exception keys
//
//				...			- list of exception keys
//
// Remarks 		BS_KEY cannot be used as an exception key
//
//
// Returns:     EOF when no exception keys are used, else returns the exception key
//
int KeyboardNumeric( char* string, int max_length, int typ, int x, int y, int display_length, int num, ... );

#endif // __INPUT_H__


