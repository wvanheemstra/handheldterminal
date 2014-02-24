//
// IceRobotics Ltd.
// Scan labels, enter wearer and scroll database with menus
// Transmit scanned data with NetO or plain ascii
// OPH Compiler: GCC Open Source Compiler Version 4.1.2
//			     available at http:/gcc.gnu.org/releases.html
// PHL Compiler: Microtec C compiler for Melps 7700  Version 2.2
//


//************************************************************************
// Include
//************************************************************************
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        "lib.h"
#include 		"database.h"
#include 		"input.h"
#include 		"menu.h"
#include 		"images.h"
#ifdef OPH1005
#include "vga_15_32.fnt"
#include "logo.h"
#endif

//************************************************************************
// Defines
//************************************************************************

#define VERSIONSUFFIX	"3092E"

// Application name
#define APPLICATION     "CowAlert"

// Device name
#define DEVICE          "Device"

// Wearer name
#define WEARER          "Cow"

// Database name
#define DBASE_NAME	   	"data.csv"  //"DATA.TXT"

// Database record
// DEPRECATED #define SZ_BARCODE		100 // Now SZ_DEVICE
#define SZ_DEVICE		8 // Storage for 8 characters
// DEPRECATED #define SZ_SIGN			1
// DEPRECATED #define SZ_QUANTITY		6 // Now SZ_WEARER
#define SZ_WEARER		8 // Storage for 8 characters
#define SZ_TIME			(2+1+2+1+2)
#define SZ_DATE			(4+1+2+1+2)
// OLD #define SZ_RECORD		(SZ_BARCODE+1+SZ_SIGN+SZ_QUANTITY+1+SZ_TIME+1+SZ_DATE+1+1)
#define SZ_RECORD		(SZ_DEVICE+1+SZ_WEARER+1+SZ_TIME+1+SZ_DATE+1+1)

// barcode menu defines
#define ID_CD39         0x0000001	// bit 0
#define ID_EAN          0x0000002	// bit 1
#define ID_UPC          0x0000004	// bit 2
#define ID_I2O5         0x0000008	// bit 3
#define ID_D2O5        	0x0000010	// bit 4
#define ID_NW7         	0x0000020	// bit 5
#define ID_CD93        	0x0000040	// bit 6
#define ID_CD128      	0x0000080	// bit 7
#define ID_MSI        	0x0000100	// bit 8
#define ID_TELEPEN    	0x0000200	// bit 9
#define ID_UK        	0x0000400	// bit 10
#define ID_ADDONS    	0x0000800	// bit 11
#define ID_IATA      	0x0001000	// bit 12
#define ID_SCODE     	0x0002000	// bit 13
#define ID_MAT2O5		0x0004000	// bit 14
#define ID_CHPOST		0x0008000	// bit 15
#define ID_CD11			0x0010000	// bit 16
#define ID_KOREAN		0x0020000	// bit 17
#define ID_RSS			0x0040000	// bit 18
#define ID_PDF417		0x0080000	// bit 19
#define ID_MICROPDF		0x0100000	// bit 20
#define ID_DATAMATRIX	0x0200000	// bit 21
#define ID_MAXICODE		0x0400000	// bit 22
#define ID_AZTEC		0x0800000	// bit 23
#define ID_QR		    0x1000000   // bit 24
//added by Khaled
#define ID_POST_NET     0x2000000   // bit 25
#define ID_IMB          0x4000000   // bit 26
//added by Bram
#define ID_UPCE1        0x8000000   // bit 27
//
// Special Terminal specific defines
//
//Load only if needed
#ifndef DISP_WIDTH
	#if PHL
		// PHL1700-10 and PHL1700-20
		#define DISP_WIDTH  96
		#define DISP_HEIGHT 48
	#elif PHL1000
		// PHL1300
		#define DISP_WIDTH  96
		#define DISP_HEIGHT 64
	#elif PHL2700
		// PHL2700
		#define DISP_WIDTH  128
		#define DISP_HEIGHT 64
	#elif OPH | OPH1004
		// OPH1000
		#define DISP_WIDTH	112
		#define DISP_HEIGHT 128
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

#define ID_STOPBITS_1		1
#define ID_STOPBITS_2		2

#define ID_PARITY_NONE		1
#define ID_PARITY_EVEN		2
#define ID_PARITY_ODD		3

#define ID_115200			1
#define ID_57600			2
#define ID_38400			3
#define ID_19200			4
#define ID_9600				5
#define ID_4800				6
#define ID_2400				7

#define ID_NO_PROTOCOL		1
#define ID_NETO_PROTOCOL	2
#define ID_OSECOMM_PROTOCOL 3

#if PX25
#define COM0 0
#endif


//************************************************************************
// Special types
//************************************************************************

// barcode record structure
typedef struct
{
	// OLD char barcode[ SZ_BARCODE + 1 ];
	char device[ SZ_DEVICE + 1 ];
	//OLD char quantity[ SZ_SIGN + SZ_QUANTITY + 1 ];
	char wearer[ SZ_WEARER + 1 ];
	char time[ SZ_TIME + 1 ];
	char date[ SZ_DATE + 1 ];
}db_record;


//************************************************************************
// Globals
//************************************************************************
long lPort;		// Serial communication port
long lProtocol;	// The protocol for transmitting the data
long lBarcodes;	// The currently supported barcode types
long lBaudrate;	// The Baudrate
long lParity;	// Parity
long lDatabits;	// Databits
long lStopbits;	// Stopbits
long lDrive;	// Drive (Internal or Flash)

//************************************************************************
// Function implementation
//************************************************************************
void set_stopbits( void )
{
	if( lStopbits == ID_STOPBITS_1 )
		systemsetting("L5");
	else
		systemsetting("L6");
}

void set_databits( void )
{
	if( lDatabits == 7 )
		systemsetting("L0");
	else
		systemsetting("L1");
}

void set_parity( void )
{
	switch( lParity )
	{
		case ID_PARITY_EVEN:
			systemsetting("L3");
			break;
		case ID_PARITY_ODD:
			systemsetting("L4");
			break;
		case ID_PARITY_NONE:
		default:
			systemsetting("L2");
			break;
	}
}

void set_baudrate( void )
{
	switch( lBaudrate )
	{
		case ID_115200:
			systemsetting("SZ");
			break;
		case ID_57600:
			systemsetting("K9");
			break;
		case ID_38400:
			systemsetting("K8");
			break;
		case ID_9600:
			systemsetting("K6");
			break;
		case ID_4800:
			systemsetting("K5");
			break;
		case ID_2400:
			systemsetting("K4");
			break;
		case ID_19200:
		default:
			systemsetting("K7");
			break;
	}
}

