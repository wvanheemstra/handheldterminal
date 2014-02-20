//
// menu.c
// implementation of the menu functions
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
// 19/06/2006:	In display_progress_bar() the static nGap variable was not initialized
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib.h"
#include "input.h"
#include "menu.h"


#if OPH1005
extern const unsigned char _vga_15_32[];
extern const unsigned char _exit_pic[];
extern const unsigned char _checked[];
extern const unsigned char _uncheck[];
#endif


#ifndef DISP_WIDTH
#if PHL
	// PHL1700-10 and PHL1700-20
	#define DISP_WIDTH  96
    #define DISP_HEIGHT 48
#elif PHL1000
	// PHL1300
	#define DISP_WIDTH  96
	// Change height to 48 when indicators need to be on (see cursor() function)
    #define DISP_HEIGHT 64
#elif PHL2700
	// PHL2700
	#define DISP_WIDTH  128
    #define DISP_HEIGHT 64
#elif OPH
	// OPH-1000 series
	#define DISP_WIDTH  112
	#define DISP_HEIGHT 128
#elif OPH1004
	#define DISP_WIDTH	128
	#define DISP_HEIGHT	128
#elif OPH1005
		// OPH1005
	#define DISP_WIDTH  240
	#define DISP_HEIGHT	320
#else
	// OPL972X series
	#define DISP_WIDTH  112
    #define DISP_HEIGHT 64
#endif
#endif

#define DISP_YSTART	10		// drawing the line progressbar graphical menu
//
// A few macros for getting the options values
//
#define SHOWHEADER( x )((int)(x!=NULL))
#define DOLOOP(x)((unsigned long)(x&MENU_LOOP))
#define MULTIPLE_SELECT(x)((unsigned long)(x&MENU_MULTIPLE))
#define RETURN_SINGLE_SELECT(x)((unsigned long)((x&MENU_RET_SEL) && !MULTIPLE_SELECT(x)))

// ===== GLOBALS =====
static int menu_layers[ MAX_LAYERS ];	// layer array for the graphical menu

// ===== IMPLEMENTATION =====

static int GetMaxCharsXPos( void )
{
	unsigned int nHeight, nWidth;
	nHeight = nWidth = 0; // prevent warning toshiba compiler
	getfontsize( &nHeight, &nWidth);

	return (DISP_WIDTH / nWidth);
}

static int GetMaxYLines( void )
{
	unsigned int nHeight, nWidth;
	nHeight = nWidth = 0; // prevent warning toshiba compiler
	getfontsize( &nHeight, &nWidth);

	return (DISP_HEIGHT / nHeight);
}


static void display_txt_header( int *nIndex, const char *txtHeader, int nMax, int nOptions )
{
	int nXChars;
	nXChars = GetMaxCharsXPos()-2;
	gotoxy(0,0);

	if( *nIndex == (nMax-1) && !DOLOOP( nOptions ))
		printf("%c%-*.*s ", 0x1E, nXChars, nXChars, txtHeader );
	else if( *nIndex == 0 && !DOLOOP(nOptions))
		printf(" %-*.*s%c", nXChars, nXChars, txtHeader, 0x1F );
	else
		printf("%c%-*.*s%c", 0x1E, nXChars, nXChars, txtHeader, 0x1F );
}


static void display_txt_menu_lines( int *nIndex, stxtMenu *menuItems, int nMax, int nShownHeader)
{
	int nCount;
	int nPos;
	int nStart;
	int nDisplayMnuLines;
	int nCalcDecimals;
	int nXChars;

	nCalcDecimals = (nMax>9)?2:1;

	nXChars = GetMaxCharsXPos()-nCalcDecimals -1;

	nDisplayMnuLines = GetMaxYLines() - ((nShownHeader)?1:0);

	if( nDisplayMnuLines > nMax )
		nStart = 0;
	else if( *nIndex + nDisplayMnuLines  > nMax )
		nStart = nMax - nDisplayMnuLines ;
	else
		nStart = *nIndex;

	for( nCount = 0; nCount < nDisplayMnuLines; nCount++ )
	{
		nPos = nCount + nStart;
		gotoxy( 0, nCount +((nShownHeader)?1:0));
		if( *nIndex == nPos )
			printf("\tr%*d %-*.*s \tr", nCalcDecimals, nPos+1, nXChars, nXChars, menuItems[ nPos ].text ); // line that is selected
		else
		{
			if( nPos < nMax )
				printf("%*d %-*.*s ", nCalcDecimals, nPos+1, nXChars, nXChars, menuItems[ nPos ].text );
			else
				printf("%-*.*s ", (nXChars+nCalcDecimals+1), (nXChars+nCalcDecimals+1), " ");
		}
	}
}

static void errorbeep( void )
{
	sound(TSTANDARD, VHIGH, SERROR, NULL);
}

