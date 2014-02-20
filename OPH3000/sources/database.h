//
// database.h
//
// header file of some fixed record size
// database functions
//
// Martin Jansen
//
// For questions:
// email:	support@opticon.com
// web:		www.opticon.com
//
// 25/07/2005: 	Bug fix in DeleteRecord() function, The Total record counter was not decreased.
//
// 23/12/2005:	Added a reset of the database error code in the function IsFileOpen
//

#ifndef __DATABASE_H__
#define __DATABASE_H__

//
// This is really the handle that is returned by open/create database
//
typedef struct
{
	short	sRecSz;				// record size of database
	int		fd;					// real handle to the open file;
	long	lCurrRecord;		// current record number
	long	lTotalRecords;		// total amount of records
	int		bOpen;				// check to see if db is open or closed
}SDBFile;

//
// Write flasg defines
//
#define WRITE_OVER		1	// Overwrite the current record
#define WRITE_APPEND	2	// Append record to the end of database

//
// database error codes
//
#define DB_OK					0x00000000	// No error ALL OK

#define DB_ERROR_ALREADY_OPEN   0x00000001	// Error Already Open
#define DB_ERROR_OPEN			0x00000002	// Error opening file open failed
#define DB_ERROR_CREATE			0x00000003	// Error creating file
#define DB_ERROR_NOT_EXIST		0x00000004	// Error DB file does not exist
#define	DB_ERROR_NOT_OPEN		0x00000005	// Error Database is not open
#define DB_ERROR_EMPTY			0x00000006	// Error Database is empty

#define DB_ERROR_RECORD_SIZE	0x00000010	// Error filesize and record size do not match
#define DB_ERROR_INVALID_REC_NO 0x00000011	// Invalid record number is requested
#define DB_ERROR_RECORD_JMP		0x00000012  // Failed jumping to the requested record
#define DB_ERROR_CHANGE_SIZE	0x00000013  // Error changing the size of the database

#define DB_ERROR_READ_FILE		0x00000020	// Error reading from file

#define DB_ERROR_WRITE_FILE		0x00000030	// Error writing to file
#define DB_ERROR_INVALID_WFLAG	0x00000031	// Invalid write flag

#define DB_ERROR_MEM			0x00000050	// Error allocating memory for HDBFILE, or for buffers

#define DB_ERROR_SORTLENGTH		0x00000100	// Offset and sort length are larger then record size

#define DB_ERROR_NOT_FOUND		0x00001000	// Error string not found in any record of the database

//-----------------------------------------------------------------------------
// Purpose:     Return the last occurred error code
//
// Parameters:  None
//
// Returns:     long		- error code see description of the error codes the defines above
//
long GetDBErrorCode( void );


// ++++++++++++++++++++++++++++++++++++++
// Database open close functions
// ++++++++++++++++++++++++++++++++++++++

//-----------------------------------------------------------------------------
// Purpose:     Open an existing database file
//
// Parameters:  filename	- database file name
//
//              recordsize	- length of one record (remember <CR><LF>
//
//				dbFile		- returns the pointer to the database handle
//
// Returns:     TRUE on success, FALSE on FAILURE
//
int OpenDatabase( const char *filename, short recordsize, SDBFile *dbFile );

//-----------------------------------------------------------------------------
// Purpose:     Create a database file or overwrite an existing one.
//
// Parameters:  filename	- database file name
//
//              recordsize	- length of one record (remember <CR><LF>
//
//				dbFile		- returns the pointer to the database handle
//
// Returns:     TRUE on success, FALSE on FAILURE
//
int CreateDatabase( const char *filename, short recordsize, SDBFile *dbFile );

//-----------------------------------------------------------------------------
// Purpose:     Close an open database file
//
// Parameters:  dbFile		- pointer to an open database handle
//
//
// Returns:     None
//
void CloseDatabase( SDBFile *dbFile );


// ++++++++++++++++++++++++++++++++++++++++
// Record reading scrolling functionality
// ++++++++++++++++++++++++++++++++++++++++

