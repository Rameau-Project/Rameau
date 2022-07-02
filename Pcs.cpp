/*-------------------------------------------------------------------------
    Dieter Neubacher	       Vers.: 1.0		  Wuhu 22.07.94
    -----------------------------------------------------------------------
    Pcs.cpp
---------------------------------------------------------------------------
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "pcs.h"



#define PCS_OUT_FILE_WRITE_ERROR -1
#define PCS_INPUT_LINE_TO_BIG	 -2
#define PCS_INPUT_INVALID_FORMAT -3
#define PCS_IN_FILE_READ_ERROR	 -4

void Pcs::Init( void )
{
    InitBar( 0 );
    CopyMsgLine = 0;
    CopyTxtLine = 1;
    PcsGroup = PcsNumber = -1;
}

Pcs::Pcs()
{
    InStream  = stdin;
    OutStream = stdout;
    ErrStream = stderr;
    MsgStream = stdout;
    Init();
}

Pcs::Pcs(FILE *inFile, FILE *outFile )
{
    ErrStream = stderr;
    MsgStream = stdout;
    InStream  = inFile;
    OutStream = outFile;
    Init();
}

Pcs::Pcs(FILE *inFile, FILE *outFile, FILE *errFile, FILE *msgFile )
{
    ErrStream =  errFile;
    MsgStream =  msgFile;
    InStream  =  inFile;
    OutStream =  outFile;
    Init();
}

Pcs::Pcs(char *inFile, char *outFile )
{
    ErrStream = stderr;
    MsgStream = stdout;
    InStream  = OpenInFile( inFile );
    OutStream = OpenOutFile( outFile );
    Init();
}

Pcs::Pcs(char *inFile, char *outFile, char *errFile, char *msgFile )
{
    ErrStream = OpenErrFile( errFile );
    MsgStream = OpenMsgFile( msgFile );
    InStream  = OpenInFile( inFile );
    OutStream = OpenOutFile( outFile );
    Init();
}

//----------------------------------
//  error handler for Pcs
//----------------------------------

void Pcs::Error(int errNum )
{
   switch( errNum )
   {
     case PCS_OUT_FILE_WRITE_ERROR :
	      fprintf( ErrStream, "Exit Pcs while OutFile write error %d!\n", errNum );
	      exit( errNum );
	      break;
     case PCS_INPUT_LINE_TO_BIG :
	      fprintf( ErrStream, "Exit Pcs while Input line to big\n" );
	      exit( errNum );
	      break;
     case PCS_INPUT_INVALID_FORMAT :
	      fprintf( ErrStream, "Pcs input file : invalid format\n" );
	      break;

     case PCS_IN_FILE_READ_ERROR :
	      fprintf( ErrStream, "Pcs file read error\n" );
	      break;


     default :
	      fprintf( ErrStream, "Exit Pcs while error %d!\n", errNum );
	      exit( errNum );
	      break;
   }
}
/*------------------------------------
--------------------------------------
*/

FILE * Pcs::OpenInFile (char *filename )
{
   InStream = fopen ( filename, "r" );
   if( InStream == NULL )
   {
      fprintf (ErrStream, "\n!!! Pcs input file %s open error !!!", filename);
      return NULL;
   }
   return InStream;
}

FILE * Pcs::OpenOutFile( char *filename )
{
   OutStream = fopen ( filename, "w" );
   if (OutStream == NULL)
   {
      fprintf (ErrStream, "\n!!! Pcs output file %s open error !!!", filename);
      return NULL;
   }
   return OutStream;
}

FILE * Pcs::OpenErrFile( char *filename )
{
   ErrStream = fopen ( filename, "w" );
   if (ErrStream == NULL)
   {
      fprintf (ErrStream, "\n!!! Pcs error file %s open error !!!", filename);
      return NULL;
   }
   return ErrStream;
}

FILE * Pcs::OpenMsgFile( char *filename )
{
   MsgStream = fopen ( filename, "w");
   if (MsgStream == NULL)
   {
      fprintf (ErrStream, "\n!!! Pcs massage file %s open error !!!", filename);
      return NULL;
   }
   return MsgStream;
}

/*-------------------------------------------------------------------------
     read Pcs line
---------------------------------------------------------------------------
*/

long Pcs::ReadPcsLine( void )
{
int	  tmpBar;
int	  i = 0, line_size, bufPos = 0;

   if ((line_size = ReadLine ()) <= 0)
      return (-1L);

   /* commentar mark or note bar mark */

   while (InBuf[0] == '*' || InBuf[0] == '#')
   {
     if (InBuf[0] == '*' && MsgStream != NULL )
     {
		if (CopyTxtLine)		/* copy commentar */
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
				fprintf (ErrStream, "Pcs read bar info format error\n");
			}
			else
			{
				SetBar( tmpBar );
				NewTime();
			}
       }
       if (CopyMsgLine && MsgStream != NULL )		/* copy massage */
       {
	      fwrite (InBuf, 1, line_size, MsgStream);
       }
     }

     /* read next line */

     if ((line_size = ReadLine ()) <= 0)
	 return (-1L);
   }
   //--------------
   // Get Pcs data
   //--------------


   if (sscanf (InBuf, "%d-%d", &PcsGroup, &PcsNumber) != 2)
   {
      Error( PCS_INPUT_INVALID_FORMAT );
   }

   return 0;

}
/*---------------------------------------------------------------------
    Flash all infos while time is changed
-----------------------------------------------------------------------
*/
void Pcs::NewTime ( void )
{
    NewBar();
}
void Pcs::NewBar ( void )
{
    // write bar info to outfile

    if( LastBar != Bar )
    {
       // fprintf( OutStream,"# bar %d\n", Bar );
       LastBar = Bar;
    }
}
/*---------------------------------------------------------------------
-----------------------------------------------------------------------
*/
int Pcs::Close ( void )
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

int Pcs::ReadLine (void)
{
   int     count = 0, ch;

   while (count < PCS_IO_BUF_SIZE)
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

   if( count == PCS_IO_BUF_SIZE )
   {
      Error( PCS_INPUT_LINE_TO_BIG ); // exit program
   }

   return (count);
}