void set_barcodes( void )
{

	systemsetting("B0");			// disable all codes

#if OPH | OPH1004 | PX25 | OPH1005
    if( lBarcodes & ID_CD39)
    	systemsetting("B2D7DB"); 	// CODE39, Ital. Ph. if possible, Leading A for Ital. Ph.
    if( lBarcodes & ID_EAN)
    	systemsetting("R4");		// EAN8 and 13
    if( lBarcodes & ID_UPC)
    	systemsetting("R1KP");		// UPC, Disable UPC-E1
    if( lBarcodes & ID_UPCE1)
    	systemsetting("KQ");		// UPC-E1
	if( lBarcodes& ID_I2O5)
		systemsetting("R8");		// Interleaved 2 of 5
    if( lBarcodes & ID_D2O5)
    	systemsetting("R7");		// Industrial 2 of 5
    if( lBarcodes & ID_NW7)
    	systemsetting("B3");		// Codabar
    if( lBarcodes & ID_CD93)
    	systemsetting("B5");		// Code93
    if( lBarcodes & ID_CD128)
    	systemsetting( "B6OG[BL6"); // Code 128, Ean 128 if possible, Dont Transmit EAN-128 composite link flag
   	if( lBarcodes & ID_MSI)
   		systemsetting("B7");		// MSI plessey
    if( lBarcodes & ID_TELEPEN)
        systemsetting("B9JZ");		// Telepen, Tri-Optic
    if( lBarcodes & ID_UK)
        systemsetting("B1");		// UK plessey
    if( lBarcodes & ID_IATA)
        systemsetting("B4");		// IATA
    if( lBarcodes & ID_SCODE)
        systemsetting("R9");		// SCode
    if( lBarcodes & ID_MAT2O5)
        systemsetting("BB");		// Matrix 2 of 5
    if( lBarcodes & ID_CHPOST)
        systemsetting("JS");		// Chinese Post Matrix 2 of 5
    if( lBarcodes & ID_CD11)
        systemsetting("[BLC");		// Code 11
    if( lBarcodes & ID_KOREAN)
        systemsetting("WH");		// Korean Post
    if( lBarcodes & ID_RSS)
        systemsetting("JXJYDR");	// RSS-14, RSS Limited, RSS Expanded
    if( lBarcodes & ID_PDF417)
        systemsetting("[BCF");		// PDF417
    if( lBarcodes & ID_MICROPDF)
        systemsetting("[BCG");		// MicroPDF417
    if( lBarcodes & ID_ADDONS)
        systemsetting("R2R3R5R6JOJPJQJUJVJW");
    if( lBarcodes & ID_DATAMATRIX)
        systemsetting("[BCC");
    if( lBarcodes & ID_AZTEC)
        systemsetting("[BCH");
    if( lBarcodes & ID_QR)
        systemsetting("[BCD");
    if( lBarcodes & ID_MAXICODE)
        systemsetting("[BCE");
	//added by Khaled to enable PostNet & IMB
	if( lBarcodes & ID_POST_NET)
        systemsetting("[D6A");
    if( lBarcodes & ID_IMB)
        systemsetting("[D5F");

									// UPC +2 +5, EAN +2 +5, EAN-8, EAN-8 +2, EAN-8 +5,
#else								// EAN-13, EAN-13 +2, EAN-13 +5
    if( lBarcodes & ID_CD39)
    	systemsetting("B2D7DB"); 	// CODE39, Ital. Ph. if possible, Leading A for Ital. Ph.
    if( lBarcodes & ID_EAN)
    	systemsetting("R4");		// EAN8 and 13
    if( lBarcodes & ID_UPC)
    	systemsetting("R1");		// UPC
	if( lBarcodes& ID_I2O5)
		systemsetting("R8");		// Interleaved 2 of 5
    if( lBarcodes & ID_D2O5)
    	systemsetting("R7");		// Industrial 2 of 5
    if( lBarcodes & ID_NW7)
    	systemsetting("B3");		// Codabar
    if( lBarcodes & ID_CD93)
    	systemsetting("B5");		// Code93
    if( lBarcodes & ID_CD128)
    	systemsetting( "B6OG[BL6"); // Code 128, Ean 128 if possible, Dont Transmit EAN-128 composite link flag
   	if( lBarcodes & ID_MSI)
   		systemsetting("B7");		// MSI plessey
    if( lBarcodes & ID_TELEPEN)
        systemsetting("B9JZ");		// Telepen, Tri-Optic
    if( lBarcodes & ID_UK)
        systemsetting("B1");		// UK plessey
    if( lBarcodes & ID_IATA)
        systemsetting("B4");		// IATA
    if( lBarcodes & ID_SCODE)
        systemsetting("R9");		// SCode
    if( lBarcodes & ID_ADDONS)
        systemsetting("R2R3R5R6H3"); // UPC +2 +5, EAN +2 +5, NW7 ABC CX
#endif
}

#if OPH								// drive selection only available for the OPH-1000
void set_drive ( void )
{
	if (lDrive == DRIVE_A)
		set_default_drive(DRIVE_A);
	else
		set_default_drive(DRIVE_C);
}
#endif

// Calculate the maximum amount of characters that fit on one line
static int GetMaxCharsXPos( void )
{
	unsigned int nHeight, nWidth;
	nHeight = nWidth = 0; // prevent warning toshiba compiler
	getfontsize( &nHeight, &nWidth);

	return (DISP_WIDTH / nWidth);
}

// Calculate the maximum amount of characters that fit on one line
static int GetMaxCharsYPos( void )
{
	unsigned int nHeight, nWidth;
	nHeight = nWidth = 0; // prevent warning toshiba compiler
	getfontsize( &nHeight, &nWidth);

	return (DISP_HEIGHT/ nHeight);
}

void display_input_data( db_record *db_rec )
{
	int nMaxX;

	nMaxX = GetMaxCharsXPos();

	gotoxy(0,1);
	//OLD printf("%-*.*s", nMaxX, nMaxX, db_rec->barcode );
	printf("\nDevice ID:\n %-*.*s\n", nMaxX, nMaxX, db_rec->device );
	//OLD printf("\nQuant: %-*.*s\n", SZ_SIGN+SZ_QUANTITY, SZ_SIGN+SZ_QUANTITY, db_rec->quantity);
	printf("\nCow ID:\n %-*.*s\n", SZ_WEARER, SZ_WEARER, db_rec->wearer);
#ifdef OPH1005
	setfont(LARGE_FONT,NULL);
#else
	setfont( SMALL_FONT, NULL);
#endif
	gotoxy(0,11);
	printf("%-*.*s\n", nMaxX, nMaxX, db_rec->date);
    printf("%-*.*s", nMaxX, nMaxX, db_rec->time);
#ifdef OPH1005
	setfont(USER_FONT,(char*)_vga_15_32);
#else
	setfont( LARGE_FONT, NULL);
#endif
}

