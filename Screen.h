/*--------------------------------------------------------------------------
Dieter Neubacher                 Vers.: 1.0                        23.06.96
	                             Vers.: 2.0                        01.02.97
----------------------------------------------------------------------------
screen.h

Version 2.0     WIN32
----------------------------------------------------------------------------
*/      

#ifndef _SCREEN_H_
#define _SCREEN_H_

#ifdef WIN32
#include <windows.h>
#endif

#include  "debug.h"                         
#include  "system.h"                         

/*--------------------*/
/* Screen color       */
/*--------------------*/

extern const int Black;
extern const int White;
extern const int Blue;
extern const int Green;
extern const int Red;
extern const int Cyan;
extern const int Maganta;
extern const int Brown;
extern const int Gray;
extern const int LightBlue;
extern const int LightGreen;
extern const int LightRed;
extern const int LightCyan;
extern const int LightMagenta; 
extern const int Yellow;
extern const int BrightWhite;

class screen
{
public:
    
    screen(int MinX,int MaxX,int MinY,int MaxY);
    ~screen();

int Clear();
int Clear(int y);
int Clear(int x,int y);
int ClearStatus();
int Put(char ch);
int Put(int x,int y,char ch);
int Put(char *str);
int Put(int x,int y,char *str);
int Pos(int x,int y );
int GetPos(int x,int y );       
int Status(char *str);
int DispError(char *str);

int GetMaxLines() { return MaxY - MinY; }; 
int GetMaxCols()  { return MaxX - MinX; }; 
int Massage (FILE * stream, char *str, int flag);
 
void    NormalCursor();
void    BigCursor();
void    NoCursor();
void    Color( int Mode );
void    Color( int Back, int Text )  { BackColor = Back; TextColor = Text; Normal(); }
void    SetBackColor( int Color )    { BackColor = Color; Normal(); }
void    SetTextColor( int Color )    { TextColor = Color; Normal(); }
int     GetBackColor()               { return BackColor; }
int     GetTextColor()               { return TextColor; }
void    OldColor()                   { Color( OldBackColor, OldTextColor); }    
void    Normal(void);
void    Invert(void);

void    ToFile( char *FileName );                   

#ifdef SCREEN_DEBUG 
void   DispVideoConfig();
#endif

private:

#ifdef WIN32
HANDLE HandelStdOut;         // WIN32 Stdout Handel for Console
#endif

int  BackColor,TextColor;
int  PosX,PosY;
int  MinX,MaxX,MinY,MaxY;
int  StatusLine;                                                         

void ScSetPos( int x, int y );                    
void ScWriteText( char *str );
int  *ScreenBuf,*ScreenBufP;
char *EmtyLine;
int  Save();
int  Restore();

//---------------

int CursorMode;
int Atribbut;    

int OldTextColor,OldBackColor ;

};
 
    
extern screen Screen;    

/*---------------------------*/
/* use ansii screen commands */
/*---------------------------*/
 
void   attrib_on( void );
void   attrib_invert( void );
void   set_cursor_mode( int );
int    set_screen_color( int color );
int    massage( FILE *stream, char *str, int flag );
int    clear_massage( int flag );
int    get_old_color( void );
int    set_old_color( void );

/*---------------------------*/
/* screen size               */
/*---------------------------*/

extern const int max_screen_lines;
extern const int max_screen_cols;



/*-----------------------------------------------------------------
   Ansii sequenses for screen moving

#define CLEAR_SCREEN          fprintf( stdout, "\x1b[2J" )

#define ANSII_CLEAR_SCREEN    fprintf( stdout, "\x1b[2J" )
#define SET_SCREEN_POS(a,b)   fprintf( stdout, "\x1b[%d;%dH", (b), (a) )
#define CURSOR_UP(a)          fprintf( stdout, "\x1b[%dA", (a) )
#define CURSOR_DOWN(a)        fprintf( stdout, "\x1b[%dB", (a) )
#define CURSOR_LEFT(a)        fprintf( stdout, "\x1b[%dC", (a) )
#define CURSOR_RIGTH(a)       fprintf( stdout, "\x1b[%dD", (a) )
#define CURSOR_STORE          fprintf( stdout, "\x1b[s", (a) )
#define CURSOR_RESTORE        fprintf( stdout, "\x1b[u", (a) )

#define ANSII_ATTRIB_OFF      fprintf( stdout, "\x1b[0m" )
#define ANSII_ATTRIB_FETT     fprintf( stdout, "\x1b[1m" )
#define ANSII_ATTRIB_INVERT   fprintf( stdout, "\x1b[7m" )

#define ANSII_ATTRIB_HIDE     fprintf( stdout, "\x1b[8m" )

----------------------------------------------------------------------
*/

#endif // _SCREEN_H_