static int get_txt_menu_input( int *nIndex, stxtMenu *menuItems, int nMax, int nOptions )
{
	int nKey = WaitForKey();
	switch( nKey )
	{
	case ENT_KEY:
	case TRIGGER_KEY:
		if( menuItems[ *nIndex ].funcitem == NULL )
			*nIndex = -1;
		return TRUE; // exit

	case UP_KEY:
#if !OPH1005
	case LEFT_KEY:
#endif
#if PHL|PHL1000|PHL2700
	case Q1_KEY:
	case F5_KEY:
	case F7_KEY:
#endif
		if( *nIndex > 0 )
			(*nIndex)--;
		else if( DOLOOP( nOptions ))
			(*nIndex) = nMax-1;
		else
			errorbeep();
		break;
	case DOWN_KEY:
#if !OPH1005
	case RIGHT_KEY:
#endif
#if PHL|PHL1000|PHL2700
	case Q2_KEY:
	case F6_KEY:
	case F8_KEY:
#endif
		if( *nIndex + 1 < nMax )
			(*nIndex)++;
		else if( DOLOOP( nOptions ))
			(*nIndex) = 0;
		else
			errorbeep();
		break;
	case CLR_KEY:
	case ESC_KEY:
		(*nIndex) = -1;
		return TRUE;

	case '1':	case '2':	case '3':
	case '4':	case '5':	case '6':
	case '7':	case '8':	case '9':
		if( (nKey - 0x31) >= 0 && (nKey - 0x31) < nMax)
		{
			(*nIndex) = nKey - 0x31;

			if( menuItems[ *nIndex ].funcitem == NULL )
				*nIndex = -1;

			return TRUE;
		}
		errorbeep();
		break;
	default:
		errorbeep();
		break;
	}
	return FALSE;
}

void ShowTextMenu( int *nIndex, const char* txtHeader, stxtMenu *menuItems, int nMax, int nOptions )
{
	cursor( NOWRAP );
	putchar('\f');
	for(;;)
	{
		if( SHOWHEADER( txtHeader ) )
			display_txt_header( nIndex, txtHeader, nMax, nOptions );

		display_txt_menu_lines( nIndex, menuItems, nMax, SHOWHEADER( txtHeader ));

		if( get_txt_menu_input( nIndex, menuItems, nMax, nOptions))
		{
			if( *nIndex == -1 )
				return;
			menuItems[ *nIndex ].funcitem();
			cursor( NOWRAP );
		}
	}
}


//-----------------------------------------------------------------------------
// The Text selection menu stuff is below here
//-----------------------------------------------------------------------------


static void txt_menu_sel_set_value( int *nIndex, sSelMenu *menuSelItems, int nOptions, long *lValue )
{
	if( menuSelItems[ *nIndex ].value == -1L )
		*nIndex = -1;
	else if( MULTIPLE_SELECT( nOptions ))
		*lValue ^= menuSelItems[ *nIndex ].value;
	else if( !MULTIPLE_SELECT( nOptions ) ) //&& menuSelItems[ *nIndex ].value != 0UL )
		*lValue = menuSelItems[ *nIndex ].value;

	if( RETURN_SINGLE_SELECT(nOptions) )
		*nIndex = -1;
}

static int get_txt_menu_sel_input( int *nIndex, sSelMenu *menuSelItems, int nMax, int nOptions, long *lValue )
{
	int nKey = WaitForKey();
	switch( nKey )
	{
	case ENT_KEY:
	case TRIGGER_KEY:

		// Change the value item first before redrawing
		txt_menu_sel_set_value( nIndex, menuSelItems, nOptions, lValue );

		return TRUE; // exit

	case UP_KEY:
#if !OPH1005
	case LEFT_KEY:
#endif
#if PHL|PHL1000|PHL2700
	case Q1_KEY:
	case F5_KEY:
	case F7_KEY:
#endif
		if( *nIndex > 0 )
			(*nIndex)--;
		else if( DOLOOP( nOptions ) )
			(*nIndex) = nMax-1;
		else
			errorbeep();
		break;

	case DOWN_KEY:
#if !OPH1005
	case RIGHT_KEY:
#endif
#if PHL|PHL1000|PHL2700
	case Q2_KEY:
	case F6_KEY:
	case F8_KEY:
#endif
		if( *nIndex + 1 < nMax )
			(*nIndex)++;
		else if( DOLOOP( nOptions ) )
			(*nIndex) = 0;
		else
			errorbeep();
		break;

	case CLR_KEY:
	case ESC_KEY:
		(*nIndex) = -1;
		return TRUE;

	case '1':	case '2':	case '3':
	case '4':	case '5':	case '6':
	case '7':	case '8':	case '9':
		if( (nKey - 0x31) >= 0 && (nKey - 0x31) < nMax)
		{
			(*nIndex) = nKey - 0x31;
			txt_menu_sel_set_value( nIndex, menuSelItems, nOptions, lValue );

			return TRUE;
		}
		errorbeep();
		break;

	default:
		errorbeep();
		break;
	}

	return FALSE;
}

