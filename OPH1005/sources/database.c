//
// database.c
//
// implementation of the database functions
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <io.h>


#ifdef OPH1004
#include <unistd.h>
#include <fcntl.h>
#endif
#include "lib.h"
#ifdef OPH1004
#undef O_BINARY
#define O_BINARY 0x00
#endif

#include "database.h"

//
// lErrorCode holds the last error that occured in the database
//
static long lErrorCode;


long GetDBErrorCode( void )
{
	return lErrorCode;
}

static int IsFileOpen( SDBFile *dbFile )
{
	lErrorCode = DB_OK;
	return (dbFile->bOpen == TRUE)?TRUE:FALSE;
}

int OpenDatabase( const char* filename, short recordsize, SDBFile *dbFile )
{
	long lfilesz;

	if( IsFileOpen( dbFile ))
	{
		lErrorCode = DB_ERROR_ALREADY_OPEN;
		return FALSE;
	}

	if( (lfilesz = fsize( (char*)filename )) == -1L )
	{
		lErrorCode = DB_ERROR_NOT_EXIST;
		return FALSE;
	}

	if( (dbFile->fd = open( (char*)filename, O_RDWR | O_BINARY, 0x777 )) == -1 )
	{
		lErrorCode = DB_ERROR_OPEN;
		return FALSE;
	}
	dbFile->bOpen = TRUE;
	if( lfilesz % recordsize)
	{
		lErrorCode = DB_ERROR_RECORD_SIZE;
		CloseDatabase( dbFile );
		return FALSE;
	}
	dbFile->sRecSz = recordsize;
	dbFile->lCurrRecord = lfilesz?0:-1L;
	dbFile->lTotalRecords = lfilesz / recordsize;

	return TRUE;
}


int CreateDatabase( const char* filename, short recordsize, SDBFile *dbFile )
{
	if( IsFileOpen( dbFile ))
	{
		lErrorCode = DB_ERROR_ALREADY_OPEN;
		return FALSE;
	}

	if( (dbFile->fd = open( (char*)filename, O_RDWR | O_BINARY | O_CREAT | O_TRUNC, 0x0 )) == -1 )
	{
		lErrorCode = DB_ERROR_OPEN;
		return FALSE;
	}
	dbFile->bOpen = TRUE;
	dbFile->sRecSz = recordsize;
	dbFile->lCurrRecord = 0L;
	dbFile->lTotalRecords = 0L;

	return TRUE;
}


void CloseDatabase( SDBFile *dbFile )
{
	if( !IsFileOpen( dbFile ))
		return;
	//
	// Close the open file handle
	//
	close( dbFile->fd );
	dbFile->bOpen = FALSE;
	dbFile->fd = -1;
}


long GetCurrentRecord( SDBFile *dbFile )
{
	if( !IsFileOpen( dbFile ) )
	{
		lErrorCode = DB_ERROR_NOT_OPEN;
		return -1L;
	}
	if( dbFile->lTotalRecords == 0L )
	{
		lErrorCode = DB_ERROR_EMPTY;
		return -1L;
	}
	return dbFile->lCurrRecord;
}


long GetTotalRecords( SDBFile *dbFile )
{
	if( !IsFileOpen( dbFile ) )
	{
		lErrorCode = DB_ERROR_NOT_OPEN;
		return -1L;
	}
	return dbFile->lTotalRecords;
}


int GotoRecord( SDBFile *dbFile, long recordnumber )
{
	long totalrecords;
	if( ( totalrecords = GetTotalRecords( dbFile )) == -1L )
		return FALSE;

	if( recordnumber < 0L || recordnumber >= totalrecords )
	{
		lErrorCode = DB_ERROR_INVALID_REC_NO;
		return FALSE;
	}

	dbFile->lCurrRecord = recordnumber;

	return TRUE;
}


int ReadCurrentRecord( SDBFile *dbFile, char* record )
{
	long curr;

	if( (curr = GetCurrentRecord( dbFile )) == -1L )
		return FALSE;

	if( lseek( dbFile->fd, (long)(curr * dbFile->sRecSz), SEEK_SET ) == -1L)
	{
		lErrorCode = DB_ERROR_RECORD_JMP;
		return FALSE;
	}

	if( read( dbFile->fd, record, dbFile->sRecSz ) != dbFile->sRecSz )
	{
		lErrorCode = DB_ERROR_READ_FILE;
		record[0] = '\0';
		return FALSE;
	}

	return TRUE;
}


int ReadFirstRecord( SDBFile *dbFile, char* record )
{
	if( !GotoRecord( dbFile, 0L ))
		return FALSE;
	if( !ReadCurrentRecord( dbFile, record ))
		return FALSE;
	return TRUE;
}