//-----------------------------------------------------------------------------
// Purpose:     Get the current record number
//
// Parameters:  dbFile		- pointer to an open database handle
//
// Returns:     long		- The current record number, -1L error
//
long GetCurrentRecord( SDBFile *dbFile );

//-----------------------------------------------------------------------------
// Purpose:     Get the total amount of records in the database
//
// Parameters:  dbFile		- pointer to an open database handle
//
// Returns:     long		- The total amount of records in the database, -1L error
//
long GetTotalRecords( SDBFile *dbFile );

//-----------------------------------------------------------------------------
// Purpose:     Jump to a specific record number in the database
//
// Parameters:  dbFile		- pointer to an open database handle
//
//				recordnumber - The record number to jump to
//
// Returns:     TRUE on success, FALSE on FAILURE
//
int GotoRecord( SDBFile *dbFile, long recordnumber );

//-----------------------------------------------------------------------------
// Purpose:     Read the current record number
//
// Parameters:  dbFile		- pointer to an open database handle
//
//				record		- pointer to record buffer
//
// Returns:     TRUE on success, FALSE on FAILURE
//
int ReadCurrentRecord( SDBFile *dbFile, char* record );

//-----------------------------------------------------------------------------
// Purpose:     Read the First record in the database
//
// Parameters:  dbFile		- pointer to an open database handle
//
//				record		- pointer to record buffer
//
// Returns:     TRUE on success, FALSE on FAILURE
//
int ReadFirstRecord( SDBFile *dbFile, char* record );

//-----------------------------------------------------------------------------
// Purpose:     Read the Last record in the database
//
// Parameters:  dbFile		- pointer to an open database handle
//
//				record		- pointer to record buffer
//
// Returns:     TRUE on success, FALSE on FAILURE
//
int ReadLastRecord( SDBFile *dbFile, char* record );

//-----------------------------------------------------------------------------
// Purpose:     Read the Previous record in the database
//
// Parameters:  dbFile		- pointer to an open database handle
//
//				record		- pointer to record buffer
//
// Returns:     TRUE on success, FALSE on FAILURE
//
int ReadPreviousRecord( SDBFile *dbFile, char* record );

//-----------------------------------------------------------------------------
// Purpose:     Read the next record in the database
//
// Parameters:  dbFile		- pointer to an open database handle
//
//				record		- pointer to record buffer
//
// Returns:     TRUE on success, FALSE on FAILURE
//
int ReadNextRecord( SDBFile *dbFile, char* record );

//-----------------------------------------------------------------------------
// Purpose:     Delete one record from the database
//
// Parameters:  dbFile		- pointer to an open database handle
//
//				recordnumber - The record number to delete
//
// Returns:     TRUE on success, FALSE on FAILURE
//
int DeleteRecord( SDBFile *dbFile, long recordnumber );

//-----------------------------------------------------------------------------
// Purpose:     overwrite or append a record in the database
//
// Parameters:  dbFile		- pointer to an open database handle
//
//				record		- pointer to record buffer
//
// Returns:     TRUE on success, FALSE on FAILURE
//
int WriteRecord( SDBFile *dbFile, char* record, int iFlag );


// ++++++++++++++++++++++++++++++++++++++++++
// Sorting functions
// ++++++++++++++++++++++++++++++++++++++++++

//-----------------------------------------------------------------------------
// Purpose:     Sort the file with insertion sort method
//				Insertion sort is a very good sort algorithm for small changes to a database
//
// Parameters:  dbFile		- pointer to an open database handle
//
//				offset		- start from what position in record sorting
//
//				checksize	- length of sorting part of database e.g. EAN8 is only 8 characters
//
// Returns:     TRUE on success, FALSE on FAILURE
//
int InsertionSort( SDBFile *dbFile, short offset, short checksize );

//-----------------------------------------------------------------------------
// Purpose:     Sort the file with heap sort method
//				Heap sort works good on not sorted databases
//
// Parameters:  dbFile		- pointer to an open database handle
//
//				offset		- start from what position in record sorting
//
//				checksize	- length of sorting part of database e.g. EAN8 is only 8 characters
//
// Returns:     TRUE on success, FALSE on FAILURE
//
int HeapSort( SDBFile *dbFile, short offset, short checksize );