// Save the data into the database
void store_input_data( db_record *db_rec, long lRecordNo )
{
	static SDBFile dbFile; // static initializes all items to 0
	static char record[ SZ_RECORD + 1 ];

	// Check if there is enough space available for storing the barcode data
	// We use 5000 because the OS (NetO) also need some memory
	if( coreleft() < 5000L )
	{
		#if OPH | OPH1004
			printf("\fRam disk full\ndata not stored!\n\n\n\n\nPress any key");
		#else
			printf("\fRam disk full\ndata not stored!\n\nPress any key");
		#endif
		WaitForKey();
		return;
	}
	// Make the record to store
	sprintf(record, "%-*.*s,%-*.*s,%-*.*s,%-*.*s\r\n",
				//OLD SZ_BARCODE, SZ_BARCODE, db_rec->barcode,
				SZ_DEVICE, SZ_DEVICE, db_rec->device,
				//OLD SZ_SIGN+SZ_QUANTITY, SZ_SIGN+SZ_QUANTITY, db_rec->quantity,
				SZ_WEARER, SZ_WEARER, db_rec->wearer,
				SZ_TIME, SZ_TIME, db_rec->time,
				SZ_DATE, SZ_DATE, db_rec->date );

	if( !OpenDatabase( (char*)DBASE_NAME, SZ_RECORD, &dbFile ))
	{
		if( !CreateDatabase( (char*)DBASE_NAME, SZ_RECORD, &dbFile ))
		{
#if OPH | OPH1004
				printf("\fError create\nDatabase\nCode=%ld\n\n\n\n\nPress any key", GetDBErrorCode() );
#else
				printf("\fError create\nDatabase\nCode=%ld\nPress any key", GetDBErrorCode() );
#endif
			WaitForKey();
			return;
		}
	}
	if( lRecordNo != -1L )
		GotoRecord( &dbFile, lRecordNo );

	if( !WriteRecord( &dbFile, record, ((lRecordNo==-1L)?WRITE_APPEND:WRITE_OVER)))
	{
		CloseDatabase( &dbFile );
#if OPH | OPH1004 | OPH1005
			printf("\fError write\nrecord\nCode=%ld\n\n\n\n\nPress any key", GetDBErrorCode() );
#else
			printf("\fError write\nrecord\nCode=%ld\nPress any key", GetDBErrorCode() );
#endif
		WaitForKey();
		return;
	}
	if( lRecordNo == -1L )
	{
		// Record was appended, now sort the database
		// to be able to use BinarySearch next time to search the database
		//OLD if( !QuickSort( &dbFile, 0, SZ_BARCODE ))
		if( !QuickSort( &dbFile, 0, SZ_DEVICE ))
		{
#if OPH | OPH1004 | OPH1005
				printf("\fError sort\nrecord\nCode=%ld\n\n\n\n\nPress any key", GetDBErrorCode() );
#else
				printf("\fError sort\nrecord\nCode=%ld\nPress any key", GetDBErrorCode() );
#endif
			WaitForKey();
		}
	}
	CloseDatabase( &dbFile );
}

//OLD long string_quantity_to_long( char* quantity, int* illegal )
long string_wearer_to_long( char* wearer, int* illegal )
{
	//OLD static char tmp[SZ_SIGN+SZ_QUANTITY+1];
	static char tmp[SZ_WEARER+1];
	char* ptr;
	int pos;

	*illegal = FALSE;

	memset( tmp, '\0', sizeof( tmp ));
	ptr = tmp;
	//OLD for( pos = 0; pos < (SZ_SIGN+SZ_QUANTITY); pos++ )
	for( pos = 0; pos < (SZ_WEARER); pos++ )
	{
		//OLD if( quantity[pos] == '\0' )
		if( wearer[pos] == '\0' )
			break;

		//if( quantity[pos] == ' ') // cut off all spaces
		if( wearer[pos] == ' ') // cut off all spaces
		 	continue;

		//OLD  *ptr++ = quantity[pos];
		 *ptr++ = wearer[pos]; 
	}
	if( atol( tmp ) == 0L && tmp[0] == '-' )
		*illegal = TRUE; // illegal value a '-' or -0 are not legal values
	return( atol( tmp ));
}

void fill_record_struct( db_record *db_rec, char* record )
{
	int offset;
	memset( db_rec, '\0', sizeof( db_record ));
	offset = 0;
	//OLD memcpy( db_rec->barcode, record+offset, SZ_BARCODE );
	//OLD offset+=SZ_BARCODE+1;
	memcpy( db_rec->device, record+offset, SZ_DEVICE );
	offset+=SZ_DEVICE+1;
	//OLD memcpy( db_rec->quantity, record+offset, SZ_SIGN+SZ_QUANTITY );
	//OLD offset+= SZ_SIGN+SZ_QUANTITY+1;
	memcpy( db_rec->wearer, record+offset, SZ_WEARER );
	offset+= SZ_WEARER+1;
	memcpy( db_rec->time, record+offset, SZ_TIME );
	offset+= SZ_TIME+1;
	memcpy( db_rec->date, record+offset, SZ_DATE );
}

//long FindBarcodeInDatabase( char *device, char *quantity )
long FindBarcodeInDatabase( char *device, char *wearer )
{
	static SDBFile dbFile;
	static char record[ SZ_RECORD + 1 ];
	static db_record db_rec;
	long lFound = -1L;
	if( !OpenDatabase( (char*)DBASE_NAME, SZ_RECORD, &dbFile ))
		return lFound;
	//OLD if( (lFound = BinarySearch( &dbFile, record, barcode, SZ_BARCODE, 0 )) != -1L )
	if( (lFound = BinarySearch( &dbFile, record, device, SZ_DEVICE, 0 )) != -1L )
	{
		// Barcode was found fill the quantity string
		fill_record_struct( &db_rec, record );
		//OLD strncpy( quantity, db_rec.quantity, SZ_SIGN+SZ_QUANTITY );
		strncpy( wearer, db_rec.wearer, SZ_WEARER );
	}
	CloseDatabase( &dbFile );
	return lFound;
}

