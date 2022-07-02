/*---------------------------------------------------------------------
	Dieter NeubacHer
	-------------------------------------------------------------------
	convert Rdl Files to Ton-Intervall
-----------------------------------------------------------------------
    rdl2tint.cpp

	OutPut Data Structure:
	
	bbbbbbbb:eeeeeeee:nnn:ccc

	bbbbbbbb   Interval Begin Time
	eeeeeeee   Interval End   Time 
	nnn        Note
    ccc        Channel

    For a Bar : Note and Channel is set to 999  
-----------------------------------------------------------------------
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>


#include "rameau.h"
#include "global.h"
#include "version.h"
#include "rdl.h"
#include "ChNotes.h"
#include "TonInt.h"


#define PROG_NAME   "rdl2tint"

#define TINT_OUT_FILE_WRITE_ERROR -1

void  RdlToTintUsage (void);

// DEBUG Options

#define STRING_SEARCH_DEBUG

/*------------------*/
/* global variables */
/*------------------*/


ChNotes Notes;

FILE   *in_stream = NULL, *out_stream = NULL, *err_stream= stderr;
int    VerboseFlag  = 0;
int    ReadLineFlag = 0;

TonInterval TonData; 


#define MAX_ENTRIES 10

long TonIntData[MAX_CHANNEL][MAX_NOTE_VAL][MAX_ENTRIES];

//----------------------------------

int InitTonIntData( void )
{
	memset( TonIntData, 0xFF, sizeof( TonIntData ));

	// fprintf( stderr, "Init ToneIntData Size : %ld Value %ld\n", (long) sizeof( TonIntData ), TonIntData[0][0][0] );
	return 0;
}

void NewInterval( int ch, int note, long time )
{
int i;
	
	for( i=0; i<MAX_ENTRIES; i++ )
	{	
		if( TonIntData[ch][note][i] < 0 )
		{
			TonIntData[ch][note][i] = time;
			return;
		}
	}
	
	// if( i == MAX_ENTRIES )
	{
		fprintf( stderr, "Error : TonIntData no free entry\n");
	}
	return;
}

void EndInterval( int ch, int note, long time )
{
int i;
TonInterval entry;

	// Get first Entry in list
	
	if( TonIntData[ch][note][0] < 0 )
	{
		fprintf( stderr, "Error : TonIntData entry not found\n");
	}
	entry.Set( TonIntData[ch][note][0], time, note,ch );
	entry.Write( out_stream );

	// delete first entry

	TonIntData[ch][note][0] = -1;

	// move entries to first position

	for( i=1; i<MAX_ENTRIES; i++ )
	{	
		if( TonIntData[ch][note][i] >= 0 )
		{
			TonIntData[ch][note][i-1] = TonIntData[ch][note][i];
			TonIntData[ch][note][i] = -1;
		}
		else
			return;
	}
	return;
}

//----------------------------------
//  error handler 
//----------------------------------

void Error(int errNum )
{
   switch( errNum )
   {
     case TINT_OUT_FILE_WRITE_ERROR :
          fprintf( err_stream, "Exit Rdl To TimeInterval while OutFile write error %d!\n", errNum );
          exit( errNum );
          break;

     default :
          fprintf( err_stream, "Exit Rdl To TimeInterval while error %d!\n", errNum );
          exit( errNum );
          break;
   }
}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void
RdlToTintUsage ()
{
char str[10] ="";

   SetRameauExt( str, RDL ); // input file extention
   
   fprintf (err_stream, "usage : %s [flags] [%s_file_name] \n", PROG_NAME, str);
   fprintf (err_stream, "\n");
   fprintf (err_stream, "flags : -? this output\n");
   fprintf (err_stream, "      : -h this output\n");
   fprintf (err_stream, "      : -v disp program version\n");
   fprintf (err_stream, "      : -o output to stdout\n");
   fprintf (err_stream, "      : -i input from stdin\n");
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
   Tint    tint;

   /*----------------------------------------------------------
   programm flags setting und validation
   ------------------------------------------------------------*/

   InitTonIntData();
	
   for (i = 1; i < argc; i++)
   {
      if (argv[i][0] == '-')    /* program flags */
      {
     switch (argv[i][1])
     {
     case '?':
     case 'h':
     case 'H':
        RdlToTintUsage ();
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
     case 'V':
        VerboseFlag = 1;
        break;

     case 'l':
        ReadLineFlag = 1;
        break;

     default:
        RdlToTintUsage ();
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
		   SetRameauExt( buf, TINT);
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
        RdlToTintUsage ();
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

   tint.SetInStream( NULL );
   tint.SetOutStream( out_stream );
   tint.SetErrStream( err_stream );
   tint.SetMsgStream( out_stream );
   tint.MsgOff();
   tint.TxtOff();

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
      
	  tint.SetBar( rdl.GetBar() );
      if (last_time < time_stamp)
      {
         tint.NewTime( time_stamp );
         last_time = time_stamp;
      }
      Notes.SetChNotes( channel_nodes );
		
	  {
		int ch;

		for( ch=0; ch < MAX_CHANNEL; ch++ )
		{
			if( channel_nodes[ch] > 0 )
			{
				 NewInterval( ch, channel_nodes[ch], time_stamp );
			}
			if( channel_nodes[ch] < 0 )
			{
				 EndInterval( ch, -channel_nodes[ch], time_stamp );
			}
		}
	  }
   }
   // tint.Write( TonData );
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
