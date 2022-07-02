/*---------------------------------------------------------------------------
    Dieter Neubacher           Vers. 1.0            17.12.93
    -------------------------------------------------------------------------
    edi.h     edit functions for RAMEAU MENU
-----------------------------------------------------------------------------
*/

#ifndef _EDI_H_
#define _EDI_H_

/*---------------------------
---------------------------*/



#define EDI_RIGHT   77
#define EDI_LEFT    75
#define EDI_P_UP    73
#define EDI_P_DOWN  81
#define EDI_POS_1   71
#define EDI_UP      72
#define EDI_DOWN    80
#define EDI_ENDE    79
#define EDI_RETURN  13
#define EDI_BS       8
#define EDI_DEL     83
#define EDI_INS     82
#define EDI_TAB      9
#define EDI_B_TAB   15
#define EDI_ESC     27



class ediwin 
{
public :
     
     ediwin();
     ediwin( int Xmin, int Xmax, int Ymin, int Ymax );
//   ~ediwin();

void WinSet( int Xmin, int Xmax, int Ymin, int Ymax );
void WinPos( int xi, int yi );    /* set cursor to pos. */
void WinMove( int xp, int yp);
void WinDisp( int xi, int yi, char *p );
int  InWin( int xi, int yi );

private :


int         WinXsize;
int         WinYsize;
int         WinXmin, WinXmax;
int         WinYmin, WinYmax;
int         WinXoffset;
int         WinYoffset;
};

class edi : ediwin
{        
public :
    
     edi( int x, int y, int wXmin, int wXmax, int wYmin, int wYmax );
     ~edi();

void Disp();
int  Edit();
int  SetStr( char * str );
int  GetStr( char * str , int MaxLen );
void SetWin( int wXmin, int wXmax, int wYmin, int wYmax );
void Clear();
int  Write( FILE *stream ) { return fwrite(p,1,Xsize, stream );};

private :

char        *p;
int         Xsize;
int         Ysize;
int         Xpos;
int         Ypos;
int         Ins;     // 0 : off  1 : on
int         BackColor;
int         TextColor;

int  GetX();
int  GetY();
void NextPos();
char GChar();
char GChar( int x, int y );
void PChar( int ch );
void PChar( int x, int y, int ch );
int  PutCh( int ch);
void InChar( int ch );
void InChar( int x, int y, int ch );
void DelChar();
void DelChar( int x, int y );
void SetPos( int x, int y );
char *Ptr( int x, int y );
void LineEndPos( int y );
/*----------------------------------------
        Function not user in this Version

void RemoveStrEnd();
void SetStrEnd();
-----------------------------------------*/
};



/*---------------------------
---------------------------*/


#endif // _EDI_H_