void ScanLabels( void )
{
	//OLD static char 		barcode[ SZ_BARCODE + 1 ];
	static char 		device[ SZ_DEVICE + 1 ];	
	//OLD static char 		quantity[ SZ_SIGN + SZ_QUANTITY + 1 ];
	static char 		wearer[ SZ_WEARER + 1 ];
	static db_record	db_rec;
    struct date 		dates;
    struct time 		times;
    long   				lFoundRecord;
    int 				nIllegal;
	int					key;
    //OLD long 				lTotal; // Now lCurrentWearer
    //OLD long 				lAdd; // Now lNewWearer
	long				lCurrentWearer;
	long				lNewWearer;

	//OLD memset( barcode, '\0', sizeof( barcode ));
	memset( device, '\0', sizeof( device ));
	memset( &db_rec, '\0', sizeof( db_record ));

	cursor(NOWRAP);
#if OPH1005
	setfont(USER_FONT,(char*)_vga_15_32);
#else
	setfont( LARGE_FONT, NULL );
#endif

	for(;;)
	{
		//OLD printf("\fScan or type...\n");
		printf("\fDevice ID:\n");
		gotoxy(0,2);
		printf("\nPress Scan\n or type ...");

		// Display some info first
		//OLD key = ScanOrKeyboardInput( barcode, 1, SZ_BARCODE, INPUT_ALL, 0, 1, GetMaxCharsXPos(), GetMaxCharsYPos()-3);
		
		// ScanOrKeyboardInput(param1, param2, param3, param4, param5, param6, param7, param8)
		// where:
		// param1 = char* string; device OK
		// param2 = int min_length; SZ_DEVICE
		// param3 = int max_length; SZ_DEVICE
		// param4 = int typ; INPUT_NUM; allows for numeric input only
		// param5 = int x; 1; one position from left
		// param6 = int y; 1; one position from top
		// param7 = int display_length; 9
		// param8 = int display_height; 1
		key = ScanOrKeyboardInput( device, 1, SZ_DEVICE, INPUT_NUM, 1, 1, GetMaxCharsXPos(), GetMaxCharsYPos()-3);
		if( key == CLR_KEY || key == ESC_KEY )
			return;

		//
		// A new for loop, so that quantity is cancelled
		// the input continues with the barcode input
		//
		for(;;)
		{
			// fill the barcode into the record structure
			//OLD sprintf( db_rec.barcode, "%-*.*s", SZ_BARCODE, SZ_BARCODE, barcode );
			sprintf( db_rec.device, "%-*.*s", SZ_DEVICE, SZ_DEVICE, device );
			//OLD memset( quantity, '\0', sizeof( quantity ));	// clear the whole quantity item
			memset( wearer, '\0', sizeof( wearer ));	// clear the whole wearer item
			//OLD if( (lFoundRecord = FindBarcodeInDatabase( db_rec.barcode, quantity )) != -1L )
			//OLD if( (lFoundRecord = FindBarcodeInDatabase( db_rec.device, quantity )) != -1L )
			//OLD	lTotal = string_quantity_to_long( quantity, &nIllegal );
			if( (lFoundRecord = FindBarcodeInDatabase( db_rec.device, wearer )) != -1L )
				lCurrentWearer = string_wearer_to_long( wearer, &nIllegal );	
			else
				//OLD lTotal = 0;
				lCurrentWearer = 0;

			gotoxy( 0, GetMaxCharsYPos()-2);
			//OLD printf("Total: %*ld\nAdd:         ", SZ_SIGN+SZ_QUANTITY, lTotal);
			printf("Current: %*ld\nNew:         ", SZ_WEARER, lCurrentWearer);

			// Set the quantity to default value
			//OLD strcpy( quantity, "1");
			strcpy( wearer, "1");

			//OLD key = KeyboardNumeric( quantity, 4, INPUT_NUM | INPUT_NEGATIVE | INPUT_SHOW_DEFAULT, 9, GetMaxCharsYPos()-1, 5, 4, CLR_KEY, ESC_KEY, ENT_KEY, TRIGGER_KEY );
			key = KeyboardNumeric( wearer, 4, INPUT_NUM | INPUT_NEGATIVE | INPUT_SHOW_DEFAULT, 9, GetMaxCharsYPos()-1, 5, 4, CLR_KEY, ESC_KEY, ENT_KEY, TRIGGER_KEY );
			if( key == CLR_KEY || key == ESC_KEY )
				break;
				

			//OLD lAdd = string_quantity_to_long( quantity, &nIllegal );
			lNewWearer = string_wearer_to_long( wearer, &nIllegal );
			//OLD if( !nIllegal && lAdd == 0L )
			if( !nIllegal && lNewWearer == 0L )
				//OLD break; // quantity is empty or zero do nothing
				break; // wearer is empty or zero do nothing

			//OLD lTotal += lAdd;
			lCurrentWearer = lNewWearer;

			//OLD if( nIllegal || lTotal < -999999L || lTotal > 9999999L )
			if( nIllegal || lCurrentWearer < -999999L || lCurrentWearer > 9999999L )
			{
#if OPH | OPH1004 | OPH1005
					printf("\fError quantity\n\n\n\n\n\n\nPress any key");
#else
					printf("\fError quantity\n\n\nPress any key");
#endif
				WaitForKey();
				break; // continue with the barcode input
			}
			// When ok fill the quantity of the db_record
			//OLD sprintf( db_rec.quantity, "%*ld", SZ_SIGN+SZ_QUANTITY, lTotal);
			sprintf( db_rec.wearer, "%*ld", SZ_WEARER, lCurrentWearer);

	        gettime( &times );
	        getdate( &dates );
	        sprintf( db_rec.time, "%02d:%02d:%02d", times.ti_hour, times.ti_min, times.ti_sec);
	        sprintf( db_rec.date, "%02d/%02d/%04d", dates.da_day, dates.da_mon, dates.da_year);

	        // Store the input data into our database
	        store_input_data( &db_rec, lFoundRecord );
	        break;
		}
	}
}

void display_scroll_data( db_record *db_rec, long curr, long max )
{
	int nMaxX;
	nMaxX = GetMaxCharsXPos();

	gotoxy(0,0);
	printf("\tr%-*.*s %04ld/%04ld\tr", nMaxX-10, nMaxX-10, "RECORD", curr+1, max);

	display_input_data( db_rec );
}