int ReadLastRecord( SDBFile *dbFile, char* record )
{
	long lastrecord;
	if( (lastrecord = GetTotalRecords( dbFile )) == -1L )
		return FALSE;
	if( !GotoRecord( dbFile, lastrecord - 1L ))
		return FALSE;
	if( !ReadCurrentRecord( dbFile, record ))
		return FALSE;
	return TRUE;
}


int ReadPreviousRecord( SDBFile *dbFile, char* record )
{
	long curr;
	if( (curr = GetCurrentRecord( dbFile )) == -1L )
		return FALSE;
	if( !GotoRecord( dbFile, curr - 1L ))
		return FALSE;
	if( !ReadCurrentRecord( dbFile, record ))
		return FALSE;
	return TRUE;
}


int ReadNextRecord( SDBFile *dbFile, char* record )
{
	long curr;
	if( (curr = GetCurrentRecord( dbFile )) == -1L )
		return FALSE;
	if( !GotoRecord( dbFile, curr + 1L ))
		return FALSE;
	if( !ReadCurrentRecord( dbFile, record ))
		return FALSE;
	return TRUE;
}


int DeleteRecord( SDBFile *dbFile, long recordnumber )
{
	char *record;
	long recno;
	long totalrecords;

	if( ( totalrecords = GetTotalRecords( dbFile )) == -1L )
		return FALSE;
	if( !GotoRecord( dbFile, recordnumber ))
		return FALSE;

	if( (record = (char*)malloc( dbFile->sRecSz)) == NULL )
	{
		lErrorCode = DB_ERROR_MEM;
		return FALSE;
	}

	for( recno = recordnumber +1L; recno < totalrecords; recno++ )
	{
		if( !GotoRecord( dbFile, recno ))
			break;
		if( !ReadCurrentRecord( dbFile, record ))
			break;
		if( !GotoRecord( dbFile, recno -1L ))
			break;
		if( !WriteRecord( dbFile, record, WRITE_OVER ))
			break;
	}
	free( record );
	if( chsize( dbFile->fd, (long)((totalrecords -1L) * (dbFile->sRecSz ))) == -1)
		lErrorCode = DB_ERROR_CHANGE_SIZE;
	if( GetDBErrorCode() != DB_OK )
		return FALSE;
	dbFile->lTotalRecords--;
	return TRUE;
}


int WriteRecord( SDBFile *dbFile, char* record, int iFlag )
{
	long curr;

	if( !IsFileOpen( dbFile ) )
	{
		lErrorCode = DB_ERROR_NOT_OPEN;
		return FALSE;
	}

	if( iFlag == WRITE_OVER )
	{
		if( (curr = GetCurrentRecord( dbFile )) == -1L )
			return FALSE;
		//
		// Jump to the position to overwrite the record
		//
		if( lseek( dbFile->fd, (long)(dbFile->sRecSz * curr), SEEK_SET ) == -1L)
		{
			lErrorCode = DB_ERROR_RECORD_JMP;
			return FALSE;
		}
	}
	else if( iFlag == WRITE_APPEND )
	{
		if( lseek( dbFile->fd, 0L, SEEK_END ) == -1L )
		{
			lErrorCode = DB_ERROR_RECORD_JMP;
			return FALSE;
		}
	}
	else
	{
		lErrorCode = DB_ERROR_INVALID_WFLAG;
		return FALSE;
	}

	if( write( dbFile->fd, record, dbFile->sRecSz ) != dbFile->sRecSz )
	{
		lErrorCode = DB_ERROR_WRITE_FILE;
		return FALSE;
	}

	if( iFlag == WRITE_APPEND )
	{
		dbFile->lTotalRecords++;
		GotoRecord( dbFile, GetTotalRecords( dbFile ) -1L );
	}

	return TRUE;
}


// ++++++++++++++++++++++++++++++++++++++
// Sorting database functions
// ++++++++++++++++++++++++++++++++++++++

