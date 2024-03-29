/*-------------------------------------------------------------------------
    Dieter Neubacher	     Vers.: 1.00		     Wuhu, 22-07-94
    -----------------------------------------------------------------------
    Pcs.h

    -----------------------------------------------------------------------
*/


#if !defined( _PCS_H_ )

#define _PCS_H_

#include <stdlib.h>
#include <stdio.h>
//------------------------------------
// Pcs Class
//------------------------------------

#define PCS_IO_BUF_SIZE 1024

#ifndef uint
#define uint unsigned int
#endif

#ifndef ulong
#define ulong unsigned long
#endif




class Pcs
{
public:

      Pcs();
      Pcs( FILE *inFile, FILE *outFile );
      Pcs( FILE *inFile, FILE *outFile, FILE *errFile, FILE *msgFile );
      Pcs(char * inFilename, char * outFilename );
      Pcs(char * inFilename, char * outFilename , char * msgFilename , char * errFilename );

FILE *OpenInFile( char *filename );
FILE *OpenOutFile( char *filename );
FILE *OpenMsgFile( char *filename );
FILE *OpenErrFile( char *filename );

long  ReadPcsLine( void );
int   Disp (void);

int   GetGroup( void ) { return PcsGroup; };
int   GetNumber( void ){ return PcsNumber;};

void  NewTime( void );
int   GetBar( void ) { return Bar; };
void  SetBar( int newbar ) { Bar = newbar; };
int   Close ( void );


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

int	Bar, LastBar;
int	CopyTxtLine, CopyMsgLine;

int PcsGroup, PcsNumber;

// Buffer for Pcs read line
//
char	InBuf[PCS_IO_BUF_SIZE];


FILE   *InStream;
FILE   *OutStream;
FILE   *ErrStream;
FILE   *MsgStream;


void	Init(void);
int	ReadLine (void);

void	NewBar( void );
void	InitBar( int newbar ) { Bar = newbar; LastBar=newbar; };
void	Error(int errNum );

//----------------------------------

protected:

};

#endif	// _PCS_H_