int get_record( db_record *db_rec, long *rec_nr, long *max_rec )
{
	static SDBFile dbFile; // static initializes all items to 0
	static char record[ SZ_RECORD + 1];

	if( !OpenDatabase( (char*)DBASE_NAME, SZ_RECORD, &dbFile ))
	{
#if OPH | OPH1004 | OPH1005
			printf("\fError open\ndatabase.\nCode=%ld\n\n\n\n\nPress any key", GetDBErrorCode());
#else
			printf("\fError open\ndatabase.\nCode=%ld\nPress any key", GetDBErrorCode());
#endif
		WaitForKey();
		return FALSE;
	}

	*max_rec = GetTotalRecords( &dbFile );

	if( !GotoRecord( &dbFile, *rec_nr ))
	{
#if OPH | OPH1004 | OPH1005
			printf("\fError goto\nrecord.\nCode=%ld\n\n\n\n\nPress any key", GetDBErrorCode());
#else
			printf("\fError goto\nrecord.\nCode=%ld\nPress any key", GetDBErrorCode());
#endif
		WaitForKey();
		return FALSE;
	}

	if( !ReadCurrentRecord( &dbFile,  record ))
	{
#if OPH | OPH1004 | OPH1005
			printf("\fError read\nrecord.\nCode=%ld\n\n\n\n\nPress any key", GetDBErrorCode());
#else
			printf("\fError read\nrecord.\nCode=%ld\nPress any key", GetDBErrorCode());
#endif
		WaitForKey();
		return FALSE;
	}
	CloseDatabase( &dbFile );

	fill_record_struct( db_rec, record );
	return TRUE;
}


void ScrollDatabase( void )
{
	long current;
	long max = 0L;
	static db_record db_rec;

	if( fsize((char*)DBASE_NAME) == -1L )
	{
#if OPH | OPH1004 | OPH1005
			printf("\fDatabase\nnot available\n\n\n\n\n\nPress any key");
#else
			printf("\fDatabase\nnot available\n\nPress any key");
#endif
		WaitForKey();
		return;
	}

	current = 0L;
	if( !get_record( &db_rec, &current, &max )  )
		return;

	putchar('\f');
	for(;;)
	{
		display_scroll_data( &db_rec, current, max );
		switch( WaitForKey() )
		{


			case DOWN_KEY:
#if !OPH1005
			case RIGHT_KEY:
#endif
#if !(OPH | OPH1004 | PX25 | OPH1005)
			case Q2_KEY:
			case F6_KEY:
			case F8_KEY:
#endif

				if(++current >= max )
					current = max-1;
				break;
			case UP_KEY:
#if !OPH1005
			case LEFT_KEY:
#endif
#if !(OPH | OPH1004 | PX25 | OPH1005)
			case Q1_KEY:
			case F5_KEY:
			case F7_KEY:
#endif

				if( --current < 0 )
					current = 0;
				break;
			case TRIGGER_KEY:
			case CLR_KEY:
			case ESC_KEY:
				return;
		}
		if( !get_record( &db_rec, &current, &max ))
			return;
	}
}

void SelectPort( void )
{
	int  nMaxItems;

	sSelMenu mnuSelPort[] =
	{
		{"Exit",              -1},
#if OPH1005
		{"IrDA",			COM2},
		{"USB",				COM9}
#else
		{"IrDA",			COM1},
		{"Cradle",			COM2},
		{"Direct cable",	COM0}
#endif
	};
	nMaxItems = sizeof( mnuSelPort ) / sizeof( sSelMenu );

#if PHL1000 | OPH | OPH1004
	nMaxItems--;	// Do not show direct cable
#endif

	ShowGraphSelectionMenu( mnuSelPort, nMaxItems, MENU_SINGLE, &lPort);
}

void SelectBaudrate( void )
{
	int  nMaxItems;

	sSelMenu mnuSelBaudrate[] =
	{
	    {"Exit",               -1},
		{"115200", 		ID_115200},
		{"57600",		ID_57600},
		{"38400",		ID_38400},
		{"19200",		ID_19200},
		{"9600",		ID_9600},
		{"4800",		ID_4800},
		{"2400",		ID_2400}
	};

	nMaxItems = sizeof( mnuSelBaudrate ) / sizeof( sSelMenu );

#if PX25
	nMaxItems-=2;	// Do not show 2400 and 4800 baud
#endif

	ShowGraphSelectionMenu( mnuSelBaudrate, nMaxItems, MENU_SINGLE, &lBaudrate);
	set_baudrate();
}

void SelectParity( void )
{

	sSelMenu mnuSelParity[] =
	{
	    {"Exit",                    -1},
		{"None", 		ID_PARITY_NONE},
		{"Even", 		ID_PARITY_EVEN},
		{"Odd", 		ID_PARITY_ODD}
	};
	ShowGraphSelectionMenu( mnuSelParity, sizeof( mnuSelParity ) / sizeof( sSelMenu ), MENU_SINGLE, &lParity);
	set_parity();
}

void SelectDatabits( void )
{

	sSelMenu mnuSelDatabits[] =
	{
	    {"Exit",       -1},
		{"7 databits", 	7},
		{"8 databits", 	8}
	};
	ShowGraphSelectionMenu( mnuSelDatabits, sizeof( mnuSelDatabits ) / sizeof( sSelMenu ), MENU_SINGLE, &lDatabits);
	set_databits();
}

void SelectStopbits( void )
{

	sSelMenu mnuSelStopbits[] =
	{
	    {"Exit",                   -1},
		{"1 stopbit", 	ID_STOPBITS_1},
		{"2 stopbits", 	ID_STOPBITS_2}
	};
	ShowGraphSelectionMenu( mnuSelStopbits, sizeof( mnuSelStopbits ) / sizeof( sSelMenu ), MENU_SINGLE, &lStopbits);
	set_stopbits();
}

void ComPortSettings( void )
{
#if PX25
	sgraphMenu mnuComm[] =
    {
	    {"Exit",         NULL,      NULL      },
        {"Baudrate",	_flash,		SelectBaudrate},
    };
#elif OPH1005
	sgraphMenu mnuComm[] =
    {
	    {"Exit",         NULL,      	NULL      },
        {"Port",		_com_port_pic,	SelectPort},
        {"Baudrate",	_baudrate_pic,	SelectBaudrate},
        {"Parity",		_parity_pic,	SelectParity},
        {"Databits",	_data_bits_pic,	SelectDatabits},
        {"Stopbits",	_stop_bits_pic,	SelectStopbits}
    };
#else
	sgraphMenu mnuComm[] =
    {
	    {"Exit",         NULL,      NULL      },
        {"Port",		_comp,		SelectPort},
        {"Baudrate",	_flash,		SelectBaudrate},
        {"Parity",		_parity,	SelectParity},
        {"Databits",	_databits,	SelectDatabits},
        {"Stopbits",	_stop,		SelectStopbits}
    };
#endif
    ShowGraphMenu( mnuComm, sizeof( mnuComm )/sizeof( sgraphMenu) );
}