static void display_txt_menu_sel_lines( int *nIndex, sSelMenu *menuSelItems, int nMax, int nShownHeader, int nOptions, long *lValue )
{
	int nCount;
	int nPos;
	int nStart;
	int nDisplayMnuLines;
	int nCalcDecimals;
	int nXChars;

	nCalcDecimals = (nMax>9)?2:1;

	nXChars = GetMaxCharsXPos()-nCalcDecimals -1;

	nDisplayMnuLines = GetMaxYLines() - ((nShownHeader)?1:0);

	if( nDisplayMnuLines > nMax )
		nStart = 0;
	else if( *nIndex + nDisplayMnuLines  > nMax )
		nStart = nMax - nDisplayMnuLines ;
	else
		nStart = *nIndex;

	for( nCount = 0; nCount < nDisplayMnuLines; nCount++ )
	{
		nPos = nCount + nStart;
		gotoxy( 0, nCount +((nShownHeader)?1:0));
		if( *nIndex == nPos )
		{
			if( MULTIPLE_SELECT( nOptions ))
				printf("\tr%*d%c%-*.*s \tr", nCalcDecimals, nPos+1, ((menuSelItems[ nPos ].value & *lValue)&& (menuSelItems[ nPos ].value!=-1L) )?'*':' ', nXChars, nXChars, menuSelItems[ nPos ].text ); // the selected line
			else
				printf("\tr%*d%c%-*.*s \tr", nCalcDecimals, nPos+1, (menuSelItems[ nPos ].value == *lValue)?'*':' ', nXChars, nXChars, menuSelItems[ nPos ].text ); // the selected line
		}
		else
		{
			if( nPos < nMax )
			{
				if( MULTIPLE_SELECT( nOptions ))
					printf("%*d%c%-*.*s ", nCalcDecimals, nPos+1, (menuSelItems[ nPos ].value & *lValue)?'*':' ', nXChars, nXChars, menuSelItems[ nPos ].text );
				else
					printf("%*d%c%-*.*s ", nCalcDecimals, nPos+1, (menuSelItems[ nPos ].value == *lValue)?'*':' ', nXChars, nXChars, menuSelItems[ nPos ].text );
			}
			else
				printf("%-*.*s ", (nXChars+nCalcDecimals+1), (nXChars+nCalcDecimals+1), " ");
		}
	}
}

void ShowTextSelectionMenu( int *nIndex, const char* txtHeader, sSelMenu *menuSelItems, int nMax, int nOptions, long *lValue )
{
	cursor( NOWRAP );
	putchar('\f');
	for(;;)
	{
		if( SHOWHEADER( txtHeader ) )
			display_txt_header( nIndex, txtHeader, nMax, nOptions );

		display_txt_menu_sel_lines( nIndex, menuSelItems, nMax, SHOWHEADER( txtHeader ), nOptions, lValue);

		if( get_txt_menu_sel_input( nIndex, menuSelItems, nMax, nOptions, lValue))
		{
			if( *nIndex == -1 )
				return;
		}
	}
}

#if !OPH1005
// ===== Graphical menu images ============
#if PHL | PX25
// PHL1700 exit sign 20*20 pixels
const unsigned char _exitsign[]=
{ 0x14, 0x14,
 0x00, 0x00, 0x0F, 0x3F, 0xFF, 0xCF, 0x60, 0x00,
 0x2F, 0x5A, 0xBB, 0xAF, 0x52, 0x91, 0x2F, 0x59,
 0x11, 0x2F, 0x52, 0x91, 0x2F, 0x5A, 0xB9, 0x2F,
 0x40, 0x00, 0x2F, 0x40, 0x00, 0x2F, 0x40, 0x3C,
 0x2F, 0x40, 0x1C, 0x2F, 0x40, 0x3C, 0x2F, 0x40,
 0x74, 0x2F, 0x40, 0xE0, 0x2F, 0x41, 0xC0, 0x2F,
 0x43, 0x80, 0x2F, 0x40, 0x00, 0x2F, 0x3F, 0xFF,
 0xCF, 0x00, 0x00, 0x0F,
};

