/*---------------------------------------------------------------------
	String search in TInt ( Ton Interval) Files 
-----------------------------------------------------------------------
    SearchStr.cpp
-----------------------------------------------------------------------
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>


#include "rameau.h"
#include "global.h"
#include "version.h"
#include "SearchStr.h" 
#include "TonInt.h"
#include "StrData.h"

#define PROG_NAME   "SearchStr"

#define STR_OUT_FILE_WRITE_ERROR -1

FILE *SearchStrStream = stdout;

// DEBUG Options

// #define STRING_SEARCH_DEBUG
// #define DEBUG_PERMUT_VEC

/*------------------*/
/* global variables */
/*------------------*/

#define MAX_SEARCH_COUNT 1000

int SearchCount, SearchValue[ MAX_SEARCH_COUNT ];

SearchData StrSearchData;

FILE   *in_stream = NULL, *out_stream = NULL, *err_stream= stderr;
int    VerboseFlag  = 0;
int    ReadLineFlag = 0;

//----------------------------------
// Serche String Parameters
//----------------------------------
int     flag_reverse = 0;
int     flag_transposition = 0;
int		flag_permutation = 0;

int *String, *StingTrans, *StringPermut;

TonInterval NextToneInt;
int	TestInputSearchString( char *str );

//----------------------------------
//  error handler for str
//----------------------------------

void Error(int errNum )
{
   switch( errNum )
   {
     case STR_OUT_FILE_WRITE_ERROR :
          fprintf( err_stream, "Exit Rdl-Str-Search while OutFile write error %d!\n", errNum );
          exit( errNum );
          break;

     default :
          fprintf( err_stream, "Exit Rdl-Str-Search while error %d!\n", errNum );
          exit( errNum );
          break;
   }
}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void
SearchStringUsage ()
{
char str[10] ="";

   SetRameauExt( str, SSTR );
   
   fprintf (err_stream, "usage : %s [flags] [%s_file_name] \"Search-String\"\n", PROG_NAME, str);
   fprintf (err_stream, "\n");
   fprintf (err_stream, "flags : -? this output\n");
   fprintf (err_stream, "      : -h this output\n");
   fprintf (err_stream, "      : -v disp program version\n");
   fprintf (err_stream, "      : -o output to stdout\n");
   fprintf (err_stream, "      : -i input from stdin\n");
   fprintf (err_stream, "      : -r reverse string\n");
   fprintf (err_stream, "      : -t transpose string\n");
   fprintf (err_stream, "      : -p perutate string\n");
   fprintf (err_stream, "      : -V verbose\n");
   fprintf (err_stream, "      : -l Disp Read line numbers\n");
   fprintf (err_stream, "\n");

   return;
}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/