void SelectProtocol( void )
{

	sSelMenu mnuSelProtocol[] =
	{
	    {"Exit",                            -1},
		{"No protocol", 		ID_NO_PROTOCOL},
		{"NetO protocol",		ID_NETO_PROTOCOL},
		{"OseComm protocol",	ID_OSECOMM_PROTOCOL}
	};
	ShowGraphSelectionMenu( mnuSelProtocol, sizeof( mnuSelProtocol ) / sizeof( sSelMenu ), MENU_SINGLE, &lProtocol);
}

void ChangeContrast( void )
{
#if !OPH1005
	int nMaxX;
    int value;

    nMaxX = GetMaxCharsXPos() >> 1;
    value = contrast(-1);
    putchar('\f');

#if OPH | OPH1004 | PX25
	printf("\tr    CHANGE    \tr\n   CONTRAST\n\n\n\n\n");
    printf("Q1: Darker\nQ2: Lighter");
#else
    printf("%-*.*s\tr%-*.*s \tr\n\tr%-*.*s\tr%-*.*s \n", nMaxX, nMaxX, "CHANGE", nMaxX, nMaxX, "CONTRAST", nMaxX, nMaxX, "CHANGE", nMaxX, nMaxX, "CONTRAST");
    printf("Using: Q1 and Q2");
#endif
    for(;;)
    {
    	switch( WaitForKeys(6, Q1_KEY, Q2_KEY, CLR_KEY, ESC_KEY, ENT_KEY, TRIGGER_KEY ))
    	{
    		case Q1_KEY:

#if PHL2700
    			if( ++value > 40 )
    				value = 40;
#else
    			if( ++value > 31 )
    				value = 31;
#endif
    			break;
    		case Q2_KEY:
    			if( --value < 0 )
    				value = 0;
    			break;

    		case CLR_KEY:
			case ESC_KEY:
    		case ENT_KEY:
    		case TRIGGER_KEY:
    			return;
    	}
    	contrast( value );
    }
#endif
}

void SetBarcodes( void )
{
#if PX25
	sSelMenu mnuSelBarcodes[] =
	{
	    {"Exit",                -1},
        {"UPC            ", ID_UPC},
        {"EAN            ", ID_EAN},
		{"Code 39        ", ID_CD39},
        {"Codabar        ", ID_NW7},
        {"Indust. 2of5   ", ID_D2O5},
        {"Interl. 2of5   ", ID_I2O5},
        {"S-Code         ", ID_SCODE},
		{"Matrix 2of5    ", ID_MAT2O5},
		{"Chinese Post   ", ID_CHPOST},
        {"IATA Code      ", ID_IATA},
        {"MSI            ", ID_MSI},
        {"Telepen        ", ID_TELEPEN},
        {"UK/Plessey     ", ID_UK},
        {"Code 128       ", ID_CD128},
        {"Code 93        ", ID_CD93},
		{"Code 11        ", ID_CD11},
		{"Korean Post    ", ID_KOREAN},
		{"RSS            ", ID_RSS},
		{"PDF417         ", ID_PDF417},
		{"MicroPDF417    ", ID_MICROPDF},
        {"All Add-Ons    ", ID_ADDONS},
		{"Datamatrix     ", ID_DATAMATRIX},
		{"Maxicode	     ", ID_MAXICODE},
		{"Aztec          ", ID_AZTEC},
		{"QR Code        ", ID_QR},
		{"PostNet        ", ID_POST_NET},
		{"IMB            ", ID_IMB},
    };
#elif OPH | OPH1004 | OPH1005
	sSelMenu mnuSelBarcodes[] =
	{
	    {"Exit",                -1},
        {"UPC            ", ID_UPC},
        {"UPC-E1         ", ID_UPCE1},
        {"EAN            ", ID_EAN},
		{"Code 39        ", ID_CD39},
        {"Codabar        ", ID_NW7},
        {"Indust. 2of5   ", ID_D2O5},
        {"Interl. 2of5   ", ID_I2O5},
        {"S-Code         ", ID_SCODE},
		{"Matrix 2of5    ", ID_MAT2O5},
		{"Chinese Post   ", ID_CHPOST},
        {"IATA Code      ", ID_IATA},
        {"MSI            ", ID_MSI},
        {"Telepen        ", ID_TELEPEN},
        {"UK/Plessey     ", ID_UK},
        {"Code 128       ", ID_CD128},
        {"Code 93        ", ID_CD93},
		{"Code 11        ", ID_CD11},
		{"Korean Post    ", ID_KOREAN},
		{"RSS            ", ID_RSS},
		{"PDF417         ", ID_PDF417},
		{"MicroPDF417    ", ID_MICROPDF},
        {"All Add-Ons    ", ID_ADDONS}
    };
#else
		sSelMenu mnuSelBarcodes[] = {
		"Exit",              -1   ,
        "Code 39        ", ID_CD39,
        "EAN            ", ID_EAN,
        "UPC            ", ID_UPC,
        "Interl. 2of5   ", ID_I2O5,
        "Indust. 2of5   ", ID_D2O5,
        "Codabar        ", ID_NW7,
        "Code 93        ", ID_CD93,
        "Code 128       ", ID_CD128,
        "MSI            ", ID_MSI,
        "Telepen        ", ID_TELEPEN,
        "UK/Plessey     ", ID_UK,
        "IATA Code      ", ID_IATA,
        "S-Code         ", ID_SCODE,
        "All Add-Ons    ", ID_ADDONS
    };
#endif
    ShowGraphSelectionMenu( mnuSelBarcodes, sizeof( mnuSelBarcodes ) / sizeof( sSelMenu ), MENU_MULTIPLE, &lBarcodes);
    set_barcodes();	// Set the new selected barcodes
}

void AvailableMemory( void )
{
   	long mem_size;
	mem_size = coreleft();
#if OPH1005
	printf("\fMEMORY LEFT:\n%ld MBytes+\n%ld Kbytes\n\n\n\n\nPress any key", mem_size/(1024*1024), (mem_size % (1024*1024)) / 1024);

#elif OPH | OPH1004
		printf("\fMEMORY LEFT:\n%ld Bytes\n\n\n\n\n\nPress any key", mem_size);
#else
		printf("\fMEMORY LEFT:\n%ld Bytes\n\nPress any key", mem_size);
#endif
	WaitForKey();
}


#if OPH								// drive selection only available for the OPH-1000
void SetDrive ( void )
{
	int nIndex;

	sSelMenu mnuSelDrive[] =
	{
	    {"Exit",                 -1},
		{"Internal RAM", 	DRIVE_A},
		{"Memory Card",		DRIVE_C}
	};
	nIndex = 0;
	ShowGraphSelectionMenu( mnuSelDrive, sizeof( mnuSelDrive ) / sizeof( sSelMenu ), MENU_SINGLE, &lDrive);
	set_drive();
}

