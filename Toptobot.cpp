/*-------------------------------------------------------------------
    Dieter Neubacher	    Vers.: 1.0		       WuHu 10.07.94
	                        Vers.: 2.0                  29-01-97
    -----------------------------------------------------------------
    TopToBot.cpp

    convert file to revers order

    top line to botten line

  Version 2.0 : WIN32
---------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>

#include "TopToBot.H"


void TopToBot::Error( int err )
{
    switch( err )
    {
      case -1:
	      fprintf( ErrStream, "TopToBot() no more memory avalible\n" );
	      Close();
	      exit( 1);
	      break;

      case -2:
	      fprintf( ErrStream, "TopToBot() file to big\n" );
	      Close();
	      exit( 1);
	      break;

      case -3:
	      fprintf( ErrStream, "TopToBot() infile read error\n" );
	      Close();
	      exit( 1);
	      break;

      case -4:
	      fprintf( ErrStream, "TopToBot() outfile write error\n" );
	      Close();
	      exit( 1);
	      break;

      default:
	      fprintf( ErrStream, "TopToBot() undefined error\n" );
	      break;
    }
}
int TopToBot::init()
{
int loop;

    PosNum = 0L;

    MaxIOBufSize  = TOP_TO_BOT_IO_MAX_SIZE;
    MaxPosBufSize = TOP_TO_BOT_POS_MAX_SIZE;

    loop=0;
    do
    {
       PosBuf = new long [MaxPosBufSize];
       if( PosBuf == NULL )
       {
	 MaxPosBufSize /= 2;
       }
       loop++;
    }while( PosBuf == NULL && loop < 10 );

    loop=0;
    do
    {
       IOBuf  = new char [MaxIOBufSize];
       if( IOBuf == NULL )
       {
	      MaxIOBufSize /= 2;
       }
       loop++;
    }while( IOBuf == NULL && loop < 10 );


    if( IOBuf == NULL || PosBuf == NULL )
      return -1;
    else
    {
      // fprintf( stderr, "TopToBot PosBuf size %ld\n",MaxPosBufSize);
      // fprintf( stderr, "TopToBot  IoBuf size %ld\n",MaxIOBufSize);
    }
    return 0;
}

TopToBot::TopToBot()
{
    InStream  = stdin;
    OutStream = stdout;
    ErrStream = stderr;
    if( init() != 0 )
    {
	Error( -1 );
    }
}

TopToBot::TopToBot(FILE *inFile, FILE *outFile )
{
    ErrStream = stderr;
    InStream  = inFile;
    OutStream = outFile;
    if( init() != 0 )
    {
	Error( -1 );
    }
}

TopToBot::TopToBot(FILE *inFile, FILE *outFile, FILE *errFile )
{
    ErrStream =  errFile;
    InStream  =  inFile;
    OutStream =  outFile;
    if( init() != 0 )
    {
	Error( -1 );
    }
}


TopToBot::TopToBot(char *inFile, char *outFile )
{
    ErrStream = stderr;
    InStream  = OpenInFile( inFile );
    OutStream = OpenOutFile( outFile );
    if( init() != 0 )
    {
	Error( -1 );
    }
}

TopToBot::TopToBot(char *inFile, char *outFile, char *errFile )
{
    ErrStream = OpenErrFile( errFile );
    InStream  = OpenInFile( inFile );
    OutStream = OpenOutFile( outFile );
    if( init() != 0 )
    {
	Error( -1 );
    }
}



/*------------------------------------
--------------------------------------
*/

FILE * TopToBot::OpenInFile (char *filename )
{
   InStream = fopen ( filename, "rb");
   if( InStream == NULL )
   {
      fprintf (ErrStream, "\n!!! input file %s open error !!!", filename);
      return NULL;
   }
   return InStream;
}

FILE * TopToBot::OpenOutFile( char *filename )
{
   OutStream = fopen ( filename, "wb");
   if (OutStream == NULL)
   {
      fprintf (ErrStream, "\n!!! output file %s open error !!!", filename);
      return NULL;
   }
   return OutStream;
}

FILE * TopToBot::OpenErrFile( char *filename )
{
   OutStream = fopen ( filename, "w");
   if (OutStream == NULL)
   {
      fprintf (ErrStream, "\n!!! error file %s open error !!!", filename);
      return NULL;
   }
   return OutStream;
}

/*-------------------------------------------------------------------------
---------------------------------------------------------------------------
*/

int  TopToBot::revers(void)
{
long  i;

    GetPos();

    PosNum -= 2;

    while( PosNum >= 0 )
    {
	// fprintf( ErrStream, "PosNum %8ld bytes %8ld\n", PosNum, PosBuf[PosNum] );

	fseek( InStream, PosBuf[PosNum], SEEK_SET );

	i = PosBuf[PosNum+1] - PosBuf[PosNum];

	while( i > 0 )
	{  // int ch;
	   // if( (ch = fputc( fgetc( InStream ), OutStream )) == '\n' || ch == EOF )
	   //  break;
	   fputc( fgetc( InStream ), OutStream );
	   i--;
	}
	if( ferror( InStream ) )
	   Error( -3 );
	if( ferror( OutStream ) )
	   Error( -4 );

	PosNum--;
    }
    return( 0 );
}

long TopToBot::GetPos(void)
{
long i,count  = 0, maxCount;

   PosBuf[0]= 0L;
   PosNum   = 1;
   fseek( InStream, 0L, SEEK_SET );
   while( feof( InStream ) == 0 )
   {
      // fprintf( ErrStream, "Read Block\n" );

      maxCount = fread( IOBuf,1, MaxIOBufSize, InStream );

      // fprintf( ErrStream, "Block Size %8ld Act Pos %8ld\n", maxCount, count );

      i = 0L;
      while ( i < maxCount )
      {
	 count++;
	 if( *(IOBuf + i) == '\n' )
	 {
	    PosBuf[PosNum] = count;

	    // fprintf( ErrStream, "%8ld\n", PosBuf[PosNum] );

	    if( PosNum == MaxPosBufSize )
	    {
	       Error( -2 );
	    }
	    PosNum++;
	 }
	 i++;
      }
   }
   // fprintf( ErrStream, "%8ld\n", count );
   return (count);
}


int TopToBot::Close ( void )
{
   if( InStream  != NULL )    fclose (InStream);
   if( OutStream != NULL )    fclose (OutStream);
   if( ErrStream != NULL && ErrStream != stderr ) fclose (ErrStream);

   return 0;
}

/*------------------------------------------------------------------------
--------------------------------------------------------------------------
*/

#ifdef TOP_TO_BOT_MAIN

main()
{
TopToBot x( "\\rameau\\data\\modul3.mvp", "\\rameau\\data\\modul3.tmp" );

   x.revers();
   return 0;
}

#endif