//
// Insertion sort is a very fast sorting method for almost sorted databases
//
int InsertionSort( SDBFile *dbFile, short offset, short checksize )
{
	long i, j;
	long totalrecords;
	char* temp1;
	char* temp2;

	if( ( totalrecords = GetTotalRecords( dbFile )) == -1L )
		return FALSE;

	if( dbFile->sRecSz < (checksize+offset) )
	{
		lErrorCode = DB_ERROR_SORTLENGTH;
		return FALSE;
	}

	lErrorCode = DB_ERROR_MEM;
	if( (temp1 = (char*) malloc( (unsigned int)dbFile->sRecSz)) == NULL )
		goto Clean1;

	if( (temp2 = (char*) malloc( (unsigned int)dbFile->sRecSz)) == NULL )
		goto Clean2;
	lErrorCode = DB_OK;
	for( i = 1L; i < totalrecords; i++ )
	{
		if( !GotoRecord( dbFile, i ))
			goto CleanAll;
		if( !ReadCurrentRecord( dbFile, temp1 ))
			goto CleanAll;
		j = i;
		while( j > 0L )
		{
			if( !GotoRecord( dbFile, (j-1L) ))
				goto CleanAll;
			if( !ReadCurrentRecord( dbFile, temp2 ))
				goto CleanAll;

			if( memcmp( temp1+offset, temp2+offset, checksize) > 0 )
				break;

			if( !GotoRecord( dbFile, j ))
				goto CleanAll;
			if( !WriteRecord( dbFile, temp2, WRITE_OVER ))
				goto CleanAll;
			j--;
		}
		if( !GotoRecord( dbFile, j ))
			goto CleanAll;
		if( !WriteRecord( dbFile, temp1, WRITE_OVER ))
			goto CleanAll;
	}

CleanAll:
	free( temp2 );
Clean2:
	free( temp1 );
Clean1:
	if( GetDBErrorCode() != DB_OK )
		return FALSE;
	return TRUE;
}


//
// Heap sort helper function
//
static int downheap( SDBFile *dbFile, long max_records, long k, short offset, short checksize, char* tmp1, char* tmp2, char* tmp3 )
{
	long j, N2;

	if( !GotoRecord( dbFile, (k-1L)))
		return FALSE;
	if( !ReadCurrentRecord( dbFile, tmp1 ))
		return FALSE;

	N2 = max_records >> 1;
	while( k <= N2)
	{
		j = k + k;

		if( !GotoRecord( dbFile, (j-1L)))
			return FALSE;
		if( !ReadCurrentRecord( dbFile, tmp2 ))
			return FALSE;

		if( j < max_records )
		{
			if( !GotoRecord( dbFile, j))
				return FALSE;
			if( !ReadCurrentRecord( dbFile, tmp3 ))
				return FALSE;

			if( memcmp( tmp2 + offset, tmp3 + offset, checksize ) < 0 )
			{
				memmove( tmp2, tmp3, dbFile->sRecSz );
				j++;
			}
		}
		if( !GotoRecord( dbFile, k-1L))
			return FALSE;

		if( memcmp( tmp1+offset, tmp2+offset, checksize ) >= 0 )
			break;

		if( !WriteRecord( dbFile, tmp2, WRITE_OVER ))
			return FALSE;
		k = j;
	}
	if( !GotoRecord( dbFile, k-1L))
		return FALSE;

	if( !WriteRecord( dbFile, tmp1, WRITE_OVER ))
			return FALSE;

	return TRUE;
}

//
// Heap sort works best on databases that have not been sorted at all
//
int HeapSort( SDBFile *dbFile, short offset, short checksize )
{
	long totalrecords;
	long k, N;
	char* temp1;
	char* temp2;
	char* temp3;

	if( ( totalrecords = GetTotalRecords( dbFile )) == -1L )
		return FALSE;

	if( dbFile->sRecSz < (checksize+offset) )
	{
		lErrorCode = DB_ERROR_SORTLENGTH;
		return FALSE;
	}

	lErrorCode = DB_ERROR_MEM;
	if( (temp1 = (char*) malloc( dbFile->sRecSz)) == NULL )
		goto Clean1;

	if( (temp2 = (char*) malloc( dbFile->sRecSz)) == NULL )
		goto Clean2;

	if( (temp3 = (char*) malloc( dbFile->sRecSz)) == NULL )
		goto Clean3;
	lErrorCode = DB_OK;
	N = totalrecords;
	for( k = N >> 1; k >= 1; k-- )
		if( !downheap( dbFile, totalrecords, k, offset, checksize, temp1, temp2, temp3 ))
			goto CleanAll;
	while( N > 1 )
	{
		// swap the records in the database
		if( !ReadFirstRecord( dbFile, temp1 ))
			goto CleanAll;
		if( !GotoRecord( dbFile, (N-1L) ))
			goto CleanAll;
		if( !ReadCurrentRecord( dbFile, temp2 ))
			goto CleanAll;
		if( !WriteRecord( dbFile, temp1, WRITE_OVER ))
			goto CleanAll;
		if( !GotoRecord( dbFile, 0L ))
			goto CleanAll;
		if( !WriteRecord( dbFile, temp2, WRITE_OVER ))
			goto CleanAll;
		N--;
		if( !downheap( dbFile, N, 1, offset, checksize, temp1, temp2, temp3 ))
			goto CleanAll;
	}
CleanAll:
	free( temp3 );
Clean3:
	free( temp2 );
Clean2:
	free( temp1 );
Clean1:
	if( GetDBErrorCode() != DB_OK )
		return FALSE;
	return TRUE;
}