void _SystemMenu( void )
{
	sgraphMenu mnuSystem[] =
	{
	    {"Exit",         NULL,      NULL           },
		{"Com Port",	_comport,	ComPortSettings},
		{"Protocol",	_protocol, 	SelectProtocol},
		{"Display",		_display,	ChangeContrast},
		{"Barcodes",	_barcode, 	SetBarcodes},
		{"Memory",		_memory, 	AvailableMemory},
		{"Drive",		_drive,		SetDrive}
	};

	ShowGraphMenu( mnuSystem, sizeof( mnuSystem ) / sizeof( sgraphMenu ));
}

#elif OPH1005
// Do not show Display, no contrast function on color display.
void _SystemMenu( void )
{
	sgraphMenu mnuSystem[] =
	{
	    {"Exit",         NULL,      	NULL           },
		{"Com Port",	_com_port_pic,	ComPortSettings},
		{"Protocol",	_protocol_pic, 		SelectProtocol},
		{"Barcodes",	_barcode_pic,	SetBarcodes},
		{"Memory",		_memory_pic, 	AvailableMemory}
	};

	ShowGraphMenu( mnuSystem, sizeof( mnuSystem ) / sizeof( sgraphMenu ));
}
#else
// Do not show drive. Only available on OPH-1000
void _SystemMenu( void )
{
	sgraphMenu mnuSystem[] =
	{
	    {"Exit",         NULL,      NULL           },
		{"Com Port",	_comport,	ComPortSettings},
		{"Protocol",	_protocol, 	SelectProtocol},
		{"Display",		_display,	ChangeContrast},
		{"Barcodes",	_barcode, 	SetBarcodes},
		{"Memory",		_memory, 	AvailableMemory}
	};

	ShowGraphMenu( mnuSystem, sizeof( mnuSystem ) / sizeof( sgraphMenu ));
}
#endif

void DeleteDatabase( void )
{
	int key;
	#if OPH | OPH1004
		printf("\fDelete\nDatabase?\n\n\n\n\n[ENTER] = YES\n[CLEAR] = NO");
	#else
		printf("\fDelete Database\n\n[ENT] = YES\n[CLR] = NO");
	#endif
	key = WaitForKeys( 4, ENT_KEY, TRIGGER_KEY, CLR_KEY, ESC_KEY );
	if( key == ENT_KEY || key == TRIGGER_KEY )
		remove(DBASE_NAME );
}

#if PX25 | OPH1004 | OPH1005
void OnOseComInfo(int status, int errorsuccess, int progress, const char *info)
{
    switch (status)
    {
    case STAT_GET_TIME_DATE:
        break;
    case STAT_SET_TIME_DATE:
        break;
    case STAT_GET_OS_VERSION:
        break;
    case STAT_GET_APPL_VERSION:
        break;
    case STAT_XMIT_FILE_TO_PC:
        // if successful then delete the file
        if (errorsuccess == SUCC_COMPLETE)
            remove(info);    // Delete the transfered file
        break;
    case STAT_RECV_FILE_FROM_PC:
        break;
    case STAT_LIST_FILES:
        break;
    }
}
#endif

void TransmitData( void )
{
	int nRet;
	static SDBFile dbFile; // static initializes all items to 0
	static char record[ SZ_RECORD + 1];

	if( fsize((char*)DBASE_NAME) == -1L )
	{
#if OPH | OPH1004 | OPH1005
			printf("\fDatabase\nnot available\n\n\n\n\n\nPress any key");
#else
			printf("\fDatabase\nnot available\n\nPress any key");
#endif
		WaitForKey();
		return;
	}
	if( comopen( (unsigned int)lPort ) != OK )
	{
#if OPH | OPH1004 | OPH1005
			printf("\fError open\nCOM port\n\n\n\n\n\nPress any key");
#else
			printf("\fError open\nCOM port\n\nPress any key");
#endif
		WaitForKey();
		return;
	}
	printf("\fTransmit data\n");
	if( lProtocol == ID_NETO_PROTOCOL )
	{
		printf("NetO protocol\n\n\n\n\n\nScan to cancel");
		nRet = neto_transmit( (char(*)[12+1+3])DBASE_NAME, 1, "123456", TRIGGER_KEY, 3 );
		cursor( NOWRAP );
		if( nRet != OK )
		{
#if OPH | OPH1004 | OPH1005
				printf("\fError NetO\nCode=%d\n\n\n\n\n\nPress any key", nRet);
#else
				printf("\fError NetO\nCode=%d\n\nPress any key", nRet);
#endif
			WaitForKey();
		}
	}
#if PX25 | OPH1004 | OPH1005
	else if( lProtocol == ID_OSECOMM_PROTOCOL)
	{
		printf("\fStart OseComm");
		if (OseComm( -1L, ANY_KEYS_ABORT, "OseComm example", OnOseComInfo) == COMM_OK)
			printf("\nOseComm Success");
		else
			printf("\nOseComm aborted");
		WaitForKey();
	}
#endif
	else
	{
		if( !OpenDatabase( (char*)DBASE_NAME, SZ_RECORD, &dbFile ))
		{
			#if OPH | OPH1004
				printf("\fError open\ndatabase.\nCode=%ld\n\n\n\n\nPress any key", GetDBErrorCode());
			#else
				printf("\fError open\ndatabase.\nCode=%ld\nPress any key", GetDBErrorCode());
			#endif
			WaitForKey();
		}
		else
		{
			if( ReadFirstRecord( &dbFile, record ))
			{
				putchar('\f');
				do
				{
					gotoxy(0,0);
					printf("Send %04ld/%04ld", dbFile.lCurrRecord+1, dbFile.lTotalRecords);
					for( nRet = 0; nRet < SZ_RECORD; nRet++ )
			            putcom( record[ nRet ]);
        			delay(10);

				}while( ReadNextRecord( &dbFile, record ) );
			}
			else
			{
				#if OPH | OPH1004
					printf("\fError\ndatabase.\nCode=%ld\n\n\n\n\nPress any key", GetDBErrorCode());
				#else
					printf("\fError\ndatabase.\nCode=%ld\nPress any key", GetDBErrorCode());
				#endif
				WaitForKey();
			}
			CloseDatabase( &dbFile );
		}
	}
	comclose( (unsigned int) lPort );
}

