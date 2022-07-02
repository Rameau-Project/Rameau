/*-------------------------------------------------------------------------
    Dieter Neubacher         Vers.: 1.00                     Wuhu, 10-07-94
	                         Vers.: 2.00                           10-02-97
    -----------------------------------------------------------------------
    MvpPs.h

    -----------------------------------------------------------------------
*/


#if !defined( _MVPPS_H_ )

#define _MVPPS_H_

#include "mvp.h"

class MvpPs
{
public:

      MvpPs();
      ~MvpPs();
      MvpPs( int voices );          /* voices pre Block */
      int PsOutputBegin( int ColorOutputFlag, FILE *InfoStream );
      int PsOutputEnd( void );

int   SetMvpEntry( int line, MvpEntry *entry );
int   NextPos();

FILE * MvpPs::OpenOutFile( char *filename );
FILE * MvpPs::OpenMsgFile( char *filename );
FILE * MvpPs::OpenErrFile( char *filename );

int   MvpTestData( void );

void  NewTime( void );

/*------------------------------------
   inline functions
--------------------------------------
*/
int   GetBar( void ) { return Bar; };
void  SetBar( int newbar ) { Bar = newbar; };


int SetPageSize( int XSize, int YSize )
{
   SizeX = XSize;
   SizeY = YSize;
   LinesPerPage  = (YSize - TopMargin - BottenMargin) / LineSpaceSize;
   ColsPerPage   = (XSize - LeftMargin - RightMargin - 2 * MarginSpace) / ColSpaceSize;

   return 0;
}

int  GetLinesPerBlock() { return LinesPerBlock; }
void SetLinesPerBlock( int nLines )
{
     int i;

        LinesPerBlock = nLines;
        BlocksPerPage = (LinesPerPage+1) / (LinesPerBlock+1);
        delete mvpEntries;
        mvpEntries = new  MvpEntry[ nLines ];
        for( i=0; i<nLines; i++ )
        {
         mvpEntries[i].Clear();
        }

}

void SetColsPerPage( int cols )
{
        ColsPerPage  = cols;
        ColSpaceSize = (SizeX - LeftMargin - RightMargin - 2 * MarginSpace - oBoxWidth * ColsPerPage ) / (ColsPerPage-1) + oBoxWidth;
}

void SetVoicesPerBlock( int nVoices )
{
     int i, nLines;

        Voices = nVoices;
        nLines = (nVoices) * (nVoices-1) / 2;
        LinesPerBlock = nLines;
        BlocksPerPage = (LinesPerPage+1) / (LinesPerBlock+1);
        delete mvpEntries;
        mvpEntries = new  MvpEntry[ nLines ];
        for( i=0; i<nLines; i++ )
        {
         mvpEntries[i].Clear();
        }

}

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
--------------------------------------
*/


private:

void    init();      // init all variables
int     MvpPs::FirstPage();
int     MvpPs::NextPage();
int     MvpPs::InitPage( void );
int     GetLine( int line );
int     PsHeader( int ColorMode ); // 0 BW 1 Color
int     MvpPs::PsMainBegin (void);
int     MvpPs::PsMainEnd (void);
void    NewBar( void );
void    InitBar( int newbar ) { Bar = newbar; LastBar=newbar; };

FILE   *InStream;
FILE   *OutStream;
FILE   *ErrStream;
FILE   *MsgStream;

MvpEntry *mvpEntries;

int     Bar, LastBar;

int     LinesPerPage, ColsPerPage;
int     MaxLinesPerPage, MaxColsPerPage;
int     LinesPerBlock;
int     Voices;
int     BlocksPerPage;
int     LeftMargin;
int     RightMargin;
int     TopMargin;
int     BottenMargin;
int     ScaleTextPos;
int     SizeX, SizeY;
double  PsLineScale;

int  fontSize;
int  noteOffset;
int  oBoxHigh;     /* oktave */
int  oBoxWidth;
int  qBoxHigh;     /* quinte */
int  qBoxWidth;
int  bBoxHigh;     /* break  */
int  bBoxWidth;

int  oLineOffset;
int  qLineOffset;

int  pointerHigh;
int  pointerWidth;


int  LineSpaceSize;
int  ColSpaceSize;
int  MarginSpace;

int   ActPos;
int   ActBlock;
int   ActPage;

char  NoteCode[12][3];

};

#endif // _MVPPS_H_
