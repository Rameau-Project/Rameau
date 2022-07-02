/*-------------------------------------------------------------------------
    Dieter Neubacher         Vers.: 1.0              Wuhu, 22-06-94
                             Vers.: 1.1              Wuhu  24-09-94
    -----------------------------------------------------------------------
    rdl.h

    -----------------------------------------------------------------------
*/




#if !defined( _RDL_H_ )

#define _RDL_H_

#include <stdlib.h>
#include <stdio.h>

#define RDL_BUF_SIZE 32
#define IO_BUF_SIZE 1024



typedef struct
{
   long    time;
   int     channel[16];
}
RDL_LINE;


class Rdl
{
public:

      Rdl();
      Rdl( FILE *inFile, FILE *outFile );
      Rdl( FILE *inFile, FILE *outFile, FILE *errFile, FILE *msgFile );
      Rdl(char * inFilename, char * outFilename );
      Rdl(char * inFilename, char * outFilename , char * msgFilename , char * errFilename );

void  Error(int errNum );

FILE *OpenInFile( char *filename );
FILE *OpenOutFile( char *filename );
FILE *OpenMsgFile( char *filename );
FILE *OpenErrFile( char *filename );

long  Read  (int channel_nodes[16]);
int   Write (long time, int channel_nodes[16]);
int   WriteBuf (long midi_clock, int *inchannel);
int   Disp (void);
int   Disp ( int *ChannelOrder );
int   Close ( void );

void  NewTime();
int   GetBar( void ) { return Bar; };
void  SetBar( int newbar ) { Bar = newbar; };

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
   return ErrStream;
}
FILE *SetMsgStream( FILE *stream )
{
   MsgStream = stream;
   return MsgStream;
}
/*------------------------------------
  Massage and Text Flages
--------------------------------------
*/
void MsgOn( void ) { CopyMsgLine = 1; };
void MsgOff( void ) { CopyMsgLine = 0; };
void TxtOn( void ) { CopyTxtLine = 1; };
void TxtOff( void ) { CopyTxtLine = 0; };



private:

int CopyTxtLine, CopyMsgLine;

int Bar, LastBar;

FILE   *InStream;
FILE   *OutStream;
FILE   *ErrStream;
FILE   *MsgStream;

char    InBuf[IO_BUF_SIZE];
char    OutBuf[IO_BUF_SIZE];

RDL_LINE        Buffer[RDL_BUF_SIZE];

void    Init( void );
int     CompressBuf (int BufPos);
int     ReadLine (void);


void  NewBar( void );
void  InitBar( int newbar ) { Bar = newbar; LastBar=newbar; };

protected:



};



#endif
