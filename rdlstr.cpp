/*-------------------------------------------------------------------------
    Dieter Neubacher           Vers.: 1.0                 Wuhu 30.06.97
    -----------------------------------------------------------------------
    rdlstr.cpp
---------------------------------------------------------------------------
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "rdlstr.h"

#define RDLSTR_OUT_FILE_WRITE_ERROR -1
#define RDLSTR_INPUT_LINE_TO_BIG    -2
#define RDLSTR_INPUT_INVALID_FORMAT -3
#define RDLSTR_IN_FILE_READ_ERROR   -4

void RdlStr::Init( void )
{
    InitBar( 0 );
    CopyMsgLine = 0;
    CopyTxtLine = 1;
    ClearModNotes();

}

RdlStr::RdlStr()
{
    InStream  = stdin;
    OutStream = stdout;
    ErrStream = stderr;
    MsgStream = stdout;
    Init();
}

RdlStr::RdlStr(FILE *inFile, FILE *outFile )
{
    ErrStream = stderr;
    MsgStream = stdout;
    InStream  = inFile;
    OutStream = outFile;
    Init();
}

RdlStr::RdlStr(FILE *inFile, FILE *outFile, FILE *errFile, FILE *msgFile )
{
    ErrStream =  errFile;
    MsgStream =  msgFile;
    InStream  =  inFile;
    OutStream =  outFile;
    Init();
}

RdlStr::RdlStr(char *inFile, char *outFile )
{
    ErrStream = stderr;
    MsgStream = stdout;
    InStream  = OpenInFile( inFile );
    OutStream = OpenOutFile( outFile );
    Init();
}

RdlStr::RdlStr(char *inFile, char *outFile, char *errFile, char *msgFile )
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

void RdlStr::Error(int errNum )
{
   switch( errNum )
   {
     case RDLSTR_OUT_FILE_WRITE_ERROR :
              fprintf( ErrStream, "Exit RdlStr while OutFile write error %d!\n", errNum );
              exit( errNum );
              break;
     case RDLSTR_INPUT_LINE_TO_BIG :
              fprintf( ErrStream, "Exit RdlStr while Input line to big\n" );
              exit( errNum );
              break;
     case RDLSTR_INPUT_INVALID_FORMAT :
              fprintf( ErrStream, "RdlStr input file : invalid format\n" );
              break;

     case RDLSTR_IN_FILE_READ_ERROR :
              fprintf( ErrStream, "RdlStr file read error\n" );
              break;


     default :
              fprintf( ErrStream, "Exit RdlStr while error %d!\n", errNum );
              exit( errNum );
              break;
   }
}
/*------------------------------------
--------------------------------------
*/

FILE * RdlStr::OpenInFile (char *filename )
{
   InStream = fopen ( filename, "r" );
   if( InStream == NULL )
   {
      fprintf (ErrStream, "\n!!! RdlStr input file %s open error !!!", filename);
      return NULL;
   }
   return InStream;
}

FILE * RdlStr::OpenOutFile( char *filename )
{
   OutStream = fopen ( filename, "w" );
   if (OutStream == NULL)
   {
      fprintf (ErrStream, "\n!!! RdlStr output file %s open error !!!", filename);
      return NULL;
   }
   return OutStream;
}

FILE * RdlStr::OpenErrFile( char *filename )
{
   ErrStream = fopen ( filename, "w" );
   if (ErrStream == NULL)
   {
      fprintf (ErrStream, "\n!!! RdlStr error file %s open error !!!", filename);
      return NULL;
   }
   return ErrStream;
}

FILE * RdlStr::OpenMsgFile( char *filename )
{
   MsgStream = fopen ( filename, "w");
   if (MsgStream == NULL)
   {
      fprintf (ErrStream, "\n!!! RdlStr massage file %s open error !!!", filename);
      return NULL;
   }
   return MsgStream;
}

/*-------------------------------------------------------------------------
     read RdlStr line
---------------------------------------------------------------------------
*/

