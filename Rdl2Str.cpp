/*---------------------------------------------------------------------
	String search in Rdl Files
-----------------------------------------------------------------------
    rdl2str.cpp
-----------------------------------------------------------------------
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>


// #include "rameau.h"
#include "rameauNew.h"
#include "global.h"
#include "version.h"
#include "rdl.h"
#include "rdlstr.h"
#include "ChNotes.h"
#include "SearchStr.h" 

#define PROG_NAME   "rdl2str"

#define STR_OUT_FILE_WRITE_ERROR -1

void  RdlToStrUsage (void);

// DEBUG Options

#define STRING_SEARCH_DEBUG

/*------------------*/
/* global variables */
/*------------------*/

#define MAX_SEARCH_COUNT 1000

int SearchCount, SearchValue[ MAX_SEARCH_COUNT ];
int SearchVector[ MAX_NOTE_VAL+1 ];
int	SearchMin, SearchMax;

ChNotes Notes;

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
RdlToStrUsage ()
{
char str[10] ="";

   SetRameauExt( str, RDL );
   
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
   int     channel_nodes[16];
   long    time_stamp, last_time = -1;

   Rdl     rdl;
   RdlStr  rdlstr;
   
   /*----------------------------------------------------------
   programm flags setting und validation
   ------------------------------------------------------------*/

	// Get search string

#ifdef STRING_SEARCH_DEBUG
	fprintf( stderr, "\nSearchString : \"%s\"\n", argv[argc-1] );
#endif
	
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
        RdlToStrUsage ();
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
              Notes.SetErrStream( stderr );
              fprintf (err_stream, "can't open rameau log file : %s\n", RAMEAU_ERROR_FILE);
           }
           Notes.SetErrStream( err_stream );
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
        RdlToStrUsage ();
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
		SetRameauExt( buf, RDL );
#endif /*  */

        if ((in_stream = rdl.OpenInFile (buf)) == NULL )
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
		   SetRameauExt( buf, RDLSTR);
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
        RdlToStrUsage ();
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
   {
   long LineReadCounter = 0L;

   if( VerboseFlag ) ReadLineFlag = 0;

   rdl.SetOutStream( NULL );
   rdl.SetErrStream( err_stream );
   rdl.SetMsgStream( out_stream );
   rdl.MsgOn();
   rdl.TxtOn();

   rdlstr.SetInStream( NULL );
   rdlstr.SetOutStream( out_stream );
   rdlstr.SetErrStream( err_stream );
   rdlstr.SetMsgStream( out_stream );
   rdlstr.MsgOff();
   rdlstr.TxtOff();

   while ((time_stamp = rdl.Read ( channel_nodes)) != -1L)
   {                 
   
      if( ReadLineFlag )
      {
         fprintf( stdout, "\r%8ld", LineReadCounter++ );
      }
      if( VerboseFlag )
      {
         fprintf( stdout, "\rTime: %8ld Bar: %8d ", time_stamp, rdl.GetBar() );
      }
      
	  rdlstr.SetBar( rdl.GetBar() );
      if (last_time < time_stamp)
      {

		 rdlstr.WriteRdlStrLine( time_stamp, Notes );
         rdlstr.NewTime();
         last_time = time_stamp;
      }
      Notes.SetChNotes( channel_nodes );

	  // Search Sting

/****		
		{
		  int i;

			for( i = 0; i < 16; i++ )
			{	
				fprintf( stderr,"%4d", channel_nodes[i] );
			}
			fprintf(stderr,"\n");
		}
****/
   }
   rdlstr.WriteRdlStrLine( last_time, Notes );
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
	
	// Set up an Vector with the search values 

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

#ifdef STRING_SEARCH_DEBUG
	for( i = 0; i < SearchCount; i++ )
		fprintf(err_stream, "%3d\n",SearchValue[i] );

	fprintf(err_stream, "SearchCount : %d  Min %3d Max : %3d\n",SearchCount, SearchMin, SearchMax );
#endif

	return 0;
}