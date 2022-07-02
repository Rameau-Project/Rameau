/*-------------------------------------------------------------------------
    Dieter Neubacher         Vers.: 1.0              Wuhu, 10-07-97
                             
    -----------------------------------------------------------------------
    TonInt.h

    -----------------------------------------------------------------------
*/




#if !defined( _TONINT_H_)

#define _TONINT_H_

#include <stdlib.h>
#include <stdio.h>

#define TONINT_BUF_SIZE 32
#define IO_BUF_SIZE 1024



class TonInterval
{
public:

		TonInterval() { StartTime = EndTime = 0; Note = 0; Channel = 0; };

void	Set( long stime, long etime, int note, int ch );
long	GetStart() { return StartTime; };
long	GetEnd() { return EndTime; };
long    IntervalTime()  { return EndTime - StartTime; };
int		GetNote() { return Note; };
int		GetChannel() { return Channel; };
int		Write( FILE *stream);
int		Write( ) { return Write( stdout); };

private:

long    StartTime;   
long    EndTime;
int     Note;
int     Channel;

};


//---------------------------------------------------------

class Tint
{
public:

      Tint();
      Tint( FILE *inFile, FILE *outFile );
      Tint( FILE *inFile, FILE *outFile, FILE *errFile, FILE *msgFile );
      Tint(char * inFilename, char * outFilename );
      Tint(char * inFilename, char * outFilename , char * msgFilename , char * errFilename );

void  Error(int errNum );

FILE *OpenInFile( char *filename );
FILE *OpenOutFile( char *filename );
FILE *OpenMsgFile( char *filename );
FILE *OpenErrFile( char *filename );

long  Read( TonInterval *tinterval );
int   Write ( TonInterval tinterval );
int   Disp (void);
int   Close ( void );

void  NewTime( long time );
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

void    Init( void );
int     ReadNextLine (void);


void  NewBar( void );
void  InitBar( int newbar ) { Bar = newbar; LastBar=newbar; };

protected:

long Time;

int Tint::ReadLine (void);

};

#endif
