/*-------------------------------------------------------------------------
    Dieter Neubacher         Vers.: 1.00                     Wuhu, 12-07-97
    -----------------------------------------------------------------------
    Tint.cpp
    -----------------------------------------------------------------------
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "TonInt.h"



#define TINT_OUT_FILE_WRITE_ERROR -1



/*-------------------------------------------------------------------------
    Functions for r/w TonIntervall files
---------------------------------------------------------------------------
*/

void Tint::Init( void )
{
    CopyTxtLine = CopyMsgLine = 1;
    InStream  = stdin;
    OutStream = stdout;
    ErrStream = stderr;
    MsgStream = stdout;
    InitBar( 0 );
}


Tint::Tint()
{
    Init();
    InStream  = stdin;
    OutStream = stdout;
    ErrStream = stderr;
    MsgStream = stdout;
}

Tint::Tint(FILE *inFile, FILE *outFile )
{
    Init();
    InStream  = inFile;
    OutStream = outFile;
}

Tint::Tint(FILE *inFile, FILE *outFile, FILE *errFile, FILE *msgFile )
{
    Init();
    ErrStream =  errFile;
    MsgStream =  msgFile;
    InStream  =  inFile;
    OutStream =  outFile;
}

Tint::Tint(char *inFile, char *outFile )
{
    Init();
    InStream  = OpenInFile( inFile );
    OutStream = OpenOutFile( outFile );
}

Tint::Tint(char *inFile, char *outFile, char *errFile, char *msgFile )
{
    Init();
    ErrStream = OpenErrFile( errFile );
    MsgStream = OpenMsgFile( msgFile );
    InStream  = OpenInFile( inFile );
    OutStream = OpenOutFile( outFile );
}
/*------------------------------------
--------------------------------------
*/

FILE * Tint::OpenInFile (char *filename )
{
   InStream = fopen ( filename, "r");
   if( InStream == NULL )
   {
      fprintf (ErrStream, "\n!!! TonIntervall input file %s open error !!!", filename);
      return NULL;
   }
   return InStream;
}

FILE * Tint::OpenOutFile( char *filename )
{
   OutStream = fopen ( filename, "w");
   if (OutStream == NULL)
   {
      fprintf (ErrStream, "\n!!! TonIntervall output file %s open error !!!", filename);
      return NULL;
   }
   return OutStream;
}

FILE * Tint::OpenErrFile( char *filename )
{
   ErrStream = fopen ( filename, "w");
   if (ErrStream == NULL)
   {
      ErrStream = stderr;
      fprintf (ErrStream, "\n!!! ToneIntervall error file %s open error !!!", filename);
      return NULL;
   }
   return ErrStream;
}

FILE * Tint::OpenMsgFile( char *filename )
{
   MsgStream = fopen ( filename, "w");
   if (MsgStream == NULL)
   {
      fprintf (ErrStream, "\n!!! ToneIntervall massage file %s open error !!!", filename);
      return NULL;
   }
   return MsgStream;
}
//----------------------------------
//  error handler for Tint
//----------------------------------

void Tint::Error(int errNum )
{
   switch( errNum )
   {
     case TINT_OUT_FILE_WRITE_ERROR :
              fprintf( ErrStream, "Exit TINT while OutFile write error %d!\n", errNum );
              exit( errNum );
              break;

     default :
              fprintf( ErrStream, "Exit TINT while error %d!\n", errNum );
              exit( errNum );
              break;
   }
}

void Tint::NewTime( long time )
{
	Time = time;
    NewBar();
}

void Tint::NewBar ( void )
{
    // write bar info to outfile

    if( LastBar != Bar )
    {
	TonInterval tmp;

       fprintf( OutStream,"# bar %04d Time %08ld\n", Bar, Time );
	   tmp.Set( Time,Time, 999,999 );
	   tmp.Write( OutStream );

       LastBar = Bar;
    }
}

/*-------------------------------------------------------------------------
---------------------------------------------------------------------------
*/

long Tint::Read ( TonInterval *tint )
{
   long    btime, etime;
   int     note, channel;
   int     i = 0, line_size, bufPos = 0;
   int     tmpBar;


   if ((line_size = ReadLine ()) <= 0)
      return (-1L);

   /* commentar mark or note bar mark */

   while (InBuf[0] == '*' || InBuf[0] == '#')
   {
     if (InBuf[0] == '*' && MsgStream != NULL )
     {
        if (CopyTxtLine)          /* copy commentar */
        {
          fwrite (InBuf, 1, line_size, MsgStream);
        }
     }
     else  /* note bar mark */
     if (InBuf[0] == '#' /* vers. 1.4 && OutStream != NULL */)
     {
       // Get bar info

       if( strncmp( "# bar ", InBuf, 6 ) == 0 )
       {
          if( sscanf( InBuf + 6, "%d", &tmpBar ) != 1 )
          {
             fprintf (ErrStream, "TINT read bar info format error\n");
          }
          else
          {
             SetBar( tmpBar );
          }
       }
     }
     else
     if (InBuf[0] == '#' && CopyMsgLine) /* copy massage */
     {
         fwrite (InBuf, 1, line_size, MsgStream);
     }


     /* read next line */

     if ((line_size = ReadLine ()) <= 0)
         return (-1L);
   }
   if (sscanf (InBuf, "%ld:%ld:%d:%d", &btime, &etime, &note, &channel ) != 4)
   {
      fprintf (ErrStream, "TINT file read error\n");
      return (-1L);
   }
	
   tint->Set( btime, etime,note,channel );
	   
   return ( btime );
}


/*---------------------------------------------------------------------
    write output line
-----------------------------------------------------------------------
*/


int Tint::Write ( TonInterval tinterval )
{
	tinterval.Write( OutStream );

	if( ferror( OutStream ) )
	{
		Error( TINT_OUT_FILE_WRITE_ERROR );
	}

   return 0;
}

int Tint::Close ( void )
{
   if( InStream  != NULL )    fclose (InStream);
   if( OutStream != NULL )    fclose (OutStream);
   if( ErrStream != NULL )    fclose (ErrStream);
   if( MsgStream != NULL )    fclose (MsgStream);

   return 0;
}



/*-------------------------------------------------------------------------
    read one line from InStream to InBuf
    set EOL at the end of the line
    Return : number of characters read sucsecfull
             of error  -1
---------------------------------------------------------------------------
*/

int Tint::ReadLine (void)
{
   int     count = 0, ch;

   while (count < IO_BUF_SIZE)
   {
      switch (ch = fgetc (InStream))
      {
      case '\n':
         InBuf[count++] = '\n';
         return (count);
         break;

      case '\t':
         do
         {
            InBuf[count++] = ' ';
         }
         while( count % 8 );
         break;
      case '\r':
         break;
      case EOF:
         InBuf[count] = '\n';
         return 0;
         break;

      default:
         InBuf[count++] = ch;
         break;
      }
   }

   return (count);
}
//-------------------------------------------------------------------

void TonInterval::Set( long stime, long etime, int note, int ch )
{
	StartTime	= stime;
	EndTime		= etime;
	Note		= note;
	Channel		= ch;
}

int TonInterval::Write( FILE *stream )
{

	return fprintf( stream, "%08ld:%08ld:%03d:%03d\n", StartTime, EndTime, Note, Channel );
}
