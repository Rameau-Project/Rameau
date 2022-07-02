/*---------------------------------------------------------------------
    Dieter Neubacher        Vers.: 1.0             WuHu       26.06.94
                            Vers.: 1.1             Wuhu       24.09.94
							Vers.: 2.0                        29-01-97 
    -------------------------------------------------------------------
    rdl2mvp.c

  Version 2.0 :  WIN32
-----------------------------------------------------------------------
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "global.h"
#include "rameau.h"
#include "version.h"
#include "rdl.h"
#include "mvp.h"
#include "TopToBot.h"

#define INP_EXT     "rdl"
#define OUT_EXT     "mvp"
#define PROG_NAME   "rdl2mvp"

#define MAX_CHANNEL 16

// #define DEBUG_NOTE_ERROR   // search error in m2. file at bar 128


/*------------------*/
/* local functions  */
/*------------------*/

void rdl_to_mvp_usage (void);

int  BuildMvpFirstPass( char *InFile, char *OutFile );
int  BuildMvpSecondPass( char *InFile, char *OutFile );
int  FileRevers( char *InFile, char *OutFile );

/*------------------*/
/* global variables */
/*------------------*/


struct
{
     // Pogramm Flags

int  ErrorStreamFlag;
int  PrintAllNotes;

char InFileName[_MAX_PATH];
char OutFileName[_MAX_PATH];
char RevFileName[_MAX_PATH];

FILE *err_stream;
FILE *in_stream;
FILE *out_stream;

int  *UseChannels;

int    VerboseFlag;
int    ReadLineFlag;
}

Rdl2Mvp =

// Default settings

{
     0,
     0,
     "",
     "",
     "",
     stderr,
     stdin,
     stdout,
     NULL,
     0,
     0
};


/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
void rdl_to_mvp_usage ()
{
   fprintf (Rdl2Mvp.err_stream, "usage : %s [flags] %s_file_name\n", PROG_NAME, INP_EXT);
   fprintf (Rdl2Mvp.err_stream, "\n");
   fprintf (Rdl2Mvp.err_stream, "flags : -? this output\n");
   fprintf (Rdl2Mvp.err_stream, "      : -h this output\n");
   fprintf (Rdl2Mvp.err_stream, "      : -v disp program version\n");
   fprintf (Rdl2Mvp.err_stream, "      : -a print all nodes\n");
   fprintf (Rdl2Mvp.err_stream, "      : -ch= channel order\n");
   fprintf (Rdl2Mvp.err_stream, "      : -V verbose\n");
   fprintf (Rdl2Mvp.err_stream, "      : -l disp read line numbers\n");
   fprintf (Rdl2Mvp.err_stream, "      : -D debug flag\n");
   fprintf (Rdl2Mvp.err_stream, "\n");

   return;
}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/

int main (int argc, char *argv[])
{
int        i;
                       
     DebugFlag = 0;                  
/*----------------------------------------------------------
programm flags setting und validation
------------------------------------------------------------
*/
   for (i = 1; i < argc; i++)
   {
      if (argv[i][0] == '-')    /* program flags */
      {
         switch (argv[i][1])
         {
         case '?':
         case 'h':
         case 'H':
            rdl_to_mvp_usage ();
            return (0);
            break;
         case 'v':
            rameau_version (Rdl2Mvp.err_stream);
            return (0);
            break;
         case 'e':
            {
               if ((Rdl2Mvp.err_stream = fopen (RAMEAU_ERROR_FILE, "at")) == NULL)
               {
                  Rdl2Mvp.err_stream = stderr;
                  fprintf (Rdl2Mvp.err_stream, "can't open rameau log file : %s\n", RAMEAU_ERROR_FILE);
               }
            }
            break;

         case 'a':
            // mvp.DispAllNotes();
            break;
         case 'D':
            DebugFlag = 1;
            break;

         case 'V':
            Rdl2Mvp.VerboseFlag = 1;
            break;

         case 'l':
            Rdl2Mvp.ReadLineFlag = 1;
            break;

         case 'c':
                    if (argv[i][2] == 'h' && argv[i][3] == '=')
                    {
                       if( (Rdl2Mvp.UseChannels = GetChParam( argv[i] + 4, MAX_CHANNEL )) == NULL )
                       {
                          rdl_to_mvp_usage ();
                          return (0);
                       }
                    }
                    else
                    {
                       rdl_to_mvp_usage ();
                       return (0);
                    }
                    break;
         default:
            rdl_to_mvp_usage ();
            return (0);
            break;
         }
      }
      else
         /* input/output file */
      {
         char    buf[_MAX_PATH];

         /* first parameter is input file */

         if( Rdl2Mvp.InFileName[0] == 0 )
         {
            strcpy (buf, argv[i]);

#ifdef AUTO_EXTENION
            strcat (buf, ".");
            strcat (buf, INP_EXT);
#endif /*  */
            strcpy( Rdl2Mvp.InFileName, buf );

            strcpy (buf, argv[i]);

#ifdef AUTO_EXTENION
            strcat (buf, ".");
            strcat (buf, OUT_EXT);
#endif /*  */
            strcpy ( Rdl2Mvp.OutFileName, buf );
            strcpy ( Rdl2Mvp.RevFileName, "mvp.tmp" );
         }
         else
         {
            rdl_to_mvp_usage ();
            return (3);
         }
      }
   }

   if( Rdl2Mvp.InFileName[0] == 0 )
   {
      rdl_to_mvp_usage ();
      return (4);
   }

   /*----------------------------------------------------------
     main loop
   ------------------------------------------------------------
   */

   if( Rdl2Mvp.VerboseFlag ) Rdl2Mvp.ReadLineFlag = 0;
   
   if( DebugFlag != 0 )
   {
      BuildMvpFirstPass( Rdl2Mvp.InFileName, "mvp.p1" );
      FileRevers( "mvp.p1", "mvp.r1" );
      BuildMvpSecondPass( "mvp.r1", "mvp.p2" );
      FileRevers( "mvp.p2", Rdl2Mvp.OutFileName );
   }
   else
   {   
      remove( "mvp.p1" );
      remove( "mvp.r1" );
      remove( "mvp.p2" );
   
      BuildMvpFirstPass( Rdl2Mvp.InFileName, Rdl2Mvp.RevFileName );
      FileRevers( Rdl2Mvp.RevFileName, Rdl2Mvp.OutFileName );
      BuildMvpSecondPass( Rdl2Mvp.OutFileName, Rdl2Mvp.RevFileName );
      FileRevers( Rdl2Mvp.RevFileName, Rdl2Mvp.OutFileName );
   }
   if( DebugFlag == 0 )
   {          // Erase mvp.tmp file
   FILE *tmp;

        tmp = fopen( Rdl2Mvp.RevFileName, "w" );
        if( tmp != NULL )
        {
            fclose( tmp );
        }
   }

   /*----------------------------------------------------------
   main loop end
   ------------------------------------------------------------
   */

   return (0);
}

