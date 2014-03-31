//
// input.c
//
// implementation of the input functions
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
//
// 03/06/2005: 	Bug fix in display_input() function, cursor position was
//				not correctly calculated.
//
// 08/06/2005: 	Added function ScanBarcodeSymbol(), ScanOrKeyboardInputSymbol(), 
//				Added INPUT_PRINT define (all printable characters)
//				Added Automatic cursor on when selecting Keyboard input
//				Automatic setecho(ON) and setecho(OFF) keyboardinput with INPUT_ALPHA or INPUT_PRINT
//				Changed display_input function to indicate difference between full string and max display_length
//
// 20/06/2005: 	Added KeyboardNumeric function for input a whole numeric values displaying is done from
//				left to right to give numeric input a more natural feeling
//

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h> // needed for va_xxxx
#include <ctype.h>
#include <string.h>

#include "input.h"
#include "lib.h"



static void keybeep( void )
{
	sound( TSHORT, VMEDIUM, SHIGH, 0);
}

static void okbeep( void )
{
	sound( TSTANDARD, VMEDIUM, SHIGH, SMEDIUM, SHIGH, 0);
}

//
// Wait for keyboard input, when key is pressed
// return the key.
//
int WaitForKey( void )
{
	int c;

	while( ( c = getchar()) == EOF)
		idle(); // idle for powersaving
	keybeep();  // make the beeping sound
	return c;
}

//
// Check if the pressed key is found in the list
// made static so it won't show up in the map file
static int check_key_input( int key, int num, va_list key_list )
{
	int count;

	for( count = 0; count < num; count++ )
	{
		if( (int) va_arg( key_list, int ) == key )
		{
			va_end( key_list ); // reset the argument ptr
			return( OK );
		}
	}
	va_end( key_list );

	return( ERROR );
}

//
// Wait for one of the specific keys in the list
// If key is found in the list return the key
//
int WaitForKeys( int num, ... )
{
	va_list key_list;
	int key;

	if( num <= 0 )
		return( EOF ); // No keys in the list
	va_start( key_list, num );
	resetkey();
	for(;;)
	{
		key = WaitForKey(); // Wait for keyboard input
		if( check_key_input( key, num, key_list ) == OK )
		{
			va_end( key_list );	// reset the argument ptr
			return( key );
		}
	}
}

int ScanBarcodeSymbol( char* string, int min_length, int max_length, int *nCodeId)
{
	int key;
	struct barcode code = {0};

	(*nCodeId) = 0;
	resetkey();
	code.min = min_length;
	code.max = max_length;
	code.text = string;
	scannerpower( SINGLE, 300 );
#if PX25
	for(;;)
	{
		if ( !scanneroff () )
		{
			if ( readbarcode ( &code ) == OK)
				break;
		}
		else
		{
			if( kbhit() )
			{
				if(( key = getchar() ) != TRIGGER_KEY )
				{
				// put the character back in the input buffer
				// so it can be handled by another function
					ungetc( key, stdin);
					scannerpower( OFF, 0);
					return( ERROR );
				}
			}
		}
	}
#else
	while( readbarcode( &code ) != OK )
	{
		if( kbhit() )
		{
			if(( key = getchar() ) != TRIGGER_KEY )
			{
				// put the character back in the input buffer
				// so it can be handled by another function
				ungetc( key, stdin);
				scannerpower( OFF, 0);
				return( ERROR );
			}
			scannerpower( SINGLE, 300 );
		}
		idle();
	}
#endif
    // Copy the code ID
    (*nCodeId) = code.id;
	goodreadled( GREEN, 10 );
	scannerpower( OFF, 0);
	okbeep();

	return( OK );
}

int ScanBarcode( char* string, int min_length, int max_length)
{
	int nCodeId = 0;
	
	return ScanBarcodeSymbol( string, min_length, max_length, &nCodeId);
}

static void remove_key_from_buffer( char* string )
{
	if( strlen( string ) > 0 )
		string[ strlen( string ) - 1 ] = '\0';
}


