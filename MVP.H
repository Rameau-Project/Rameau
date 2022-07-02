/*-------------------------------------------------------------------------
    Dieter Neubacher         Vers.: 1.00             Wuhu, 22-06-94
    -----------------------------------------------------------------------
    mvp.h
    -----------------------------------------------------------------------
*/


#if !defined( _MVP_H_ )

#define _MVP_H_

#include <stdlib.h>
#include <stdio.h>

#include "ChNotes.h"
#include "MvpEntry.h"

#define MVP_MAX_CHANNEL 16
#define MVP_MAX_ENTRIES 10
#define MVP_IO_BUF_SIZE 1024
#define MVP_MAX_READ_ENTRIES_PER_COL 10

#define MAX_MVP_LINES   (MVP_MAX_CHANNEL * MVP_MAX_CHANNEL - MVP_MAX_CHANNEL) / 2


class Mvp
{
public:

      Mvp();
      Mvp( FILE *inFile, FILE *outFile );
      Mvp( FILE *inFile, FILE *outFile, FILE *errFile, FILE *msgFile );
      Mvp(char * inFilename, char * outFilename );
      Mvp(char * inFilename, char * outFilename , char * msgFilename , char * errFilename );

FILE *OpenInFile( char *filename );
FILE *OpenOutFile( char *filename );
FILE *OpenMsgFile( char *filename );
FILE *OpenErrFile( char *filename );

void  DispAllNotes( void );
long  ReadMvpLine( void );
int   Write (long time );
int   WritePairs( long time );
int   SetNotes( long time, int nodes[MVP_MAX_CHANNEL] );

int   WriteInfo ( int cannel);
int   SetInfo ( int channel );
void  ClearChFlags( void );
void  ClearNewNoteBuf( void );
int   SetChFlag( int channel );
int   GetChFlag( int channel ) { return UsedChFlag[ channel ]; };
int   ResetChFlag( int channel );
int   Disp (void);

MvpEntry GetEntry( int line, int num );

void  NewTime( void );
void  NextOutputLine( void ) { fputc( '\n', OutStream ); };

int   GetBar( void ) { return Bar; };
void  SetBar( int newbar ) { Bar = newbar; };

int   Close ( void );

void  DebugDispNoteBuffer( void );

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
void MsgOn( void ) { CopyMsgLine = 1; }
void MsgOff( void ) { CopyMsgLine = 0; }
void TxtOn( void ) { CopyTxtLine = 1; }
void TxtOff( void ) { CopyTxtLine = 0; }



/*------------------------------------
--------------------------------------
*/

private:

MvpEntry MvpErrorEntry;

int Bar, LastBar;
int CopyTxtLine, CopyMsgLine;

int UsedChFlag[MVP_MAX_CHANNEL];
int SpeedupChFlag[MVP_MAX_CHANNEL];
int SpeedupChNote[MVP_MAX_CHANNEL];
ChNotes Notes;
MvpEntry pairHistory[MAX_MVP_LINES][2];


// Buffer for MVP read line entries
//

MvpEntry ReadEntryBuf[MAX_MVP_LINES][MVP_MAX_READ_ENTRIES_PER_COL];
int  ReadEntryBufNum[MAX_MVP_LINES];
int  ReadPairCols;


int flag_all_notes; /* print all notes */

    /* functions for reading note Informatins form ChannelNotes */

int readChStatus[MVP_MAX_CHANNEL][2];
int readChannel, readSubChannel;

int initChannel( int channel );
int initReadChannel( int channel );
int initReadSubChannel( int channel );
int ReadChNote( void );
int ReadSubChNote( void );
int ModDiff( int ,int );

FILE   *InStream;
FILE   *OutStream;
FILE   *ErrStream;
FILE   *MsgStream;

char    InBuf[MVP_IO_BUF_SIZE];

void    Init(void);
void    ClearNoteBuf();
int     ReadLine (void);
int     WriteChInfo( int channel );
int     WritePairInfo( int ch1, int ch2);
int     WritePairColorInfo( int ch1, int ch2);
void    Error(int errNum );
int     histoIndex( int ch1, int ch2 );
void    ClearHisto( void );

MvpEntry   getMvp( int ch1, int ch2 );
MvpEntry   nextMvp( int ch1, int ch2 );

MvpEntry   getLastQ( int ch1, int ch2 );
MvpEntry   getLastE( int ch1, int ch2 );

int   setHisto( int ch1, int ch2, MvpEntry pair );
int   setHisto( int ch1, int ch2, int status, int modNote1, int modNote2 );

void  NewBar( void );
void  InitBar( int newbar ) { Bar = newbar; LastBar=newbar; };

//----------------------------------


protected:



};



#endif