//----------------------------------------------------------------------
// 1 Pass 1 Pass 1 Pass 1 Pass 1 Pass 1 Pass 1 Pass 1 Pass 1 Pass 1 Pass
//----------------------------------------------------------------------


int BuildMvpFirstPass( char *InFile, char *OutFile )
{
int   i, channel_notes[16];
long  time_stamp, last_time = -1;
Rdl   rdl;
Mvp   mvp;
long  LineReadCounter = 0L;

   // fprintf( stderr, "first part\n");

   rdl.SetErrStream( Rdl2Mvp.err_stream );
   mvp.SetErrStream( Rdl2Mvp.err_stream );

   Rdl2Mvp.in_stream  = rdl.OpenInFile ( InFile );
   Rdl2Mvp.out_stream = mvp.OpenOutFile( OutFile );

   if( Rdl2Mvp.in_stream == NULL || Rdl2Mvp.out_stream == NULL )
   {
      exit( 5 );
   }

   rdl.SetMsgStream( Rdl2Mvp.out_stream );
   rdl.TxtOn();
   mvp.SetMsgStream( Rdl2Mvp.out_stream );


   // set channel flags

   for( i=0; i<MAX_CHANNEL; i++ )
   {
      if( Rdl2Mvp.UseChannels == NULL )
      {
         mvp.SetChFlag(i);

         // fprintf( stderr, "use channel %d\n",i );
      }
      else
      if( Rdl2Mvp.UseChannels[i] != -1 )
      {
         mvp.SetChFlag( Rdl2Mvp.UseChannels[i] );

         // fprintf( stderr, "use channel %d\n",UseChannels[i] );
      }
   }

   /* read rdl data */

   mvp.ClearNewNoteBuf();

   while ((time_stamp = rdl.Read (channel_notes)) != -1L)
   {
      mvp.SetBar( rdl.GetBar() );

      if( Rdl2Mvp.ReadLineFlag )
      {
         fprintf( stdout, "\r%8ld", LineReadCounter++ );
      }
      if( Rdl2Mvp.VerboseFlag )
      {
         fprintf( stdout, "\rLine: %8ld Time: %8ld Bar: %8d Rdl2Mvp first Pass ", LineReadCounter++, time_stamp, rdl.GetBar() );
      }

      if ((last_time < time_stamp) && (last_time >= 0L))
      {
         /* write mvp-string to output file */
         // mvp.Write( last_time );

         mvp.WritePairs( last_time );
         mvp.NextOutputLine();

         mvp.NewTime();

         // mvp.ClearNewNoteBuf();

      }

      mvp.SetNotes( time_stamp, channel_notes );

      last_time = time_stamp;


#ifdef DEBUG_NOTE_ERROR
      if( rdl.GetBar() == 116 )
      {
         printf( "OK DEBUG" );
      }
#endif

   }

   /* Last line ; write mvp-string to output file */

   mvp.SetBar( rdl.GetBar() );
   mvp.NewTime();

   mvp.WritePairs( last_time );
   mvp.NextOutputLine();

   fclose( Rdl2Mvp.in_stream );
   fclose( Rdl2Mvp.out_stream );

   return 0;
}

//----------------------------------------------------------------------
// 2 Pass 2 Pass 2 Pass 2 Pass 2 Pass 2 Pass 2 Pass 2 Pass 2 Pass 2 Pass
//----------------------------------------------------------------------