//-----------------------------------------------------------------------------
// Purpose:     Sort the file with heap sort method
//				Quick sort works very good on not sorted databases
//				Faster then HeapSort but uses more resources
//
// Parameters:  dbFile		- pointer to an open database handle
//
//				offset		- start from what position in record sorting
//
//				checksize	- length of sorting part of database e.g. EAN8 is only 8 characters
//
// Returns:     TRUE on success, FALSE on FAILURE
//
int QuickSort( SDBFile *dbFile, short offset, short checksize );

// +++++++++++++++++++++++++++++++++++++++++
// Searching  functions
// +++++++++++++++++++++++++++++++++++++++++

//-----------------------------------------------------------------------------
// Purpose:     Find the search key in the sorted database
//
// Parameters:  dbFile		- pointer to an open database handle
//
//				record		- pointer to record buffer
//
//				searchkey	- the string to search for in the database
//
//				checksize	- the length of the searchkey
//
//				offset		- how many positions to the right must the searchstring be compared
//
// Returns:     record number on success, -1L on FAILURE
//
long BinarySearch( SDBFile *dbFile, char* record, char* searchkey, int checksize, int offset );

//-----------------------------------------------------------------------------
// Purpose:     Find the search key in a database (slow but does not need to be sorted )
//
// Parameters:  dbFile		- pointer to an open database handle
//
//				record		- pointer to record buffer
//
//				searchkey	- the string to search for in the database
//
//				checksize	- the length of the searchkey
//
//				offset		- how many positions to the right must the searchstring be compared
//
// Returns:     record number on success, -1L on FAILURE
//
long LineairSearch( SDBFile *dbFile, char* record, char* searchkey, int checksize, int offset );

// +++++++++++++++++++++++++++++++++++++++++
// Special index functions
// +++++++++++++++++++++++++++++++++++++++++

//-----------------------------------------------------------------------------
// Purpose:     Make a sorted indexed file from a not sorted database for fast searching in non sorted databases
//
// Parameters:  dbFile		- pointer to an open database handle to make index from
//
//				offset		- how many positions to the right to start making index
//
//				keysize		- the size of the search key to make the index on
//
//				indexfilename - name of the to create index file
//
//				dbIndex		- On success holds a pointer to open index file
//
// Remark:		dbIndex needs to be closed with CloseDatabase( SDBFile *dbFile );
//
// Returns:     TRUE on success, FALSE on FAILURE
//
int CreateIndexFile( SDBFile *dbFile, short offset, short keysize, const char* indexfilename, SDBFile *dbIndex );

//-----------------------------------------------------------------------------
// Purpose:     Opens an existing index database file
//
// Parameters:  indexfilename - index database file name
//
//              keysize		- length of keysize to search for in index
//
//				dbIndex		- returns the pointer to the index database handle
//
// Remark:		dbIndex needs to be closed with CloseDatabase( SDBFile *dbFile );
//
// Returns:     TRUE on success, FALSE on FAILURE
//
int OpenIndexFile( const char *indexfilename, short keysize, SDBFile *dbIndex );

//-----------------------------------------------------------------------------
// Purpose:     Find the search string in the index database
//
// Parameters:  dbIndex		- pointer to an open index database handle
//
//				searchkey	- the string to search for in the database
//
// Returns:     record number on success, -1L on FAILURE
//
long SearchIndexFile( SDBFile *dbIndex, char* searchkey );

//-----------------------------------------------------------------------------
// Purpose:     Add a new item to the index database
//
// Parameters:  dbIndex		- pointer to an open index database handle
//
//				searchkey	- the searchkey that needs to be added to the index file
//
//				recordnumber- the record number of the original database where the index file is made on
//
// Returns:     TRUE on success, FALSE on failure
//
int AddNewSearchkeyToIndex( SDBFile *dbIndex, char *nwsearchkey, long recordnumber );

#endif // __DATABASE_H__


