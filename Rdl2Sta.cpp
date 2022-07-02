/*---------------------------------------------------------------------
	Dieter Neubacher 
	-------------------------------------------------------------------
	Build a statistic of a Rdl File

	rdl2sta.cpp
-----------------------------------------------------------------------
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "rameau.h"
#include "global.h"
#include "version.h"
#include "rdl.h"
#include "ChNotes.h"


#define PROG_NAME   "rdl2sta"

#define STA_OUT_FILE_WRITE_ERROR -1

void  RdlToStaUsage (void);

/*
#define FREQUENZ_DEBUG
*/

/*------------------*/
/* global variables */
/*------------------*/

ChNotes Notes, OldNotes;

FILE   *in_stream = NULL, *out_stream = NULL, *err_stream= stderr;
int    VerboseFlag  = 0;
int    ReadLineFlag = 0;

// Funktions

void	OutputLine( FILE * stream );

// use Channel -1 for Global Statistic Output
int		OutputStatistic( int Channal, long TotalNoteCount, long TotalNoteCountVec[], long TotalModNoteCount[], long TotalFrequenz[], long TotalFrequenzMod[] );

//--------------------------------
// Print a "* ----" Line 
//--------------------------------

void OutputLine( FILE * stream )
{
	char Buffer[80];

	memset( Buffer, '-', sizeof( Buffer ));
	Buffer[sizeof( Buffer ) -1] = '\n';
	Buffer[0] = '*';
	fwrite(	Buffer, 1, sizeof( Buffer ), stream );

	return;
}

//----------------------------------
//  error handler for str
//----------------------------------

void Error(int errNum )
{
   switch( errNum )
   {
     case STA_OUT_FILE_WRITE_ERROR :
          fprintf( err_stream, "Exit Rdl2Sta while OutFile write error %d!\n", errNum );
          exit( errNum );
          break;

     default :
          fprintf( err_stream, "Exit Rdl2Sta while error %d!\n", errNum );
          exit( errNum );
          break;
   }
}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void
RdlToStaUsage ()
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
   int     i,j,ch;
   int     channel_nodes[16];
   long    time_stamp, last_time = -1;
   long    LineReadCounter = 0L;

   Rdl     rdl;
   
double Sum = 0.0;
int    Min=MAX_NOTE_VAL, Max = -1;

long NoteCount[MAX_CHANNEL];
long NoteCountVec[MAX_CHANNEL][MAX_NOTE_VAL + 1];
long ModNoteCount[MAX_CHANNEL][12];

long TotalNoteCount = 0L;
long TotalNoteCountVec[MAX_NOTE_VAL + 1];
long TotalModNoteCount[12];

long Frequenz[MAX_CHANNEL][MAX_NOTE_VAL + 1];
long FrequenzMod[MAX_CHANNEL][12];