int BuildMvpSecondPass( char *InFile, char *OutFile )
{
int      i, help,LineCount;
long     time_stamp, last_time = -1;
Mvp      mvp;
MvpEntry mvpEntry;
MvpEntry mvpEntryBuf[MAX_MVP_LINES];
char     OCol[MAX_MVP_LINES], QCol[MAX_MVP_LINES];
long     LineReadCounter = 0L;

   // fprintf( stderr, "second part\n");

   Rdl2Mvp.in_stream  = mvp.OpenInFile ( InFile );
   Rdl2Mvp.out_stream = mvp.OpenOutFile( OutFile );
   mvp.SetMsgStream( Rdl2Mvp.out_stream );
   mvp.SetErrStream( Rdl2Mvp.err_stream );
   mvp.TxtOn(); 

   if( Rdl2Mvp.in_stream == NULL || Rdl2Mvp.out_stream == NULL )
   {
      exit( 5 );
   }
   // set channel flags

   for( i=0; i<MAX_CHANNEL; i++ )
   {
      if( Rdl2Mvp.UseChannels == NULL )
      {
         mvp.SetChFlag(i);

         // fprintf( stderr, "use channel %d\n",i );
      }
      else
      if( Rdl2Mvp.UseChannels[i] != -1 )
      {
         // fprintf( stderr, "use channel %d\n",Rdl2Mvp.UseChannels[i] );
         mvp.SetChFlag( Rdl2Mvp.UseChannels[i] );
      }
   }
   // clear color flag buffer

   for( i=0; i < MAX_MVP_LINES; i++ )
   {
      OCol[i] = QCol[i] = 0;
   }
   last_time = 0x7FFFFFFL;
   while ((time_stamp = mvp.ReadMvpLine ()) != -1L)
   {
      if( Rdl2Mvp.ReadLineFlag )
      {
         fprintf( stdout, "\r%8ld", LineReadCounter++ );
      }
      if( Rdl2Mvp.VerboseFlag )
      {
         fprintf( stdout, "\rLine: %8ld Time: %8ld Bar: %8d Rdl2Mvp second Pass", LineReadCounter++, time_stamp, mvp.GetBar() );
      }
      if ((last_time > time_stamp) && (last_time >= 0L))
      {

        // fprintf( stderr, "read mvp line % 8ld\n", time_stamp );

        //--------------------
        // read new entry line
        //--------------------

        for( i=0,LineCount=0; i < MAX_CHANNEL; i++ )
        {
           if( !mvp.GetChFlag( i ) )
             continue;
           else
             LineCount++;
        }
        LineCount = LineCount * (LineCount-1) / 2;

        for( i=0; i < LineCount; i++ )
        {

           mvpEntryBuf[i] = mvp.GetEntry( i, 0 );

           // update OKTAVEN color informations

           if( (help = mvpEntryBuf[i].GetOColor()) != MVP_NO_COLOR )
           {
              mvpEntryBuf[i].SetOColor( OCol[i] );
              OCol[i] = help;
           }
           else
           if( mvpEntryBuf[i].GetStatus() == MVP_OKTAVE )
           {
              mvpEntryBuf[i].SetOColor( OCol[i] );
              OCol[i] = MVP_NO_COLOR;
           }
           else
           {
              mvpEntryBuf[i].SetOColor( OCol[i] );
           }

           // update QUINTEN color informations

           if( (help = mvpEntryBuf[i].GetQColor()) != MVP_NO_COLOR )
           {
              mvpEntryBuf[i].SetQColor( QCol[i] );
              QCol[i] = help;
           }
           else
           if( mvpEntryBuf[i].GetStatus() == MVP_QUINTE )
           {
              mvpEntryBuf[i].SetQColor( QCol[i] );
              QCol[i] = MVP_NO_COLOR;
           }
           else
           {
              mvpEntryBuf[i].SetQColor( QCol[i] );
           }
        }



        //--------------------
        // write entry line
        //--------------------


        fprintf( Rdl2Mvp.out_stream, "%9ld ", time_stamp );

        for( i=0; i < LineCount; i++ )
        {
           fputc( '(', Rdl2Mvp.out_stream );
           mvpEntryBuf[i].Write( Rdl2Mvp.out_stream );
           fputc( ')', Rdl2Mvp.out_stream );
        }
        mvp.NextOutputLine();
      }
      mvp.NewTime();
      last_time = time_stamp;
   }

   fclose( Rdl2Mvp.in_stream );
   SetFileInfoLine( Rdl2Mvp.out_stream, InFile );
   fclose( Rdl2Mvp.out_stream );

   return 0;
}

int FileRevers( char *InFile, char *OutFile )
{
TopToBot file( InFile, OutFile );

// fprintf( Rdl2Mvp.stderr, "revers \n");

   if( Rdl2Mvp.VerboseFlag      ||  Rdl2Mvp.ReadLineFlag )
   {
      fprintf( stdout, "\rrevers  " );
   }

   file.SetErrStream( Rdl2Mvp.err_stream );
   file.revers();

   return 0;
}