//
// Very fast sort function faster then heapsort but uses more resources
//
int QuickSort( SDBFile *dbFile, short offset, short checksize )
{
	long totalrecords;
	char* temp1;
	char* temp2;
	char* temp3;
	long  l,r,s,i,j;
	long* stackl;
	long* stackr;
	long n1;
	int  n2;

	if( ( totalrecords = GetTotalRecords( dbFile )) == -1L )
		return FALSE;

	if( dbFile->sRecSz < (checksize+offset) )
	{
		lErrorCode = DB_ERROR_SORTLENGTH;
		return FALSE;
	}
	lErrorCode = DB_ERROR_MEM;
	if( (temp1 = (char*) malloc( dbFile->sRecSz)) == NULL )
		goto Clean1;

	if( (temp2 = (char*) malloc( dbFile->sRecSz)) == NULL )
		goto Clean2;

	if( (temp3 = (char*) malloc( dbFile->sRecSz)) == NULL )
		goto Clean3;
	//
	// Calculate size for stack
	//
	n2 = 1;
	n1 = totalrecords;
	while( (n1 /= 10) != 0 )
		n2++;
	n2 *= 4;
	n2 += 10;

	if( (stackl = (long*) malloc( (n2 * sizeof(long)))) == NULL )
		goto Clean4;

	if( (stackr = (long*) malloc( (n2 * sizeof(long)))) == NULL )
		goto Clean5;
	lErrorCode = DB_OK;

	s = 1;
	stackl[ s ] = 0L;
	stackr[ s ] = totalrecords - 1L;
	do
	{
		l = stackl[ s ];
		r = stackr[ s ];
		s--;
		do
		{
			i = l;
			j = r;
			if( !GotoRecord( dbFile, ((l+r)>>1) ))
				goto CleanAll;
			if( !ReadCurrentRecord( dbFile, temp1 ))
				goto CleanAll;


			do
			{
				if( !GotoRecord( dbFile, i ))
					goto CleanAll;
				if( !ReadCurrentRecord( dbFile, temp2 ))
					goto CleanAll;
				while( memcmp( temp2 + offset, temp1 + offset, checksize ) < 0 )
				{
					i++;
					if( !GotoRecord( dbFile, i ))
						goto CleanAll;
					if( !ReadCurrentRecord( dbFile, temp2 ))
						goto CleanAll;
				}

				if( !GotoRecord( dbFile, j ))
					goto CleanAll;
				if( !ReadCurrentRecord( dbFile, temp3 ))
					goto CleanAll;
				while( memcmp( temp3 + offset, temp1 + offset, checksize ) > 0 )
				{
					j--;
					if( !GotoRecord( dbFile, j ))
						goto CleanAll;
					if( !ReadCurrentRecord( dbFile, temp3 ))
						goto CleanAll;
				}

				if( i <= j  )
				{
					if( !WriteRecord( dbFile, temp2, WRITE_OVER ))
						goto CleanAll;

					if( !GotoRecord( dbFile, i ))
						goto CleanAll;
					if( !WriteRecord( dbFile, temp3, WRITE_OVER ))
						goto CleanAll;
					i++;
					j--;
				}
			} while( i <= j  );
			if( i < r )
			{
				s++;
				stackl[ s ] = i;
				stackr[ s ] = r;
			}
			r = j;
		} while( l < r );
	} while( s > 0L );

CleanAll:
	free( stackr );
Clean5:
	free( stackl );
Clean4:
	free( temp3 );
Clean3:
	free( temp2 );
Clean2:
	free( temp1 );
Clean1:
	if( GetDBErrorCode() != DB_OK )
		return FALSE;
	return TRUE;
}


// ++++++++++++++++++++++++++++++++++++++
// Searching database function
// ++++++++++++++++++++++++++++++++++++++


long BinarySearch( SDBFile *dbFile, char* record, char* searchkey, int checksize, int offset )
{
	int test;
	long min, max, current;

	if( (max = GetTotalRecords( dbFile )) == -1L )
		return -1L;
	min = 0L;
	for (;;)
	{
		current = ((max - min) >> 1) + min;
		if( !GotoRecord( dbFile, current ))
			return -1L;
		if( !ReadCurrentRecord( dbFile, record ))
			return -1L;

		if( (test = memcmp( searchkey, record + offset, checksize )) == 0 )
			return current; // found the searchstring

		if( test < 0 )
			max = current - 1L;
		else
			min = current + 1L;
		if( min > max)
		{
			lErrorCode = DB_ERROR_NOT_FOUND;
			record[0] = '\0';
			return (-1L);
		}
	}
}