int main (int argc, char *argv[])
{
int     i;
long    time_stamp = 0, last_time = -1;
long	LineReadCounter = 0L;

   Tint			tint;
   StrDataFile  strdatafile;
   
   /*----------------------------------------------------------
   programm flags setting und validation
   ------------------------------------------------------------*/

	// Get search string

#ifdef STRING_SEARCH_DEBUG
	fprintf( stderr, "\nSearchString : \"%s\"\n", argv[argc-1] );
#endif
		
	if( argc < 2)
	{
		SearchStringUsage ();
		// exit program
		return (1);
	}
	if( TestInputSearchString( argv[argc-1] ) != 0 )
	{
		fprintf(err_stream, "!!! Searchstring \"%s\" format error !!!\n", argv[argc-1] );
	    return (2);
	}
	
	
   for (i = 1; i < argc-1; i++)
   {
      if (argv[i][0] == '-')    /* program flags */
      {
     switch (argv[i][1])
     {
     case '?':
     case 'h':
     case 'H':
        SearchStringUsage ();
        return (0);
        break;
     case 'v':
        rameau_version (err_stream);
        return (0);
        break;
     case 'e':
        {
           if ((err_stream = fopen (RAMEAU_ERROR_FILE, "at")) == NULL)
           {
              err_stream = stderr;
              strdatafile.SetErrStream( stderr );
              fprintf (err_stream, "can't open rameau log file : %s\n", RAMEAU_ERROR_FILE);
           }
           strdatafile.SetErrStream( err_stream );
        }
        break;
     case 'i':
        in_stream = stdin;
        break;
     case 'o':
        out_stream = stdout;
        break;
     case 'r':
        flag_reverse = 1;
        break;
     case 't':
        flag_transposition = 1;
        break;
     case 'p':
        flag_permutation = 1;
        break;
     case 'V':
        VerboseFlag = 1;
        break;

     case 'l':
        ReadLineFlag = 1;
        break;

     default:
        SearchStringUsage ();
        return (0);
        break;
     }
      }
      else

      {  
      /*-------------------*/
      /* input/output file */
      /*-------------------*/
      char    buf[MAX_PATH_LENGTH];
      /*-------------------------------*/
      /* first parameter is input file */
      /*-------------------------------*/
      if (in_stream == NULL)
      {
        strcpy (buf, argv[i]);

#ifdef AUTO_EXTENION
        strcat (buf, ".");
		SetRameauExt( buf, TINT );
#endif /*  */

        if ((in_stream = tint.OpenInFile (buf)) == NULL )
        {
           return (1);
        }

     /*--------------------------------*/
     /* Second parameter is input file */
     /*--------------------------------*/
        if (out_stream == NULL)
        {
           strcpy (buf, argv[i]);

#ifdef AUTO_EXTENION
           strcat (buf, ".");
		   SetRameauExt( buf, SSTR);
#endif /*  */       
           remove( buf );
           out_stream = fopen (buf, "w");

           if (out_stream == NULL)
           {
              fprintf (err_stream, "!!! File %s open error !!!\n", buf);
              return (2);
           }
           SetFileInfoLine( out_stream, buf );
        }
     }
     else
     {
        SearchStringUsage ();
        return (3);
     }
      }
   }

   /*-----------------------------------------------
   if no in/out file specified use stdin/stdout
   -------------------------------------------------*/
   if (in_stream == NULL)
   {
      in_stream = stdin;
   }

   if (out_stream == NULL)
   {
      out_stream = stdout;
   }

   /*----------------------------------------------------------
   main loop
   ------------------------------------------------------------*/

   if( VerboseFlag ) ReadLineFlag = 0;

   tint.SetOutStream( NULL );
   tint.SetErrStream( err_stream );
   tint.SetMsgStream( out_stream );
   tint.MsgOn();
   tint.TxtOn();

   strdatafile.SetInStream( NULL );
   strdatafile.SetOutStream( out_stream );
   strdatafile.SetErrStream( err_stream );
   strdatafile.SetMsgStream( out_stream );
   strdatafile.MsgOff();
   strdatafile.TxtOff();

   // Set output Stream for Sting Found() 
   SearchStrStream = out_stream;
   // init Search String
   StrSearchData.SetSearchString( SearchCount, SearchValue );

   // Write SearchString to output file 
   fprintf(out_stream, "* SearchString : " );
   for( i = 0; i < SearchCount; i++ )
   {
		fprintf(out_stream, "%d ",SearchValue[i] );
   }
   fprintf(out_stream, "\n" );

   // set Search flags

   if( flag_reverse == 1 ) 
	   StrSearchData.ReversOn();
   if( flag_transposition == 1 ) 
	   StrSearchData.TransOn();
   if( flag_permutation == 1 ) 
	   StrSearchData.PermutationOn();

   while( (time_stamp = tint.Read( &NextToneInt )) >= 0 )
   {                 
      if( ReadLineFlag )
      {
         fprintf( stdout, "\r%8ld", LineReadCounter++ );
      }
      if( VerboseFlag )
      {
         fprintf( stdout, "\rTime: %8ld Bar: %8d ", time_stamp, tint.GetBar() );
      }
      
	  strdatafile.SetBar( tint.GetBar() );

	  // Set/Add TonInterval Data to the search database
	  // NextToneInt.Write( stderr );	
	  // Search Sting

	  if( NextToneInt.GetNote() == BAR_NOTE_VAL ) 
	  {
		  time_stamp = NextToneInt.GetStart();
	  }
	  else
	  {
		  StrSearchData.SetData( &NextToneInt );
	  }
      if (last_time < time_stamp)
      {
         strdatafile.NewTime();
         last_time = time_stamp;
      }

   }
   /*----------------------------------------------------------
   main loop end
   ------------------------------------------------------------*/

   if (in_stream != stdin)
      fclose (in_stream);
   if (out_stream != stdout)
      fclose (out_stream);
   if (err_stream != stderr)
      fclose (err_stream);

   return (0);
}
/*-----------------------------------------------------------
	read search string and check for errors
-------------------------------------------------------------
*/

