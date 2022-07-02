/*-------------------------------------------------------------------------
    Dieter Neubacher           Vers.: 1.0                 Wuhu 10.07.97
    -----------------------------------------------------------------------
    StrData.cpp
---------------------------------------------------------------------------
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "StrData.h"

#define STRDATA_OUT_FILE_WRITE_ERROR -1
#define STRDATA_INPUT_LINE_TO_BIG    -2
#define STRDATA_INPUT_INVALID_FORMAT -3
#define STRDATA_IN_FILE_READ_ERROR   -4

void StrDataFile::Init( void )
{
    InitBar( 0 );
    CopyMsgLine = 0;
    CopyTxtLine = 1;
}

StrDataFile::StrDataFile()
{
    InStream  = stdin;
    OutStream = stdout;
    ErrStream = stderr;
    MsgStream = stdout;
    Init();
}

StrDataFile::StrDataFile(FILE *inFile, FILE *outFile )
{
    ErrStream = stderr;
    MsgStream = stdout;
    InStream  = inFile;
    OutStream = outFile;
    Init();
}

StrDataFile::StrDataFile(FILE *inFile, FILE *outFile, FILE *errFile, FILE *msgFile )
{
    ErrStream =  errFile;
    MsgStream =  msgFile;
    InStream  =  inFile;
    OutStream =  outFile;
    Init();
}

StrDataFile::StrDataFile(char *inFile, char *outFile )
{
    ErrStream = stderr;
    MsgStream = stdout;
    InStream  = OpenInFile( inFile );
    OutStream = OpenOutFile( outFile );
    Init();
}

StrDataFile::StrDataFile(char *inFile, char *outFile, char *errFile, char *msgFile )
{
    ErrStream = OpenErrFile( errFile );
    MsgStream = OpenMsgFile( msgFile );
    InStream  = OpenInFile( inFile );
    OutStream = OpenOutFile( outFile );
    Init();
}

//----------------------------------
//  error handler for VPS
//----------------------------------

void StrDataFile::Error(int errNum )
{
   switch( errNum )
   {
     case STRDATA_OUT_FILE_WRITE_ERROR :
              fprintf( ErrStream, "Exit StrData while OutFile write error %d!\n", errNum );
              exit( errNum );
              break;
     case STRDATA_INPUT_LINE_TO_BIG :
              fprintf( ErrStream, "Exit StrData while Input line to big\n" );
              exit( errNum );
              break;
     case STRDATA_INPUT_INVALID_FORMAT :
              fprintf( ErrStream, "StrData input file : invalid format\n" );
              break;

     case STRDATA_IN_FILE_READ_ERROR :
              fprintf( ErrStream, "StrData file read error\n" );
              break;


     default :
              fprintf( ErrStream, "Exit StrData while error %d!\n", errNum );
              exit( errNum );
              break;
   }
}
/*------------------------------------
--------------------------------------
*/

FILE * StrDataFile::OpenInFile (char *filename )
{
   InStream = fopen ( filename, "r" );
   if( InStream == NULL )
   {
      fprintf (ErrStream, "\n!!! StrData input file %s open error !!!", filename);
      return NULL;
   }
   return InStream;
}

FILE * StrDataFile::OpenOutFile( char *filename )
{
   OutStream = fopen ( filename, "w" );
   if (OutStream == NULL)
   {
      fprintf (ErrStream, "\n!!! StrData output file %s open error !!!", filename);
      return NULL;
   }
   return OutStream;
}

FILE * StrDataFile::OpenErrFile( char *filename )
{
   ErrStream = fopen ( filename, "w" );
   if (ErrStream == NULL)
   {
      fprintf (ErrStream, "\n!!! StrData error file %s open error !!!", filename);
      return NULL;
   }
   return ErrStream;
}

FILE * StrDataFile::OpenMsgFile( char *filename )
{
   MsgStream = fopen ( filename, "w");
   if (MsgStream == NULL)
   {
      fprintf (ErrStream, "\n!!! StrData massage file %s open error !!!", filename);
      return NULL;
   }
   return MsgStream;
}

/*-------------------------------------------------------------------------
     read StrData line
---------------------------------------------------------------------------
*/

long StrDataFile::ReadLine( void )
{
long      time = 0;
int       tmpBar;
int       i = 0, line_size, bufPos = 0, err=0;
char      *StrPos;


   if ((line_size = ReadNextLine ()) <= 0)
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
     if (InBuf[0] == '#' )
     {
       // Get bar info

       if( strncmp( "# bar ", InBuf, 6 ) == 0 )
       {
          if( sscanf( InBuf + 6, "%d", &tmpBar ) != 1 )
          {
             fprintf (ErrStream, "StrData read bar info format error\n");
          }
          else
          {
             SetBar( tmpBar );
             NewTime();
          }
       }
       if (CopyMsgLine)         /* copy massage */
       {
          fwrite (InBuf, 1, line_size, OutStream);
       }
     }

     /* read next line */

     if ((line_size = ReadNextLine ()) <= 0)
         return (-1L);
   }
   //--------------
   // Get StrData data
   //--------------

   StrPos = InBuf;
   
   {  
       Error( STRDATA_INPUT_INVALID_FORMAT );  // exit Program
       *StrPos = 0;                                       
   }
   return (time);

}
/*---------------------------------------------------------------------
    Flash all infos while time is changed
-----------------------------------------------------------------------
*/
void StrDataFile::NewTime ( void )
{
    NewBar();
}
void StrDataFile::NewBar ( void )
{
    // write bar info to outfile

    if( LastBar != Bar )
    {
       fprintf( OutStream,"# bar %d\n", Bar );
       LastBar = Bar;
    }
}
/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
int StrDataFile::Close ( void )
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

int StrDataFile::ReadNextLine (void)
{
   int     count = 0, ch;

   while (count < STRDATA_IO_BUF_SIZE)
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

   if( count == STRDATA_IO_BUF_SIZE )
   {
      Error( STRDATA_INPUT_LINE_TO_BIG ); // exit program
   }

   return (count);
}

/*-----------------------------------------------------------------
-------------------------------------------------------------------
*/
int  StrDataFile::WriteLine( )
{
     if( ferror( ErrStream ) )
     {
        Error( STRDATA_OUT_FILE_WRITE_ERROR );
     }
     return 0;
}
