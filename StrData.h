/*-------------------------------------------------------------------------
    Dieter Neubacher         Vers.: 1.00                     Wuhu, 30-06-97
    -----------------------------------------------------------------------
    StrData.h

    -----------------------------------------------------------------------
*/


#if !defined( _STRDATA_H_ )

#define _STRDATA_H_

#include <stdlib.h>
#include <stdio.h>


//------------------------------------
// STRDATA Class
//------------------------------------

#define STRDATA_IO_BUF_SIZE 1024

#ifndef uint
#define uint unsigned int
#endif

#ifndef ulong
#define ulong unsigned long
#endif



class StrData
{
public:

		StrData(int len ) { length = len; data = new int [len+1]; };
void	SetStart( long time) { StartTime = time; };
void	SetEnd( long time ) { EndTime = time; };

private:

int		Value( int pos ) { return data[pos]; };
void    StrData::Sort(); 
void    StrData::Compare();
void    StrData::Search( int trans, int reverse, int permut ); 

int  length;
long StartTime;
long EndTime;
int  *data;

};



class StrDataFile
{
public:

      StrDataFile();
      StrDataFile( FILE *inFile, FILE *outFile );
      StrDataFile( FILE *inFile, FILE *outFile, FILE *errFile, FILE *msgFile );
      StrDataFile(char * inFilename, char * outFilename );
      StrDataFile(char * inFilename, char * outFilename , char * msgFilename , char * errFilename );

FILE *OpenInFile( char *filename );
FILE *OpenOutFile( char *filename );
FILE *OpenMsgFile( char *filename );
FILE *OpenErrFile( char *filename );

long  ReadLine( void );
int   WriteLine();
int   Disp (void);

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

int     Bar, LastBar;
int     CopyTxtLine, CopyMsgLine;

// Buffer for VPS read line entries
//
char    InBuf[STRDATA_IO_BUF_SIZE];


FILE   *InStream;
FILE   *OutStream;
FILE   *ErrStream;
FILE   *MsgStream;


void    Init(void);
int     ReadNextLine (void);

void    NewBar( void );
void    InitBar( int newbar ) { Bar = newbar; LastBar=newbar; };
void    Error(int errNum );

//----------------------------------

protected:

};

#endif  // _STRDATA_H_