static void display_input( char* string, int x, int y, int display_length, int max_length )
{
	int length;
	int output;

	if( x < 0 || y < 0 )
		return;
	length = strlen( string );
	gotoxy( x, y);
	if( length < max_length ) 
		printf("%-*.*s ", display_length-1, display_length-1, string + ((length<display_length)?0:(length - display_length + 1))); 
	else 
		printf("%-*.*s", display_length, display_length, string + ( length - display_length )); 

	output = (length < display_length)?(length+1):display_length;
	gotoxy( x + output -1, y ); // set cursor to the correct position 
}


static void store_key_in_string( int key, char* string, int max_len, int typ )
{
	int length, ok;

	length = strlen( string );
	if( length >= max_len )
		return;

	ok = 0;
	if( typ & INPUT_FLOAT )
		ok = ( isdigit( key ) || ((strchr( string, '.') == NULL) && ( key == '.' )));
	if( !ok && ( typ & INPUT_NUM ))
		ok = isdigit( key );
	if( !ok && ( typ & INPUT_ALPHA ))
		ok = ( isalpha(	key ) || ( key == ' ' ));
	if( !ok && ( typ & INPUT_NEGATIVE ) && ( length == 0 ))
		ok = ( key == '-' );
	if( !ok && ( typ & INPUT_PRINT ))
		ok = isprint( key );

	if( ok )
	{
		string[ length++ ] = (char)key;
		string[ length ] ='\0';
	}
}


int KeyboardInput( char* string, int min_length, int max_length, int typ, int x, int y, int display_length, int num, ... )
{
	va_list key_list;
	int key;

	if( num <= 0 )
		return( EOF );
#if (PHL|PHL1000|PHL2700)
	if( typ & INPUT_ALPHA || typ & INPUT_PRINT)
		setecho( ON );
#endif
	if( display_length > max_length )
		display_length = max_length;
	display_input(string, x, y, display_length, max_length );

	cursor( ON );
	for(;;)
	{
		va_start( key_list, num );

		if( ((key = WaitForKey()) == ENT_KEY ) && (strlen( string ) < min_length ))
			continue;

		if( check_key_input( key, num, key_list ) == OK )
		{
			va_end( key_list );	
#if (PHL|PHL1000|PHL2700)
			if( typ & INPUT_ALPHA || typ & INPUT_PRINT)
				setecho( OFF );
#endif
			cursor( OFF );
			return( key );
		}
		va_end( key_list );

		if( key == BS_KEY )
		{
			remove_key_from_buffer( string );
			display_input( string, x, y, display_length, max_length );
			continue;
		}

		store_key_in_string( key,  string, max_length, typ );
		display_input(string, x, y, display_length, max_length );
	}
}



int ScanOrKeyboardInput( char* string, int min_length, int max_length, int typ, int x, int y, int display_length, int display_height )
{
	int	key;
	int i, j;
	int index = 0;

	for(;;)
	{
		string[0] = '\0';
		if( ScanBarcode( string, min_length, max_length ) == OK )
		{
			int length;
//			int output;

			length = strlen( string );
			gotoxy( x, y);
//			if( length < max_length ) 
//			{
				for (j=0; j<display_height; j++)
				{
					for (i=0; i<display_length; i++)
						if (index < length)
							putchar(string[index++]);
						else
							return (SCANNED); // Input by scanning
					putchar('\n');
				}
//			}
//			else 
//				printf("%-*.*s", display_length, display_length, string + ( length - display_length )); 

			return( SCANNED ); // Input by scanning
		}

		key = KeyboardInput( string, min_length, max_length, typ, x, y, display_length, 4, TRIGGER_KEY, CLR_KEY, ESC_KEY, ENT_KEY );
		switch( key )
		{
			case ENT_KEY:
				return( KEYBOARD );	// Keyboard input
			case TRIGGER_KEY:
				return( KEYBOARD );	// Keyboard input 

			case CLR_KEY:
			case ESC_KEY:
				if( strlen( string ) != 0)
				{
					break;
				} // fall through
			default:
				return( key );
		}
	}
}


