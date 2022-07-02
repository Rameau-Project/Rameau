/*-------------------------------------------------------------------
    Dieter Neubacher	    Vers.: 1.0		       WuHu 10.07.94
	                        Vers.: 2.0                  29-01-97
    -----------------------------------------------------------------
    TopToBot.h

    convert file to revers order

    top line to botten line

    Version 2.0 : WIN32
---------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>


#ifndef _TOP_TO_BOT_

#define _TOP_TO_BOT_

#ifdef WIN32
// Use 256 KB Buffer
#define TOP_TO_BOT_IO_MAX_SIZE  0x4FF80L
#define TOP_TO_BOT_POS_MAX_SIZE 0x43F80L
#else
// 64 K i/o buffer
#define TOP_TO_BOT_IO_MAX_SIZE 0xFF80L
// 64 K line position buffer
#define TOP_TO_BOT_POS_MAX_SIZE 0x3F80L
#endif

class TopToBot
{
public:

      TopToBot();
      TopToBot( FILE *inFile, FILE *outFile );
      TopToBot( FILE *inFile, FILE *outFile, FILE *errFile );
      TopToBot(char * inFilename, char * outFilename );
      TopToBot(char * inFilename, char * outFilename, char * errFilename );
      ~TopToBot() { delete IOBuf; delete PosBuf;Close(); };

int   revers(void);

FILE *OpenInFile( char *filename );
FILE *OpenOutFile( char *filename );
FILE *OpenErrFile( char *filename );

/*------------------------------------
   inline Functions
--------------------------------------
*/
FILE *SetInStream( FILE *stream )
{
   InStream = stream;
   return InStream;
}
FILE *SetOutStream( FILE *stream )
{
   OutStream = stream;
   return OutStream;
}
FILE *SetErrStream( FILE *stream )
{
   ErrStream = stream;
   return OutStream;
}

private:

FILE   *InStream;
FILE   *OutStream;
FILE   *ErrStream;

char   *IOBuf;
long   *PosBuf;

long   MaxIOBufSize;
long   MaxPosBufSize;

long   PosNum;

int  init ( void );
int  Close ( void );
void Error ( int err );
long GetPos(void);
};

#endif // _TOP_TO_BOT_