int	TestInputSearchString( char *str )
{
int length, i, val;

    if( (length = strlen( str )) == 0 )
	{
		return 1;
	}

	i = SearchCount = 0;

	// Get Values from input string

	do
	{
		if( isspace( str[i] ) )
		{
			i++;
		}
		else
		if( isdigit( str[i] ) )
		{
			// printf( "%c,",str[i]);
			val = str[i++] - '0';
			while( isdigit( str[i] ) )
			{
				val *= 10;
				val +=  str[i++] - '0';

				if( val > MAX_NOTE_VAL ) 
					return 3;
			}
			// printf( "%d;",val );
			SearchValue[ SearchCount++ ] = val;
			if( SearchCount >= MAX_SEARCH_COUNT )
				return 4;
		}
		else
			return 2;
			
	}while( i < length );
	

#ifdef STRING_SEARCH_DEBUG
	for( i = 0; i < SearchCount; i++ )
		fprintf(err_stream, "%3d\n",SearchValue[i] );
#endif

	return 0;
}
//------------------------------------------------
//--- Set Data to String
//------------------------------------------------
int	SearchStr::Set( TonInterval * tint ) 
{
		Time[Pos] = tint->GetStart();
		Channel[Pos] = tint->GetChannel();
		Note[Pos] = tint->GetNote();
		EndTime = tint->GetEnd();

		if( PermutVec != NULL )
		{
			SetPermutVal( tint->GetNote() );
		}

		if( Pos == Length )
			return 1;
		else
		Pos++;
		return 0;
};
//--------------------------------

int SearchStr::Found() 
{	
		return Print( SearchStrStream );
};
//--------------------------------

int SearchStr::Print( FILE *stream ) 
{	
int i;

		// fprintf( stream, "Pos %d : ", Pos );
		// fprintf( stream, "%02x", GetFlags() );
		

		// Print Flags
		fprintf( stream, "%c%c%c:", 
				GetTrans() ? 'T' : '-', 
				GetRevers() ? 'R' : '-', 
				GetPermut() ? 'P' : '-'
				);
		// Print String Data
		for( i=0; i < Pos; i++ )
		{
		   fprintf( stream, "%8ld:%2d:%3d;", Time[i], Channel[i], Note[i] );
		}
		fprintf( stream, "%8ld\n", EndTime );

		return 0;
};

int SearchStr::PrintPermutVec( FILE *stream ) 
{	
int i;

		// Print Permut Data
		
		if( PermutVec == NULL )
		{
			fprintf( stream,"PermutVev == NULL\n" );
		}
		else
		for( i=0; i < MAX_NOTE_VAL; i++ )
		{
			if( GetPermutVal( i ) >0 )
				fprintf( stream, "%3d*%2d;", i, GetPermutVal( i ) );
		}
		fprintf( stream, "\n" );

		return 0;
};

int SearchStr::Copy( SearchStr *p )
{
	this->SetFlags( p->GetFlags() );
	this->SetTransVal( p->GetTransVal() );
	this->Length = p->Length;
	this->Pos  = p->Pos;
	this->Time = new long[Length];
	this->Channel = new int[Length];
	this->Note = new int[Length];
	memcpy( this->Time, p->Time, sizeof( long ) * Pos );
	memcpy( this->Channel, p->Channel, sizeof( int ) * Pos );
	memcpy( this->Note, p->Note, sizeof( int ) * Pos );
	if( GetPermut() != 0 )
	{
		PermutVec = new int [MAX_NOTE_VAL + 1];
		memcpy( this->PermutVec, p->PermutVec, sizeof( int ) * MAX_NOTE_VAL );
#ifdef DEBUG_PERMUT_VEC
		fprintf( stderr, "PermutVec Size %d\n",sizeof( int ) * MAX_NOTE_VAL );
#endif //DEBUG_PERMUT_VEC
	}
	return 0;
}

int SearchStr::SetPermutVec( int min, int max, int *vec )
{
int i;

	delete PermutVec;
	PermutVec = new int[ MAX_NOTE_VAL + 1 ];
	memset( PermutVec, 0, sizeof( PermutVec ));
	for( i=min; i<=max; i++ )
	{
		PermutVec[i] = vec[i];
	}
	return 0;
}
//---------------------------------------------------------