int ScanOrKeyboardInputSymbol( char* string, int min_length, int max_length, int typ, int x, int y, int display_length, int *nCodeId )
{
	int	key;
	
	for(;;)
	{
		string[0] = '\0';
		display_input( string, x, y, display_length, max_length );
		if( ScanBarcodeSymbol( string, min_length, max_length, nCodeId ) == OK )
		{
			display_input( string, x, y, display_length, max_length );
			return( SCANNED ); // Input by scanning
		}

		key = KeyboardInput( string, min_length, max_length, typ, x, y, display_length, 4, TRIGGER_KEY, CLR_KEY, ESC_KEY, ENT_KEY );
		switch( key )
		{
			case ENT_KEY:
				return( KEYBOARD );	// Keyboard input
			case TRIGGER_KEY:
				return( KEYBOARD );	// Keyboard input  

			case CLR_KEY:
			case ESC_KEY:
				if( strlen( string ) != 0)
				{
					break;
				} // fall through
			default:
				return( key );
		}
	}
}

// ----------------------------------------------------------------------------
// Special numeric input below
// ----------------------------------------------------------------------------

static void store_char_in_string_numeric( int key, int max_len, char *string, int typ, int defaul )
{
	int i, ok;
	static char tmp[INPUT_BCR_MAX+1];
	
	max_len += ((typ & INPUT_NEGATIVE)&&(string[0]=='-'))?1:0;
	i = strlen( string );

	if(( i >= max_len ) && (key != '-') && !defaul)
		return;

	ok = 0;

	if( typ & INPUT_NUM )
		ok = isdigit( key );
	if( !ok && ( typ & INPUT_NEGATIVE ))
		ok = ( key == '-');
	if( ok )
	{
		if( defaul )
		{
			string[0] = (char)key;
			string[1] = '\0';
		}
		else
		{
			if( key == '-')
			{
				if( string[0] == '-' )
					strcpy( tmp, string+1 );					
				else
				{
					strcpy( tmp, "-" );
					strcat(tmp, string );
				}
				strcpy( string, tmp );
			}
			else
			{
				string[ i++ ] = (char)key;
				string[i] = '\0';	
			}
		}
	}
}

static void display_input_numeric( char* string, int x, int y, int max_x )
{
	int length;

	if( x < 0 || y < 0 )
		return;

	length = strlen( string );

	gotoxy(x,y);
	if( length > max_x )
	{
		length -= max_x; 
		printf("%*.*s", max_x, max_x, string+length );
	}
	else
		printf("%*.*s", max_x, max_x, string );

	gotoxy( x+max_x-1, y); // set the cursor to the correct position 
}

//
// Special input function for numeric input
// Alignment is done from right to left
//
// max_length is the maximum length without the minus sign (-) if type also has INPUT_NEGATIVE 
// then max length is increase by 1
//
int KeyboardNumeric( char* string, int max_length, int typ, int x, int y, int display_length, int num, ... )
{
	va_list key_list;
	int key;
	int defaul;

	if( num <= 0 )
		return( EOF );

	if( typ & ~(INPUT_NEGATIVE|INPUT_NUM|INPUT_SHOW_DEFAULT))
		return( EOF ); // Only INPUT_NEGATIVE and INPUT_NUM are allowed

	if( typ & INPUT_SHOW_DEFAULT )
		defaul = TRUE;
	else
	{
		string[0] = '\0';
		defaul = FALSE;
	}
	cursor( ON );
	for(;;)
	{
		va_start( key_list, num );

		display_input_numeric(string, x, y, display_length ); 

		key = WaitForKey(); 

		if( check_key_input( key, num, key_list ) == OK )
		{
			va_end( key_list );
			
			cursor( OFF );
			return( key  );
		}
		va_end(key_list);

		if( key == BS_KEY )
		{
			if( defaul )
				string[0] = '\0';
			remove_key_from_buffer( string );
			defaul = FALSE;	// default value is turned off
			continue;
		}

		store_char_in_string_numeric( key,  max_length, string, typ, defaul );
		defaul = FALSE;	// default value is turned off
	}
}

