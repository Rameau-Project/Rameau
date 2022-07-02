/*---------------------------------------------------------------------
    Dieter Neubacher        Vers.: 1.0                         25.06.96
	                        Vers.: 2.0                         01.02.97
    -------------------------------------------------------------------
    screen.cpp

    Version 2.0  WIN32
-----------------------------------------------------------------------
*/


#ifdef WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <string.h> 
#include <stdlib.h>

#ifdef WIN32
#include <wincon.h>
/* used to set our initial console screen buffer size */
#define CONX 80
#define CONY 25
#else
   #ifdef __MS_DOS__
   #include <graph.h>
   #else
   #error  screen.cpp  only for MSDOS implemented
   #endif /* __MS_DOS__ */
#endif /* WIN32 */

#include "debug.h"      
#include "screen.h"
#include "rameau.h"

      
screen Screen(0,79,0,24);  // Output Screen

#ifdef WIN32
const int BackBlack         = 0;
const int TextBlack         = 0;
const int TextWhite			= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
const int BackWhite			= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
const int TextBlue			= FOREGROUND_BLUE;
const int BackBlue			= BACKGROUND_BLUE;
const int TextRed			= FOREGROUND_RED | FOREGROUND_INTENSITY;
const int BackRed			= BACKGROUND_RED | BACKGROUND_INTENSITY;

#endif

const int Black         = 0;
const int Blue			= 1;
const int Green			= 2;
const int Cyan			= 3;
const int Red			= 4;
const int Maganta		= 5;
const int Brown			= 6;
const int White			= 7;
const int Gray          = 8;
const int LightBlue     = 9;
const int LightGreen    = 10;
const int LightCyan     = 11;
const int LightRed      = 12;
const int LightMagenta  = 13; 
const int Yellow        = 14;
const int BrightWhite   = 15;

#ifdef WIN32
int ToWinColor( int TextColor, int BackColor )
{
int color = 0;

	switch( TextColor )
	{
	case White :
				color |= TextWhite;
				break;
	case Blue  :
				color |= TextBlue;
				break;
	case Red  :
				color |= TextRed;
				break;
    case Black  :
				color |= TextBlack;
				break;
	}

	switch( BackColor )
	{
	case White :
				color |= BackWhite;
				break;
	case Red  :
				color |= BackRed;
				break;
	case Blue  :
				color |= BackBlue;
				break;
    case Black  :
				color |= BackBlack;
				break;
	}
	return color;
}
#endif // WIN32

screen::screen(int MinX,int MaxX,int MinY,int MaxY)
{                                  
   this->MinX = MinX;
   this->MinY = MinY;
   this->MaxX = MaxX;
   this->MaxY = MaxY; 
   
   BackColor = White;
   TextColor = Black;

   PosX = MinX; PosY = MinY;
   StatusLine =MaxY;
   
   ScreenBuf = new int[(MaxX-MinX+1)*(MaxY-MinY+1) + 256 /*save*/ ];  
   EmtyLine  = new char[MaxX-MinX+2];
   
   memset( EmtyLine, ' ', MaxX-MinX+1 );
   EmtyLine[MaxX-MinX] = 0;

   #ifdef WIN32
   AllocConsole();
   HandelStdOut = GetStdHandle( STD_OUTPUT_HANDLE );
   Screen.Normal();

   #else
   OldTextColor = _gettextcolor ();
   OldBackColor = (int) _getbkcolor ();
   #endif

   NoCursor();
   
   #ifdef SCREEN_DEBUG
     Pos( 0,0 );
     DispVideoConfig();
     DispError( "hit any key to continue" );
   #endif
}

screen::~screen()
{                                  
   delete  ScreenBuf;        
   delete  EmtyLine;
   
#ifdef WIN32
   // close windows console
   // FreeConsole();
   system( "cls" );
#else
	Color( OldBackColor, OldTextColor );
	NormalCursor();
#endif
   
#ifdef SCREEN_DEBUG
    Pos( 0,0 );
    DispVideoConfig();
    DispError( "hit any key to continue" );
#endif
}
int screen::Clear()    
{          
#ifdef WIN32
int i;
    
	for ( i=MinY; i <= MaxY; i++ )
	{
		ScSetPos( 0, i);                    
		ScWriteText( EmtyLine );	
	}
#else
   _clearscreen( _GCLEARSCREEN );
#endif
   memset( ScreenBuf, ' ', 2 * (MaxX-MinX+1)*(MaxY-MinY+1) );
   return 0;
}
int screen::Clear(int y)
{
    Pos( MinX,y );
    ScWriteText( EmtyLine );         
    memset( ScreenBufP, ' ', 2 * (MaxY-MinY+1) );
    return 0;
}
int screen::Clear(int x,int y)
{
char help[2];

    Pos( x,y );
    help[0]=' ';
    help[1]=0;
    ScWriteText( help );
    return 0;
}
int screen::ClearStatus()
{
    Pos( MinX,MaxY );
    ScWriteText( EmtyLine );
    return 0;
}
int screen::Put(char ch)
{   
char help[2];

    help[0]=ch;
    help[1]=0;
    ScWriteText( help );
    return 0;
}
int screen::Put(int x,int y,char ch)
{
char help[2];

    help[0]=ch;
    help[1]=0;
    Pos( x,y );
    ScWriteText( help );
    return 0;
}
int screen::Put( char *str)
{
    ScWriteText( str );
    return 0;
}
int screen::Put(int x,int y,char *str)
{                             
    Pos( x,y );
    ScWriteText( str );
    return 0;
}
int screen::Pos(int x,int y )
{   
    if( x >= MaxX ) PosX = MaxX; else PosX = x;
    if( y >= MaxY ) PosY = MaxY; else PosY = y;
    ScSetPos( PosX,PosY );
    return 0;
}
int screen::GetPos(int x,int y )
{
    return 0;
}                  
int screen::Status( char *str)
{
    Pos( MinX,MaxY );
    Put( str );
    return 0;
}
/*--------------------------------------------------------------------------
        Funktion whitch use spacific Graphic Funktions
----------------------------------------------------------------------------
*/