long TotalFrequenz[MAX_NOTE_VAL + 1];
long TotalFrequenzMod[12];


   /*----------------------------------------------------------
   programm flags setting und validation
   ------------------------------------------------------------*/

	memset( NoteCount, 0, sizeof( NoteCount ));
	memset( NoteCountVec, 0, sizeof( NoteCountVec ));
	memset( ModNoteCount, 0, sizeof( ModNoteCount ));
	memset( TotalNoteCountVec, 0, sizeof( TotalNoteCountVec ));
	memset( TotalModNoteCount, 0, sizeof( TotalModNoteCount ));
	memset( Frequenz, 0, sizeof( Frequenz ));
	memset( FrequenzMod, 0, sizeof(	FrequenzMod ));
	memset( TotalFrequenz, 0, sizeof( TotalFrequenz ));
	memset( TotalFrequenzMod, 0, sizeof( TotalFrequenzMod ));

   for (i = 1; i < argc; i++)
   {
      if (argv[i][0] == '-')    /* program flags */
      {
     switch (argv[i][1])
     {
     case '?':
     case 'h':
     case 'H':
        RdlToStaUsage ();
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
        RdlToStaUsage ();
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
		   SetRameauExt( buf, RDLSTA );
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
        RdlToStaUsage ();
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

   rdl.SetOutStream( NULL );
   rdl.SetErrStream( err_stream );
   rdl.SetMsgStream( out_stream );
   rdl.MsgOn();
   rdl.TxtOn();

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


      if (last_time < time_stamp)
      {
  	     // Calculate Frequenses for each Channel


		 // Store Last Note-Values

	/****
 	     fprintf( out_stream, "\nOld Ch 0: ");
		 for( i=40; i<90; i++ )
		 {
			if( (i%10) == 0 ) fprintf( out_stream, " ");
			fprintf( out_stream, "%1d", OldNotes.Get( 0, i ) );
		 }
 	     fprintf( out_stream, "\nVAL Ch 0: ");
		 for( i=40; i<90; i++ )
		 {
			if( (i%10) == 0 ) fprintf( out_stream, " ");
			fprintf( out_stream, "%1d", Notes.Get( 0, i ) );
		 }
 	     fprintf( out_stream, "\nNEW Ch 0: ");
		 for( i=40; i<90; i++ )
		 {
			if( (i%10) == 0 ) fprintf( out_stream, " ");
			fprintf( out_stream, "%1d", Notes.GetNew( 0, i ) );
		 }
		 fprintf( out_stream, "\n");
	****/


		Notes.SetMinMax();    // Set min and max Value for each Channel	
		OldNotes.SetMinMax(); // Set min and max Value for each Channel	
		
		for( ch = 0; ch < MAX_CHANNEL; ch++ )
		{

#ifdef FREQUENZ_DEBUG

			fprintf( out_stream, "TIME : %8ld\n", last_time );
			fprintf( out_stream, "Old : ");

			for(int max = OldNotes.GetMax(ch), i = OldNotes.GetMin(ch); i <= max; i++ )
			{
				if( OldNotes.Get( ch, i ) > 0 ) fprintf(out_stream, "%3d*%1d ", i, OldNotes.Get( ch, i ) );
			}
			fprintf( out_stream, "\n");
			fprintf( out_stream, "New : ");

#endif // FREQUENZ_DEBUG

			for( i = Notes.GetNewMin(ch); i <= Notes.GetNewMax(ch); i++ )
			{
				if( Notes.GetNew( ch, i ) > 0 ) 
				{
#ifdef FREQUENZ_DEBUG
					fprintf(out_stream, "%3d*%1d ", i, Notes.GetNew( ch, i ) );
#endif // FREQUENZ_DEBUG
					for( j=OldNotes.GetMin(ch); j <= OldNotes.GetMax(ch); j++ )
					{
						if( OldNotes.Get( ch, j ) > 0 )
						{
							int freq = abs( i-j );
#ifdef FREQUENZ_DEBUG
							fprintf( out_stream, "Freq: %d ", freq );
#endif // FREQUENZ_DEBUG
							// inc counter in Fequenz Vector

							Frequenz[ch][freq]         += OldNotes.Get( ch, j ) *  Notes.GetNew( ch, i );
							FrequenzMod[ch][freq % 12] += OldNotes.Get( ch, j ) *  Notes.GetNew( ch, i );
						}
					}
				}
			} // for channel
		}
#ifdef FREQUENZ_DEBUG
		fprintf( out_stream, "\n");
#endif // FREQUENZ_DEBUG

		//----------------------------------
		// Total Frequenz
		//----------------------------------
		
#ifdef FREQUENZ_DEBUG
		fprintf( out_stream, "Total Min %3d Max %3d, OldNotes Min %3d, Max %3d\n",
			Notes.GetNewMin(),
			Notes.GetNewMax(),
			OldNotes.GetMin(),
			OldNotes.GetMax()
			);
#endif // FREQUENZ_DEBUG


		for( i = Notes.GetNewMin(); i <= Notes.GetNewMax(); i++ )
		{
			if( Notes.GetNew( i ) > 0 ) 
			{
				for( j = OldNotes.GetMin(); j <= OldNotes.GetMax(); j++ )
				{
					if( OldNotes.Get( j ) > 0 ) 
					{
							int freq = abs( i-j );
							// inc counter in Fequenz Vector
							TotalFrequenz[freq]         += OldNotes.Get( j ) *  Notes.GetNew( i );
							TotalFrequenzMod[freq % 12] += OldNotes.Get( j ) *  Notes.GetNew( i );
					}
				}
			}
		}
         last_time = time_stamp;
		 OldNotes = Notes;
		 Notes.NewTime();
      }
      Notes.SetChNotes( channel_nodes );

	  for( ch = 0; ch < MAX_CHANNEL; ch++ )
	  {	
			if( channel_nodes[ch] > 0 )
			{       
					NoteCount[ch]++;
					NoteCountVec[ch][channel_nodes[ch]]++;
					ModNoteCount[ch][channel_nodes[ch]%12]++;

					TotalNoteCount++;
					TotalNoteCountVec[channel_nodes[ch]]++;
					TotalModNoteCount[channel_nodes[ch]%12]++;
			}
			// fprintf( stderr,"%4d", channel_nodes[i] );
	  }
	  /// fprintf(stderr,"\n");
	
   } // While

   // Output RDL-Statistic Data


//----------------------------------------
//	OutPut for Global (Total) Statistic
//----------------------------------------


	// use channelnumber -1 for Global Statistic output

	OutputStatistic( -1, TotalNoteCount, TotalNoteCountVec, TotalModNoteCount, TotalFrequenz, TotalFrequenzMod );

//-----------------------------------------------------------------
//	OutPut Statistic for each Channal
//-----------------------------------------------------------------

   for( ch=0; ch < MAX_CHANNEL; ch++ )
   {

	OutputStatistic( ch, NoteCount[ch], NoteCountVec[ch], ModNoteCount[ch], Frequenz[ch], FrequenzMod[ch] );
   } // for channel


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

/*-----------------------------------------------------------------
	Output Statistic
	---------------------------------------------------------------
	if channel = -1		print Global Statistic
	else				print Statistic for this channel

	this information is only used for output massages 
-------------------------------------------------------------------
*/

int OutputStatistic( int channel, long TotalNoteCount, long TotalNoteCountVec[], long TotalModNoteCount[], long TotalFrequenz[], long TotalFrequenzMod[] )
{
int		Min, Max, i;
double	Sum, Total, persent;

	Min=MAX_NOTE_VAL, Max = -1;
	Sum = 0.0;

	for( i=1; i<=MAX_NOTE_VAL; i++ )
	{   
		if(	TotalNoteCountVec[i] > 0 && Min > i ) 
			Min = i;
		if(	TotalNoteCountVec[i] > 0 && Max < i ) 
			Max = i;
		Sum += i * TotalNoteCountVec[i]; 
	}
	if( Min <= Max )
	{

		OutputLine( out_stream ); // -----------------------------
		fprintf( out_stream, "* Basic Statistics " );

		if( channel == -1 )
		    fprintf( out_stream, ":\n" );
		else
		    fprintf( out_stream, "channel %d:\n", channel);

		OutputLine( out_stream ); // -----------------------------
	    fprintf( out_stream, "* absolute and relative frequency of pitches " );
		if( channel == -1 )
		    fprintf( out_stream, ":\n" );
		else
		    fprintf( out_stream, "channel %d:\n", channel);
		OutputLine( out_stream ); // -----------------------------
		fprintf( out_stream, "* Total   : %6ld\n", TotalNoteCount );
		fprintf( out_stream, "* Minimum : %6d\n", Min );
		fprintf( out_stream, "* Average : %6f\n", Sum / (double) TotalNoteCount );
		fprintf( out_stream, "* Maximum : %6d\n", Max );
		OutputLine( out_stream ); // -----------------------------


		for( i=Min; i <= Max; i++ )
		{   
		   persent = (double)TotalNoteCountVec[i] / (double)TotalNoteCount * 100.0; 
		   fprintf( out_stream,"%3d: %6ld   %5.2f %c\n", i, TotalNoteCountVec[i] , persent, '%');
		}

		OutputLine( out_stream ); // -----------------------------
// 14.07.97  fprintf( out_stream, "* Modulo 12 " );
	    fprintf( out_stream, "* absolute and relative frequency of pitches classes (pitches modulo 12) " );

		if( channel == -1 )
		    fprintf( out_stream, ":\n" );
		else
		    fprintf( out_stream, "channel %d:\n", channel);

		OutputLine( out_stream ); // -----------------------------

	   for( i=0; i<12; i++ )
	   {
		   persent = (double)TotalModNoteCount[i] / (double)TotalNoteCount * 100.0; 
		   fprintf( out_stream,"%3d: %6ld   %5.2f %c\n", i, TotalModNoteCount[i] , persent, '%' );
	   }
		//------------------------
		//  output Frequenz Data
		//------------------------

		    Min = MAX_NOTE_VAL;
			Max = -1;
			Sum = 0.0;
			Total = 0.0;

			for( i=0; i <= MAX_NOTE_VAL; i++ )
			{   
				if(	TotalFrequenz[i] > 0 && Min > i ) 
					Min = i;
				if(	TotalFrequenz[i] > 0 && Max < i ) 
					Max = i;
				Sum   += TotalFrequenz[i]; 
				Total += TotalFrequenz[i] * i;
			}
			if( Min <= Max )
			{
				OutputLine( out_stream ); // -----------------------------
// 14.07.97		fprintf( out_stream, "* absolute frequency of successive intervals " );
				fprintf( out_stream, "* absolute and relative frequency of successive intervals " );

				if( channel == -1 )
				    fprintf( out_stream, ":\n" );
				else
					fprintf( out_stream, "channel %d:\n", channel);

				OutputLine( out_stream ); // -----------------------------
				fprintf( out_stream, "* Minimal Frequenz: %3d\n", Min);
				fprintf( out_stream, "* Maximal Frequenz: %3d\n", Max);
				fprintf( out_stream, "* Average Frequenz: %3.2f\n", (double)Total / (double) Sum );
				OutputLine( out_stream ); // -----------------------------
				
				// Frequenz
    			
				for( i=Min; i <= Max; i++ )
				{   
					persent = 100.0 * (double)TotalFrequenz[i] / (double)Sum;
					fprintf( out_stream,"%3d: %6ld   %5.2f %c\n", i, TotalFrequenz[i], persent, '%' );
				}
				// Frequenz Modulo
				OutputLine( out_stream ); // -----------------------------
//  14.07.97	fprintf( out_stream, "* absolute frequency of successive intervals modulo 12 " );
				fprintf( out_stream, "* absolute and relative frequency of successive intervals modulo 12 " );

				if( channel == -1 )
				    fprintf( out_stream, ":\n" );
				else
					fprintf( out_stream, "channel %d:\n", channel);

				OutputLine( out_stream ); // -----------------------------
    			for( i=0; i <= 11; i++ )
				{   
					persent = 100.0 * (double)TotalFrequenzMod[i] / (double)Sum;
					fprintf( out_stream,"%3d: %6ld   %5.2f %c\n", i, TotalFrequenzMod[i], persent, '%' );
				}
			}
	}
	return 0;
}