long LineairSearch( SDBFile *dbFile, char* record, char* searchkey, int checksize, int offset )
{
	long totalrecords, i;
	if( (totalrecords = GetTotalRecords( dbFile )) == -1L )
		return -1L;
	for( i = 0L; i < totalrecords; i++ )
	{
		if( !GotoRecord( dbFile, i ) )
			return -1L;
		if( !ReadCurrentRecord( dbFile, record ))
			return -1L;
		if( memcmp( searchkey, record + offset, checksize ) == 0 )
			return i;
	}
	lErrorCode = DB_ERROR_NOT_FOUND;
	record[0] = '\0';
	return -1L;
}


// +++++++++++++++++++++++++++++++++++++++++
// Special index functions
// +++++++++++++++++++++++++++++++++++++++++

//
// Make an index file on dbFile
// On ok ends with an open indexed file with recordsize of keysize + 4 bytes
// the 4 bytes holds the index record number
int CreateIndexFile( SDBFile *dbFile, short offset, short keysize, const char* indexfilename, SDBFile *dbIndex )
{
	char* record;
	long i, totalrecords;

	if( !IsFileOpen( dbFile ))
		return FALSE;
	if( ( totalrecords = GetTotalRecords( dbFile )) == 0L )
	{
		lErrorCode = DB_ERROR_EMPTY;
		return FALSE;
	}

	if( !CreateDatabase( indexfilename, (short)(keysize + sizeof( long )), dbIndex)	)
		return FALSE;

	lErrorCode = DB_ERROR_MEM;
	record = (char*)malloc( dbFile->sRecSz + sizeof( long ) );
	if( record != NULL )
	{
		lErrorCode = DB_OK;

		for( i = 0; i < totalrecords; i++ )
		{
			if( !GotoRecord( dbFile, i ))
				break;
			if( !ReadCurrentRecord( dbFile, record ))
				break;
			memcpy( record + (offset + keysize), &i, sizeof( long ));
			if( !WriteRecord( dbIndex, record + offset, WRITE_APPEND ))
				break;
		}
		free( record );
		//
		// Sort the index database
		//
		if( GetDBErrorCode() == DB_OK )
		{
			if( QuickSort( dbIndex, 0, keysize ))
				return TRUE;
		}
	}
	CloseDatabase( dbIndex );
	remove( indexfilename );
	return FALSE;
}

int OpenIndexFile( const char *indexfilename, short keysize, SDBFile *dbIndex )
{
	return OpenDatabase( indexfilename, (short)(keysize + sizeof( long )), dbIndex );
}


long SearchIndexFile( SDBFile *dbIndex, char *searchkey )
{
	static long recnr;
	char* record;

	if( !IsFileOpen( dbIndex ))
		return -1L;
	if( ( recnr = GetTotalRecords( dbIndex )) == 0L )
	{
		lErrorCode = DB_ERROR_EMPTY;
		return -1L;
	}

	if( ( record = (char*) malloc( dbIndex->sRecSz )) == NULL )
	{
		lErrorCode = DB_ERROR_MEM;
		return -1L;
	}
	recnr = -1L;
	if( BinarySearch( dbIndex, record, searchkey, (dbIndex->sRecSz - sizeof( long ) ), 0 ) != -1L )
	{
		memcpy( &recnr, record + (dbIndex->sRecSz - sizeof( long )), sizeof( long ) );
	}
	free( record );
	return recnr;
}

int AddNewSearchkeyToIndex( SDBFile *dbIndex, char *nwsearchkey, long recordnumber )
{
	char* record;
	if( !IsFileOpen( dbIndex ))
		return FALSE;

	if( ( record = (char*) malloc( dbIndex->sRecSz )) == NULL )
	{
		lErrorCode = DB_ERROR_MEM;
		return -1L;
	}
	memcpy( record, nwsearchkey, dbIndex->sRecSz - sizeof( long ) );
	memcpy( record + dbIndex->sRecSz - sizeof( long ), &recordnumber, sizeof( long ) );
	if( WriteRecord( dbIndex, record, WRITE_APPEND ) )
	{
		free( record );
		return (  InsertionSort( dbIndex, 0, (short)(dbIndex->sRecSz - sizeof( long )) ));
	}
	free( record );
	return FALSE;
}