#elif OPH | OPH1004
const unsigned char _exitsign[]=
{
 0x40, 0x40,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xF8, 0x00, 0x00,
 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xFC, 0x00, 0x00,
 0x00, 0x00, 0x70, 0x00, 0x00, 0x0E, 0x00, 0x00,
 0x00, 0x00, 0x60, 0x00, 0x00, 0x06, 0x00, 0x00,
 0x00, 0x00, 0x67, 0xD9, 0xB7, 0xE6, 0x00, 0x00,
 0x00, 0x00, 0x67, 0xD9, 0xB7, 0xE6, 0x00, 0x00,
 0x00, 0x00, 0x66, 0x0F, 0x31, 0x86, 0x00, 0x00,
 0x00, 0x00, 0x67, 0x86, 0x31, 0x86, 0x00, 0x00,
 0x00, 0x00, 0x67, 0x86, 0x31, 0x86, 0x00, 0x00,
 0x00, 0x00, 0x66, 0x0F, 0x31, 0x86, 0x00, 0x00,
 0x00, 0x00, 0x67, 0xD9, 0xB1, 0x86, 0x00, 0x00,
 0x00, 0x00, 0x67, 0xD9, 0xB1, 0x86, 0x00, 0x00,
 0x00, 0x00, 0x60, 0x00, 0x00, 0x06, 0x00, 0x00,
 0x00, 0x00, 0x60, 0x00, 0x00, 0x06, 0x00, 0x00,
 0x00, 0x00, 0x60, 0x01, 0xFC, 0x06, 0x00, 0x00,
 0x00, 0x00, 0x60, 0x00, 0xFC, 0x06, 0x00, 0x00,
 0x00, 0x00, 0x60, 0x00, 0x7C, 0x06, 0x00, 0x00,
 0x00, 0x00, 0x60, 0x00, 0xFC, 0x06, 0x00, 0x00,
 0x00, 0x00, 0x60, 0x01, 0xFC, 0x06, 0x00, 0x00,
 0x00, 0x00, 0x60, 0x03, 0xEC, 0x06, 0x00, 0x00,
 0x00, 0x00, 0x60, 0x07, 0xC4, 0x06, 0x00, 0x00,
 0x00, 0x00, 0x60, 0x0F, 0x80, 0x06, 0x00, 0x00,
 0x00, 0x00, 0x60, 0x1F, 0x00, 0x06, 0x00, 0x00,
 0x00, 0x00, 0x60, 0x3E, 0x00, 0x06, 0x00, 0x00,
 0x00, 0x00, 0x60, 0x3C, 0x00, 0x06, 0x00, 0x00,
 0x00, 0x00, 0x60, 0x18, 0x00, 0x06, 0x00, 0x00,
 0x00, 0x00, 0x60, 0x00, 0x00, 0x06, 0x00, 0x00,
 0x00, 0x00, 0x70, 0x00, 0x00, 0x0E, 0x00, 0x00,
 0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xFC, 0x00, 0x00,
 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xF8, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

};
#else
const unsigned char _exitsign[]=
{ 0x20, 0x20,
 0x00, 0x00, 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xF8,
 0x3F, 0xFF, 0xFF, 0xFC, 0x70, 0x00, 0x00, 0x0E,
 0x60, 0x00, 0x00, 0x06, 0x67, 0xD9, 0xB7, 0xE6,
 0x67, 0xD9, 0xB7, 0xE6, 0x66, 0x0F, 0x31, 0x86,
 0x67, 0x86, 0x31, 0x86, 0x67, 0x86, 0x31, 0x86,
 0x66, 0x0F, 0x31, 0x86, 0x67, 0xD9, 0xB1, 0x86,
 0x67, 0xD9, 0xB1, 0x86, 0x60, 0x00, 0x00, 0x06,
 0x60, 0x00, 0x00, 0x06, 0x60, 0x01, 0xFC, 0x06,
 0x60, 0x00, 0xFC, 0x06, 0x60, 0x00, 0x7C, 0x06,
 0x60, 0x00, 0xFC, 0x06, 0x60, 0x01, 0xFC, 0x06,
 0x60, 0x03, 0xEC, 0x06, 0x60, 0x07, 0xC4, 0x06,
 0x60, 0x0F, 0x80, 0x06, 0x60, 0x1F, 0x00, 0x06,
 0x60, 0x3E, 0x00, 0x06, 0x60, 0x3C, 0x00, 0x06,
 0x60, 0x18, 0x00, 0x06, 0x60, 0x00, 0x00, 0x06,
 0x70, 0x00, 0x00, 0x0E, 0x3F, 0xFF, 0xFF, 0xFC,
 0x1F, 0xFF, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00,
};
#endif

const unsigned char _up_arrow[]=
{ 0x08, 0x08,
 0x7E, 0xE7, 0xC3, 0x81, 0xE7, 0xE7, 0xE7, 0xFF,
};

const unsigned char _down_arrow[]=
{ 0x08, 0x08,
 0x7E, 0xE7, 0xE7, 0xE7, 0x81, 0xC3, 0xE7, 0xFF,
};


//
// Change the select image for you own language
// When you keep the name the same then the menu will
// show it. keep the height to 8 pixels
//
const unsigned char _select[]=
{ 0x20, 0x08,
 0x7F, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF,
 0xC4, 0x2F, 0x0C, 0x41, 0xBD, 0xEF, 0x7B, 0xF7,
 0xCC, 0x6F, 0x1B, 0xF7, 0xF5, 0xEF, 0x7B, 0xF7,
 0x8C, 0x21, 0x0C, 0x77, 0xFF, 0xFF, 0xFF, 0xFF,
};