void ShowVersion( void )
{
	static char os[ 8 + 1 ];
#if OPH1005
	int x,y,x1,y1;
	int count;
#endif
	version_os(os);
    os[3] = '\0';
    os[1] = 'F';

#if OPH | OPH1004 | OPH1005
		printf("\f\tr    CowAlert    \tr\nSoftware\nVersion #");
		printf("\n\n%s%s\n\n\nPress any key", os,VERSIONSUFFIX);
#else
		printf("\fSoftware Version");
		printf("\n%s%s\nPress any key", os,VERSIONSUFFIX);
#endif
#if OPH1005
	for(count=0;count < (sizeof(logo_pix)/sizeof(struct logo));count++)
	{
		x=logo_pix[count].x;
		y=logo_pix[count].y;
		if (logo_pix[count].action == 2)
			drawline(5+x1/40,270+y1/40,5+x/40,270+y/40,0x3399FF);
		x1=x;
		y1=y;
	}

#endif
    WaitForKey();
}


void ShowFonts(void)
{
#if OPH1005
	int x,y;

	setfont(HUGE_FONT,NULL);
	x=GetMaxCharsXPos();
	y=GetMaxCharsYPos();
	gotoxy(0,0);
	SetTextColor(0xFF3333);
	printf("\fHuge Font\n %d chars * %d lines",x,y);

	setfont(LARGE_FONT,NULL);
	gotoxy(0,3);
	x=GetMaxCharsXPos();
	y=GetMaxCharsYPos();
	SetTextColor(0x33FF33);
	printf("Large Font\n%d chars * %d lines",x,y);

	setfont(MEDIUM_FONT,NULL);
	gotoxy(0,7);
	x=GetMaxCharsXPos();
	y=GetMaxCharsYPos();
	SetTextColor(0x3333FF);
	printf("Medium Font\n%d chars * %d lines",x,y);

	setfont(SMALL_FONT,NULL);
	x=GetMaxCharsXPos();
	y=GetMaxCharsYPos();
	SetTextColor(0xFF8000);
	gotoxy(0,16);
	printf("Small Font\n%d chars * %d lines",x,y);

	setfont(TINY_FONT,NULL);
	x=GetMaxCharsXPos();
	y=GetMaxCharsYPos();
	SetTextColor(0xFF00FF);
	gotoxy(0,29);
	printf("Tiny Font\n%d chars * %d lines",x,y);
	WaitForKey();
	SetTextColor(0xFFFFFF);
	setfont(USER_FONT,(char*)_vga_15_32);

#elif OPH | OPH1004

#if OPH
	setfont(EXTRA_LARGE_FONT,NULL);
	printf("\fExtraLarge\n8 chars * 8 lines");
#endif
#if OPH1004
	setfont(HUGE_FONT,NULL);
	printf("\fExtraLarge\n10 chars * 5 lines");
#endif
	setfont(LARGE_FONT,NULL);
	gotoxy(0,2);
	printf("Large Font\n14 chars * 8 lines");
	setfont(MEDIUM_FONT,NULL);
	gotoxy(0,5);
	printf("Medium Font\n18 chars * 10 lines");
	setfont(SMALL_FONT,NULL);
	gotoxy(0,11);
	printf("Small Font\n14 chars * 16 lines");
	setfont(TINY_FONT,NULL);
	gotoxy(0,18);
	printf("Tiny Font\n18 chars * 21 lines");
	WaitForKey();
#endif

}

void main( void )
{

#if OPH1005
	sgraphMenu mnuMain[] =
	{
	    {"Scan Labels",			_barcode_pic,		ScanLabels},
		{"Scroll DBase",		_open_file_pic,		ScrollDatabase},
		{"Transmit Data",  		_wireless_pic,		TransmitData},
		{"Delete DBase",		_recycle_pic,		DeleteDatabase},
		{"System Menu", 		_setting_pic,		_SystemMenu},
		{"Show fonts",          _folder_fonts_pic,	ShowFonts},
		{"Version",     		_info_pic,			ShowVersion}
	};
#elif OPH | OPH1004
	sgraphMenu mnuMain[] =
	{
	    {"Scan Labels",			_scan,		ScanLabels},
		{"Scroll DBase",		_scroll,	ScrollDatabase},
		{"Transmit Data",  		_transmit,	TransmitData},
		{"Delete DBase",		_trash,		DeleteDatabase},
		{"System Menu", 		_tools,		_SystemMenu},
		{"Show fonts",          _fonts,     ShowFonts},
		{"Version",     		_version,	ShowVersion}
	};
#else
	sgraphMenu mnuMain[] =
	{
	   	{"Scan Labels",			_scan,		ScanLabels},
		{"Scroll Database",		_scroll,	ScrollDatabase},
		{"Transmit Data",  		_transmit,	TransmitData},
		{"Delete Database",		_trash,		DeleteDatabase},
		{"System Menu", 		_tools,		_SystemMenu},
		{"Version",     		_version,	ShowVersion}
	};
#endif

#if PHL1000
	cursor( NOICONS );
#endif

#ifdef OPH1005
//	SetBackColor(0xFFFFFF);
//	SetTextColor(0x0094da);
#endif

	// Set some default values
	lPort = COM9; // USB port is used as default
	lProtocol = ID_NETO_PROTOCOL;

#if OPH | OPH1004 | PX25 | OPH1005 | OPH3000
	lBarcodes = ID_CD39;  // Code 39 is set as default barcode
	lBaudrate = ID_19200; // 19200 is set as default baudrate
#ifdef OPH
	lDrive = DRIVE_A;
#endif
#if PX25
	lBarcodes |= ID_DATAMATRIX | ID_MAXICODE | ID_AZTEC | ID_QR;
#endif
#else
	lBarcodes = ID_CD39 | ID_EAN | ID_UPC | ID_I2O5 | ID_D2O5 | ID_NW7 | ID_CD93 | ID_CD128;
	lBarcodes |= ID_MSI | ID_TELEPEN | ID_UK | ID_IATA | ID_SCODE;
	lBaudrate = ID_19200;
#endif
	lParity = ID_PARITY_NONE;
	lDatabits = 8;
	lStopbits = ID_STOPBITS_1;

	set_barcodes();			// set the enabled barcodes

#if !PX25
	set_stopbits();			// set the number of stopbits
	set_databits();			// set the number of databits
	set_parity();			// set the parity
#endif
	set_baudrate();			// set the baudrate
#if OPH
	set_drive();			// set the default drive
#endif

#if OPH1005 | OPH3000
	systemsetting("*Q");	// shift key handled by OS
#endif
	systemsetting("7G");	// Charging indication LED enabled

	InitGraphMenu();

	for(;;)
	{
		ShowGraphMenu( mnuMain, sizeof( mnuMain ) / sizeof( sgraphMenu ));
	}
}