/*-------------------------------------------------------------------------
    Dieter Neubacher         Vers.: 1.00                     Wuhu, 30-06-97
    -----------------------------------------------------------------------
    rdlstr.h

    -----------------------------------------------------------------------
*/


#if !defined( _RDLSTR_H_ )

#define _RDKSTR_H_

#include <stdlib.h>
#include <stdio.h>

#include "chnotes.h"

//------------------------------------
// RDLSTR Class
//------------------------------------

#define RDLSTR_IO_BUF_SIZE 1024

#ifndef uint
#define uint unsigned int
#endif

#ifndef ulong
#define ulong unsigned long
#endif



class RdlString
{
public:

		RdlString(int len ) { length = len; data = new int [len+1]; };
void	SetStart( long time) { StartTime = time; };
void	SetEnd( long time ) { EndTime = time; };

private:

int		Value( int pos ) { return data[pos]; };
void    RdlString::Sort(); 
void    RdlString::Compare();
void    RdlString::Search( int trans, int reverse, int permut ); 

int  length;
long StartTime;
long EndTime;
int  *data;

};



class RdlStr
{
public:

      RdlStr();
      RdlStr( FILE *inFile, FILE *outFile );
      RdlStr( FILE *inFile, FILE *outFile, FILE *errFile, FILE *msgFile );
      RdlStr(char * inFilename, char * outFilename );
      RdlStr(char * inFilename, char * outFilename , char * msgFilename , char * errFilename );

FILE *OpenInFile( char *filename );
FILE *OpenOutFile( char *filename );
FILE *OpenMsgFile( char *filename );
FILE *OpenErrFile( char *filename );

long  ReadRdlStrLine( void );
int   WriteRdlStrLine( long Time, ChNotes &Notes );
int   Disp (void);

void  NewTime( void );
int   GetBar( void ) { return Bar; };
void  SetBar( int newbar ) { Bar = newbar; };
int   Close ( void );

int   GetModNote( int i ) { return ModNote[i]; };
uint  GetCode(void);
uint  GetPrimCode(void);

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

int     Bar, LastBar;
int     CopyTxtLine, CopyMsgLine;
int     ModNote[12];

// Buffer for VPS read line entries
//
char    InBuf[RDLSTR_IO_BUF_SIZE];


FILE   *InStream;
FILE   *OutStream;
FILE   *ErrStream;
FILE   *MsgStream;


void    Init(void);
int     ReadLine (void);

void    NewBar( void );
void    InitBar( int newbar ) { Bar = newbar; LastBar=newbar; };
void    Error(int errNum );

void    ClearModNotes( void ) { for( int i=0; i<12; i++) ModNote[i]=0; };

//----------------------------------

protected:

};

#endif  // _RDLSTR_H_