#if PHL
// checked and unchecked bitmaps for the selection menu
// special 20*20 pixels for PHL1700
const unsigned char _checked[]=
{ 0x14, 0x14,
 0x00, 0x00, 0x0F, 0x7F, 0xFF, 0xEF, 0x40, 0x00,
 0x2F, 0x40, 0x00, 0x2F, 0x40, 0x00, 0xAF, 0x40,
 0x01, 0xAF, 0x40, 0x03, 0xAF, 0x40, 0x07, 0xAF,
 0x58, 0x0F, 0xAF, 0x5C, 0x1F, 0x2F, 0x5E, 0x3E,
 0x2F, 0x4F, 0x7C, 0x2F, 0x47, 0xF8, 0x2F, 0x43,
 0xF0, 0x2F, 0x41, 0xE0, 0x2F, 0x40, 0xC0, 0x2F,
 0x40, 0x00, 0x2F, 0x40, 0x00, 0x2F, 0x7F, 0xFF,
 0xEF, 0x00, 0x00, 0x0F,
};

const unsigned char _uncheck[]=
{ 0x14, 0x14,
 0x00, 0x00, 0x0F, 0x7F, 0xFF, 0xEF, 0x40, 0x00,
 0x2F, 0x40, 0x00, 0x2F, 0x40, 0x00, 0x2F, 0x40,
 0x00, 0x2F, 0x40, 0x00, 0x2F, 0x40, 0x00, 0x2F,
 0x40, 0x00, 0x2F, 0x40, 0x00, 0x2F, 0x40, 0x00,
 0x2F, 0x40, 0x00, 0x2F, 0x40, 0x00, 0x2F, 0x40,
 0x00, 0x2F, 0x40, 0x00, 0x2F, 0x40, 0x00, 0x2F,
 0x40, 0x00, 0x2F, 0x40, 0x00, 0x2F, 0x7F, 0xFF,
 0xEF, 0x00, 0x00, 0x0F,
};

