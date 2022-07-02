/*-------------------------------------------------------------------------
    Dieter Neubacher           Vers.: 1.0                 Wuhu 16.02.97
    -----------------------------------------------------------------------
    Atv.cpp
---------------------------------------------------------------------------
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "atv.h"

#define ATV_OUT_FILE_WRITE_ERROR -1
#define ATV_INPUT_LINE_TO_BIG    -2
#define ATV_INPUT_INVALID_FORMAT -3
#define ATV_IN_FILE_READ_ERROR   -4

void Atv::Init( void )
{
    InitBar( 0 );
    CopyMsgLine = 0;
    CopyTxtLine = 1;
    ClearModNotes();

}

Atv::Atv()
{
    InStream  = stdin;
    OutStream = stdout;
    ErrStream = stderr;
    MsgStream = stdout;
    Init();
}

Atv::Atv(FILE *inFile, FILE *outFile )
{
    ErrStream = stderr;
    MsgStream = stdout;
    InStream  = inFile;
    OutStream = outFile;
    Init();
}

Atv::Atv(FILE *inFile, FILE *outFile, FILE *errFile, FILE *msgFile )
{
    ErrStream =  errFile;
    MsgStream =  msgFile;
    InStream  =  inFile;
    OutStream =  outFile;
    Init();
}

Atv::Atv(char *inFile, char *outFile )
{
    ErrStream = stderr;
    MsgStream = stdout;
    InStream  = OpenInFile( inFile );
    OutStream = OpenOutFile( outFile );
    Init();
}

Atv::Atv(char *inFile, char *outFile, char *errFile, char *msgFile )
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

void Atv::Error(int errNum )
{
   switch( errNum )
   {
     case ATV_OUT_FILE_WRITE_ERROR :
              fprintf( ErrStream, "Exit Atv while OutFile write error %d!\n", errNum );
              exit( errNum );
              break;
     case ATV_INPUT_LINE_TO_BIG :
              fprintf( ErrStream, "Exit Atv while Input line to big\n" );
              exit( errNum );
              break;
     case ATV_INPUT_INVALID_FORMAT :
              fprintf( ErrStream, "Atv input file : invalid format\n" );
              break;

     case ATV_IN_FILE_READ_ERROR :
              fprintf( ErrStream, "Atv file read error\n" );
              break;


     default :
              fprintf( ErrStream, "Exit Atv while error %d!\n", errNum );
              exit( errNum );
              break;
   }
}
/*------------------------------------
--------------------------------------
*/

FILE * Atv::OpenInFile (char *filename )
{
   InStream = fopen ( filename, "r" );
   if( InStream == NULL )
   {
      fprintf (ErrStream, "\n!!! Atv input file %s open error !!!", filename);
      return NULL;
   }
   return InStream;
}

FILE * Atv::OpenOutFile( char *filename )
{
   OutStream = fopen ( filename, "w" );
   if (OutStream == NULL)
   {
      fprintf (ErrStream, "\n!!! Atv output file %s open error !!!", filename);
      return NULL;
   }
   return OutStream;
}

FILE * Atv::OpenErrFile( char *filename )
{
   ErrStream = fopen ( filename, "w" );
   if (ErrStream == NULL)
   {
      fprintf (ErrStream, "\n!!! Atv error file %s open error !!!", filename);
      return NULL;
   }
   return ErrStream;
}

FILE * Atv::OpenMsgFile( char *filename )
{
   MsgStream = fopen ( filename, "w");
   if (MsgStream == NULL)
   {
      fprintf (ErrStream, "\n!!! Atv massage file %s open error !!!", filename);
      return NULL;
   }
   return MsgStream;
}

/*-------------------------------------------------------------------------
     read Atv line
---------------------------------------------------------------------------
*/

long Atv::ReadAtvLine( void )
{
int       tmpBar;
int       i = 0, line_size, bufPos = 0, err=0;
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
             fprintf (ErrStream, "Atv read bar info format error\n");
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
   // Get Atv data
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
                Error( ATV_INPUT_INVALID_FORMAT );  // exit Program
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
   return 0L;

}
/*---------------------------------------------------------------------
    Flash all infos while time is changed
-----------------------------------------------------------------------
*/
void Atv::NewTime ( void )
{
    NewBar();
}
void Atv::NewBar ( void )
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
int Atv::Close ( void )
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

int Atv::ReadLine (void)
{
   int     count = 0, ch;

   while (count < ATV_IO_BUF_SIZE)
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

   if( count == ATV_IO_BUF_SIZE )
   {
      Error( ATV_INPUT_LINE_TO_BIG ); // exit program
   }

   return (count);
}

/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
uint
Atv::GetCode(void)
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
Atv::GetPrimCode(void)
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
int  Atv::WriteAtvLine( long time, ChNotes &Notes )
{
int    i, n, Count;
long   min, max, total; 
double average;

     /* write Atv-string to output file */

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
        Error( ATV_OUT_FILE_WRITE_ERROR );
     }
     return 0;
}