void screen::NormalCursor()
{
#ifdef WIN32
CONSOLE_CURSOR_INFO mode;

    CursorMode = 1;
	mode.dwSize = 15; mode.bVisible = 1;
	SetConsoleCursorInfo( HandelStdOut, &mode );
#else
   CursorMode = 1; _settextcursor (0x0707);
#endif
}
void screen::BigCursor()
{
#ifdef WIN32
CONSOLE_CURSOR_INFO mode;

    CursorMode = 2;
	mode.dwSize = 99; mode.bVisible = 1;
    SetConsoleCursorInfo( HandelStdOut, &mode );
#else
   CursorMode = 2; _settextcursor (0x0007);
#endif
}
void screen::NoCursor()
{
#ifdef WIN32
CONSOLE_CURSOR_INFO mode;

    CursorMode = 0;
	mode.dwSize = 50; mode.bVisible = 0;
    SetConsoleCursorInfo( HandelStdOut, &mode );

#else
   CursorMode = 0; _settextcursor (0x2000);
#endif
}
void screen::Normal(void)
{
#ifdef WIN32
    SetConsoleTextAttribute( HandelStdOut, ToWinColor( TextColor, BackColor ) );
#else
   _setbkcolor( BackColor);    _settextcolor( TextColor);
#endif
}
void screen::Invert(void)
{
#ifdef WIN32
    SetConsoleTextAttribute( HandelStdOut, ToWinColor( BackColor, TextColor ) );
#else
	_setbkcolor( TextColor);    _settextcolor( BackColor);
#endif

}

void screen::Color( int Mode )
{ 
   switch( Mode )
   {
    case 2: // Black White
            Color( White, Black);         
            break;
    case 1: // Blue  White
            Color( Blue, White);         
            break;
   }        
}          
#pragma optimize( "", on )
inline void screen::ScWriteText( char *str )
{
int i, x, y;                                  
    
    x = PosX; y=PosY;
    #ifdef WIN32
     printf( str ); 
    #else
    _outtext( str ); 
	#endif
    
    for( i=0; i<80; i++,x++ )
    {                      
      if( x > MaxX ) break;
      if( str[i] == 0 ) break;                   
      ScreenBuf[ y*(MaxX-MinX+1)+x ] = str[i];
    } 
}
#pragma optimize( "", on )
inline void screen::ScSetPos( int x, int y )                    
{
    ScreenBufP = &ScreenBuf[ y*(MaxX-MinX+1)+x ]; 
    #ifdef WIN32
	COORD pos;	

    	pos.X = x; pos.Y = y;
		SetConsoleCursorPosition( HandelStdOut, pos);
    #else
    _settextposition( y+1, x+1 );
    #endif
} 
void screen::ToFile( char *FileName )                   
{
FILE *stream;
int x,y;
    
    if( (stream = fopen( FileName, "a" )) != NULL )
    {   
        fprintf( stream,"-------------------- new page --------------------\n" );
        for( y=MinY;y<=MaxY; y++ )
        {
           for( x=MinX; x<=MaxX; x++ )
           {
              fputc( ScreenBuf[ y*(MaxX-MinX+1)+x ], stream );
           }
           fputc( '\n', stream );
        } 
        fclose( stream );
    }
}
#ifdef SCREEN_DEBUG
/*--------------------------------------------------------------------------
----------------------------------------------------------------------------
*/
void screen::DispVideoConfig()
{
   struct _videoconfig vc;
   short  c;
   char   b[500];               /* Buffer for string */
   _getvideoconfig( &vc );
   /* Write all information to a string, then output string. */
   c  = sprintf( b,     "X pixels:     %d\n", vc.numxpixels );
   c += sprintf( b + c, "Y pixels:     %d\n", vc.numypixels );
   c += sprintf( b + c, "Text columns: %d\n", vc.numtextcols );
   c += sprintf( b + c, "Text rows:    %d\n", vc.numtextrows );
   c += sprintf( b + c, "Colors:       %d\n", vc.numcolors );
   c += sprintf( b + c, "Bits/pixel:   %d\n", vc.bitsperpixel );
   c += sprintf( b + c, "Video pages:  %d\n", vc.numvideopages );
   c += sprintf( b + c, "Mode:         %d\n", vc.mode );
   c += sprintf( b + c, "Adapter:      %d\n", vc.adapter );
   c += sprintf( b + c, "Monitor:      %d\n", vc.monitor );
   c += sprintf( b + c, "Memory:       %d\n", vc.memory );
   _outtext( b );
}                  
#endif      
/*--------------------------------------------------------------------------
    disp error masssge in the last screen line
----------------------------------------------------------------------------
*/
int screen::DispError(char *str)
{
int  ret,color;

   NoCursor();
   color = GetBackColor();
   SetBackColor( Red );
   ClearStatus();
   Status(str);
   ret =  GetKey();
   SetBackColor( color );
   ClearStatus();
   Normal();
   return (ret);
}
/*---------------------------------------------------------------------
   display working massage
-----------------------------------------------------------------------
*/
int screen::Massage (FILE * stream, char *str, int flag)
{
   if (flag == 0)
   {
      fprintf (stream, "%s\n", str);
   }
   else
   {
      Invert ();
      Status( str );
      Normal();
   }
   return 0;
}