//------------------
// Set new Note Data
//------------------

int	SearchData::SetData( TonInterval  *Tint )
{
int i=0;

	// delete all "unused" entries for this Time
	// and continued string data
	
	for( i=0; i < SearchDataListLength; i++ )
	{
		if( List[i] != NULL && List[i]->GetEndTime() < Tint->GetStart() )
		{
			delete List[i];
			List[i] = NULL;
		}
		
		if( List[i] != NULL )
		{
			// continue String Data ?
			TestAndSetData( List[i], Tint );
		}
	}

	// create a new Entries 	

	int sparam = 0;

	if( GetRevers() ) sparam |= SEARCH_REVERS_FLAG;
	if( GetTrans() != 0 )  sparam |= SEARCH_TRANS_FLAG;
	if( GetPermut() != 0 )  sparam |= SEARCH_PERMUT_FLAG;

	CreateNewEntries( Tint, sparam );

	return 0;
}
//---------------------------------------------------------
// create new entries
//---------------------------------------------------------
int SearchData::CreateNewEntries( TonInterval  *Tint, int flags )
{
	// entry for eq. string
	 
	if( Tint->GetNote() == SearchStringData[0] )
	{
	SearchParameter param;

		NewEntry( Tint, param );
	}
	else // search for Transposition
	if( flags & SEARCH_TRANS_FLAG )
	{
	SearchParameter param;

		param.SetTransVal( Tint->GetNote() - SearchStringData[0] );
		param.TransOn();
		NewEntry( Tint, param );
	}
	// entry for revers string

	if( (flags & SEARCH_REVERS_FLAG) && Tint->GetNote() == ReversSearchStringData[0] )
	{
	SearchParameter param;

		param.ReversOn();
		NewEntry( Tint, param );
	}
	else // search for Transposition in reverse string
	if( (flags & SEARCH_REVERS_FLAG) && (flags & SEARCH_TRANS_FLAG) )
	{
	SearchParameter param;

		param.SetTransVal( Tint->GetNote() - ReversSearchStringData[0] );
		param.TransOn();
		param.ReversOn();
		NewEntry( Tint, param );
	}
	
	// entry for Permutation

    if( (flags & SEARCH_PERMUT_FLAG) && SearchVector[Tint->GetNote()] > 0 )
	{
	SearchParameter param;
	int ret;

		param.PermutationOn();
		ret = NewEntry( Tint, param );

	}
	return 0;
}

int SearchData::NewEntry( TonInterval  *Tint, SearchParameter param )
{
		int num = FreeListEntry();
		List[num] = new SearchStr( length, Tint, param );

		// Set PermutVec Data

		if( GetPermut() )
		{
			List[num]->SetPermutVec( SearchMin, SearchMax, SearchVector );
			List[num]->SetPermutVal( Tint->GetNote() );
		}

		if( List[num]->GetPos() == length )
		{   
			// fprintf( stderr, "String found : " );
			List[num]->Found();
			// remove entry from List
			delete List[num];
			List[num] = NULL;
			return -1;
		}
		return num;
}
//------------------------------------------------
//--- Test String if data is eq. Search String
//------------------------------------------------