#else
// checked and unchecked bitmaps for the selection menu
const unsigned char _checked[]=
{ 0x20, 0x20,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
 0x00, 0x00, 0x01, 0xE0, 0x00, 0x00, 0x01, 0xF0,
 0x00, 0x00, 0x03, 0xF8, 0x00, 0x00, 0x07, 0xF0,
 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x0F, 0xE0,
 0x00, 0x00, 0x1F, 0xC0, 0x7F, 0xFF, 0xDF, 0x80,
 0x7F, 0xFF, 0xBF, 0x00, 0x60, 0x00, 0x3E, 0x00,
 0x66, 0x00, 0x7C, 0x00, 0x6F, 0x00, 0xFC, 0x00,
 0x6F, 0x00, 0xF8, 0x00, 0x7F, 0x81, 0xF4, 0x00,
 0x7F, 0x83, 0xEC, 0x00, 0x6F, 0xC3, 0xCC, 0x00,
 0x67, 0xC7, 0xCC, 0x00, 0x63, 0xEF, 0x8C, 0x00,
 0x63, 0xEF, 0x0C, 0x00, 0x61, 0xFE, 0x0C, 0x00,
 0x60, 0xFC, 0x0C, 0x00, 0x60, 0xF8, 0x0C, 0x00,
 0x60, 0x78, 0x0C, 0x00, 0x60, 0x70, 0x0C, 0x00,
 0x60, 0x30, 0x0C, 0x00, 0x60, 0x20, 0x0C, 0x00,
 0x60, 0x00, 0x0C, 0x00, 0x7F, 0xFF, 0xFC, 0x00,
 0x7F, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char _uncheck[]=
{ 0x20, 0x20,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFC, 0x00,
 0x7F, 0xFF, 0xFC, 0x00, 0x60, 0x00, 0x0C, 0x00,
 0x60, 0x00, 0x0C, 0x00, 0x60, 0x00, 0x0C, 0x00,
 0x60, 0x00, 0x0C, 0x00, 0x60, 0x00, 0x0C, 0x00,
 0x60, 0x00, 0x0C, 0x00, 0x60, 0x00, 0x0C, 0x00,
 0x60, 0x00, 0x0C, 0x00, 0x60, 0x00, 0x0C, 0x00,
 0x60, 0x00, 0x0C, 0x00, 0x60, 0x00, 0x0C, 0x00,
 0x60, 0x00, 0x0C, 0x00, 0x60, 0x00, 0x0C, 0x00,
 0x60, 0x00, 0x0C, 0x00, 0x60, 0x00, 0x0C, 0x00,
 0x60, 0x00, 0x0C, 0x00, 0x60, 0x00, 0x0C, 0x00,
 0x60, 0x00, 0x0C, 0x00, 0x7F, 0xFF, 0xFC, 0x00,
 0x7F, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00,
};
#endif
#endif

// ===== Graphical menu implementation =====

//=========================================================
// VERY IMPORTANT !!!
// THIS FUNCTION NEEDS TO BE PLACED IN THE BEGINNING
// OF THE MAIN LOOP.
//
// This clears the menu layer array counter
//=========================================================
void InitGraphMenu( void )
{
	memset( menu_layers, 0, MAX_LAYERS );
}

// Add a new item to the layer counter
int GraphIncLayer( int nSelect )
{
	int x;
	// Find an empty spot in the menu_layer
	for( x = 0; x < MAX_LAYERS; x++ )
	{
		if( menu_layers[ x ] == 0 )
		{
			menu_layers[ x ] = nSelect;	// Set the new layer item
			return TRUE;
		}
	}
	return FALSE; // Not enough layers for the menu
}

static int build_layer_string( char* string )
{
	int x;
	static char tmp[ 3 ];
	char *ptr;

	string[0] = '\0';
	for( x = 0; x < MAX_LAYERS; x++ )
	{
		if( menu_layers[ x ] == 0 )
		{
			x--;
			break;
		}

		if( strlen( string ) > 0 )
			strcat( string, "-" );	// a submenu add a separator marker

		sprintf( tmp, "%d", menu_layers[ x ] );
		strcat( string, tmp );
	}
	ptr = strrchr( string, '-');
	if( ptr )
		string[ ptr-string ] = '\0';
	else
		string[0] = '\0';

	return x;
}

static void display_layer( char *string, int current )
{
#if !OPH1005
	int nMaxX;

	gotoxy(0,0);
	setfont( SMALL_FONT, NULL );
	nMaxX = GetMaxCharsXPos();
	nMaxX -= 3; // keep some additional space for the last '-' and current value
	printf("%*.*s%c%-2d", nMaxX, nMaxX, string, string[0]?'-':' ', current );
#endif
}


static void display_menu_item_text( char *string )
{
	int nMaxX;
#if OPH1005
	setfont(USER_FONT,(char*)_vga_15_32);
	nMaxX = GetMaxCharsXPos();
#else
	setfont( LARGE_FONT, NULL );
	nMaxX = GetMaxCharsXPos()-1; // leave some space for the progress bar
#endif


#if PHL|PHL1000
	systemsetting("-1");	// enable CLT gotoxy mode
	gotoxy(0,2);
#elif PHL2700
	systemsetting("-1");	// enable CLT gotoxy mode
	gotoxy(0,3);
#else
	gotoxy(0,0);
#if !OPH1005
	set_offset(8);
#endif
#endif

#ifdef OPH1005
	printf("%*s%-*.*s",(nMaxX-(int)strlen(string))/2," ",nMaxX,nMaxX,string);
#else
	printf("%-*.*s", nMaxX, nMaxX, string );
#endif

#if PHL|PHL1000|PHL2700
	systemsetting("-0");	// disable CLT gotoxy mode
#elif !OPH1005
	set_offset(0);
#endif
}

static void display_image( unsigned char *picture )
{
	if( picture == NULL )
	{
#if OPH1005
			ShowJPG( 20, 40, WRITE_TO_SCREEN, (unsigned char *)_exit_pic );
#elif PHL
			showpicture( (DISP_WIDTH >> 1)- (_exitsign[1] >> 1), (DISP_HEIGHT >> 1)-5, (const unsigned char*)_exitsign );
#elif OPH | OPH1004 | OPH1005
			showpicture( (DISP_WIDTH >> 1)- (_exitsign[1] >> 1 ), (DISP_HEIGHT >> 1)-25, (const unsigned char*)_exitsign );
#else
			showpicture( (DISP_WIDTH >> 1)- (_exitsign[1] >> 1), (DISP_HEIGHT >> 1)-8, (const unsigned char*)_exitsign );
#endif
	}
	else if (picture == _checked || picture == _uncheck)
	{
#if OPH1005
			ShowJPG( 20, 40, WRITE_TO_SCREEN, (unsigned char *) picture);
#elif OPH | OPH1004
			showpicture( (DISP_WIDTH >> 1)- (picture[1] >> 1 ), (DISP_HEIGHT >> 1)-15, (const unsigned char*)picture );
#elif PHL
			showpicture( (DISP_WIDTH >> 1)- (picture[1] >> 1 ), (DISP_HEIGHT >> 1)-5, (const unsigned char*)picture );
#else
			showpicture( (DISP_WIDTH >> 1)- (picture[1] >> 1 ), (DISP_HEIGHT >> 1)-8, (const unsigned char*)picture );
#endif
	}
	else
	{
#if OPH1005
			ShowJPG( 20, 40, WRITE_TO_SCREEN, (unsigned char *)picture );
#elif PHL
			showpicture( (DISP_WIDTH >> 1)- (picture[1] >> 1 ), (DISP_HEIGHT >> 1)-5, (const unsigned char*)picture );
#elif OPH | OPH1004
			showpicture( (DISP_WIDTH >> 1)- (picture[1] >> 1 ), (DISP_HEIGHT >> 1)-25, (const unsigned char*)picture );
#else
			showpicture( (DISP_WIDTH >> 1)- (picture[1] >> 1 ), (DISP_HEIGHT >> 1)-8, (const unsigned char*)picture );
#endif
	}
}

static void display_selection_bar(void )
{
	// Show the up arrow
#ifdef OPH1005

	gotoxy(0,9);
	printf("%c",24);
	gotoxy(5,9);
	printf("SELECT");
	gotoxy(15,9);
	printf("%c",25);
#else
	showpicture( 0, DISP_HEIGHT - _up_arrow[1], (const unsigned char*)_up_arrow);

	// Show the selection item
	showpicture( (DISP_WIDTH >> 1)- (_select[0] >> 1), DISP_HEIGHT - _select[1], (const unsigned char*)_select );

	// Show the down arrow
	showpicture( DISP_WIDTH - (_down_arrow[0] << 1), DISP_HEIGHT - _down_arrow[1], (const unsigned char*)_down_arrow);
#endif
}


static void display_progress_bar( int nCurr, int nMax )
{
#if !OPH1005
	int nStages;
	int nGap;

	nCurr--;
	nStages = (DISP_HEIGHT-DISP_YSTART-1-5)/(nMax-1);
	nGap = (nCurr!=(nMax-1))?((nCurr*nStages)+DISP_YSTART):(DISP_HEIGHT-7);

	drawline(DISP_WIDTH-1 ,0, DISP_WIDTH-1, DISP_HEIGHT,0);		// clear the progress bar
	drawline(DISP_WIDTH-2 ,0, DISP_WIDTH-2, DISP_HEIGHT,0);		// clear the progress bar
	drawline(DISP_WIDTH-3 ,0, DISP_WIDTH-3, DISP_HEIGHT,0);		// clear the progress bar

	drawline( DISP_WIDTH-4, DISP_YSTART, DISP_WIDTH-4, DISP_HEIGHT-1, 1 ); // top bottom line
	drawline( DISP_WIDTH-4, nGap+1, DISP_WIDTH-4, nGap+5, 0 ); // small gap in line
	drawline( DISP_WIDTH-2, nGap+1, DISP_WIDTH-2, nGap+5, 1 ); // draw gapline a bit to the left
	setpixel( DISP_WIDTH-3, nGap, 1 );
	setpixel( DISP_WIDTH-3, nGap+6, 1 );
#endif
}

static int get_graph_menu_input( int nCurrLayer, sgraphMenu *menuItems, int nMax )
{
	int nKey;
	int n;

	nKey = WaitForKey();

	switch( nKey )
	{
	case ENT_KEY:
	case TRIGGER_KEY:
		if( menuItems[ menu_layers[ nCurrLayer ]-1].funcitem == NULL )
			menu_layers[ nCurrLayer ]= 0;
		return TRUE; // exit

	case UP_KEY:
#if !OPH1005
	case LEFT_KEY:
#endif
#if PHL|PHL1000|PHL2700
	case Q1_KEY:
	case F5_KEY:
	case F7_KEY:
#endif
		if( menu_layers[ nCurrLayer ] > 1 )
			menu_layers[ nCurrLayer ]--;
		else
			menu_layers[ nCurrLayer ] = nMax;
		break;

	case DOWN_KEY:
#if !OPH1005
	case RIGHT_KEY:
#endif
#if PHL|PHL1000|PHL2700
	case Q2_KEY:
	case F6_KEY:
	case F8_KEY:
#endif
		if( menu_layers[ nCurrLayer ] < nMax )
			menu_layers[ nCurrLayer ]++;
		else
			menu_layers[ nCurrLayer ] = 1;
		break;

	case CLR_KEY:
	case ESC_KEY:
			for(n=nCurrLayer;n < MAX_LAYERS;n++)
				menu_layers[ n ] = 0;
		return TRUE;

	case '1':	case '2':	case '3':
	case '4':	case '5':	case '6':
	case '7':	case '8':	case '9':
		if( (nKey - 0x30) > 0 && (nKey - 0x30) <= nMax)
		{
			menu_layers[ nCurrLayer ] = nKey - 0x30;
			if( menuItems[ menu_layers[ nCurrLayer ] -1 ].funcitem == NULL )
				menu_layers[ nCurrLayer ]= 0;
			return TRUE;
		}
		errorbeep();
		break;
	default:
		errorbeep();
		break;
	}
	return FALSE;
}

void ShowGraphMenu( sgraphMenu *menuItems, int nMax )
{
	int nCurrLayer;
	static char layer_string[ MAX_LAYERS*3 ];

#if PHL1000
	cursor( NOICONS );
#endif
	cursor( NOWRAP );
	GraphIncLayer( 1 );
	nCurrLayer  = build_layer_string( layer_string );

	putchar('\f');
	resetkey();
	for(;;)
	{
		display_layer(layer_string, menu_layers[nCurrLayer]);

		display_menu_item_text(menuItems[menu_layers[nCurrLayer]-1].text);

		display_image((unsigned char*)menuItems[menu_layers[nCurrLayer]-1].picture);

		display_selection_bar();

		display_progress_bar(menu_layers[nCurrLayer], nMax);

		if(get_graph_menu_input(nCurrLayer, menuItems, nMax))
		{
			if(menu_layers[nCurrLayer] == 0)
			{
				nCurrLayer  = build_layer_string( layer_string );
				return;
			}
			menuItems[menu_layers[nCurrLayer]-1].funcitem();
			putchar('\f');
			resetkey();
			cursor( NOWRAP );
		}
	}
}

//
// Graphical selection menu
//
static void graph_menu_sel_set_value( int nCurr, sSelMenu *menuSelItems, int nOptions, long *lValue )
{
	if( menuSelItems[ menu_layers[ nCurr ] -1 ].value == -1L )
		 menu_layers[ nCurr ] = 0;
	else if( MULTIPLE_SELECT( nOptions ))
		*lValue ^= menuSelItems[ menu_layers[ nCurr ] -1 ].value;
	else if( !MULTIPLE_SELECT( nOptions ) ) //&& menuSelItems[ *nIndex ].value != 0UL )
		*lValue = menuSelItems[ menu_layers[ nCurr ] -1 ].value;

	if( RETURN_SINGLE_SELECT(nOptions) )
		menu_layers[ nCurr ] = 0;
}

static void display_check_box( int nCurr, sSelMenu *menuSelItems, int nOptions, long *lValue )
{
	int nCheck;
	nCheck = FALSE;
	if( menuSelItems[ menu_layers[ nCurr ] -1 ].value == -1L )
	{
		display_image( NULL ); // show exit sign
		return;
	}
	else if( MULTIPLE_SELECT( nOptions ) && menuSelItems[ menu_layers[ nCurr ] -1 ].value & *lValue )
	{
		nCheck = TRUE;
	}
	else if( !MULTIPLE_SELECT( nOptions ) && menuSelItems[ menu_layers[ nCurr ] -1 ].value == *lValue )
		nCheck = TRUE;

	display_image( (unsigned char*)((nCheck)?_checked:_uncheck));
}

static int get_graph_menu_sel_input( int nCurr, sSelMenu *menuSelItems, int nMax, int nOptions, long *lValue)
{
	int nKey = WaitForKey();
	switch( nKey )
	{
		case ENT_KEY:
		case TRIGGER_KEY:

			// Change the value item first before redrawing
			graph_menu_sel_set_value( nCurr, menuSelItems, nOptions, lValue );

			return TRUE; // exit

		case UP_KEY:
#if !OPH1005
		case LEFT_KEY:
#endif
#if PHL|PHL1000|PHL2700
	case Q1_KEY:
	case F5_KEY:
	case F7_KEY:
#endif
			if( menu_layers[ nCurr ] > 1 )
				menu_layers[ nCurr ]--;
			else
				menu_layers[ nCurr ] = nMax;
			break;

		case DOWN_KEY:
#if !OPH1005
		case RIGHT_KEY:
#endif
#if PHL|PHL1000|PHL2700
	case Q2_KEY:
	case F6_KEY:
	case F8_KEY:
#endif
			if( menu_layers[ nCurr ] < nMax )
				menu_layers[ nCurr ]++;
			else
				menu_layers[ nCurr ] = 1;
			break;
		case CLR_KEY:
		case ESC_KEY:
			menu_layers[ nCurr ] = 0;
			return TRUE;

		case '1':	case '2':	case '3':
		case '4':	case '5':	case '6':
		case '7':	case '8':	case '9':
			if( (nKey - 0x30) > 0 && (nKey - 0x30) <= nMax)
			{
				menu_layers[ nCurr ] = nKey - 0x30;
				graph_menu_sel_set_value( nCurr, menuSelItems, nOptions, lValue );

				return TRUE;
			}
			errorbeep();
			break;
		default:
			errorbeep();
			break;
	}
	return FALSE;
}

void ShowGraphSelectionMenu( sSelMenu *menuSelItems, int nMax, int nOptions, long *lValue )
{
	int nCurrLayer;
	static char layer_string[ MAX_LAYERS*3 ];

#if PHL1000
	cursor( NOICONS );
#endif
	cursor( NOWRAP );
	GraphIncLayer( 1 );
	nCurrLayer  = build_layer_string( layer_string );

	putchar('\f');
	resetkey();
	for(;;)
	{
		display_layer( layer_string, menu_layers[ nCurrLayer ] );

		display_menu_item_text( menuSelItems[ menu_layers[ nCurrLayer ]-1].text );

		display_check_box( nCurrLayer, menuSelItems, nOptions, lValue );

		display_selection_bar();

		display_progress_bar( menu_layers[ nCurrLayer ], nMax );

		if( get_graph_menu_sel_input( nCurrLayer, menuSelItems, nMax, nOptions, lValue))
		{
			if( menu_layers[ nCurrLayer ] == 0 )
				return;
		}
	}
}