long RdlStr::ReadRdlStrLine( void )
{
long      time;
int           tmpBar;
int           i = 0, line_size, bufPos = 0, err=0;
char      *StrPos;

   ClearModNotes();

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
     if (InBuf[0] == '#' )
     {
       // Get bar info

       if( strncmp( "# bar ", InBuf, 6 ) == 0 )
       {
          if( sscanf( InBuf + 6, "%d", &tmpBar ) != 1 )
          {
             fprintf (ErrStream, "RdlStr read bar info format error\n");
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

     if ((line_size = ReadLine ()) <= 0)
         return (-1L);
   }
   //--------------
   // Get RdlStr data
   //--------------

   StrPos = InBuf;
   
   do
   {  
   char buf[10];
   int  count, val;

      err   = 1;
      count = 0;
         
      /* TrennZeichen */
      while( *StrPos == ',' || *StrPos == ';' || *StrPos == ' ' || *StrPos == '\t' )
      {
         StrPos++;
         err = 0;
      }           
      /* Value */
      while(isdigit (*StrPos))
      { 
         buf[count++] = *StrPos++;
         buf[count]   = 0;
         // printf( "%s,", buf );
         if( count > 4 ) 
            err = 1;
         else
            err = 0;
          }   
          
      if( err == 1 )
          { 
                Error( RDLSTR_INPUT_INVALID_FORMAT );  // exit Program
                *StrPos = 0;                                       
          }
      else if( count > 0 )
      {         
         // printf( "%s ", buf );
         val = atoi( buf );
         ModNote[ val%12 ]++;
      }
   }
   while ( *StrPos != 0 && *StrPos != '\n'  && *StrPos != '\r' );
   return (time);

}
/*---------------------------------------------------------------------
    Flash all infos while time is changed
-----------------------------------------------------------------------
*/
void RdlStr::NewTime ( void )
{
    NewBar();
}
void RdlStr::NewBar ( void )
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
int RdlStr::Close ( void )
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

int RdlStr::ReadLine (void)
{
   int     count = 0, ch;

   while (count < RDLSTR_IO_BUF_SIZE)
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

   if( count == RDLSTR_IO_BUF_SIZE )
   {
      Error( RDLSTR_INPUT_LINE_TO_BIG ); // exit program
   }

   return (count);
}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
uint
RdlStr::GetCode(void)
{
   int     i;
   uint    code = 0;
   
   for( i=0; i<12; i++ )
   {
      if( ModNote[i] > 0)
      {
             code |= (0x8000 >> (i));
      }
   }

   return (code);
}
/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
uint
RdlStr::GetPrimCode(void)
{
   int     i, j, length = 32;
   uint    prim, test = 0, code;
   ulong   trans;

   code = GetCode();

   trans = prim = 0xFFF0 & code;/* left 12 bit */
   trans <<= 12;
   trans |= (ulong) prim;
   trans <<= 4;

   for (i = 0; i < 12; i++)
   {
      if (trans & 0x80000000L)
      {
         test = (uint) (trans >> 16);
         test &= 0xFFF0;
         /* test length */
         for (j = 11; j >= 0; j--)
         {
            if (test & (0x8000 >> j))
               break;
         }
         if (j < length)
         {
            prim = test;
            length = j;
         }
         else if (test > prim && j <= length)
         {
            prim = test;
            length = j;
         }
      }
      trans <<= 1;              /* shift one pos to left */
   }

   /* mirror note-code */

   for (i = 0, test = 0; i < 12; i++)
   {
      if (code & 0x8000)        /* test first bit */
      {
         test |= (0x8000 >> (11-i));
      }
      code <<= 1;
   }

   trans = test;
   trans <<= 12;
   trans |= (ulong) test;
   trans <<= 4;

   for (i = 0; i < 12; i++)
   {
      if (trans & 0x80000000L)
      {
         test = (uint) (trans >> 16);
         test &= 0xFFF0;
         /* test length */
         for (j = 11; j >= 0; j--)
         {
            if (test & (0x8000 >> j))
               break;
         }
         if (j < length)
         {
            prim = test;
            length = j;
         }
         else if (test > prim && j <= length)
         {
            prim = test;
            length = j;
         }
      }
      trans <<= 1;              /* shift one pos to left */
   }

   return (prim);
}
/*-----------------------------------------------------------------
-------------------------------------------------------------------
*/
int  RdlStr::WriteRdlStrLine( long time, ChNotes &Notes )
{
int    i, n, Count;
long   min, max, total; 
double average;

     /* write RdlStr-string to output file */

     max = total= 0;
     min = MAX_NOTE_VAL;
	 Count = 0;

     for (n = 0; n < MAX_NOTE_VAL; n++)
     {
        for( i=0; i < MAX_CHANNEL; i++ )
        {
          if( Notes.Get( i, n ) != 0 )
		  {
			  if( min > n ) min = n;
			  if( max < n ) max = n;

			  Count += Notes.Get( i, n );
			  total += Notes.Get( i, n ) * n;
		  }
        }
     }
     if( Count > 0 )
     {
	    average = ((double)total) / ((double) Count);
		fprintf( OutStream, "%3d %7.4f %3d\n", min, average, max );
	 }
	 else
	 {
		 fprintf( OutStream, "* no value\n" );
	 }
     if( ferror( ErrStream ) )
     {
        Error( RDLSTR_OUT_FILE_WRITE_ERROR );
     }
     return 0;
}