int	SearchData::TestAndSetData( SearchStr *Sstr, TonInterval *Tint ) 
{
int num, ok = 0;

	if( Sstr->GetEndTime() != Tint->GetStart() )
		return -1;

	// Search for eq. and trans
	if( !Sstr->GetRevers() && !Sstr->GetPermut() && ((SearchStringData[ Sstr->GetPos() ] + Sstr->GetTransVal()) == Tint->GetNote()) )
		ok = 1;
	else
	// Search for eq. and trans in reverse string
	if( Sstr->GetRevers() && ((ReversSearchStringData[ Sstr->GetPos() ] + Sstr->GetTransVal()) == Tint->GetNote()) )
		ok = 1;
	else
	// Test Permutation
	if( Sstr->GetPermut() )
	{
		// Sstr->Print( stderr );
		// Sstr->PrintPermutVec( stderr );

		if( (Sstr->GetPermutVal(Tint->GetNote()) > 0) )
		{
#ifdef DEBUG_PERMUT_VEC
			fprintf( stderr, "Per Entry " ); 
			Sstr->Print( stderr ); 
			Sstr->PrintPermutVec( stderr );
#endif //DEBUG_PERMUT_VEC
			ok = 1;
		}
	}
//	if( Sstr->GetEndTime() == Tint->GetStart() && SearchStringData[ Sstr->GetPos() ] == Tint->GetNote() )
	if( ok == 1 )
	{
	// entry found
	    num = FreeListEntry(); 

		// create new entry

		List[num] = new SearchStr( );
#ifdef DEBUG_PERMUT_VEC
			fprintf( stderr, "New  Entry " ); 
			List[num]->Print( stderr ); 
			List[num]->PrintPermutVec( stderr );
#endif //DEBUG_PERMUT_VEC
		// Copy String Information 
		List[num]->Copy( Sstr );	
#ifdef DEBUG_PERMUT_VEC
			fprintf( stderr, "Copy Entry " ); 
			List[num]->Print( stderr ); 
			List[num]->PrintPermutVec( stderr );
#endif //DEBUG_PERMUT_VEC
		// Add new data to Sting
		List[num]->Set( Tint );
#ifdef DEBUG_PERMUT_VEC
			fprintf( stderr, "Set  Entry " ); 
			List[num]->Print( stderr ); 
			List[num]->PrintPermutVec( stderr );
#endif //DEBUG_PERMUT_VEC

		// is Sring eq. Search String ?
		if( List[num]->GetPos() == length )
		{   
			// fprintf( stderr, "String found : " );
			// List[num]->Print( stderr );
			List[num]->Found();

			// remove entry from List

			delete List[num];
			List[num] = NULL;

			return 1;	// String is eq.
		}
	}
	return 0;
}
//--------------------------------------------------------
// get index of the next free List Entry
//--------------------------------------------------------
int	SearchData::FreeListEntry() 
{
int i=0, FreeEntry;

	FreeEntry = -1;

	for( i=0; i<SearchDataListLength; i++ )
	{
		if( List[i] == NULL )
		{
			FreeEntry = i;
			break;
		}
	}

	// resize SearchDataList

	if( FreeEntry == -1 )
	{
		int i, NewCount = SEARCH_DATA_LIST_RESIZE_VALUE;
		SearchStr   **NewList;

		NewList = new SearchStr * [SearchDataListLength+NewCount];
		if( NewList == NULL )
		{
			fprintf( stderr, "SEARCH_DATA_LIST no free entry\n" );
			exit(1); 
		}

		for( i=0; i<SearchDataListLength; i++ )
		{
			// copy old Endtries
			NewList[i] = List[i];
		}
		for( i=0; i<NewCount; i++ )
		{
			// init New entries
			NewList[SearchDataListLength+i] = NULL;
		}
		FreeEntry = SearchDataListLength;
		delete List;
		List = NewList;

		SearchDataListLength += NewCount;

#ifdef STRING_SEARCH_DEBUG
		fprintf( stderr, "Resize SearchDataList %d\n", SearchDataListLength );
#endif
	}
	return FreeEntry;
}
//---------------------------------------------------------
// Int search string and Search Information 
//---------------------------------------------------------
int	SearchData::SetSearchString( int len, int str[] )
{
int i, val;
			
		length = len;
		delete SearchStringData;
		delete ReversSearchStringData;
		SearchStringData = new int[len+1];
		ReversSearchStringData = new int[len+1];

		for( i=0; i<len; i++ )
		{
			SearchStringData[i] = str[i];
			ReversSearchStringData[len-i-1] = str[i];
		}

		// Set up an Vector with the search values 
		// This data is used to find permutations from SearchString
		// each Element repressent the frequentcy for this ToneValue

		memset( SearchVector, 0, sizeof( SearchVector ));
		SearchMin = MAX_NOTE_VAL; SearchMax = 0;

		for( i=0; i < SearchCount; i++ )
		{
			val = SearchValue[ i ];
			SearchVector[val]++;
		
			if( val < SearchMin )  SearchMin = val;
			if( val > SearchMax )  SearchMax = val;
		}

		// Sort String Values for Permutaiton  

		for( i=0; i < SearchCount; i++ )
		{
			// sort();
		}

		return 0;
};
